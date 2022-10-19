# cython:language_level=3
from base.common import *
from exiftool import ExifToolHelper
import colour_demosaicing
import enum
import torch


def read_raw(fp):
    bayer = return_blc_bayer(fp)
    with ExifToolHelper() as eh:
        exif = eh.get_metadata([fp])
    return bayer, exif

def bayer_to_rgb(bayer, wb, sp=[0, 0], DEMOSACING_METHOD = 3, device = torch.device('cpu')):
    rawImage_wb = white_balance(bayer, wb, sp)
    image_rgb = demosaicing(rawImage_wb, sp, DEMOSACING_METHOD = DEMOSACING_METHOD, device = device)
    return image_rgb

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

    # logger.debug("WB coefficient is {}".format(wb_coeff))

    scale_coeff = wb_coeff

    # logger.debug("Scale coefficient is {}".format(scale_coeff))

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

def demosaicing(src, sp, DEMOSACING_METHOD = 0, device = torch.device('cpu')):
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
            2: colour_demosaicing.demosaicing_CFA_Bayer_Menon2007,
            # Menon2007 needs more than 20 GB memory
        }
        method = numbers.get(
            DEMOSACING_METHOD, colour_demosaicing.demosaicing_CFA_Bayer_bilinear)

        # logger.debug("Demosacing using [{}]...".format(method))
        rslt = method(src.astype('float32'), Bayer_Pattern)
    elif DEMOSACING_METHOD == 3:
        rslt = mhc_demosaicing_gpu(src, Bayer_Pattern, device)
    elif DEMOSACING_METHOD == 4:
        rslt = mhc_demosaicing_gpu(src, Bayer_Pattern, device)
    else:
        logger.error('Demosaicing method not supported.')
        raise Exception("ISP error")

    return np.clip(rslt, 0, 65535).astype(np.uint16)

def mhc_demosaicing_gpu(cfa, bayer_pattern='RGGB', device = torch.device('cpu')):
    class Layout(enum.Enum):
        """Possible Bayer color filter array layouts.
        The value of each entry is the color index (R=0,G=1,B=2)
        within a 2x2 Bayer block.
        """

        RGGB = (0, 1, 1, 2)
        GRBG = (1, 0, 2, 1)
        GBRG = (1, 2, 0, 1)
        BGGR = (2, 1, 1, 0)

    class Debayer5x5(torch.nn.Module):
        """Demosaicing of Bayer images using Malver-He-Cutler algorithm.
        Requires BG-Bayer color filter array layout. That is,
        the image[1,1]='B', image[1,2]='G'. This corresponds
        to OpenCV naming conventions.
        Compared to Debayer2x2 this method does not use upsampling.
        Compared to Debayer3x3 the algorithm gives sharper edges and
        less chromatic effects.
        ## References
        Malvar, Henrique S., Li-wei He, and Ross Cutler.
        "High-quality linear interpolation for demosaicing of Bayer-patterned
        color images." 2004
        """

        def __init__(self, layout: Layout = Layout.RGGB):
            super(Debayer5x5, self).__init__()
            self.layout = layout
            # fmt: off
            self.kernels = torch.nn.Parameter(
                torch.tensor(
                    [
                        # G at R,B locations
                        # scaled by 16
                        [ 0,  0, -2,  0,  0], # noqa
                        [ 0,  0,  4,  0,  0], # noqa
                        [-2,  4,  8,  4, -2], # noqa
                        [ 0,  0,  4,  0,  0], # noqa
                        [ 0,  0, -2,  0,  0], # noqa

                        # R,B at G in R rows
                        # scaled by 16
                        [ 0,  0,  1,  0,  0], # noqa
                        [ 0, -2,  0, -2,  0], # noqa
                        [-2,  8, 10,  8, -2], # noqa
                        [ 0, -2,  0, -2,  0], # noqa
                        [ 0,  0,  1,  0,  0], # noqa

                        # R,B at G in B rows
                        # scaled by 16
                        [ 0,  0, -2,  0,  0], # noqa
                        [ 0, -2,  8, -2,  0], # noqa
                        [ 1,  0, 10,  0,  1], # noqa
                        [ 0, -2,  8, -2,  0], # noqa
                        [ 0,  0, -2,  0,  0], # noqa

                        # R at B and B at R
                        # scaled by 16
                        [ 0,  0, -3,  0,  0], # noqa
                        [ 0,  4,  0,  4,  0], # noqa
                        [-3,  0, 12,  0, -3], # noqa
                        [ 0,  4,  0,  4,  0], # noqa
                        [ 0,  0, -3,  0,  0], # noqa

                        # R at R, B at B, G at G
                        # identity kernel not shown
                    ]
                ).view(4, 1, 5, 5).float() / 16.0,
                requires_grad=False,
            )
            # fmt: on

            self.index = torch.nn.Parameter(
                # Below, note that index 4 corresponds to identity kernel
                self._index_from_layout(layout),
                requires_grad=False,
            )

        def forward(self, x):
            """Debayer image.
            Parameters
            ----------
            x : Bx1xHxW tensor
                Images to debayer
            Returns
            -------
            rgb : Bx3xHxW tensor
                Color images in RGB channel order.
            """
            B, C, H, W = x.shape

            xpad = torch.nn.functional.pad(x, (2, 2, 2, 2), mode="reflect")
            planes = torch.nn.functional.conv2d(xpad, self.kernels, stride=1)
            planes = torch.cat(
                (planes, x), 1
            )  # Concat with input to give identity kernel Bx5xHxW
            rgb = torch.gather(
                planes,
                1,
                self.index.repeat(
                    1,
                    1,
                    torch.div(H, 2, rounding_mode="floor"),
                    torch.div(W, 2, rounding_mode="floor"),
                ).expand(
                    B, -1, -1, -1
                ),  # expand for singleton batch dimension is faster
            )
            return torch.clamp(rgb, 0, 1)

        def _index_from_layout(self, layout: Layout) -> torch.Tensor:
            """Returns a 1x3x2x2 index tensor for each color RGB in a 2x2 bayer tile.
            Note, the index corresponding to the identity kernel is 4, which will be
            correct after concatenating the convolved output with the input image.
            """
            #       ...
            # ... b g b g ...
            # ... g R G r ...
            # ... b G B g ...
            # ... g r g r ...
            #       ...
            # fmt: off
            rggb = torch.tensor(
                [
                    # dest channel r
                    [4, 1],  # pixel is R,G1
                    [2, 3],  # pixel is G2,B
                    # dest channel g
                    [0, 4],  # pixel is R,G1
                    [4, 0],  # pixel is G2,B
                    # dest channel b
                    [3, 2],  # pixel is R,G1
                    [1, 4],  # pixel is G2,B
                ]
            ).view(1, 3, 2, 2)
            # fmt: on
            return {
                Layout.RGGB: rggb,
                Layout.GRBG: torch.roll(rggb, 1, -1),
                Layout.GBRG: torch.roll(rggb, 1, -2),
                Layout.BGGR: torch.roll(rggb, (1, 1), (-1, -2)),
            }.get(layout)

    layout = {
        "RGGB": Layout.RGGB,
        "GRBG": Layout.GRBG,
        "GBRG": Layout.GBRG,
        "BGGR": Layout.BGGR,
    }[bayer_pattern]

    # if torch.cuda.mem_get_info(device=torch.device('cuda:0'))[0] < 7*1024**3:
    #     device = torch.device("cpu")
    
    cfa = cfa.astype(np.float32)/65535.0

    with torch.no_grad():
        deb = Debayer5x5(layout=layout).to(device).to(torch.float16)
        cfa = torch.from_numpy(cfa).to(torch.float16).to(device).unsqueeze(0).unsqueeze(0)
        rgb = deb(cfa).squeeze(0).permute(1, 2, 0).cpu().numpy()

    del cfa, deb
    torch.cuda.empty_cache()
    return (rgb*65535).astype(np.uint16)
