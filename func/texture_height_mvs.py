# cython:language_level=3
import sys, os
sys.path.append(os.getcwd())
os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'
ext_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "../ext"))
os.environ['PATH'] = os.environ['PATH'] + f"{ext_dir};"

from base.common import *
from base.common_image import *
from base.config_handler import read_conf
import ctypes
from ctypes import c_float, c_int
import open3d as o3d
from scipy.interpolate import griddata

@count_time
@logger.catch
def produce_height_map(conf):
    # read configs
    input_path = conf["input_folder"]
    output_path = conf["output_folder"]

    scale = conf["scale"]
    name = conf["name"]

    camera_height = conf["geometry"]["camera_height"]
    base_height = conf["geometry"]["base_height"]

    conf["settings"]["sort_images"] = False

    # Load images
    scale_archive = conf["scale"]
    conf["scale"] = 1
    mid_rgb, _, cache = read_folder(os.path.join(input_path, "MVS", "mid"), conf, keep_rgb=True)
    side_rgb, _, cache = read_folder(os.path.join(input_path, "MVS", "side"), conf, keep_rgb=True)
    conf["scale"] = scale_archive
    logger.success("MVS images loaded")

    # slove depth and height
    depth, mat_k = solve_depth(mid_rgb, side_rgb, conf)

    # depth to height
    height = camera_height - base_height - depth
    height_world = camera_height - depth
    height[height < 0] = 0
    height_normalized = height / np.max(height)
    cv2.imencode('.exr', height_normalized)[1].tofile(f'{output_path}/T_{name}_Displacement_{8//scale}K.exr')
    logger.success(f"Height map generated successfully !")

    return depth, height_world, mat_k

@count_time
def solve_depth(mid_data, side_data, conf):
    scale = conf["scale"]
    fp = conf["output_folder"]
    camera_height = conf["geometry"]["camera_height"]
    base_height = conf["geometry"]["base_height"]

    if os.path.exists(os.path.join(fp, "acmmp_reference", "ACMMP", "result.json")):
        with open(os.path.join(fp, "acmmp_reference", "ACMMP", "result.json"), "r") as f:
            result = json.load(f)
            base_depth, focal_length = result["base_depth"], result["focal_length"]
        logger.success("ACMMP result found. Skip processing.")
        fp = os.path.join(fp, "acmmp_reference")
    else:
        fp = os.path.join(fp, "acmmp_reference")
        os.makedirs(os.path.join(fp, 'images'), exist_ok=True)
        os.makedirs(os.path.join(fp, 'masks'), exist_ok=True)

        img_data_raw = np.vstack((np.expand_dims(mid_data[0], axis=0), side_data))
        mask_raw = mid_data[1]

        logger.info("Converting data depth and writing pngs...")
        img_data = []
        executor = ProcessPoolExecutor()
        tasks = [executor.submit(image_subprocess, img_data_raw[i], fp, i) for i in range(img_data_raw.shape[0])]
        for task in tqdm(as_completed(tasks), total=len(tasks)):
            img_data.append(task.result())
        img_data = np.array(img_data)

        del img_data_raw

        # Process mask
        mask = (mask_raw/257).astype(np.uint8)
        mask = rgb_to_srgb(mask, 255)
        if len(mask.shape) == 3:
            mask = cv2.cvtColor(mask, cv2.COLOR_RGB2GRAY)
        mask = convert_mask(mask)
        # mask = cv2.resize(mask, (3200, 2400))
        cv2.imencode('.png', mask)[1].tofile(f'{fp}/masks/0.png')

        # Process aruco
        _, upper_left, lower_right = extract_aruco(img_data[0], 9)
        c1, r1, c2, r2 = upper_left[0], upper_left[1], lower_right[0], lower_right[1]
        logger.info(f"UL: {upper_left}, LR: {lower_right}")
        
        base_depth, focal_length = call_colmap_and_acmmp(fp, r1, c1, r2, c2)

    # load point cloud result
    pcd = o3d.io.read_point_cloud(os.path.join(fp, "ACMMP", "ACMMP_model.ply").encode('gbk'))
    logger.info(f"Point cloud count: {len(pcd.points)}")

    pcd = pointcloud_filtering(pcd)
    pcd = remove_hidden_points(pcd)

    o3d.io.write_point_cloud(os.path.join(fp, "ACMMP", "ACMMP_model_filtered.ply").encode('gbk'), pcd)
    mat_k = np.array([[focal_length, 0, 1600], [0, focal_length, 1200], [0, 0, 1]])

    cropped_depth = pointcloud_to_depth(pcd, mat_k, scale)

    mat_k[:2] = mat_k[:2] * 3.64
    mat_k[0,2] = mat_k[0,2] - 1728
    mat_k[1,2] = mat_k[1,2] - 272
    mat_k[:2] = mat_k[:2] // scale

    # depth to real depth
    real_depth = cropped_depth * (camera_height-base_height) / base_depth

    return real_depth, mat_k

def image_subprocess(img, fp, i):
    img = (img / 257).astype(np.uint8)
    img = rgb_to_srgb(img, 255)
    img = cv2.resize(img, (3200, 2400))
    cv2.imencode('.png', img)[1].tofile(f'{fp}/images/{i}.png')
    return img

@count_time
def call_colmap_and_acmmp(fp, r1, c1, r2, c2):

    class StructPointer(ctypes.Structure):
        _fields_ = [("base_depth", c_float), ("focal_length", c_float)]
    
    fp = fp.replace('/', '\\')
    dll_path = os.path.join(os.getcwd(), 'ext/')
    dll_path = dll_path.replace('/', '\\')

    r1, c1, r2, c2 = int(r1), int(c1), int(r2), int(c2)
    r1, c1, r2, c2 = c_int(r1), c_int(c1), c_int(r2), c_int(c2)

    lib = ctypes.windll.LoadLibrary("ext/ma_mvs.dll")
    lib.stereo_depth.restype = ctypes.POINTER(StructPointer)

    try:
        p = lib.stereo_depth(bytes(fp, encoding='GBK'), bytes(dll_path, encoding='GBK'), r1, c1, r2, c2)
    except:
        logger.error("Error in calling mvs dll")
        raise

    base_depth = p.contents.base_depth
    focal_length = p.contents.focal_length

    logger.info(f"Base depth: {base_depth} | Focal length: {focal_length}")

    result = {
        "base_depth": base_depth,
        "focal_length": focal_length
    }
    # save result to json file
    with open(os.path.join(fp, "ACMMP", "result.json"), "w") as f:
        json.dump(result, f)
    logger.info("Result saved to [result.json]")
    return base_depth, focal_length

def extract_aruco(image, id0):
    image = cv2.resize(image, (3200, 2400))
    params = cv2.aruco.DetectorParameters_create()
    dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_1000)
    params.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX
    params.adaptiveThreshWinSizeMin = 3
    params.adaptiveThreshWinSizeMax = 123
    params.adaptiveThreshWinSizeStep = 2
    markers, ids, _ = cv2.aruco.detectMarkers(image, dictionary, parameters=params)
    markers, ids = np.array(markers), np.array(ids)
    logger.info(f"Found {len(ids)} markers:")
    logger.info(ids.reshape(-1))

    try:
        marker = markers[ids == id0][0]
    except:
        exit_with_error("No preset aruco marker found")
    center = np.mean(marker, axis=0)

    left_2, right_2 = [], []
    for i in range(marker.shape[0]):
        if marker[i, 1] < center[1]:
            left_2.append(marker[i])
        else:
            right_2.append(marker[i])
    
    if left_2[0][0] < left_2[1][0]:
        upper_left = left_2[0]
    else:
        upper_left = left_2[1]
    
    if right_2[0][0] < right_2[1][0]:
        lower_right = right_2[1]
    else:
        lower_right = right_2[0]
    return marker, upper_left, lower_right

def convert_mask(mask):
    _, thresh = cv2.threshold(mask, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
    mask = cv2.dilate(thresh, None, iterations=4)
    mask = cv2.erode(mask, None, iterations=4)

    mask = cv2.erode(mask, None, iterations=4)
    mask = cv2.medianBlur(mask, 5)
    mask = cv2.dilate(mask, None, iterations=4)

    def remove_small_components(mask, min_size=2000):
        nb_components, labels, stats, _ = cv2.connectedComponentsWithStats(mask, connectivity=4)
        sizes = stats[1:, -1]
        nb_components = nb_components - 1
        mask2 = np.zeros((mask.shape), dtype=np.uint8)
        for i in range(0, nb_components):
            if sizes[i] >= min_size:
                mask2[labels == i + 1] = 255
        return mask2

    mask2 = remove_small_components(mask)
    mask3 = remove_small_components(255-mask2)
    
    return mask3

@count_time
def remove_hidden_points(pcd):
    diameter = np.linalg.norm(np.asarray(pcd.get_max_bound()) - np.asarray(pcd.get_min_bound()))
    camera = [0, 0, -diameter]
    radius = diameter * 10000
    _, pt_map = pcd.hidden_point_removal(camera, radius)
    pcd = pcd.select_by_index(pt_map)
    return pcd

# remove the noise in point cloud
@count_time
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
        return pcd

    logger.info('Start to voxel down')
    voxel_down_pcd = pcd.voxel_down_sample(voxel_size = 0.005) # 0.005 two_views:1
    logger.success('Finish voxel down')

    # logger.info('Start to guide filter')
    # voxel_down_pcd = guided_filter(voxel_down_pcd, 0.05, 0.1)
    # logger.success('Finish guide filter')

    logger.info('Start to remove outlier')
    pcd_denoise, ind = voxel_down_pcd.remove_statistical_outlier(nb_neighbors=200, std_ratio=5, print_progress=True) # 0.5 5  two_views:0.1 0.5
    logger.success('Finish remove outlier')

    return pcd_denoise

# point cloud to depth map
@count_time
def pointcloud_to_depth(pcd, intrinsic, scale):
    pts = np.asarray(pcd.points)
    pts = np.concatenate([pts, np.ones((pts.shape[0], 1))], axis=-1)
    #pts_cam = pts.dot(extrinsic.T)
    pts_cam = pts[:, 0:3]

    pts_2d = pts_cam.dot(intrinsic.T)

    xy = pts_2d[:, 0:2] / pts_2d[:, 2:3]
    z = pts_2d[:, 2]

    # H, W = int(intrinsic[1][2]*2), int(intrinsic[0][2]*2)
    H, W = 2400, 3200
    xy = np.round(xy)
    val_inds = (xy[:, 0] >= 0) & (xy[:, 1] >= 0) & (xy[:, 0] < W) & (xy[:, 1] < H)

    xy = xy[val_inds, :].astype(np.int32)
    z = z[val_inds]

    depth = np.zeros((H, W), dtype=np.float32)
    depth[xy[:, 1], xy[:, 0]] = z

    for i in range(xy.shape[0]):
        x_t = xy[i, 0]
        y_t = xy[i, 1]
        z_t = z[i]
        if depth[y_t, x_t] > z_t:
            depth[y_t, x_t] = z_t

    depth[depth < 0] = 0

    grid_x, grid_y = np.mgrid[0:depth.shape[0], 0:depth.shape[1]]
    valid = np.stack((grid_x, grid_y, depth), axis=-1)[depth > 0]
    points = valid[..., 0:2]
    values = valid[..., 2]
    base_depth = np.mean(sorted(depth[depth > 0], reverse=True)[: int(len(depth) * 0.1)])
    full_depth = griddata(points, values, (grid_x, grid_y), method='cubic', fill_value=base_depth)
    full_depth = cv2.resize(full_depth, (11648 // scale, 8736 // scale))
    cropped_depth = full_depth[68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale].astype(np.float32)
    return cropped_depth

if __name__ == '__main__':

    set_log_level("INFO", "DEBUG")
    input_path = r"D:\images\20221012test\material"
    output_path = os.path.join(input_path, "out")
    scale = 1

    lens = 50
    focus_id = 0
    conf = read_conf(input_path, lens, scale, shoot_type="stereo", focus_id=focus_id)

    conf["settings"]["sort_images"] = False
    conf["scale"] = 1
    conf["input_folder"] = input_path
    conf["output_folder"] = output_path
    conf["name"] = os.path.basename(input_path)

    depth, height_world, mat_k = produce_height_map(conf)