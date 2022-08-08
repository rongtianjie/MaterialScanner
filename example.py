from process import stereo
from func.calib_grayboard import generate_grayboard




if __name__ == "__main__":
    
    if False:
        grayboard_path = r"Z:\双目\20220803旋转光源测试10鹅卵石\灰度板\data"
        generate_grayboard(grayboard_path, "120", blur_size=21, max_sample_count=5)

    stereo.process(
        r"D:\images\20220728deshadow\data\testcase",
        r"D:\images\20220728deshadow\out", "120", scale=4, cache=True)
