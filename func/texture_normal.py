# cython:language_level=3
from numpy import ndarray
from base.common import *
import cv2
import torch
import torch.nn.functional as F

@count_time
def produce_normal_map(mid_undist_image, depth, conf):
    # load conf
    output_dir = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]
    lens = conf["lens"]

    camera_height = conf["geometry"]["camera_height"]
    grayboard_height = conf["geometry"]["grayboard_height"]
    rot_count = conf["geometry"]['rot_count']
    light_num = len(conf["geometry"]['light_str'])//2 * rot_count
    mat_k = conf["camera_matrices"]["mid_mat_k"]
    gray_scale = conf["grayboard"]["gray_scale"]
    kernel_size = conf["normal_map"]["filter_original_size"]
    grayboard_data_path = conf["grayboard_data_path"]

    specular_removal = conf["normal_map"]["specular_removal"]
    deshadow_ratio = conf["normal_map"]["deshadow_ratio"]
    batch_count = conf["normal_map"]["normal_batch_count"]

    os.makedirs(f'{output_dir}/reference', exist_ok=True)

    # generate reference normal map based on depth
    if depth is not None:
        normal_ref = dsp2norm(depth, alpha=1)
        normal_ref = normal_ref * 255
        normal_ref = normal_ref.astype(np.uint8)
        cv2.imencode('.png', normal_ref)[1].tofile(f'{output_dir}/reference/normal_ref.png')
        logger.success(f'[normal_ref] saved.')

    plain_depth = np.ones([8192//scale, 8192//scale]) * camera_height
    light_coord = get_light_coord(conf["geometry"])[:light_num]
    pixel_coord = get_pixel_coord(mat_k, camera_height, plain_depth, scale)

    cache_file = f"pre_data/lightfield_{lens}_{scale}_{camera_height}.npz"
    if not os.path.exists(grayboard_data_path):
        exit_with_error(f"{grayboard_data_path} not exist")
    if os.path.exists(cache_file):
        lightfield_image = np.load(cache_file)['data']
        logger.success(f'Lightfield cache loaded')
    else:
        # load grayboard data
        grayboard_data = np.load(grayboard_data_path, allow_pickle=True)

        grayboard_depth = camera_height - grayboard_height
        logger.info(f'No grayboard_depth loaded, use calculated grayboard_depth={grayboard_depth} instead.')

        # calculate light field
        grayboard_image = grayboard_data['data'][:mid_undist_image.shape[0]]
        g_wb = grayboard_data['wb']
        if scale != 1:
            grayboard_image = cv2.resize(grayboard_image.transpose(1, 2, 0),
                        (grayboard_image.shape[2]//scale, grayboard_image.shape[1]//scale)).transpose(2, 0, 1)

        lightfield_image = get_lightfield_from_grayboard(
                grayboard_image, gray_scale, light_coord, pixel_coord,
                mat_k, camera_height, grayboard_depth)
        np.savez(cache_file, data=lightfield_image)
        logger.success(f'Lightfield cached')

    # for i in range(light_num):
    #     cv2.imencode('.png', (lightfield_image[i]*65535).astype(np.uint16))[1].tofile(
    #         f'{output_dir}/reference/lightfield_{i}.png')
    
    mid_texture_image = mid_undist_image[:, 68*4//scale:-68*4//scale, 432*4//scale:-432*4//scale]
    
    # slove normal without shadow
    logger.info("Processing original normal map")
    normal, albedo_weight = solve_normal(mid_texture_image, lightfield_image, light_coord, pixel_coord, specular_removal, int(mid_texture_image.shape[0]*deshadow_ratio), batch_count)

    normal_clip = np.zeros((normal.shape[0], normal.shape[1]), dtype=np.uint8)
    normal_clip[normal[...,2] < 0] = 255
    cv2.imencode('.png', normal_clip)[1].tofile(f'{output_dir}/reference/Normal_Original_Clip_{8//scale}K.png')
    normal[...,2][normal[...,2] < 0] = 1e-3

    save_normal(normal, output_dir, f'T_{name}_Normal_Original_{8//scale}K.png')

    # correct normal use solved depth
    if depth is not None:
        logger.info("Depth correction")
        normal_corrected, normal_shift, loss_u, loss_v = correct_normal(normal, depth, mat_k, scale, kernel_size)

        normal_clip = np.zeros((normal.shape[0], normal.shape[1]), dtype=np.uint8)
        normal_clip[normal[...,2] < 0] = 255
        cv2.imencode('.png', normal_clip)[1].tofile(f'{output_dir}/reference/Normal_Clip_{8//scale}K.png')
        normal_corrected[...,2][normal_corrected[...,2] < 0] = 1e-3

        save_normal(normal_corrected, output_dir, f'T_{name}_Normal_Corrected_{8//scale}K.png')

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
    logger.info("Plane correction")
    plain_normal, normal_shift, loss_u, loss_v = correct_normal(normal, plain_depth, mat_k, scale, kernel_size)

    normal_clip = np.zeros((normal.shape[0], normal.shape[1]), dtype=np.uint8)
    normal_clip[normal[...,2] < 0] = 255
    cv2.imencode('.png', normal_clip)[1].tofile(f'{output_dir}/reference/Normal_Plain_Clip_{8//scale}K.png')
    plain_normal[...,2][plain_normal[...,2] < 0] = 1e-3

    save_normal(plain_normal, output_dir, f'T_{name}_Normal_Plain_{8//scale}K.png')

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
    logger.info("Processing shadow normal map")
    normal, albedo_weight_shadow = solve_normal(mid_texture_image, lightfield_image, light_coord, pixel_coord, 0, mid_texture_image.shape[0], batch_count)

    normal_clip = np.zeros((normal.shape[0], normal.shape[1]), dtype=np.uint8)
    normal_clip[normal[...,2] < 0] = 255
    cv2.imencode('.png', normal_clip)[1].tofile(f'{output_dir}/reference/Normal_Shadow_Clip_{8//scale}K.png')
    normal[...,2][normal[...,2] < 0] = 1e-3

    save_normal(normal, output_dir, f"T_{name}_Normal_Shadow_Original_{8//scale}K.png")

    if depth is not None:
        logger.info("Depth correction")
        normal_corrected, normal_shift, loss_u, loss_v = correct_normal(normal, depth, mat_k, scale, kernel_size)

        normal_clip = np.zeros((normal.shape[0], normal.shape[1]), dtype=np.uint8)
        normal_clip[normal[...,2] < 0] = 255
        # cv2.imencode('.png', normal_clip)[1].tofile(f'{output_dir}/reference/Normal_Clip_{8//scale}K.png')
        normal_corrected[...,2][normal_corrected[...,2] < 0] = 1e-3

        save_normal(normal_corrected, output_dir, f'T_{name}_Normal_Corrected_Shadow_{8//scale}K.png')

        # normal_shift = normal_shift / 2 + 0.5
        # normal_shift = normal_shift * 65535
        # normal_shift = normal_shift[..., ::-1]
        # normal_shift = normal_shift.astype(np.uint16)
        # cv2.imencode('.png', normal_shift)[1].tofile(
        #     f'{output_dir}/reference/Normal_Shift_{8//scale}K.png')
        # cv2.imencode('.png', (loss_u/loss_u.max()*65535).astype(np.uint16))[1].tofile(
        #     f'{output_dir}/reference/Normal_Shift_Loss_U_{8//scale}K.png')
        # cv2.imencode('.png', (loss_v/loss_v.max()*65535).astype(np.uint16))[1].tofile(
        #     f'{output_dir}/reference/Normal_Shift_Loss_V_{8//scale}K.png')

    return albedo_weight, albedo_weight_shadow, grayboard_image, g_wb



def get_light_coord(conf):
    light_strs = conf['light_str']
    rot_count = conf['rot_count']
    rot_clockwise = conf['rot_clockwise']

    start_pos = [list(map(float, light_str.split(','))) for light_str in light_strs]
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
    return np.stack((coord_x, coord_y, camera_height - depth)).transpose(1,2,0).astype(np.float32)

@count_time
def get_lightfield_from_grayboard(grayboard_image, gray_scale, light_coord, pixel_coord, mat_k, camera_height, grayboard_depth):
    z_coord = (camera_height - grayboard_depth)
    lightfields = []
    executor = ProcessPoolExecutor(max_workers=2)
    tasks = [executor.submit(lightfield_subprocess, light_coord[i], pixel_coord, z_coord, grayboard_depth, mat_k, grayboard_image[i], gray_scale) for i in range(light_coord.shape[0])]
    for task in tqdm(as_completed(tasks), total=len(tasks)):
        lightfields.append(task.result())
    wait(tasks)
    return np.array(lightfields)

def lightfield_subprocess(light_coord, pixel_coord, z_coord, grayboard_depth, mat_k, grayboard_image, gray_scale):
    line_diff = light_coord.reshape(1, 1, 3).astype(np.float32) - pixel_coord
    z_diff = z_coord - pixel_coord[..., 2:]
    grayboard_plane = z_diff * line_diff[..., :2] / line_diff[..., 2:] + pixel_coord[..., :2]

    idx_x = grayboard_plane[..., 1] / (-grayboard_depth / float(mat_k[1, 1])) + float(mat_k[1, 2])
    idx_y = grayboard_plane[..., 0] / (grayboard_depth / float(mat_k[0, 0])) + float(mat_k[0, 2])
    sp_x = (grayboard_image.shape[0] - idx_x.shape[0])//2
    sp_y = (grayboard_image.shape[1] - idx_y.shape[1])//2
    fp_x = (grayboard_image.shape[0] + idx_x.shape[0])//2
    fp_y = (grayboard_image.shape[1] + idx_y.shape[1])//2
    idx_x = np.maximum(sp_x, np.minimum(fp_x, idx_x)).astype(np.uint32)
    idx_y = np.maximum(sp_y, np.minimum(fp_y, idx_y)).astype(np.uint32)
    idx_image = grayboard_image.shape[1]*idx_x + idx_y

    lightfield = grayboard_image.reshape(-1)[idx_image].astype(np.float32)/65535/gray_scale

    grayboard_coord = np.concatenate((
        grayboard_plane, np.ones_like(grayboard_plane[..., 1:]) * z_coord), axis=-1)
    dis_square_pixel = (line_diff**2).sum(axis=-1)
    dis_square_grayboard = ((light_coord.reshape(1, 1, 3).astype(np.float32) - grayboard_coord)**2).sum(axis=-1)
    lightfield *= dis_square_grayboard / dis_square_pixel

    light_cos = line_diff[...,2]/np.sqrt((line_diff**2).sum(axis = -1))
    lightfield/=light_cos
    return lightfield

@count_time
@torch.no_grad()
def solve_normal(mid_img, gb_img, light_coord, pixel_coord, rank_start, rank_finish, batch_count=16):
        
    light_num = light_coord.shape[0]
    logger.info(f'Solve normal for {light_num} lights.')
    img_width = mid_img.shape[1]
    batch_size = (img_width*img_width) // batch_count
    normal_list = []
    albedo_weight_list = []
    svd_count = rank_finish - rank_start

    light_coord = light_coord.reshape(-1, 1, 3)
    pixel_coord = np.expand_dims(pixel_coord.reshape(-1, 3), axis = 0).astype(np.float32)
    mid_img = mid_img.reshape(light_num, -1)
    gb_img = gb_img.reshape(light_num, -1)

    with trange(batch_count) as t:
        for i in t:
            t.set_description(f'Solving normal batch [{i}]')
            light_dir_batch = torch.FloatTensor(light_coord - pixel_coord[:, i*batch_size:(i+1)*batch_size, :])
            tsr_a_batch = F.normalize(light_dir_batch.transpose(0, 1), dim = -1)
            tsr_b_batch = torch.FloatTensor(mid_img[:, i*batch_size:(i+1)*batch_size]/65535/gb_img[:, i*batch_size:(i+1)*batch_size]).view(light_num, -1).transpose(0, 1).unsqueeze(-1)
            weight = torch.squeeze(tsr_b_batch)
            rank = weight.sort(dim=1, descending = True)[1][:, rank_start:rank_finish]
            idx = (rank + torch.arange(batch_size).view(-1, 1)*light_num).reshape(-1)

            tsr_svd_a = tsr_a_batch.reshape(-1, 3)[idx].view(-1, svd_count, 3)
            tsr_svd_b = tsr_b_batch.reshape(-1)[idx].view(-1, svd_count, 1)

            normal = torch.linalg.lstsq(tsr_svd_a, tsr_svd_b)[0]
            normal = F.normalize(normal.squeeze_(), dim=-1)
            normal_list.append(normal)

            tsr_lightfield_batch = torch.FloatTensor(gb_img[:, i*batch_size:(i+1)*batch_size]).transpose(0, 1).unsqueeze(-1)
            all_weight = 1/(tsr_a_batch.bmm(normal.unsqueeze(-1))*tsr_lightfield_batch).reshape(-1)
            albedo_weight = torch.zeros_like(all_weight)
            albedo_weight[idx] = all_weight[idx]
            albedo_weight_list.append(albedo_weight.view(-1, light_num))

    tsr_normal = torch.stack(normal_list)
    tsr_normal = tsr_normal.view(img_width,-1,3)
    tsr_albedo_weight = torch.stack(albedo_weight_list)
    tsr_albedo_weight = tsr_albedo_weight.view(img_width,-1, light_num).permute(2,0,1)
    return tsr_normal.numpy(), tsr_albedo_weight.numpy()

# def correct_normal_gpu(normal, depth, mat_k, scale, kernel_size):
#     try:
#         logger.info('Try to use CUDA for normal correction.')
#         normal_corrected, normal_shift, loss_u, loss_v = correct_normal(normal, depth, mat_k, scale, kernel_size, use_cuda = True)
#     except:
#         logger.warning('Failed to correct normal with GPU, use CPU instead.')
#         normal_corrected, normal_shift, loss_u, loss_v = correct_normal(normal, depth, mat_k, scale, kernel_size, use_cuda = False)
#     return normal_corrected, normal_shift, loss_u, loss_v

@count_time
def correct_normal(normal, depth, mat_k, scale, kernel_size, use_cuda = False):
    if torch.cuda.is_available():
        if torch.cuda.mem_get_info(device=torch.device('cuda:0'))[0] > 44*1024**3:
            use_cuda = True
        elif normal.shape[0]<4100 and torch.cuda.mem_get_info(device=torch.device('cuda:0'))[0] > 20*1024**3:
            use_cuda = True
        elif normal.shape[0]<2050 and torch.cuda.mem_get_info(device=torch.device('cuda:0'))[0] > 10*1024**3:
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

    tsr_r, tsr_n_after = torch.zeros_like(tsr_n), torch.zeros_like(tsr_n)

    with trange(15) as t:
        t.set_description('Correcting normal')
        for _ in t:
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
            # t.set_postfix(loss=float(loss))

    img_normal = F.normalize(tsr_n_after, dim=-1)
    tsr_reg = torch.zeros_like(tsr_n)
    tsr_reg[...,2] = 1
    tsr_n_after = tsr_r.view(-1,3,3).bmm(tsr_reg.view(-1,3,1)).view(tsr_n.shape[0],tsr_n.shape[1],3)
    img_normal_correct = F.normalize(tsr_n_after, dim=-1)
    E0 = tsr_n_after[1:]*tsr_udiff
    E1 = tsr_n_after[:,1:]*tsr_vdiff
    return img_normal.cpu().detach().numpy(), img_normal_correct.cpu().detach().numpy(), E0.sum(dim = -1).abs_().cpu().detach_().numpy(), E1.sum(dim = -1).abs_().cpu().detach_().numpy()

# from Pytorch3D
def euler_angles_to_matrix(euler_angles, convention):
    def _axis_angle_rotation(axis, angle):
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

def save_normal(normal: ndarray, output_dir: str, filename: str):
    normal = normal / 2 + 0.5
    normal = normal * 65535
    normal = normal[..., ::-1]
    normal = normal.astype(np.uint16)
    cv2.imencode('.png', normal)[1].tofile(f'{output_dir}/{filename}')
    logger.success(f'[{filename}] saved.')

def dsp2norm(img, alpha=0.1):
    g_x = cv2.Sobel(img, cv2.CV_32F, 1, 0, ksize=3)
    g_y = cv2.Sobel(img, cv2.CV_32F, 0, 1, ksize=3)

    h,w = img.shape
    normal_map = np.zeros((h,w,3))
    normal_map[...,0] = g_x
    normal_map[...,1] = -g_y
    normal_map[...,2] = 1/alpha

    normal_map = (normal_map.transpose(2,0,1) / np.linalg.norm(normal_map, axis=2)).transpose(1,2,0)
    normal_map = 0.5*normal_map.copy()+0.5

    return normal_map[...,::-1]