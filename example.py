from base.common import *
from process.stereo import process_stereo
from process.stereo import process_mono
from func.calib_grayboard import generate_grayboard
import os
from main import parse

input_path = r"C:\Users\ecoplants\Desktop\叶子_6"
output_path = os.path.join(input_path, "test")

if __name__ == "__main__":

    set_log_level("INFO", "DEBUG")

    if False:
        grayboard_path = r"\\EcoPlants-AI\material_scan\双目\20220906拍摄50灰度板"
        generate_grayboard(grayboard_path, "50", blur_size=21, max_sample_count=5)

    lens, focus_id = "50", 0
    try:
        with open(os.path.join(input_path, "CameraPara.json"), "r") as f:
            cam_para = json.load(f)
            logger.info("[CameraPara.json] loaded")
            lens = cam_para["LensModel"]
            focus_id = cam_para["FocusDisIndex"]
            logger.info(f"Lens: {lens}, Focus_ID: {focus_id}")
    except:
        pass

    parse(input_path, output_path, lens, "stereo", 1, focus_id, cache=True)