# cython:language_level=3
import hashlib
from base.common import *
from base.common_isp import read_raw, bayer_to_rgb
import cv2
import torch
from time import sleep

def read_folder(folder_path, conf, keep_rgb, start_id=None, finish_id=None):
    logger.info(f"Reading images from [{folder_path}] | keep_rgb: {keep_rgb}")
    if start_id or finish_id:
        logger.info(f"Read mode: [{start_id}: {finish_id}]")
    else:
        logger.info(f"Read mode: [all]")
    # read conf
    camera_matrices = conf["camera_matrices"]
    process_num = conf["settings"]["multi_process_num"]
    console_level = conf["settings"]["console_log_level"]
    file_level = conf["settings"]["file_log_level"]
    sort_images = conf["settings"]["sort_images"]
    scale = conf["scale"]
    DEMOSACING_METHOD = conf["settings"]["demosaicing_method_index"]

    cache_file = None 

    key = f"read_folder_{folder_path}_{keep_rgb}_{start_id}_{finish_id}_{scale}"
    cache_file = f"cache/{hashlib.md5(key.encode('utf-8')).hexdigest()}.npz"

    if os.path.exists(cache_file):
        # read cache
        try:
            logger.info(f"Loading cache: {cache_file}")
            data = np.load(cache_file)
            if keep_rgb:
                rgb = data["rgb"]
                if rgb is None:
                    logger.warning("[rgb] is None.")
                    raise Exception
            else:
                rgb = None
            gray = data["gray"]
            if gray is None:
                logger.warning("[gray] is None.")
                raise Exception
            logger.success("Cache loaded.")
            return rgb, gray, cache_file
        except:
            logger.warning(f"Cache file is broken: {cache_file}")

    # get images list
    img_list = []
    for filename in os.listdir(folder_path):
        if filename.lower().endswith(".raf"):
            img_list.append(filename)
    
    if img_list[0].startswith("DSCF") or img_list[0].startswith("MS"):
        logger.info("Find fuji official images.")
        img_list.sort()
    elif img_list[0].startswith("mid_") or img_list[0].startswith("right_") or img_list[0].startswith("side_"):
        logger.info("Find material scanner images.")
        img_list.sort(key=lambda x: int(x.split('.')[0].split('_')[-1]))
    else:
        exit_with_error(f"Unknown image name: {img_list[0]}")

    if sort_images:
        rot = conf["geometry"]["rot_count"]//2
        light_num = len(conf["geometry"]['light_str'])
        img_list_sorted = [0] * len(img_list)
        for i, j in [[i,j] for i in range(rot) for j in range(light_num*2)]:
            idx_list = [rot*x*2+i for x in range(light_num//2)]
            idx_list = [x+rot for x in idx_list] + idx_list
            idx_list = idx_list + [x+rot*light_num for x in idx_list]
            img_list_sorted[idx_list[j]] = img_list[i*light_num*2+j]
        if len(img_list)>rot*light_num*2:
            img_list_sorted[rot*light_num*2:] = img_list[rot*light_num*2:]
        img_list = img_list_sorted
        logger.info("Images sorted. Processing...")

    img_list = img_list[start_id : finish_id]

    # read images
    results = []
    # fix process num
    if len(img_list) < process_num:
        process_num = len(img_list)

    # Adjust process num based on GPU memory
    if DEMOSACING_METHOD == 3:
        if torch.cuda.is_available() and torch.cuda.mem_get_info(device=torch.device('cuda:0'))[0] > 7*1024**3:
            device = torch.device("cuda:0")
        else:
            device = torch.device("cpu")
        logger.info(f"Process num: {process_num} Device: {device}")
    elif DEMOSACING_METHOD == 4:
        device = torch.device("cpu")
        logger.info(f"Process num: {process_num} Device: {device}")
    else:
        device = None
        logger.info(f"Process num: {process_num}")
    
    if process_num == 1:
        for filename in tqdm(img_list):
            img_path = os.path.join(folder_path, filename)
            results.append(read_undistort_image(img_path, conf, keep_rgb, device))
    else:
        executor = ProcessPoolExecutor(max_workers=process_num)
        tasks = [executor.submit(read_undistort_image, os.path.join(folder_path, img_list[i]), conf, keep_rgb, device, i) for i in range(len(img_list))]
        for task in tqdm(as_completed(tasks), total=len(tasks)):
            results.append(task.result())
    
    gray = np.array([x[1] for x in results])
    if keep_rgb:
        rgb = np.array([x[0] for x in results])
    else: 
        rgb = None

    # write cache
    os.makedirs(os.path.dirname(cache_file), exist_ok=True)
    logger.info("Saving cache...")
    np.savez(cache_file, gray=gray, rgb=rgb)
    logger.info(f"Cache [{cache_file}] saved.")
    logger.success("Folder read.")

    return rgb, gray, cache_file


def read_undistort_image(image_path, conf, keep_rgb, device, idx=0):
    DEMOSACING_METHOD = conf["settings"]["demosaicing_method_index"]
    # logger.debug(f"Reading raw image: {image_path}")
    if idx < 8 and DEMOSACING_METHOD == 3:
        sleep(idx/2)
    # isp
    bayer, exif = read_raw(image_path)
    wb_grb = exif[0]['RAF:WB_GRBLevels'].split()
    wb = [float(wb_grb[1]), float(wb_grb[0]), float(wb_grb[2])]
    out_img_rgb = bayer_to_rgb(bayer, wb, (0, 1), DEMOSACING_METHOD = DEMOSACING_METHOD, device = device)

    # scale
    scale = conf["scale"]
    if scale != 1:
        out_img_rgb = cv2.resize(out_img_rgb, (0,0), fx=1/scale, fy=1/scale)

    if conf["lens"] == "120":
        # undistort
        camera_matrices = conf["camera_matrices"]
        mid_mat_k = camera_matrices['mid_mat_k']
        mid_mat_dist = camera_matrices['mid_mat_d']
        out_img_rgb = cv2.undistort(out_img_rgb, mid_mat_k, mid_mat_dist, mid_mat_k)
    # cv2.imwrite(image_path+".png", rgb_to_srgb(out_img, 65535))

    out_img_gray = cv2.cvtColor(out_img_rgb, cv2.COLOR_RGB2GRAY)

    if keep_rgb:
        return out_img_rgb, out_img_gray
    else:
        return None, out_img_gray

def rgb_to_srgb(image, max_value = -1):
    if max_value == -1:
        if image.dtype == np.uint8:
            max_value = 255
        elif image.dtype == np.uint16:
            max_value = 65535
        elif image.dtype == np.float32:
            max_value = 1
        else:
            raise ValueError("Unknown image type.")
    ret = image.astype(np.float32)
    ret /= max_value
    ret = np.where(ret > 0.0031308, 1.055 *
                   np.power(ret.clip(min=0.0031308), 1 / 2.4) - 0.055, 12.92 * ret)
    ret *= max_value
    ret = ret.astype(image.dtype)
    ret = np.maximum(ret, 0)
    ret = np.minimum(ret, max_value)
    return ret

@count_time
def check_images(input_path):
    if os.path.exists(os.path.join(input_path, "checked")):
        logger.info("Images already checked.")
    elif os.path.exists(os.path.join(input_path, "broken")):
        exit_with_error("One of the files is not a valid raw file")
    else:
        img_list = glob(os.path.join(input_path, "*.raf"))
        img_list += glob(os.path.join(input_path, "*", "*.raf"))
        img_list += glob(os.path.join(input_path, "*", "*", "*.raf"))
        try:
            executor = ProcessPoolExecutor(max_workers=8)
            tasks = [executor.submit(check, fp) for fp in img_list]
            for task in tqdm(as_completed(tasks), total=len(tasks)):
                task.result()
            wait(tasks)
        except:
            exit_with_error("One of the files is not a valid raw file")
        with open(os.path.join(input_path, "checked"), "w") as f:
            f.write("")
    logger.success("All images are valid.")

def check(fp):
    t1 = mp.Process(target=check_sub, args=(fp,))
    t1.start()
    t1.join()
    if t1.exitcode != 0:
        logger.error(f"Error in {fp}")
        raise Exception(f"Error in {fp}")

def check_sub(fp):
    _ = rawpy.imread(fp).sizes


