# cython:language_level=3
from base.common import *
from base.common_image import rgb_to_srgb
import cv2


def calib(marker_img, conf, mat_k, mat_dist):
    logger.debug("----- Calib grayboard marker begin")
    
    conf_grid_size = conf["marker"]["grid_size"]
    conf_height = conf["marker"]["height"]
    detect_areas = conf["marker"]["detection_areas"]

    markers = extract_aruco_corners(marker_img, detect_areas)
    
    for marker in markers:
        if marker is None:
            logger.warning('Not all markers in configuration detected')
            return detect_areas, markers, None
    
    undist_markers = cv2.undistortPoints(np.array([marker[1] for marker in markers]).reshape(-1, 2), mat_k, mat_dist, None, mat_k).reshape(-1, 2)

    mat_coord = np.stack((
        (undist_markers[:, 0] - mat_k[0, 2]) / mat_k[0, 0],
        (undist_markers[:, 1] - mat_k[1, 2]) / mat_k[1, 1])).transpose(1, 0).reshape(-1, 4, 2)

    grid_size = (np.sqrt(np.diff(np.concatenate((mat_coord, mat_coord[:, :1]), axis=1), axis=1)**2).sum(axis=2)).mean()

    camera_height = conf_grid_size/grid_size + conf_height

    return detect_areas, markers, camera_height

@count_time
def get_camera_height(gray, mat_k, mat_dist, conf):
    logger.info('Calculate camera height from markers.')
    detect_areas = conf["marker"]["detection_areas"]
    height = conf["marker"]["height"]
    grid_size = conf["marker"]["grid_size"]

    markers = extract_aruco_corners(gray, detect_areas)
    
    # check mark detect results
    id_list = []
    for i in range(len(detect_areas)):
        id_list.extend(list(map(int, (detect_areas[i]['id_str'].split(',')))))

    succ_markers = []
    for marker in markers:
        if marker is not None and marker[0] in id_list:
            succ_markers.append(marker[1])

    if not (len(id_list)/2 < len(succ_markers) and len(succ_markers) <= len(id_list)):
        logger.warning('Markers detecte failed.')
        return None

    # calculate height
    undist_markers = np.array(succ_markers).reshape(-1, 2)
    mat_coord = np.stack((
        (undist_markers[:, 0] - mat_k[0, 2]) / mat_k[0, 0],
        (undist_markers[:, 1] - mat_k[1, 2]) / mat_k[1, 1])).transpose(1, 0).reshape(-1, 4, 2)

    grid_size_ = (np.sqrt(np.diff(np.concatenate((mat_coord, mat_coord[:, :1]), axis=1), axis=1)**2).sum(axis=2)).mean()

    camera_height = grid_size/grid_size_ + height
    print("???", camera_height)

    return camera_height

def extract_aruco_corners(gray, detect_areas):
    params = cv2.aruco.DetectorParameters_create()
    dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_1000)
    params.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX
    marker_list = []
    for i in range(len(detect_areas)):
        sub_gray = gray[
            int(detect_areas[i]['bottom']*gray.shape[0]) : int(detect_areas[i]['top']*gray.shape[0]),
            int(detect_areas[i]['left']*gray.shape[1]) : int(detect_areas[i]['right']*gray.shape[1])]
        id_list = list(map(int, (detect_areas[i]['id_str'].split(','))))
        sub_gray = rgb_to_srgb(sub_gray, 65535)
        sub_gray = (np.minimum(sub_gray / sub_gray.mean() * 128, 255))
        sub_gray = sub_gray.astype(np.uint8)

        # sub_gray = (sub_gray / 257).astype(np.uint8)
        # sub_gray = cv2.equalizeHist(sub_gray)

        markers, ids, _ = cv2.aruco.detectMarkers(sub_gray, dictionary=dictionary, parameters=params)

        for j in range(len(id_list)):
            idx = np.where(ids == id_list[j])[0]
            if idx.shape[0] == 0:
                marker_list.append(None)
            else:
                coord = markers[int(idx)].reshape(4, 2)
                coord[:, 0] += int(detect_areas[i]
                                    ['left']*gray.shape[1])
                coord[:, 1] += int(detect_areas[i]
                                    ['bottom']*gray.shape[0])
                marker_list.append((id_list[j], coord))
    return marker_list



def draw_markers(gray, markers, detect_areas):
    color_list = [0xd50000, 0xc51162, 0xaa00ff, 0x6200ea, 0x304ffe, 0x2962ff, 0x0091ea,
                    0x00b8d4, 0x00bfa5, 0x00c853, 0x64dd17]
    image_results = []
    for i in range(len(detect_areas)):
        sub_gray = gray[int(detect_areas[i]['bottom']*gray.shape[0]):int(detect_areas[i]['top']*gray.shape[0]),
                        int(detect_areas[i]['left']*gray.shape[1]):int(detect_areas[i]['right']*gray.shape[1])]
        # sub_gray = rgb_to_srgb(sub_gray, 65535)
        sub_gray = (np.minimum(sub_gray / sub_gray.mean() * 128, 255))
        sub_gray = sub_gray.astype(np.uint8)
        image = cv2.cvtColor(sub_gray, cv2.COLOR_GRAY2BGR)
        coord = []
        for marker in markers:
            if marker is not None:
                coord.append(marker[1])
        coord = np.array(coord)
        if coord.shape[0] == 0:
            image_results.append(sub_gray)
            continue
        grid_size = np.sqrt(
            (np.diff(np.concatenate((coord, coord[:, :1]), axis=1), axis=1)**2).sum(axis=2)).mean()
        font_size = int(grid_size * 0.005) + 1
        point_size = 2
        line_width = 1
        font_width = font_size * 3
        for j in range(len(markers)):
            if markers[j] is None:
                continue
            idx = markers[j][0]
            coord = markers[j][1]
            coord = coord.astype(np.int32)

            color = color_list[idx % len(color_list)]
            b = color % 256
            g = (color // 256) % 256
            r = color // 65536

            if (coord[:, 0].min() >= detect_areas[i]['left']*gray.shape[1]
                and coord[:, 0].max() < detect_areas[i]['right']*gray.shape[1]
                and coord[:, 1].min() >= detect_areas[i]['bottom']*gray.shape[0]
                    and coord[:, 1].max() < detect_areas[i]['top']*gray.shape[0]):
                coord[:, 0] -= int(detect_areas[i]
                                    ['left']*gray.shape[1])
                coord[:, 1] -= int(detect_areas[i]
                                    ['bottom']*gray.shape[0])
                index_coord = coord.mean(axis=0).astype(np.int32)
                index_coord[0] -= int(0.3*grid_size)
                sub_gray = cv2.putText(image, f"{j}:{idx}", index_coord,
                                        cv2.FONT_HERSHEY_SIMPLEX, font_size, (b, g, r, 200), font_width)
                for k in range(4):
                    cv2.circle(
                        image, coord[k], 1, (b, g, r), point_size)
                    sub_gray = cv2.putText(image, f"{k}", (int(coord[k][0] + 0.1 * grid_size), int(coord[k][1] + 0.2 * grid_size)),
                                            cv2.FONT_HERSHEY_SIMPLEX, font_size, (b, g, r, 200), font_width)
                    cv2.line(image, coord[k], coord[(k+1) %
                                4], (b, g, r, 200), line_width)
        image_results.append(sub_gray)
    return image_results