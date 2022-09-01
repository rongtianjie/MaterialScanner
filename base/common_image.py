import hashlib
from base.common import *
from base.common_isp import read_raw, bayer_to_bgr
from tqdm import tqdm
import os
import cv2
import multiprocessing
import numpy as np


def read_folder(folder_path, conf, convert_to_gray=False, start_id=None, finish_id=None, cache=False):
    # read conf
    camera_matrices = conf["camera_matrices"]
    process_num = conf["settings"]["multi_process_num"]
    console_level = conf["settings"]["console_log_level"]
    file_level = conf["settings"]["file_log_level"]
    sort_images = conf["settings"]["sort_images"]

    # read cache
    if cache:
        scale = camera_matrices["scale"]
        key = f"read_folder_{folder_path}_{convert_to_gray}_{start_id}_{finish_id}_{scale}"
        cache_file = f"cache/{hashlib.md5(key.encode('utf-8')).hexdigest()}.npz"
        if os.path.exists(cache_file):
            logger.info(f"Load cache: {cache_file}")
            data = np.load(cache_file)
            return data["data"]
        
    # get images list
    img_list = []
    for filename in os.listdir(folder_path):
        if filename.lower().endswith(".raf"):
            img_list.append(filename)
    
    if img_list[0].startswith("DSCF") or img_list[0].startswith("MS"):
        logger.info("Find fuji official images.")
        img_list.sort()
    elif img_list[0].startswith("mid_") or img_list[0].startswith("right_"):
        logger.info("Find material scanner images.")
        img_list.sort(key=lambda x: int(x.split('.')[0].split('_')[-1]))
    else:
        exit_with_error(f"Unknown image name: {img_list[0]}")

    if sort_images:
        rot = conf["geometry"]["rot_count"]//2
        img_list_sorted = [0] * len(img_list)
        for i, j in [[i,j] for i in range(rot) for j in range(16)]:
            idx_list = [rot+i, rot*3+i, rot*5+i, rot*7+i, i, rot*2+i, rot*4+i, rot*6+i]
            idx_list = idx_list + [x+rot*8 for x in idx_list]
            img_list_sorted[idx_list[j]] = img_list[i*16+j]
        img_list = img_list_sorted
        logger.info("Images sorted.")

    img_list = img_list[start_id : finish_id]
    
    for filename in img_list:
        img_path = os.path.join(folder_path, filename)
        # logger.info(f"Read img: {img_path}")

    # read images
    results = []
    # fix process num
    if len(img_list) < process_num:
        process_num = len(img_list)
    if process_num == 1:
        for filename in tqdm(img_list):
            img_path = os.path.join(folder_path, filename)
            results.append(read_undistort_image(img_path, camera_matrices, convert_to_gray))
    else:
        pbar = tqdm(total=len(img_list))
        update = lambda x: pbar.update()
        pool = multiprocessing.Pool(processes=process_num, initializer=set_log_level, initargs=(console_level, file_level))
        for filename in img_list:
            img_path = os.path.join(folder_path, filename)
            results.append(pool.apply_async(read_undistort_image, args=(img_path, camera_matrices, convert_to_gray), callback=update))
        results = list(map(lambda x: x.get(), results))
        pool.close()
        pool.join()
    
    out_res = np.stack(results)

    # write cache
    if cache:
        logger.info(f"Save cache: {cache_file}")
        os.makedirs(os.path.dirname(cache_file), exist_ok=True)
        np.savez(cache_file, data=out_res)
    
    return out_res


def read_undistort_image(image_path, camera_matrices, convert_to_gray=False):
    logger.debug(f"Reading raw image: {image_path}")
    # isp
    bayer, exif = read_raw(image_path)
    wb_grb = exif[0]['RAF:WB_GRBLevels'].split()
    wb = [float(wb_grb[1]), float(wb_grb[0]), float(wb_grb[2])]
    out_img = bayer_to_bgr(bayer, wb, (0, 1))

    if convert_to_gray:
        out_img = cv2.cvtColor(out_img, cv2.COLOR_BGR2GRAY)

    # scale
    scale = camera_matrices["scale"]
    if scale != 1:
        out_img = cv2.resize(out_img, (0,0), fx=1/scale, fy=1/scale)

    # undistort
    mid_mat_k = camera_matrices['mid_mat_k']
    mid_mat_dist = camera_matrices['mid_mat_d']
    out_img = cv2.undistort(out_img, mid_mat_k, mid_mat_dist, mid_mat_k)
    # cv2.imwrite(image_path+".png", rgb_to_srgb(out_img, 65535))
    return out_img

def rgb_to_srgb(image, max_value):
    ret = image.astype(np.float32)
    ret /= max_value
    ret = np.where(ret > 0.0031308, 1.055 *
                   np.power(ret.clip(min=0.0031308), 1 / 2.4) - 0.055, 12.92 * ret)
    ret *= max_value
    ret = ret.astype(image.dtype)
    ret = np.maximum(ret, 0)
    ret = np.minimum(ret, max_value)
    return ret
