# cython:language_level=3
import os
os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'
ext_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), "../ext"))
os.environ['PATH'] = os.environ['PATH'] + f"{ext_dir};"

from threading import Thread

from base.common import *
from base.common_image import *
from func.texture_height import produce_height_map as produce_height_map_120
from func.texture_height_mvs import produce_height_map as produce_height_map_50
from func.texture_normal import produce_normal_map
from func.texture_albedo import produce_albedo_map
from func.texture_roughness import produce_roughness_map
from func.texture_sss import produce_sss_map
from base.config_handler import read_conf
from process.mono import process_mono

@count_time
@logger.catch
def process_stereo(input_path: str, output_path: str, lens: str, scale=1, focus_id=0, cache=False, version=None):
    logger.info("++++++++++++++++ Start processing stereo ++++++++++++++++")

    sub_folders = os.listdir(input_path)
    if 'mid' not in sub_folders:
        exit_with_error("No mid folder found!")
    elif 'right' not in sub_folders and "MVS" not in sub_folders:
        logger.warning("No right folder found! Use mono mode.")
        process_mono(input_path, output_path, lens, scale, focus_id, cache, version)
    else:
        # handle config
        conf = read_conf(input_path, lens, scale, shoot_type="stereo", focus_id=focus_id)
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
        logger.success("Mid gray images loaded.")
        
        if lens == "120":
            try:
                logger.info("Loading side gray images...")
                _, right_undist_gray_image, cache = read_folder(os.path.join(input_path, "right"), conf, start_id=0, finish_id=light_num, keep_rgb=False)
                cache_list.append(cache)
                logger.success("Side gray images loaded.")
            except:
                right_undist_gray_image = None
                logger.warning("right image not found")

            depth, height = produce_height_map_120(mid_undist_gray_image, right_undist_gray_image, conf)
            del right_undist_gray_image
        elif lens == "50":
            depth, height, mat_k = produce_height_map_50(conf)
            conf["camera_matrices"] = {"mid_mat_k": mat_k}
        else:
            exit_with_error("Lens not supported!")
        
        # calculate normal
        logger.info("Processing normal...")
        albedo_weight, albedo_weight_shadow, grayboard_image, g_wb = produce_normal_map(mid_undist_gray_image, depth, conf)
        
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

        # calculate AO
        def solve_AO():
            from func.texture_ao import produce_ao_map
            if height is not None:
                produce_ao_map(height, conf)
                logger.success("AO map generated.")
            else:
                logger.warning("No height map, skip AO map")
        t = Thread(target=solve_AO)
        t.start()
        t.join()

        if not cache:
            delete_cache(cache_list)
            logger.info("Cache deleted.")

        logger.success("All Done!")