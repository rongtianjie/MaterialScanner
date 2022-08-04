import os
os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'
ext_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "../ext"))
os.environ['PATH'] = os.environ['PATH'] + f"{ext_dir};"

from base.common import *
from base.common_image import read_folder
from func.texture_height import produce_height_map
from func.texture_normal import produce_normal_map
from func.texture_albedo import produce_albedo_map
from func.texture_roughness import produce_roughness_map
from func.texture_sss import produce_sss_map
import numpy as np
import json


def read_conf(lens, scale=1):
    if lens == "30":
        conf_path = "conf/fuji_30.json"
        grayboard_data_path = "pre_data/grayboard_result_30.npz"
    if lens == "50":
        conf_path = "conf/fuji_50.json"
        grayboard_data_path = "pre_data/grayboard_result_50.npz"
    elif lens == "120":
        conf_path = "conf/fuji_120.json"
        grayboard_data_path = "pre_data/grayboard_result_120.npz"

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
    conf["grayboard_data_path"] = grayboard_data_path

    # settings
    conf_path = "conf/settings.json"
    with open(conf_path) as f:
        raw = f.read()
    settings_conf = json.loads(raw)
    conf["settings"] = settings_conf

    return conf

@count_time
def process(inupt_path, output_path, lens, shoot_type="stereo", scale=1, cache=False):
    set_log_level("INFO", "DEBUG")

    # handle config
    conf = read_conf(lens, scale)
    conf["cache"] = cache
    conf["name"] = os.path.basename(inupt_path)
    conf["input_folder"] = inupt_path
    conf["output_folder"] = output_path
    os.makedirs(output_path, exist_ok=True)

    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2
    
    # calculate stereo depth
    mid_undist_gray_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=0, finish_id=rot_count*light_num, convert_to_gray=True, cache=cache)
    right_undist_gray_image = read_folder(os.path.join(inupt_path, "right"), conf, start_id=0, finish_id=rot_count*light_num, convert_to_gray=True, cache=cache)

    depth, height = produce_height_map(mid_undist_gray_image, right_undist_gray_image, conf)
    
    del right_undist_gray_image
    
    # calculate normal
    albedo_weight, albedo_weight_shadow, grayboard_image, g_wb = produce_normal_map(mid_undist_gray_image, depth, conf)

    # calculate AO
    from func.texture_ao import produce_ao_map
    produce_ao_map(height, conf)
    del height
    
    # calculate albedo
    mid_undist_bgr_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=0, finish_id=rot_count*light_num, convert_to_gray=False, cache=cache)
    roughness_albedo = produce_albedo_map(mid_undist_bgr_image, g_wb, albedo_weight, albedo_weight_shadow, grayboard_image, conf)

    del mid_undist_bgr_image

    # calculate roughness
    specular_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=rot_count*light_num, finish_id=rot_count*light_num*2, convert_to_gray=True, cache=cache)
    produce_roughness_map(mid_undist_gray_image, specular_image, roughness_albedo, grayboard_image,  conf)

    del specular_image
    del mid_undist_gray_image

    # calculate sss
    produce_sss_map(conf)

    logger.info("All Done!")
