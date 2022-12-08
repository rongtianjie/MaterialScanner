# cython:language_level=3
import psutil
from base.common import *


def read_conf(input_path, lens, scale, shoot_type, focus_id):
    if str(lens) == "30":
        conf_path = "conf/fuji_30.json"
        cam_conf_path = os.path.join(input_path, "cam_30.json")
        grayboard_data_path = "pre_data/grayboard_result_30.npz"
    if str(lens) == "50":
        conf_path = "conf/fuji_50.json"
        grayboard_data_path = "pre_data/grayboard_result_50.npz"
    elif str(lens) == "120":
        conf_path = "conf/fuji_120.json"
        cam_conf_path = os.path.join(input_path, "cam_120.json")
        grayboard_data_path = "pre_data/grayboard_result_120.npz"

    with open(conf_path) as f:
        raw = f.read()
    conf = json.loads(raw)
    conf["lens"] = lens

    # Mono scan mode support
    if shoot_type == "mono":
        conf["geometry"]['rot_count'] = 8

    if lens == "120":
        if os.path.exists(cam_conf_path):
            with open(cam_conf_path) as f:
                raw = f.read()
                logger.info(f"Camera config file loaded.")
            cam_conf = json.loads(raw)
        else:
            exit_with_error(f"Camera config file not found: {cam_conf_path}")

        camera_matrices = cam_conf["camera_matrices"][focus_id]
        logger.info(f"Load camera matrix [{focus_id}]")
        # convert matrices
        mid_mat_k = np.array(camera_matrices['mid_mat_k']).reshape(3, 3)
        mid_mat_dist = np.array(camera_matrices['mid_mat_d']).reshape(5, 1)

        mid_mat_k[:2] *= camera_matrices['matrices_scale']
        mid_mat_k[:2] /= scale

        matrices_np = {}
        matrices_np['mid_mat_k'] = mid_mat_k
        matrices_np['mid_mat_d'] = mid_mat_dist

        if shoot_type == "stereo":
            right_mat_k = np.array(camera_matrices['side_mat_k']).reshape(3, 3)
            right_mat_dist = np.array(camera_matrices['side_mat_d']).reshape(5, 1)
            mat_rt = np.array(camera_matrices['mat_rt']).reshape(4, 4)

            right_mat_k[:2] *= camera_matrices['matrices_scale']
            right_mat_k[:2] /= scale

            matrices_np['side_mat_k'] = right_mat_k
            matrices_np['side_mat_d'] = right_mat_dist
            matrices_np['mat_rt'] = mat_rt

        elif shoot_type == "mono":
            matrices_np['side_mat_k'] = None
            matrices_np['side_mat_d'] = None
            matrices_np['mat_rt'] = None

        conf["camera_matrices"] = matrices_np
    elif lens == "50":
        mid_mat_k = np.array(conf["camera_matrices"]["mid_mat_k"]).reshape(3, 3)
        mid_mat_k[:2] *= 3.64
        mid_mat_k[:2] /= scale
        conf["camera_matrices"]["mid_mat_k"] = mid_mat_k

    conf["scale"] = scale
    conf["grayboard_data_path"] = grayboard_data_path

    # settings
    conf_path = "conf/settings.json"
    with open(conf_path) as f:
        raw = f.read()
    settings_conf = json.loads(raw)

    if settings_conf["dynamic_process_num"]:
        mem_available = psutil.virtual_memory().available
        if mem_available > 240*1024**3:
            settings_conf["multi_process_num"] = settings_conf["process_profile"]["256G"]
        elif mem_available > 110*1024**3:
            settings_conf["multi_process_num"] = settings_conf["process_profile"]["128G"]
        else:
            settings_conf["multi_process_num"] = settings_conf["process_profile"]["default"]
    else:
        settings_conf["multi_process_num"] = settings_conf["process_profile"]["default"]

    # logger.info(f"Process number: {settings_conf['multi_process_num']}")
    
    conf["settings"] = settings_conf

    return conf