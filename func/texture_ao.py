# cython:language_level=3
from base.common import *
import taichi as ti
import cv2
import torch

# if available gpu memory larger than 20G, use CUDA to init taichi
if torch.cuda.is_available() and torch.cuda.mem_get_info(device=torch.device('cuda:0'))[0] > 20*1024**3:
    ti.init(arch=ti.cuda, device_memory_GB=20)
    logger.info("Use CUDA to init taichi")
else:
    ti.init(arch=ti.cpu)
    logger.info("Use CPU to init taichi")



@count_time
def produce_ao_map(height_img, conf):
    # read conf
    output_folder = conf["output_folder"]
    scale = conf["scale"]
    name = conf["name"]

    height_scale = conf["ao_map"]["height_scale"]
    dir_count = conf["ao_map"]["dir_count"]
    strength = conf["ao_map"]["strength"]

    height, width = height_img.shape[:2]
    logger.debug(f"Input height shape: {height_img.shape}")

    height_scale_val = width * height_scale * 2

    pad_img = np.pad(height_img, (1, 1), mode='edge')
    pad_img = pad_img * height_scale_val
    ti_pad_img = ti.field(dtype=ti.f32, shape=pad_img.shape[:2])
    ti_pad_img.from_numpy(pad_img)
    max_height = np.max(height_img) * height_scale_val
    min_height = np.min(height_img) * height_scale_val
    logger.debug(f"Height range: {min_height} - {max_height}")

    ti_height_image = ti.field(dtype=float, shape=(height, width))
    ti_height_image.from_numpy(height_img*height_scale_val) 

    ti_ao_image = ti.field(dtype=float, shape=(height, width))

    @ti.func
    def bilinear_interpolation(w:ti.f32, h:ti.f32):
        w1 = ti.floor(w)
        h1 = ti.floor(h)
        w2 = ti.ceil(w)
        h2 = ti.ceil(h)
        return ti_height_image[h1,w1]*(w2-w)*(h2-h) + ti_height_image[h1,w2]*(w-w1)*(h2-h) + ti_height_image[h2,w1]*(w2-w)*(h-h1) + ti_height_image[h2,w2]*(w-w1)*(h-h1)

    @ti.func
    def rand_point():
        '''
            rand-sample in semi-sphere:
            phi = arccos(sqrt(1-r2)))
            theta = 2*pi*r1
        :return:
        '''
        r1 = ti.random(dtype=float)
        r2 = ti.random(dtype=float)
        # x = ti.cos(2 * ti.math.pi * r1) * 2 * ti.sqrt(r2 * (1-r2))
        # y = ti.sin(2 * ti.math.pi * r1) * 2 * ti.sqrt(r2 * (1-r2))
        # z = 1 - 2 * r2
        x = ti.cos(2 * ti.math.pi * r1) * ti.sqrt(r2)
        y = ti.sin(2 * ti.math.pi * r1) * ti.sqrt(r2)
        z = ti.sqrt(1 - r2)

        return ti.Vector([x, y, z])

    @ti.func
    def rand_dir_along_normal(n):
        rand = rand_point()
        return ti.math.sign(ti.math.dot(n, rand)) * rand
    
    @ti.func 
    def rand_cosine_weighted_dir_along_normal(n):
        rand = rand_point()
        r = rand[0]*0.5 + 0.5
        angle = (rand[1] +1.0) * ti.math.pi
        sr = ti.sqrt(r)
        p = ti.Vector([sr * ti.cos(angle), sr * ti.sin(angle)])
        ph = ti.Vector([p[0],p[1], ti.sqrt(1.0-ti.math.dot(p, p))])

        tangent = ti.math.normalize(rand)
        bitangent = ti.math.cross(tangent, n)
        tangent = ti.math.cross(bitangent, n)

        return tangent*ph[0] + bitangent*ph[1] + n*ph[2]

    @ti.kernel
    def rtao():
        for h, w in ti.ndrange(height, width):
            v00 = ti.Vector([w,   h,   ti_pad_img[h,   w]])
            v01 = ti.Vector([w+1, h,   ti_pad_img[h,   w+1]])
            v02 = ti.Vector([w+2, h,   ti_pad_img[h,   w+2]])
            v03 = ti.Vector([w  , h+1, ti_pad_img[h+1, w]])
            v05 = ti.Vector([w+2, h+1, ti_pad_img[h+1, w+2]])
            v06 = ti.Vector([w  , h+2, ti_pad_img[h+2, w]])
            v07 = ti.Vector([w+1, h+2, ti_pad_img[h+2, w+1]])
            v08 = ti.Vector([w+2, h+2, ti_pad_img[h+2, w+2]])
            v_s = 10*(v05-v03) + 3*(v02-v00) + 3*(v08-v06)
            v_t = 10*(v07-v01) + 3*(v06-v00) + 3*(v08-v02)
            normal = ti.math.cross(v_s, v_t)
            n = ti.math.normalize(normal)

            ao = 0.0

            pix_depth = ti_height_image[h, w]
            pix_pos = ti.Vector([w, h, pix_depth])

            for _ in range(dir_count):
                dir = ti.math.normalize(rand_dir_along_normal(n))
                cosn = ti.math.dot(dir, n)
                if dir[0] == 0 and dir[1] == 0:
                    continue
                # dir_2d = ti.Vector([dir[0], dir[1]])
                # norm = dir_2d.norm()
                # dir = dir/norm
                step = 1

                for j in range(width):
                    if step > width/8:
                        break
                    current_pos = pix_pos + dir * step

                    if current_pos[0] < 0 or current_pos[0] > height or current_pos[1] < 0 or current_pos[1] > width:
                        break

                    current_z = ti_height_image[int(ti.round(current_pos[1])), int(ti.round(current_pos[0]))]
                    
                    if dir[2] > 0 and current_pos[2] > max_height:
                        break

                    if current_pos[2] - 0.01 < current_z:
                        ao += cosn
                        break
                    step += j
            
            ao /= dir_count
            ao = ti.pow(ao, 1/strength)
            ao = 1.0 - ao

            ti_ao_image[h, w] = ao

    rtao()
    out_ao = ti_ao_image.to_numpy()
    out_ao = denormalize_img(out_ao, dtype="uint16")

    cv2.imencode('.png', out_ao)[1].tofile(f'{output_folder}/T_{name}_AO_{8//scale}K.png')

def denormalize_img(in_img, dtype="uint8"):
    if dtype == "uint8":
        image = (in_img*255).astype(np.uint8)
    elif dtype == "uint16":
        image = (in_img*65535).astype(np.uint16)
    else:
        image = None
        exit_with_error("Invalid dtype")
    return image