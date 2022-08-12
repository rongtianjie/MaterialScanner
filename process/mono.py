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
from conf.config_handler import read_conf

@count_time
def process_mono(inupt_path, output_path, lens, scale=1, cache=False):
    set_log_level("INFO", "DEBUG")

    # handle config
    conf = read_conf(lens, scale, shoot_type="mono")
    conf["cache"] = cache
    conf["name"] = os.path.basename(inupt_path)
    conf["input_folder"] = inupt_path
    conf["output_folder"] = output_path
    os.makedirs(output_path, exist_ok=True)

    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2
    
    # calculate stereo depth
    mid_undist_gray_image = read_folder(os.path.join(inupt_path, "mid"), conf, start_id=0, finish_id=rot_count*light_num, convert_to_gray=True, cache=cache)
    
    # calculate normal
    logger.info("Processing normal...")
    albedo_weight, albedo_weight_shadow, grayboard_image, g_wb = produce_normal_map(mid_undist_gray_image, depth=None, conf=conf)
    
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