from base.common import *
import numpy as np
import cv2
from tqdm import trange
import torch
import torch.nn.functional as F
from math import cos, pi
import kornia
from kmeans_pytorch import kmeans
import os

@count_time
def produce_normal_map(mid_undist_image, depth, conf):
    # load conf
    output_dir = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]

    camera_height = conf["geometry"]["camera_height"]
    grayboard_height = conf["geometry"]["grayboard_height"]
    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2
    mid_mat_k = conf["camera_matrices"]["mid_mat_k"]
    mid_mat_dist = conf["camera_matrices"]["mid_mat_d"]
    gray_scale = conf["grayboard"]["gray_scale"]
    kernel_size = conf["normal_map"]["filter_original_size"]
    grayboard_data_path = conf["grayboard_data_path"]


    # test calculate camera height
    if False:
        from func.calib_marker import get_camera_height
        marker_image = mid_undist_image.mean(axis=0).astype(np.uint16)
        cal_camera_height = get_camera_height(marker_image, mid_mat_k, mid_mat_dist, conf)

    # load gray board data
    if not os.path.exists(grayboard_data_path):
        exit_with_error(f"{grayboard_data_path} not exist")

    grayboard_data = np.load(grayboard_data_path, allow_pickle=True)

    if True or grayboard_data['camera_height'] is None:
        grayboard_depth = camera_height - grayboard_height
        logger.info(f'No grayboard_depth loaded, use calculated grayboard_depth={grayboard_depth} instead.')
    else: 
        grayboard_depth = grayboard_data['camera_height'].tolist()

    # calculate light field
    plain_depth = np.ones([8192//scale, 8192//scale]) * camera_height
    light_coord = get_light_coord(conf["geometry"])[:rot_count*light_num]
    pixel_coord = get_pixel_coord(mid_mat_k, camera_height, plain_depth, scale)

    grayboard_image = grayboard_data['data']
    g_wb = grayboard_data['wb']
    if scale != 1:
        grayboard_image = cv2.resize(grayboard_image.transpose(1, 2, 0),
                    (grayboard_image.shape[2]//scale, grayboard_image.shape[1]//scale)).transpose(2, 0, 1)
    # grayboard_image = np.array([cv2.undistort(grayboard_image[_], mid_mat_k, mid_mat_dist, mid_mat_k) for _ in trange(rot_count*light_num)])

    lightfield_image = get_lightfield_from_grayboard(
                grayboard_image, gray_scale, light_coord, pixel_coord,
                mid_mat_k, camera_height, grayboard_depth, scale)

    if False:
        for i in range(rot_count*light_num):
            cv2.imencode('.png', (lightfield_image[i]*65535).astype(np.uint16))[1].tofile(
                f'{output_dir}/reference/lightfield_{i}.png')
    
    mid_texture_image = mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale]

    # # use weight
    # print(mid_texture_image.shape)
    # print(lightfield_image.shape)
    # b = mid_texture_image/65535/lightfield_image
    # print(b.shape)
    # b = b.transpose(1,2,0)
    # light_coord = light_coord.reshape(-1, 1, 3)
    # pixel_coord = np.expand_dims(pixel_coord.reshape(-1, 3), axis = 0).astype(np.float32)
    # light_dir = light_coord - pixel_coord
    # a = F.normalize(torch.FloatTensor(light_dir).transpose(0, 1), dim = -1).numpy()
    # np.savez(f'C:/Users/ecoplants/Desktop/data.npz', a = a, b = b)
    # logger.info('save data')
    
    # slove normal without shadow
    normal, albedo_weight = solve_normal(mid_texture_image, lightfield_image, light_coord, pixel_coord, 0, rot_count, rot_count, light_num)

    original_normal = normal / 2 + 0.5
    original_normal = original_normal * 65535
    original_normal = original_normal[..., ::-1]
    original_normal = original_normal.astype(np.uint16)
    cv2.imencode('.png', original_normal)[1].tofile(f'{output_dir}/T_{name}_Normal_Original_{8//scale}K.png')
    logger.info('Original normal solved.')


    os.makedirs(f'{output_dir}/reference', exist_ok=True)

    # correct normal use solved depth
    if depth is not None:
        normal_corrected, normal_shift, loss_u, loss_v = correct_normal(normal, depth, mid_mat_k, scale, kernel_size)
        normal_corrected = normal_corrected / 2 + 0.5
        normal_corrected = normal_corrected * 65535
        normal_corrected = normal_corrected[..., ::-1]
        normal_corrected = normal_corrected.astype(np.uint16)

        cv2.imencode('.png', normal_corrected)[1].tofile(f'{output_dir}/T_{name}_Normal_{8//scale}K.png')
        logger.info('Corrected normal solved.')

        normal_shift = normal_shift / 2 + 0.5
        normal_shift = normal_shift * 65535
        normal_shift = normal_shift[..., ::-1]
        normal_shift = normal_shift.astype(np.uint16)
        cv2.imencode('.png', normal_shift)[1].tofile(
            f'{output_dir}/reference/Normal_Shift_{8//scale}K.png')
        cv2.imencode('.png', (loss_u/loss_u.max()*65535).astype(np.uint16))[1].tofile(
            f'{output_dir}/reference/Normal_Shift_Loss_U_{8//scale}K.png')
        cv2.imencode('.png', (loss_v/loss_v.max()*65535).astype(np.uint16))[1].tofile(
            f'{output_dir}/reference/Normal_Shift_Loss_V_{8//scale}K.png')

    # correct normal use plane depth
    plain_normal, normal_shift, loss_u, loss_v = correct_normal(normal, plain_depth, mid_mat_k, scale, kernel_size)
    plain_normal = plain_normal / 2 + 0.5
    plain_normal = plain_normal * 65535
    plain_normal = plain_normal[..., ::-1]
    plain_normal = plain_normal.astype(np.uint16)

    cv2.imencode('.png', plain_normal)[1].tofile(f'{output_dir}/T_{name}_Normal_Plain_{8//scale}K.png')
    logger.info('Plain normal solved.')

    normal_shift = normal_shift / 2 + 0.5
    normal_shift = normal_shift * 65535
    normal_shift = normal_shift[..., ::-1]
    normal_shift = normal_shift.astype(np.uint16)

    cv2.imencode('.png', normal_shift)[1].tofile(
        f'{output_dir}/reference/Normal_Plain_Shift_{8//scale}K.png')
    cv2.imencode('.png', (loss_u/loss_u.max()*65535).astype(np.uint16))[1].tofile(
        f'{output_dir}/reference/Normal_Plain_Shift_Loss_U_{8//scale}K.png')
    cv2.imencode('.png', (loss_v/loss_v.max()*65535).astype(np.uint16))[1].tofile(
        f'{output_dir}/reference/Normal_Plain_Shift_Loss_V_{8//scale}K.png')

    # normal with shadow
    normal, albedo_weight_shadow = solve_normal(mid_texture_image, lightfield_image, light_coord, pixel_coord, 0, rot_count*light_num, rot_count, light_num)

    # np.save(f'C:/Users/ecoplants/Desktop/normal.npy', normal)
    # print(1)

    original_normal = normal / 2 + 0.5
    original_normal = original_normal * 65535
    original_normal = original_normal[..., ::-1]
    original_normal = original_normal.astype(np.uint16)
    cv2.imencode('.png', original_normal)[1].tofile(f'{output_dir}/T_{name}_Normal_Shadow_Original_{8//scale}K.png')

    logger.info('Shadow normal solved.')

    #########################
    normal1, shadow1, mask = solve_normal_deshadow(mid_texture_image, lightfield_image, normal, light_coord, pixel_coord)
    normal1 = normal1 / 2 + 0.5
    normal1 = normal1 * 65535
    normal1 = normal1[..., ::-1]
    normal1 = normal1.astype(np.uint16)
    cv2.imencode('.png', normal1)[1].tofile(
        f'{output_dir}/T_{name}_Normal_New_{8//scale}K.png')
    cv2.imencode('.png', shadow1)[1].tofile(f'{output_dir}/shadow_map_new_{8//scale}K.png')
    cv2.imencode('.png', mask)[1].tofile(f'{output_dir}/shadow_mask_{8//scale}K.png')

    return albedo_weight, albedo_weight_shadow, grayboard_image, g_wb



def get_light_coord(conf):
    light_str = conf['light_str']
    rot_count = conf['rot_count']
    rot_clockwise = conf['rot_clockwise']

    start_pos = [list(map(float, light_str.split(','))) for light_str in light_str]
    light_coord = []
    if rot_clockwise:
        angle = -np.pi/rot_count*2
    else:
        angle = np.pi/rot_count*2
    rot = np.array([[np.cos(angle), -np.sin(angle),0],[np.sin(angle), np.cos(angle),0],[0,0,1]])
    for pos in start_pos:
        for _ in range(rot_count):
            light_coord.append(pos)
            pos = rot@pos
    return np.array(light_coord)


def get_pixel_coord(mat_k, camera_height, depth, scale):
    grid_y, grid_x = np.mgrid[0:depth.shape[0], 0:depth.shape[1]]
    grid_x += (11648 - 8192) // 2 //scale
    grid_y += (8736 - 8192) // 2 //scale
    coord_x = (grid_x - mat_k[0,2]) * depth / mat_k[0,0]
    coord_y = (grid_y - mat_k[1,2]) * depth / mat_k[1,1] * -1
    return np.stack((coord_x, coord_y, camera_height - depth)).transpose(1,2,0)


def get_lightfield_from_grayboard(grayboard_image, gray_scale, light_coord, pixel_coord, mat_k, camera_height, grayboard_depth, scale):
    z_coord = (camera_height - grayboard_depth)
    # pixel_coord = np.expand_dims(pixel_coord, axis=0).astype(np.float32)

    lightfields = []
    with trange(54) as t:
        t.set_description('Calculate lightfield')
        for i in t:
            pixel_coord = pixel_coord.astype(np.float32)
            line_diff = light_coord[i].reshape(1, 1, 3).astype(np.float32) - pixel_coord
            z_diff = z_coord - pixel_coord[..., 2:]
            grayboard_plane = z_diff * line_diff[..., :2] / line_diff[..., 2:] + pixel_coord[..., :2]

            idx_x = grayboard_plane[..., 1] / (-grayboard_depth / float(mat_k[1, 1])) + float(mat_k[1, 2])
            idx_y = grayboard_plane[..., 0] / (grayboard_depth / float(mat_k[0, 0])) + float(mat_k[0, 2])
            sp_x = (grayboard_image.shape[1] - idx_x.shape[0])//2
            sp_y = (grayboard_image.shape[2] - idx_y.shape[1])//2
            fp_x = (grayboard_image.shape[1] + idx_x.shape[0])//2
            fp_y = (grayboard_image.shape[2] + idx_y.shape[1])//2
            idx_x = np.maximum(sp_x, np.minimum(fp_x, idx_x)).astype(np.uint32)
            idx_y = np.maximum(sp_y, np.minimum(fp_y, idx_y)).astype(np.uint32)
            idx_image = grayboard_image.shape[2]*idx_x + idx_y
            # idx_image += (np.arange(grayboard_image.shape[0])*grayboard_image.shape[1]*grayboard_image.shape[2]).reshape(-1, 1, 1).astype(np.uint32)

            lightfield = grayboard_image[i].reshape(-1)[idx_image].astype(np.float32)/65535/gray_scale

            grayboard_coord = np.concatenate((
                grayboard_plane, np.ones_like(grayboard_plane[..., 1:]) * z_coord), axis=-1)
            dis_square_pixel = (line_diff**2).sum(axis=-1)
            dis_square_grayboard = ((light_coord[i].reshape(1, 1, 3).astype(np.float32) - grayboard_coord)**2).sum(axis=-1)
            lightfield *= dis_square_grayboard / dis_square_pixel

            light_cos = line_diff[...,2]/np.sqrt((line_diff**2).sum(axis = -1))
            lightfield/=light_cos
            lightfields.append(lightfield)

    return np.array(lightfields)


@torch.no_grad()
def solve_normal(mid_img, gb_img, light_coord, pixel_coord, rank_start, rank_finish, rot_count, light_num):
    img_width = mid_img.shape[1]
    batch_count = 16
    batch_size = (img_width*img_width) // batch_count
    normal_list = []
    albedo_weight_list = []
    svd_count = rank_finish - rank_start

    light_coord = light_coord.reshape(-1, 1, 3)
    pixel_coord = np.expand_dims(pixel_coord.reshape(-1, 3), axis = 0).astype(np.float32)
    mid_img = mid_img.reshape(rot_count*light_num, -1)
    gb_img = gb_img.reshape(rot_count*light_num, -1)

    with trange(batch_count) as t:
        for i in t:
            t.set_description(f'Solving batch [{i}]')
            light_dir_batch = light_coord - pixel_coord[:, i*batch_size:(i+1)*batch_size, :]
            tsr_a_batch = F.normalize(torch.FloatTensor(light_dir_batch).transpose(0, 1), dim = -1)
            tsr_b_batch = torch.FloatTensor(mid_img[:, i*batch_size:(i+1)*batch_size]/65535/gb_img[:, i*batch_size:(i+1)*batch_size]).view(rot_count*light_num, -1).transpose(0, 1).unsqueeze(-1)
            weight = torch.squeeze(tsr_b_batch)
            rank = weight.sort(dim=1, descending = True)[1][:, rank_start:rank_finish]
            idx = (rank + torch.arange(batch_size).view(-1, 1)*rot_count*light_num).reshape(-1)

            tsr_svd_a = tsr_a_batch.reshape(-1, 3)[idx].view(-1, svd_count, 3)
            tsr_svd_b = tsr_b_batch.reshape(-1)[idx].view(-1, svd_count, 1)

            normal = torch.linalg.lstsq(tsr_svd_a, tsr_svd_b)[0]
            normal = F.normalize(normal.squeeze_(), dim=-1)
            normal_list.append(normal)

            tsr_lightfield_batch = torch.FloatTensor(gb_img[:, i*batch_size:(i+1)*batch_size]).transpose(0, 1).unsqueeze(-1)
            all_weight = 1/(tsr_a_batch.bmm(normal.unsqueeze(-1))*tsr_lightfield_batch).reshape(-1)
            albedo_weight = torch.zeros_like(all_weight)
            albedo_weight[idx] = all_weight[idx]
            albedo_weight_list.append(albedo_weight.view(-1, rot_count*light_num))

    tsr_normal = torch.stack(normal_list)
    tsr_normal = tsr_normal.view(img_width,-1,3)
    tsr_albedo_weight = torch.stack(albedo_weight_list)
    tsr_albedo_weight = tsr_albedo_weight.view(img_width,-1, rot_count*light_num).permute(2,0,1)
    return tsr_normal.numpy(), tsr_albedo_weight.numpy()

def solve_normal_deshadow(mid_img, gb_img, shadow_normal, light_coord, pixel_coord):
    img_width = shadow_normal.shape[0]
    light_num = 54
    batch_count = 16
    batch_size = (img_width*img_width) // batch_count
    normal_list = []
    shadow_list = []
    shadow_mask_list = []
    tsr_w = []
    light_coord = light_coord.reshape(-1, 1, 3)
    pixel_coord = np.expand_dims(pixel_coord.reshape(-1, 3), axis = 0).astype(np.float32)
    shadow_normal = shadow_normal.reshape(-1, 1, 3)
    mid_img = mid_img.reshape(light_num, -1)
    gb_img = gb_img.reshape(light_num, -1)

    with trange(batch_count) as t:
        for i in t:
            t.set_description(f'Solving batch [{i}]')
            light_dir_batch = (light_coord - pixel_coord[:, i*batch_size:(i+1)*batch_size, :])
            tsr_a_batch = F.normalize(torch.FloatTensor(light_dir_batch).transpose(0, 1), dim = -1)
            tsr_b_batch = torch.FloatTensor(mid_img[:, i*batch_size:(i+1)*batch_size]/65535/gb_img[:, i*batch_size:(i+1)*batch_size]).view(light_num, -1).transpose(0, 1)
            shadow_normal_batch = torch.FloatTensor(shadow_normal[i*batch_size:(i+1)*batch_size])
            tsr_w_batch = torch.cosine_similarity(tsr_a_batch, shadow_normal_batch, dim = -1).reshape(batch_size, -1)
            tsr_w.append(tsr_w_batch.clone())
            p=0.2
            tsr_w_batch[tsr_w_batch <= p] = 0
            tsr_w_batch[tsr_w_batch > p] = 1
            shadow_batch = tsr_w_batch.sum(dim=-1)
            shadow_mask = torch.zeros_like(shadow_batch)
            shadow_mask[shadow_batch != light_num] = 1

            # process shadow mask
            shadow_mask = shadow_mask.reshape(1, 1, -1, img_width).to(torch.float32)
            kernel = torch.ones((3, 3)).to(torch.float32)
            for n in range(5):
                shadow_mask = kornia.morphology.dilation(shadow_mask, kernel)
            shadow_mask = shadow_mask.reshape(-1)

            for p in trange(batch_size):
                if shadow_mask[p] == 1:
                    cluster_ids_x, cluster_centers = kmeans(
                        X=tsr_b_batch[p].reshape(-1, 1), num_clusters=2, device=torch.device('cuda:0'), tqdm_flag=False, seed=0, iter_limit=300)
                    if cluster_centers[0] > cluster_centers[1]:
                        cluster_ids_x = 1 - cluster_ids_x
                    tsr_w_batch[p] = cluster_ids_x.reshape(-1)


            tsr_w_batch = tsr_w_batch.reshape(batch_size, -1, 1)
            tsr_svd_a = tsr_a_batch.reshape(batch_size, -1, 3)*tsr_w_batch
            tsr_svd_b = tsr_b_batch.reshape(batch_size, -1, 1)*tsr_w_batch

            normal = torch.linalg.lstsq(tsr_svd_a, tsr_svd_b)[0]
            normal = F.normalize(normal.squeeze_(), dim=-1)
            normal_list.append(normal)
            shadow_list.append(shadow_batch)
            shadow_mask_list.append(shadow_mask)

    tsr_normal = torch.stack(normal_list).view(img_width,-1,3)
    tsr_shadow = torch.stack(shadow_list).view(img_width,-1)
    w = torch.stack(tsr_w).view(img_width,-1,light_num).numpy()
    # np.save(f'C:/Users/ecoplants/Desktop/w.npy', w)
    tsr_shadow = (tsr_shadow/72)*255

    tsr_shadow_mask = torch.stack(shadow_mask_list).view(img_width,-1)*255

    return tsr_normal.numpy(), tsr_shadow.numpy().astype(np.uint8), tsr_shadow_mask.numpy().astype(np.uint8)


def correct_normal(normal, depth, mat_k, scale, kernel_size, use_cuda = False):
    if scale >= 4:
        use_cuda = True

    grid_y, grid_x = np.mgrid[0:depth.shape[0], 0:depth.shape[1]]
    grid_x += (11648 - 8192) //2 //scale
    grid_y += (8736 - 8192) //2 //scale
    coord_x = (grid_x - mat_k[0,2]) * depth / mat_k[0,0]
    coord_y = (grid_y - mat_k[1,2]) * depth / mat_k[1,1] * -1
    coord = np.stack((coord_x, coord_y, -depth)).transpose(1,2,0)

    if use_cuda:
        tsr_a = torch.zeros([kernel_size, kernel_size, 3]).cuda().requires_grad_()
        tsr_n = torch.cuda.FloatTensor(normal)
        tsr_c = (torch.cuda.FloatTensor(coord))
    else:
        tsr_a = torch.zeros([kernel_size, kernel_size, 3]).requires_grad_()
        tsr_n = torch.FloatTensor(normal)
        tsr_c = (torch.FloatTensor(coord))

    optimizer = torch.optim.Adam([
        {'params': [tsr_a], 'lr': 1e-1},
    ])

    tsr_udiff = torch.diff(tsr_c,dim=0)
    tsr_vdiff = torch.diff(tsr_c,dim=1)
    tsr_udiff = F.normalize(tsr_udiff, dim = -1)
    tsr_vdiff = F.normalize(tsr_vdiff, dim = -1)

    scale_factor = tsr_n.shape[0]//kernel_size

    with trange(20) as t:
        t.set_description('Correcting normal')
        for i in t:
            optimizer.zero_grad()
            tsr_a_after = F.interpolate(tsr_a.permute(2,0,1).unsqueeze(0),scale_factor=scale_factor, mode='bilinear', align_corners =True).squeeze().permute(1,2,0)
            tsr_r = euler_angles_to_matrix(tsr_a_after.reshape(-1,3), 'XYZ')
            tsr_n_after = tsr_r.view(-1,3,3).bmm(tsr_n.view(-1,3,1)).view(tsr_n.shape[0],tsr_n.shape[1], 3)
            tsr_n_after = F.normalize(tsr_n_after, dim=-1)
            E0 = tsr_n_after[1:]*tsr_udiff
            E1 = tsr_n_after[:,1:]*tsr_vdiff
            loss = E0.abs().mean()+E1.abs().mean()
            loss.backward()
            optimizer.step()
            if loss.isnan():
                break
            t.set_postfix(loss=float(loss))

    img_normal = F.normalize(tsr_n_after, dim=-1)
    tsr_reg = torch.zeros_like(tsr_n)
    tsr_reg[...,2] = 1
    tsr_n_after = tsr_r.view(-1,3,3).bmm(tsr_reg.view(-1,3,1)).view(tsr_n.shape[0],tsr_n.shape[1], 3)
    img_normal_correct = F.normalize(tsr_n_after, dim=-1)
    E0 = tsr_n_after[1:]*tsr_udiff
    E1 = tsr_n_after[:,1:]*tsr_vdiff
    return img_normal.cpu().detach().numpy(), img_normal_correct.cpu().detach().numpy(), E0.sum(dim = -1).abs_().cpu().detach_().numpy(), E1.sum(dim = -1).abs_().cpu().detach_().numpy()


def euler_angles_to_matrix(euler_angles, convention):
    """
    Convert rotations given as Euler angles in radians to rotation matrices.
    Args:
        euler_angles: Euler angles in radians as tensor of shape (..., 3).
        convention: Convention string of three uppercase letters from
            {"X", "Y", and "Z"}.
    Returns:
        Rotation matrices as tensor of shape (..., 3, 3).
    """
    def _axis_angle_rotation(axis, angle):
        """
        Return the rotation matrices for one of the rotations about an axis
        of which Euler angles describe, for each value of the angle given.
        Args:
            axis: Axis label "X" or "Y or "Z".
            angle: any shape tensor of Euler angles in radians
        Returns:
            Rotation matrices as tensor of shape (..., 3, 3).
        """
        cos = torch.cos(angle)
        sin = torch.sin(angle)
        one = torch.ones_like(angle)
        zero = torch.zeros_like(angle)
        if axis == "X":
            R_flat = (one, zero, zero, zero, cos, -sin, zero, sin, cos)
        elif axis == "Y":
            R_flat = (cos, zero, sin, zero, one, zero, -sin, zero, cos)
        elif axis == "Z":
            R_flat = (cos, -sin, zero, sin, cos, zero, zero, zero, one)
        else:
            raise ValueError("letter must be either X, Y or Z.")
        return torch.stack(R_flat, -1).reshape(angle.shape + (3, 3))
    if euler_angles.dim() == 0 or euler_angles.shape[-1] != 3:
        raise ValueError("Invalid input euler angles.")
    if len(convention) != 3:
        raise ValueError("Convention must have 3 letters.")
    if convention[1] in (convention[0], convention[2]):
        raise ValueError(f"Invalid convention {convention}.")
    for letter in convention:
        if letter not in ("X", "Y", "Z"):
            raise ValueError(f"Invalid letter {letter} in convention string.")
    matrices = [
        _axis_angle_rotation(c, e)
        for c, e in zip(convention, torch.unbind(euler_angles, -1))
    ]
    # return functools.reduce(torch.matmul, matrices)
    return torch.matmul(torch.matmul(matrices[0], matrices[1]), matrices[2])