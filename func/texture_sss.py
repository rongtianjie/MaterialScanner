from base.common import *
from base.common_image import read_folder, rgb_to_srgb
import numpy as np
import cv2
import os


@count_time
def produce_sss_map(conf):
    # read conf
    cache = conf["cache"]
    in_folder = conf["input_folder"]
    out_folder = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]
    gain = conf["sss_map"]["gain"]
    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2 * rot_count

    # check img count
    img_list = []
    for filename in os.listdir(os.path.join(in_folder, "mid")):
        if filename.lower().endswith(".raf"):
            img_list.append(filename)
    
    if len(img_list) > light_num*2:
        sss_image = read_folder(os.path.join(in_folder, "mid"), conf, start_id=light_num*2, finish_id=light_num*2+1, convert_to_gray=False, cache=cache)
        sss_image = sss_image[0]
    else:
        logger.warning("No sss image found.")
        return

    # solve sss
    sss = sss_image[68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale].astype(np.float32) / 65535
    sss = rgb_to_srgb(sss*gain, 1)
    sss = (sss * 65535).astype(np.uint16)
    cv2.imencode('.png', sss)[1].tofile(f'{out_folder}/T_{name}_SSS_{8//scale}K.png')