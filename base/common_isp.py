from base.common import *
from exiftool import ExifToolHelper
import colour_demosaicing
import rawpy
import numpy as np


def read_raw(fp):
    bayer = return_blc_bayer(fp)
    with ExifToolHelper() as eh:
        exif = eh.get_metadata([fp])
    return bayer, exif

def bayer_to_bgr(bayer, wb, sp=[0, 0]):
    rawImage_wb = white_balance(bayer, wb, sp)
    image_demosaiced = demosaicing(rawImage_wb, None, sp, 1)

    return image_demosaiced

# will return the bayer pattern of the image with the official size 11648x8736
def return_blc_bayer(fp, cam_model = "GFX100S"):
    if cam_model == "GFX100S":
        if not fp.lower().endswith(".raf"):
            logger.error('Image format does not match the camera model. Please check the file format.')
            raise Exception("ISP error")

        with rawpy.imread(fp) as rawData:
            raw_image = rawData.raw_image.astype(np.float32)
            for i, bl in enumerate(rawData.black_level_per_channel):
                raw_image[rawData.raw_colors == i] = (
                    raw_image[rawData.raw_colors == i]-bl) / ((65535 - bl)/65535)
            raw_image = np.clip(raw_image, 0, 65535).astype(np.uint16)
            return raw_image[7:-11, 8:-152]
    else:
        logger.error('Camera model not supported.')
        raise Exception("ISP error")

def white_balance(src, wb, sp):
    wb = np.array(wb)
    wb_coeff = np.asarray(wb[:3]) / min(wb[:3])
    wb_coeff = np.append(wb_coeff, wb_coeff[1])


    logger.debug("WB coefficient is {}".format(wb_coeff))

    scale_coeff = wb_coeff

    logger.debug("Scale coefficient is {}".format(scale_coeff))

    buffer_height = src.shape[0] // 2 * 2 + 2
    buffer_width = src.shape[1] // 2 * 2 + 2
    scale_matrix = np.empty([buffer_height, buffer_width], dtype=np.float32)
    raw_colors = np.array([[0, 1], [3, 2]]).astype(np.uint8).repeat(
        buffer_width//2, axis=0).reshape(1, -1).repeat(buffer_height//2, axis=0).reshape(-1, buffer_width)
    for i, scale_co in enumerate(scale_coeff):
        scale_matrix[raw_colors == i] = scale_co
    src = src.astype('float32')
    scale_matrix = scale_matrix[sp[0]%2:sp[0]%2+src.shape[0], sp[1]%2:sp[1]%2+src.shape[1]]
    rslt = np.clip(src * scale_matrix, 0, 65535).astype(np.uint16)

    return rslt

def demosaicing(src, raw, sp, DEMOSACING_METHOD = 0):
    # Demosaicing. Default using colour_demosaicing.demosaicing_CFA_Bayer_bilinear

    if sp[0]%2 == 0 and sp[1]%2 == 0:
        Bayer_Pattern = 'RGGB'
    if sp[0]%2 == 1 and sp[1]%2 == 0:
        Bayer_Pattern = 'GBRG'
    if sp[0]%2 == 0 and sp[1]%2 == 1:
        Bayer_Pattern = 'GRBG'
    if sp[0]%2 == 1 and sp[1]%2 == 1:
        Bayer_Pattern = 'BGGR'

    if DEMOSACING_METHOD < 3:
        numbers = {
            0: colour_demosaicing.demosaicing_CFA_Bayer_bilinear,
            1: colour_demosaicing.demosaicing_CFA_Bayer_Malvar2004,
            2: colour_demosaicing.demosaicing_CFA_Bayer_Menon2007
            # Menon2007 needs more than 20 GB memory
        }
        method = numbers.get(
            DEMOSACING_METHOD, colour_demosaicing.demosaicing_CFA_Bayer_bilinear)

        logger.debug("Demosacing using [{}]...".format(method))
        rslt = method(src.astype('float32'), Bayer_Pattern)

    rslt = np.clip(rslt, 0, 65535).astype(np.uint16)

    return rslt