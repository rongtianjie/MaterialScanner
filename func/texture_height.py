# cython:language_level=3
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
    rot_count_half = conf["geometry"]["rot_count"]//2

    depth_min = camera_height + conf["height_map"]['depth_min']
    depth_max = camera_height + conf["height_map"]['depth_max']
    std_ratio = conf["height_map"]['filtering_std_ratio']

    mid_mat_k = camera_matrices['mid_mat_k']
    right_mat_k = camera_matrices['side_mat_k']
    mat_rt = camera_matrices['mat_rt']

    scale = conf["scale"]
    name = conf["name"]

    model_filtering = conf["settings"]["model_filtering"]

    logger.info(f'Start to process depth with depth_min={depth_min}, depth_max={depth_max}, camera_height={camera_height}')

    depth, height = None, None

    if right_undist_image is not None:
        # slove depth and height
        depth = solve_depth(mid_undist_image, right_undist_image, scale, output_folder,
                            mid_mat_k, right_mat_k, mat_rt, camera_height, depth_min, depth_max, rot_count_half, model_filtering, std_ratio)
        height = depth_to_height(depth, camera_height, conf["height_map"]['scale_factor'])
        cv2.imencode('.exr', height)[1].tofile(f'{output_folder}/T_{name}_Displacement_{8//scale}K.exr')
        logger.success(f"Height map generated successfully !")

    return depth, height


def solve_depth(left_data, right_data, scale, fp, mat_l, mat_r, mat_rt, camera_height, depth_min, depth_max, rot_count_half, model_filtering, std_ratio):
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

    for i in range(left_data.shape[0]//rot_count_half):
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

    if model_filtering:
        pcd = o3d.io.read_point_cloud((f'{fp}ACMMP/ACMMP_model.ply').encode('gbk'))
        logger.info('Filtering cloud points...')
        pcd_denoise = pointcloud_filtering(pcd, std_ratio)
        o3d.io.write_point_cloud((f'{fp}ACMMP/ACMMP_model_filtered.ply').encode('gbk'), pcd_denoise)
        mat_l[0, 2] += 1024//scale
        mat_l[:2] *= scale
        mat_l[:2] /= 3
        mat_l[0, 2] -= 1024//3
        logger.info("Converting cloud points to depth map...")
        cropped_depth = pointcloud_to_depth(pcd_denoise, mat_l, scale)
    else:
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
def pointcloud_filtering(pcd, std_ratio):

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
        return pcd

    logger.info('Start to voxel down !')
    voxel_down_pcd = pcd.voxel_down_sample(voxel_size = 1) # 0.005 two_views:1
    logger.success('Finish voxel down !')

    logger.info('Start to guide filter !')
    voxel_down_pcd = guided_filter(voxel_down_pcd, 0.05, 0.1)
    logger.success('Finish guide filter !')

    logger.info('Start to remove outlier !')
    pcd_denoise, ind = voxel_down_pcd.remove_statistical_outlier(nb_neighbors=50, std_ratio=std_ratio, print_progress=True) # 0.5 5  two_views:0.1 0.5
    logger.success('Finish remove outlier !')

    return pcd_denoise

# point cloud to depth map
def pointcloud_to_depth(pcd, intrinsic, scale):
    pts = np.asarray(pcd.points)
    pts = np.concatenate([pts, np.ones((pts.shape[0], 1))], axis=-1)
    #pts_cam = pts.dot(extrinsic.T)
    pts_cam = pts[:, 0:3]

    pts_2d = pts_cam.dot(intrinsic.T)

    xy = pts_2d[:, 0:2] / pts_2d[:, 2:3]
    z = pts_2d[:, 2]

    # H, W = int(intrinsic[1][2]*2), int(intrinsic[0][2]*2)
    H, W = 2912, 3200
    xy = np.round(xy)
    val_inds = (xy[:, 0] >= 0) & (xy[:, 1] >= 0) & (xy[:, 0] < W) & (xy[:, 1] < H)

    xy = xy[val_inds, :].astype(np.int32)
    z = z[val_inds]

    depth = np.zeros((H, W), dtype=np.float32)
    depth[xy[:, 1], xy[:, 0]] = z

    # more than one vertex contribute to a pixel
    # inds = sub2ind(depth.shape, xy[:, 1], xy[:, 0])   # flattened_xy
    # dupe_inds = [item for item, count in Counter(inds).items() if count > 1]
    #
    for i in range(xy.shape[0]):
        x_t = xy[i, 0]
        y_t = xy[i, 1]
        z_t = z[i]
        if depth[y_t, x_t] > z_t:
            depth[y_t, x_t] = z_t
    #
    # def remove_dupe_inds(dupe_inds):
    #     for dd in dupe_inds:
    #         pts = np.where(inds == dd)[0]
    #         x_loc = xy[pts[0], 0]
    #         y_loc = xy[pts[0], 1]
    #         depth[y_loc, x_loc] = z[pts].min()
    #
    # num_workers = os.cpu_count()
    # len_ = len(dupe_inds) // num_workers
    # thds = []
    # for i in range(num_workers):
    #     if i < num_workers - 1:
    #         t = threading.Thread(target=remove_dupe_inds, args=[dupe_inds[i*len_: (i+1)*len_]])
    #     else:
    #         t = threading.Thread(target=remove_dupe_inds, args=[dupe_inds[i * len_:]])
    #     thds.append(t)
    #     t.start()
    # for t in thds:
    #     t.join()
    depth[depth < 0] = 0

    grid_x, grid_y = np.mgrid[0:depth.shape[0], 0:depth.shape[1]]
    valid = np.stack((grid_x, grid_y, depth), axis=-1)[depth > 0]
    points = valid[..., 0:2]
    values = valid[..., 2]
    base_depth = np.mean(sorted(depth[depth > 0], reverse=True)[: int(len(depth) * 0.1)])
    full_depth = griddata(points, values, (grid_x, grid_y), method='cubic', fill_value=base_depth)
    # full_depth = full_depth[:, W//2-4800: W//2+4800]  # (9600, 8736)
    full_depth = cv2.resize(full_depth, (9600 // scale, 8736 // scale))
    cropped_depth = full_depth[68*4//scale:-68*4//scale, 176*4//scale:-176*4//scale].astype(np.float32)
    return cropped_depth
