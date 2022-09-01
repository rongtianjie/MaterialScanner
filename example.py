from base.common import *
from process.stereo import process_stereo
from process.mono import process_mono
from func.calib_grayboard import generate_grayboard
import os

VERSION = "0.3.0"

input_path = r"Z:\双目\20220826\布0-1"
output_path = os.path.join(input_path, "out")

if __name__ == "__main__":

    if False:
        grayboard_path = r"D:\images\120灰度板"
        generate_grayboard(grayboard_path, "120", blur_size=21, max_sample_count=5)

    try:
        with open(os.path.join(input_path, "CameraPara.json"), "r") as f:
            cam_para = json.load(f)
            logger.info("[CameraPara.json] loaded")
            lens = cam_para["LensModel"]
            focus_id = cam_para["FocusDisIndex"]
            logger.info(f"Lens: {lens}, Focus_ID: {focus_id}")
    except:
        pass

    try:
        process_stereo(input_path, output_path, lens=lens, scale=2, focus_id=focus_id, cache=True, version=VERSION)
    except Exception as e:
        print("Error: ", e)
        error = {"Error": str(e)}
        add2json(error, os.path.join(output_path, "AlgorithmPara.json"))