from base.common import *
from base.common_image import read_folder
import cv2
import numpy as np
import os
from base.config_handler import read_conf
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
        logger.info(f"Processing [{subfolder}]:")
        grayboard_subfolder = os.path.join(grayboard_path, subfolder, "mid")

        img_num = len(glob(grayboard_subfolder + "/*.RAF"))
        logger.info(f"Find {img_num} images.")
        logger.info("Load gray ref_data:")
        sub_grays = read_folder(grayboard_subfolder, conf, finish_id=img_num//2, convert_to_gray=True)
        gray_img_packs.append(sub_grays)
        del sub_grays

        logger.info("Load BGR ref_data:")
        sub_grays_bgr = read_folder(grayboard_subfolder, conf, finish_id=img_num//2, convert_to_gray=False)
        wb_sum += sub_grays_bgr[:,68*4:-68*4, 432*4:-432*4].reshape(-1, 3).mean(axis = 0)

        del sub_grays_bgr

    sample_count = len(gray_img_packs)
    wb_sum /= wb_sum.max()

    if sample_count == 0:
        raise("Calib grayboard error: no grayboard data get.")
    elif sample_count == 1:
        out_grays = gray_img_packs[0]
        del gray_img_packs
    else:
        logger.info('Median filter on grayboard data...')
        out_grays = np.stack(gray_img_packs)
        del gray_img_packs
        out_grays = np.median(out_grays, axis=0)

    if blur_size > 0:
        logger.info("Blur grayboard data...")
        out_grays_blur = cv2.GaussianBlur(out_grays.transpose(1,2,0), (blur_size, blur_size), 0)
        del out_grays
        out_grays_blur = out_grays_blur.transpose(2,0,1).astype(np.uint16)

    np.savez(f'./pre_data/grayboard_result_{lens}.npz',sn=None, lens_sn=None, data=out_grays_blur, wb=wb_sum, camera_height=None)
    logger.info(f'Grayboard data saved to pre_data/grayboard_result_{lens}.npz')
