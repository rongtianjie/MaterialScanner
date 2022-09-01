import os
os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'
ext_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "../ext"))
os.environ['PATH'] = os.environ['PATH'] + f"{ext_dir};"

from base.common import *
from base.common_image import read_folder
from func.texture_normal import produce_normal_map
from func.texture_albedo import produce_albedo_map
from func.texture_roughness import produce_roughness_map
from func.texture_sss import produce_sss_map
from base.config_handler import read_conf

@count_time
def process_mono(inupt_path, output_path, lens, scale=1, focus_id=0, cache=False, version=None):
    set_log_level("INFO", "DEBUG")

    sub_folders = os.listdir(inupt_path)
    if 'mid' not in sub_folders:
        exit_with_error("No mid folder found!")
    else:
        # handle config
        conf = read_conf(lens, scale, shoot_type="mono", focus_id=focus_id)
        conf["cache"] = cache
        conf["name"] = os.path.basename(inupt_path)
        conf["input_folder"] = inupt_path
        conf["output_folder"] = output_path
        os.makedirs(output_path, exist_ok=True)

        save_json(version, lens, scale, focus_id, os.path.join(output_path, "AlgorithmPara.json"))

        rot_count = conf["geometry"]['rot_count']
        light_num = len(conf["geometry"]['light_str'])//2 * rot_count
        
        logger.info("Loading gray images...")
        mid_undist_gray_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=0, finish_id=light_num, convert_to_gray=True, cache=cache)
        mid_undist_gray_image = mid_undist_gray_image[:48]
        
        # calculate normal
        logger.info("Processing normal...")
        albedo_weight, albedo_weight_shadow, grayboard_image, g_wb = produce_normal_map(mid_undist_gray_image, depth=None, conf=conf)
        
        # calculate albedo
        logger.info("Loading BGR images...")
        mid_undist_bgr_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=0, finish_id=light_num, convert_to_gray=False, cache=cache)
        mid_undist_bgr_image = mid_undist_bgr_image[:48]
        roughness_albedo = produce_albedo_map(mid_undist_bgr_image, g_wb, albedo_weight, albedo_weight_shadow, grayboard_image, conf)

        del mid_undist_bgr_image

        # calculate roughness
        logger.info("Loading specular images...")
        specular_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=light_num, finish_id=light_num*2, convert_to_gray=True, cache=cache)
        specular_image = specular_image[:48]
        
        produce_roughness_map(mid_undist_gray_image, specular_image, roughness_albedo, grayboard_image,  conf)

        del specular_image
        del mid_undist_gray_image

        # calculate sss
        produce_sss_map(conf)

        logger.info("All Done!")