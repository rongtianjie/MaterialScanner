# cython:language_level=3
from base.common import *
import torch
import cv2


@count_time
def produce_roughness_map(mid_undist_image, specular_image, roughness_albedo, grayboard_image, conf):
    # read conf
    out_folder = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]
    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2 * rot_count

    grayboard_image = grayboard_image[:light_num]

    roughness = solve_roughness(
        mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale], 
        specular_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale])
    roughness =  (roughness * 65535).astype(np.uint16)
    cv2.imencode('.png', roughness)[1].tofile(f'{out_folder}/T_{name}_Roughness_0_{8//scale}K.png')

    roughness = solve_roughness(
        mid_undist_image[:, 68*4//scale:-68*4//scale,432*4//scale:-432*4//scale],
        specular_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale], 
        best_k = 2)
    roughness = (roughness * 65535).astype(np.uint16)
    cv2.imencode('.png', roughness)[1].tofile(f'{out_folder}/T_{name}_Roughness_2_{8//scale}K.png')

    roughness = solve_roughness(
        mid_undist_image[:, 68*4//scale:-68*4//scale,432*4//scale:-432*4//scale] / roughness_albedo,
        specular_image[:, 68*4//scale:-68*4//scale,432*4//scale:-432*4//scale] / roughness_albedo)
    roughness = (roughness * 65535).astype(np.uint16)
    cv2.imencode('.png', roughness)[1].tofile(f'{out_folder}/T_{name}_Roughness_ABD_{8//scale}K.png')
    
    # roughness = solve_roughness(
    #     (mid_undist_image / grayboard_image)[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale],
    #     (specular_image / grayboard_image)[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale])
    # roughness = (roughness * 65535).astype(np.uint16)
    # cv2.imencode('.png', roughness)[1].tofile(f'{out_folder}/T_{name}_Roughness_REF_{8//scale}K.png')

    logger.success(f"Roughness map saved.")

def solve_roughness(diffuse_img, specular_img, best_k = -1):
    roughness = (torch.FloatTensor(specular_img/65535) - torch.FloatTensor(diffuse_img/65535)).abs_()
    if best_k > 0:
        roughness = torch.topk(roughness, best_k, largest=False, dim=0)[0]
    roughness = torch.mean(roughness, 0)
    roughness = roughness / torch.max(roughness)
    roughness[roughness > 1] = 1
    roughness = 1 - roughness
    return roughness.numpy()