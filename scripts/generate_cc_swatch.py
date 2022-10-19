import sys, os
sys.path.append(os.getcwd())
import colour
from func.addon_colour_correction import getColorCorrectionSwatches
import numpy as np
import cv2
from tqdm import trange

def generate_swatch(img):
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    if img.dtype == np.uint8:
        img = img.astype(np.float32) / 255
    elif img.dtype == np.uint16:
        img = img.astype(np.float32) / 65535
    else:
        raise ValueError("Unsupported image dtype")

    img_lrgb = colour.cctf_decoding(img)

    ratio = 1
    with trange(50) as t:
        for _ in t:
            ratio = round(ratio * 0.95, 3)
            t.set_description(f'Image resize ratio [{ratio}]')
            img_resize = cv2.resize(img_lrgb, (0, 0), fx = ratio, fy = ratio)
            swatch = getColorCorrectionSwatches(img_resize, verbose = False)
            if swatch is not None:
                print("Found swatch with ratio: ", ratio)
                np.save(os.path.join(os.getcwd(), "pre_data/swatch.npy"), np.array(swatch))
                print(np.array(swatch))
                return True
    
    return False

if __name__ == "__main__":

    try:
        albedo_path = sys.argv[1]
    except:
        albedo_path = r"\\EcoPlants-AI\material_scan\双目\20220908色卡\out\T_20220908色卡_Albedo_8K.png"
    albedo = cv2.imdecode(np.fromfile(albedo_path,dtype=np.uint8),-1)
    if generate_swatch(albedo):
        print("Swatch generated.")
    else:
        print("Swatch not found.")
    
