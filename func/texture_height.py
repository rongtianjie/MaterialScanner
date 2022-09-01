import os
from base.common import *
from base.common_image import *
from scipy.interpolate import griddata
import ctypes
from ctypes import *
import xml.etree.ElementTree as ET
import open3d as o3d

@count_time
def produce_height_map(mid_undist_image, right_undist_image, conf):
    # read configs
    output_folder = conf["output_folder"]
    camera_matrices = conf["camera_matrices"]
    camera_height = conf["geometry"]["camera_height"]

    depth_min = camera_height + conf["height_map"]['depth_min']
    depth_max = camera_height + conf["height_map"]['depth_max']

    mid_mat_k = camera_matrices['mid_mat_k']
    right_mat_k = camera_matrices['side_mat_k']
    mat_rt = camera_matrices['mat_rt']

    scale = conf["scale"]
    name = conf["name"]

    logger.info(f'Start to process depth with depth_min={depth_min}, depth_max={depth_max}, camera_height={camera_height}')

    depth, height = None, None

    if right_undist_image is not None:
        # slove depth and height
        depth = solve_depth(mid_undist_image, right_undist_image, scale, output_folder,
                            mid_mat_k, right_mat_k, mat_rt, camera_height, depth_min, depth_max)
        height = depth_to_height(depth, camera_height, conf["height_map"]['scale_factor'])
        cv2.imencode('.exr', height)[1].tofile(f'{output_folder}/T_{name}_Displacement_{8//scale}K.exr')

    return depth, height


def solve_depth(left_data, right_data, scale, fp, mat_l, mat_r, mat_rt, camera_height, depth_min, depth_max):
    lib = ctypes.windll.LoadLibrary("ext/ma_stereo.dll")

    k_left = (c_float*9)(*[0 for _ in range(9)])
    k_right = (c_float*9)(*[0 for _ in range(9)])
    rt = (c_float*16)(*[0 for _ in range(16)])

    fp += '/acmmp_reference/'
    os.makedirs(fp + 'images/', exist_ok=True)
    mat_l = mat_l.copy()
    mat_r = mat_r.copy()
    mat_l[0, 2] -= 1024 // scale
    mat_r[0, 2] -= 1024 // scale
    rot_count_half = left_data.shape[0]//8
    for i in range(8):
        img = np.concatenate(
            (left_data[i*rot_count_half][:, 1024//scale:-1024//scale], right_data[i*rot_count_half][:, 1024//scale:-1024//scale]))
        img = rgb_to_srgb(img, 65535)

        #img = (img*255).astype('uint8')
        #img = cv2.equalizeHist(img).astype('uint16')*257
        # cv2.imwrite(f'{fp}/images/{"%08d"%(i*2)}.png',
        #            gray[0][i+8][:,1024//4:-1024//4])
        #cv2.imwrite(f'{fp}/images/{"%08d"%(i*2+1)}.png', gray[3][i+8][:,:-2048//4])
        cv2.imencode('.png', img[:8736//scale]
                     )[1].tofile(f'{fp}/images/{"%08d"%(i*2)}.png')
        cv2.imencode('.png', img[8736//scale:]
                     )[1].tofile(f'{fp}/images/{"%08d"%(i*2+1)}.png')
    fp = fp.replace('/', '\\')
    lib.stereo_depth.argtypes = [c_char_p, POINTER(c_float), POINTER(c_float), POINTER(c_float), c_float, c_float]
    lib.stereo_depth.restype = None

    for i in range(mat_l.reshape(-1).shape[0]):
        k_left[i] = mat_l.reshape(-1)[i]

    for i in range(mat_r.reshape(-1).shape[0]):
        k_right[i] = mat_r.reshape(-1)[i]

    for i in range(mat_rt.reshape(-1).shape[0]):
        rt[i] = mat_rt.reshape(-1)[i]

    lib.stereo_depth(bytes(fp, encoding='GBK'), k_left, k_right, rt, depth_min, depth_max)

    use_xml = True
    if use_xml:
        # read depth map from .xml
        root = ET.parse(f'{fp}ACMMP/depth.xml').getroot()
        depth = [[x] for x in root[0][3].text.split( )]
        row = int(root[0][0].text)
        col = int(root[0][1].text)
        depth = np.array(depth, dtype=np.float32).reshape(row, col)

        grid_x, grid_y = np.mgrid[0:depth.shape[0], 0:depth.shape[1]]
        valid = np.stack((grid_x, grid_y, depth), axis=-1)[depth > 0]
        points = valid[..., :2]
        values = valid[..., 2]
        add_valid = np.stack((grid_x, grid_y, np.zeros(depth.shape)), axis=-1)
        add_valid[0, :, 2] = camera_height
        add_valid[-1, :, 2] = camera_height
        add_valid[:, 0, 2] = camera_height
        add_valid[:, -1, 2] = camera_height
        add_valid = add_valid[add_valid[..., 2] != 0]
        add_points = add_valid[..., :2]
        add_values = add_valid[..., 2]
        full_depth = griddata(np.concatenate((points, add_points)), np.concatenate((values, add_values)), (grid_x, grid_y), method='cubic')
        full_depth = cv2.resize(full_depth, (9600//scale, 8736//scale))
        cropped_depth = full_depth[68*4//scale:-68*4//scale, 176*4//scale:-176*4//scale].astype(np.float32)
    return cropped_depth

def depth_to_height(depth, camera_height, scale_factor):
    height = (camera_height - depth) * scale_factor
    height += 0.5
    height = np.maximum(0, height)
    height = np.minimum(1, height)
    return height

# remove the noise in point cloud
def pointcloud_filtering(pcd):

    def guided_filter(pcd, radius, epsilon):
        kdtree = o3d.geometry.KDTreeFlann(pcd)

        points_copy = np.array(pcd.points)
        points = np.asarray(pcd.points)
        num_points = len(pcd.points)

        for i in range(num_points):
            k, idx, _ = kdtree.search_radius_vector_3d(pcd.points[i], radius)
            if k < 3:
                continue

            neighbors = points[idx, :]
            mean = np.mean(neighbors, 0)
            cov = np.cov(neighbors.T)
            e = np.linalg.inv(cov + epsilon * np.eye(3))

            A = cov @ e
            b = mean - A @ mean

            points_copy[i] = A @ points[i] + b

        pcd.points = o3d.utility.Vector3dVector(points_copy)

    def display_inlier_outlier(cloud, ind):
        inlier_cloud = cloud.select_by_index(ind)
        o3d.io.write_point_cloud("./save/cloth-1-3.ply", inlier_cloud)

        outlier_cloud = cloud.select_by_index(ind, invert=True)
        outlier_cloud.paint_uniform_color([0, 1, 0])
        #inlier_cloud.paint_uniform_color([0.8, 0.8, 0.8])
        # 可视化
        o3d.visualization.draw_geometries([inlier_cloud, outlier_cloud])

    print('Start to voxel down !')
    voxel_down_pcd = pcd.voxel_down_sample(voxel_size = 1) # 0.005 two_views:1
    print('Finish voxel down !')

    print('Start to guide filter !')
    guided_filter(voxel_down_pcd, 0.05, 0.1)
    print('Finish guide filter !')

    print('Start to remove outlier !')
    pcd_denoise, ind = voxel_down_pcd.remove_statistical_outlier(nb_neighbors=50, std_ratio= 0.5, print_progress=True) # 0.5 5  two_views:0.1 0.5
    print('Finish remove outlier !')

    # display_inlier_outlier(voxel_down_pcd, ind)

    return pcd_denoise

# point cloud to depth map
def pointcloud_to_depth(pointcloud, intrinstic):
    pass
