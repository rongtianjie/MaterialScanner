from base.common import *
from base.common_image import read_folder
import cv2
import numpy as np
import json
import os
from glob import glob

@count_time
def generate_grayboard(grayboard_path, lens, blur_size=21, max_sample_count=5):
    logger.info('Generate grayboard data begin.')

    conf = read_conf(lens, 1)

    # print(conf["settings"]["multi_process_num"])

    grayboard_subfolder_list = os.listdir(grayboard_path)
    if len(grayboard_subfolder_list) > max_sample_count:
        grayboard_subfolder_list = grayboard_subfolder_list[:max_sample_count]
    
    gray_img_packs = []
    wb_sum = np.zeros([3])
    
    for subfolder in grayboard_subfolder_list:
        logger.info(f"Processing [{subfolder}]")
        grayboard_subfolder = os.path.join(grayboard_path, subfolder, "mid")

        img_num = len(glob(grayboard_subfolder + "/*.RAF"))
        sub_grays = read_folder(grayboard_subfolder, conf, finish_id=img_num//2, convert_to_gray=True)
        gray_img_packs.append(sub_grays)

        sub_grays_bgr = read_folder(grayboard_subfolder, conf, finish_id=img_num//2, convert_to_gray=False)

        wb_sum += sub_grays_bgr[:,68*4:-68*4, 432*4:-432*4].reshape(-1, 3).mean(axis = 0)

    sample_count = len(gray_img_packs)
    wb_sum /= wb_sum.max()

    if sample_count == 0:
        raise("Calib grayboard error: no grayboard data get.")
    elif sample_count == 1:
        out_grays = gray_img_packs[0]
    else:
        out_grays = np.stack(gray_img_packs)
        out_grays = np.median(out_grays, axis=0)

    if blur_size > 0:
        logger.info("Blur grayboard data.")
        out_grays_blur = cv2.GaussianBlur(out_grays.transpose(1,2,0), (blur_size, blur_size), 0)
        out_grays_blur = out_grays_blur.transpose(2,0,1).astype(np.uint16)

    np.savez(f'./pre_data/grayboard_result_{lens}.npz',sn=None, lens_sn=None, data=out_grays_blur, wb=wb_sum, camera_height=np.array(919.17493749))
    logger.info(f'Grayboard data saved to pre_data/grayboard_result_{lens}.npz')


def read_conf(lens, scale=1):
    if lens == "30":
        conf_path = "conf/fuji_30.json"
    if lens == "50":
        conf_path = "conf/fuji_50.json"
    elif lens == "120":
        conf_path = "conf/fuji_120.json"

    with open(conf_path) as f:
        raw = f.read()
    conf = json.loads(raw)

    camera_matrices = conf["camera_matrices"]
    # convert matrices
    mid_mat_k = np.array(camera_matrices['mid_mat_k']).reshape(3, 3)
    mid_mat_dist = np.array(camera_matrices['mid_mat_d']).reshape(5, 1)
    right_mat_k = np.array(camera_matrices['side_mat_k']).reshape(3, 3)
    right_mat_dist = np.array(camera_matrices['side_mat_d']).reshape(5, 1)
    mat_rt = np.array(camera_matrices['mat_rt']).reshape(4, 4)

    mid_mat_k[:2] *= camera_matrices['matrices_scale']
    right_mat_k[:2] *= camera_matrices['matrices_scale']

    mid_mat_k[:2] /= scale
    right_mat_k[:2] /= scale

    matrices_np = {}
    matrices_np['mid_mat_k'] = mid_mat_k
    matrices_np['mid_mat_d'] = mid_mat_dist
    matrices_np['side_mat_k'] = right_mat_k
    matrices_np['side_mat_d'] = right_mat_dist
    matrices_np['mat_rt'] = mat_rt
    matrices_np['scale'] = scale

    conf["camera_matrices"] = matrices_np
    conf["scale"] = scale

    # settings
    conf_path = "conf/settings.json"
    with open(conf_path) as f:
        raw = f.read()
    settings_conf = json.loads(raw)
    conf["settings"] = settings_conf

    return conf