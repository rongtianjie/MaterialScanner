import argparse
import sys
from process.stereo import process_stereo
from process.mono import process_mono
import multiprocessing


def parse(inupt_path, output_path, lens, shoot_type="stereo", scale=1, cache=False):
    try:
        if shoot_type == "stereo":
            process_stereo(inupt_path, output_path, lens, scale, cache)
        elif shoot_type == "mono":
            process_mono(inupt_path, output_path, lens, scale, cache)
    except Exception as e:
        print("Error: ", e)
        return -1

    print("end")
    return 0

if __name__ == '__main__':
    multiprocessing.freeze_support()
    # load args
    parser = argparse.ArgumentParser(description='material analysis command line tool for texture processing algorithms.')
    parser.add_argument('-i, --input', type=str, help='input dir', dest='input', required=True)
    parser.add_argument('-o, --output', type=str, help='output dir', dest='output', required=True)
    parser.add_argument('-l, --lens', type=str, help='camera lens',  choices=["30", "50", "120"], dest='lens', required=True)
    parser.add_argument('-t, --type', type=str, help='use mono or stereo setting', choices=['mono', 'stereo'], dest='type', default="stereo")
    parser.add_argument('-c, --cache', help='use cache or not', action="store_true", dest='cache')
    parser.add_argument('-s, --scale', type=int, help='image scale',  choices=[1, 2, 4, 8], dest='scale', default=1)

    args = parser.parse_args()

    sys.exit(parse(args.input, args.output, args.lens, args.type, args.scale, args.cache))
