import os

def check():
    # ext
    file_list = ["ACMMP.exe", "exiftool.exe", "ma_stereo.dll", "opencv_world454d.dll"]
    copy("ext", file_list, r"\\10.0.0.3\ai\MaterialScanner\0.1.0\ext")

    # pre data
    file_list = ["grayboard_result_30.npz", "grayboard_result_50.npz", "grayboard_result_120.npz"]
    copy("pre_data", file_list, r"\\10.0.0.3\ai\MaterialScanner\0.1.0\pre_data")

def copy(check_folder, filelist, copy_from_folder):
    if not os.path.exists(check_folder):
        print(f"Folder {check_folder} not exists, create.")
        os.makedirs(check_folder, exist_ok=True)
    for filename in filelist:
        local_file = os.path.normpath(os.path.join(check_folder, filename))
        if not os.path.exists(local_file):
            remote_file = os.path.normpath(os.path.join(copy_from_folder, filename))
            if os.path.exists(remote_file):
                print(f"Copy [{remote_file}]  to  [{local_file}]")
                command = f"copy {remote_file} {local_file}"
                os.popen(command)
            else:
                print(f"File [{remote_file}] dose not exists, can not copy.")

if __name__ == "__main__":
    check()