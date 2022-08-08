import os
from base.common import *
from base.common_image import *
from scipy.interpolate import griddata
import ctypes
from ctypes import *

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

    if False and right_undist_image is not None:
        # slove depth and height
        depth = solve_depth(mid_undist_image, right_undist_image, scale, output_folder,
                            mid_mat_k, right_mat_k, mat_rt, camera_height, depth_min, depth_max)
        cv2.imencode('.exr', height)[1].tofile(f'{output_folder}/T_{name}_Displacement_{8//scale}K.exr')

        height = depth_to_height(depth, camera_height, conf["height_map"]['scale_factor'])

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

    lib.stereo_depth(bytes(fp, encoding='utf-8'), k_left, k_right, rt, depth_min, depth_max)
    fs = cv2.FileStorage(f'{fp}ACMMP/depth.xml', cv2.FileStorage_READ)
    depth = fs.getNode('depth').mat()
    fs.release()
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