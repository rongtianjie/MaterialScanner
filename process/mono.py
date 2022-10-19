# cython:language_level=3
import os
os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'
ext_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "../ext"))
os.environ['PATH'] = os.environ['PATH'] + f"{ext_dir};"

from base.common import *
from base.common_image import *
from func.texture_normal import produce_normal_map
from func.texture_albedo import produce_albedo_map
from func.texture_roughness import produce_roughness_map
from func.texture_sss import produce_sss_map
from base.config_handler import read_conf

@count_time
@logger.catch
def process_mono(input_path: str, output_path: str, lens: str, scale=1, focus_id=0, cache=False, version=None):
    logger.info("++++++++++++++++ Start processing mono ++++++++++++++++")

    sub_folders = os.listdir(input_path)
    if 'mid' not in sub_folders:
        exit_with_error("No mid folder found!")
    else:
        # handle config
        conf = read_conf(input_path, lens, scale, shoot_type="mono", focus_id=focus_id)
        conf["cache"] = cache
        conf["name"] = os.path.basename(input_path)
        conf["input_folder"] = input_path
        conf["output_folder"] = output_path
        os.makedirs(output_path, exist_ok=True)

        logger.info(f"Lens: {lens} | Focus_ID: {focus_id} | Scale: {scale}")
        save_json(version, lens, scale, focus_id, os.path.join(output_path, "AlgorithmPara.json"))

        rot_count = conf["geometry"]['rot_count']
        light_num = len(conf["geometry"]['light_str'])//2 * rot_count

        # check images
        if conf["settings"]["check_images"]:
            check_images(input_path)

        cache_list = []
        
        logger.info("Loading gray images...")
        _, mid_undist_gray_image, cache = read_folder(os.path.join(input_path, "mid"), conf, start_id=0, finish_id=light_num, keep_rgb=True)
        cache_list.append(cache)
        
        # calculate normal
        logger.info("Processing normal...")
        albedo_weight, albedo_weight_shadow, grayboard_image, g_wb = produce_normal_map(mid_undist_gray_image, depth=None, conf=conf)
        
        # calculate albedo
        logger.info("Loading RGB images...")
        mid_undist_rgb_image, _, cache = read_folder(os.path.join(input_path, "mid"), conf, start_id=0, finish_id=light_num, keep_rgb=True)
        cache_list.append(cache)
        roughness_albedo = produce_albedo_map(mid_undist_rgb_image, g_wb, albedo_weight, albedo_weight_shadow, grayboard_image, conf)

        del mid_undist_rgb_image

        # calculate roughness
        logger.info("Loading specular images...")
        _, specular_image, cache = read_folder(os.path.join(input_path, "mid"), conf, start_id=light_num, finish_id=light_num*2, keep_rgb=False)
        cache_list.append(cache)
        
        produce_roughness_map(mid_undist_gray_image, specular_image, roughness_albedo, grayboard_image,  conf)

        del specular_image
        del mid_undist_gray_image

        # calculate sss
        produce_sss_map(conf)

        if not cache:
            delete_cache(cache_list)
            logger.info("Cache deleted.")

        logger.success("All Done!")