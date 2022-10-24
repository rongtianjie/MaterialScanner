# cython:language_level=3
from base.common import *
from base.common_image import rgb_to_srgb
import torch
import cv2
from func.addon_colour_correction import albedo_colour_correction


@count_time
def produce_albedo_map(mid_undist_image, g_wb, albedo_weight, albedo_weight_shadow, grayboard_image, conf):
    # read conf
    output_dir = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]
    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2 * rot_count
    gray_scale = conf["grayboard"]["gray_scale"]
    # batch_count = conf["albedo_map"]["albedo_batch_count"]

    grayboard_image = grayboard_image[:light_num]

    b_weight = g_wb[0]/g_wb[0]*0.114+g_wb[1]/g_wb[0]*0.587+g_wb[2]/g_wb[0]*0.299
    g_weight = g_wb[0]/g_wb[1]*0.114+g_wb[1]/g_wb[1]*0.587+g_wb[2]/g_wb[1]*0.299
    r_weight = g_wb[0]/g_wb[2]*0.114+g_wb[1]/g_wb[2]*0.587+g_wb[2]/g_wb[2]*0.299

    # solve albedo without shadow
    logger.info("Solving albedo without shadow...")
    albedo = solve_albedo(mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale], albedo_weight, 6, rot_count, light_num)
    del albedo_weight
    
    albedo[...,0] *= b_weight
    albedo[...,1] *= g_weight
    albedo[...,2] *= r_weight
    albedo = rgb_to_srgb(albedo, 1)
    albedo = (albedo * 65535).astype(np.uint16)
    roughness_albedo = np.expand_dims(cv2.cvtColor(albedo, cv2.COLOR_RGB2GRAY), axis = 0)
    cv2.imencode('.png', albedo)[1].tofile(
        f'{output_dir}/T_{name}_Albedo_{8//scale}K.png')
    logger.success("Solved albedo without shadow.")

    # colour correction
    logger.info("Correcting albedo colour...")
    albedo_corrected = albedo_colour_correction(albedo)
    cv2.imencode('.png', albedo_corrected)[1].tofile(
        f'{output_dir}/T_{name}_Albedo_Corrected_{8//scale}K.png')
    logger.success("Albedo colour corrected.")
    
    # solve albedo with shadow
    # logger.info("Solving albedo with shadow...")
    # albedo = solve_albedo(mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale], albedo_weight_shadow, 6, rot_count, light_num)
    # del albedo_weight_shadow

    # albedo[...,0] *= b_weight
    # albedo[...,1] *= g_weight
    # albedo[...,2] *= r_weight
    # albedo = rgb_to_srgb(albedo, 1)
    # albedo = (albedo * 65535).astype(np.uint16)
    # cv2.imencode('.png', albedo)[1].tofile(
    #     f'{output_dir}/T_{name}_Albedo_Shadow_{8//scale}K.png')
    
    # solve old version albedo
    # logger.info("Solving old version albedo...")
    # albedo = mid_undist_image.mean(axis = 0)/np.expand_dims(grayboard_image, axis = -1).mean(axis = 0)
    # albedo = albedo[68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale]
    # albedo[...,0] *= b_weight
    # albedo[...,1] *= g_weight
    # albedo[...,2] *= r_weight
    # albedo *= gray_scale
    # albedo = rgb_to_srgb(albedo, 1)
    # albedo = (albedo * 65535).astype(np.uint16)
    # roughness_albedo = np.expand_dims(cv2.cvtColor(albedo, cv2.COLOR_RGB2GRAY), axis = 0)
    # cv2.imencode('.png', albedo)[1].tofile(
    #     f'{output_dir}/T_{name}_Albedo_Legacy_0_{8//scale}K.png')

    # albedo = mid_undist_image/np.expand_dims(grayboard_image, axis = -1)
    # albedo = albedo.mean(axis = 0)
    # albedo = albedo[68*4//scale:-68*4//scale,
    #                                     432*4//scale:-432*4//scale]
    # albedo[...,0] *= b_weight
    # albedo[...,1] *= g_weight
    # albedo[...,2] *= r_weight
    # albedo *= gray_scale
    # albedo = rgb_to_srgb(albedo, 1)
    # albedo = (albedo * 65535).astype(np.uint16)
    # cv2.imencode('.png', albedo)[1].tofile(
    #     f'{output_dir}/T_{name}_Albedo_Legacy_1_{8//scale}K.png')
    
    return roughness_albedo

def solve_albedo(albedo_img, albedo_weight, specular_remove_count, rot_count, light_num, batch_count=8):
    batch_size = albedo_img.shape[1]//batch_count
    albedo_list = []
    with trange(batch_count) as t:
        for i in t:
            t.set_description(f'Solving albedo batch [{i}]')
            albedo_img_batch = (albedo_img[:, i*batch_size:(i+1)*batch_size, ...]/65535).astype(np.float32)
            albedo_weight_batch = np.expand_dims(albedo_weight[:, i*batch_size:(i+1)*batch_size, ...], axis=-1).astype(np.float32)
            albedo_batch = np.maximum(albedo_img_batch*albedo_weight_batch, 0)
            albedo_batch = albedo_batch.transpose(1, 2, 3, 0).reshape(-1, light_num)
            for _ in range(specular_remove_count):
                rank = albedo_batch.argmax(axis = 1, keepdims = True)
                idx = (rank + np.arange(rank.shape[0]).reshape(-1, 1)*light_num).reshape(-1)
                albedo_batch = albedo_batch.reshape(-1)
                albedo_batch[idx] = 0
                albedo_batch = albedo_batch.reshape(-1, light_num)
            albedo_batch = albedo_batch.reshape(albedo_weight_batch.shape[1], albedo_weight_batch.shape[2], 3, -1).transpose(3, 0, 1, 2)
            albedo_batch = (albedo_batch).sum(axis = 0) / (np.maximum((albedo_batch>0).sum(axis = 0), 1))
            albedo_list.append(albedo_batch)
    albedo = np.concatenate(albedo_list, axis = 0)
    return albedo