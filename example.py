from process.stereo import process_stereo
from process.mono import process_mono
from func.calib_grayboard import generate_grayboard




if __name__ == "__main__":
    
    if False:
        grayboard_path = r"Z:\双目\20220803旋转光源测试10鹅卵石\灰度板\data"
        generate_grayboard(grayboard_path, "120", blur_size=21, max_sample_count=5)

    process_mono(
        r"D:\images\20220810tile50\testcase",
        r"D:\images\20220810tile50\out", "50", scale=4, cache=True)
