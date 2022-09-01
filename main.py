import argparse
import sys
import os
from base.common import *
from process.stereo import process_stereo
from process.mono import process_mono
import multiprocessing

VERSION = "0.3.0"

def parse(input_path, output_path, lens, shoot_type, scale, focus_id, cache=False):
    if output_path is None:
        output_path = os.path.join(input_path, "out")

    # try:
    #     with open(os.path.join(input_path, "CameraPara.json"), "r") as f:
    #         cam_para = json.load(f)
    #         logger.info("[CameraPara.json] loaded")
    #         lens = int(cam_para["LensModel"])
    #         focus_id = cam_para["FocusDisIndex"]
    #         logger.info(f"Lens: {lens}, Focus_ID: {focus_id}")
    # except:
    #     pass

    try:
        if shoot_type == "stereo":
            process_stereo(input_path, output_path, lens, scale, focus_id, cache, version=VERSION)
        elif shoot_type == "mono":
            process_mono(input_path, output_path, lens, scale, focus_id, cache, version=VERSION)
    except Exception as e:
        print("Error: ", e)
        error = {"Error": str(e)}
        add2json(error, os.path.join(output_path, "AlgorithmPara.json"))
        return -1

    print("end")
    return 0

if __name__ == '__main__':
    multiprocessing.freeze_support()
    # load args
    parser = argparse.ArgumentParser(description='material analysis command line tool for texture processing algorithms.')
    parser.add_argument('-i, --input', type=str, help='input dir', dest='input', required=True)
    parser.add_argument('-o, --output', type=str, help='output dir', dest='output', default=None)
    parser.add_argument('-l, --lens', type=str, help='camera lens',  choices=["30", "50", "120"], dest='lens', default=None)
    parser.add_argument('-t, --type', type=str, help='use mono or stereo setting', choices=['mono', 'stereo'], dest='type', default="stereo")
    parser.add_argument('-c, --cache', help='use cache or not', action="store_true", dest='cache')
    parser.add_argument('-s, --scale', type=int, help='image scale',  choices=[1, 2, 4, 8], dest='scale', default=1)
    parser.add_argument('-f, --focus_id', type=int, help='focus_id position',  choices=[0,1,2,3], dest='focus_id', default=None)

    args = parser.parse_args()

    sys.exit(parse(args.input, args.output, args.lens, args.type, args.scale, args.focus_id, args.cache))
