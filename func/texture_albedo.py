from base.common import *
from base.common_image import rgb_to_srgb
import torch
import cv2
import numpy as np


@count_time
def produce_albedo_map(mid_undist_image, g_wb, albedo_weight, albedo_weight_shadow, grayboard_image, conf):
    # read conf
    output_dir = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]
    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2
    gray_scale = conf["grayboard"]["gray_scale"]

    b_weight = g_wb[0]/g_wb[0]*0.114+g_wb[1]/g_wb[0]*0.587+g_wb[2]/g_wb[0]*0.299
    g_weight = g_wb[0]/g_wb[1]*0.114+g_wb[1]/g_wb[1]*0.587+g_wb[2]/g_wb[1]*0.299
    r_weight = g_wb[0]/g_wb[2]*0.114+g_wb[1]/g_wb[2]*0.587+g_wb[2]/g_wb[2]*0.299

    # solve albedo without shadow
    albedo = solve_albedo(mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale], albedo_weight, 6, rot_count, light_num)
    albedo[...,0] *= b_weight
    albedo[...,1] *= g_weight
    albedo[...,2] *= r_weight
    albedo = rgb_to_srgb(albedo, 1)
    albedo = (albedo * 65535).astype(np.uint16)
    cv2.imencode('.png', albedo)[1].tofile(
        f'{output_dir}/T_{name}_Albedo_{8//scale}K.png')
    
    # solve albedo with shadow
    albedo = solve_albedo(mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale], albedo_weight_shadow, 6, rot_count, light_num)
    albedo[...,0] *= b_weight
    albedo[...,1] *= g_weight
    albedo[...,2] *= r_weight
    albedo = rgb_to_srgb(albedo, 1)
    albedo = (albedo * 65535).astype(np.uint16)
    cv2.imencode('.png', albedo)[1].tofile(
        f'{output_dir}/T_{name}_Albedo_Shadow_{8//scale}K.png')
    
    # solve old version albedo
    albedo = mid_undist_image.mean(axis = 0)/np.expand_dims(grayboard_image, axis = -1).mean(axis = 0)
    albedo = albedo[68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale]
    albedo[...,0] *= b_weight
    albedo[...,1] *= g_weight
    albedo[...,2] *= r_weight
    albedo *= gray_scale
    albedo = rgb_to_srgb(albedo, 1)
    albedo = (albedo * 65535).astype(np.uint16)
    roughness_albedo = np.expand_dims(cv2.cvtColor(albedo, cv2.COLOR_BGR2GRAY), axis = 0)
    cv2.imencode('.png', albedo)[1].tofile(
        f'{output_dir}/T_{name}_Albedo_Legacy_0_{8//scale}K.png')

    albedo = mid_undist_image/np.expand_dims(grayboard_image, axis = -1)
    albedo = albedo.mean(axis = 0)
    albedo = albedo[68*4//scale:-68*4//scale,
                                        432*4//scale:-432*4//scale]
    albedo[...,0] *= b_weight
    albedo[...,1] *= g_weight
    albedo[...,2] *= r_weight
    albedo *= gray_scale
    albedo = rgb_to_srgb(albedo, 1)
    albedo = (albedo * 65535).astype(np.uint16)
    cv2.imencode('.png', albedo)[1].tofile(
        f'{output_dir}/T_{name}_Albedo_Legacy_1_{8//scale}K.png')
    
    return roughness_albedo

@torch.no_grad()
def solve_albedo(albedo_img, albedo_weight, specular_remove_count, rot_count, light_num):
    tsr_albedo_img = torch.FloatTensor(albedo_img/65535)
    tsr_albedo_weight = torch.FloatTensor(albedo_weight).unsqueeze_(-1)
    tsr_albedo = (tsr_albedo_img*tsr_albedo_weight).maximum(torch.FloatTensor([0]))
    tsr_albedo = tsr_albedo.permute(1, 2, 3, 0).view(-1, rot_count*light_num).contiguous()
    for _ in range(specular_remove_count):
        rank = tsr_albedo.max(dim = 1, keepdim=True)[1]
        idx = (rank + torch.arange(rank.shape[0]).view(-1, 1)*rot_count*light_num).reshape(-1)
        tsr_albedo = tsr_albedo.view(-1)
        tsr_albedo[idx] = 0
        tsr_albedo = tsr_albedo.view(-1, rot_count*light_num)
    tsr_albedo = tsr_albedo.reshape(albedo_weight.shape[1], albedo_weight.shape[2], 3, -1).permute(3, 0, 1, 2)
    tsr_albedo = (tsr_albedo).sum(dim = 0) / ((tsr_albedo>0).sum(dim = 0).maximum(torch.FloatTensor([1])))
    return tsr_albedo.numpy()