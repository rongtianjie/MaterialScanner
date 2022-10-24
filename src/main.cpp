#include "CalibProcess.h"
#include "config/json/json.h"


bool Recalibration(std::string& exe_input_img_path, std::string& exe_input_img_path_2, std::string& result_save_path, std::string& json_path, int focal_height){
    cv::Size board_size;
    float square_size;

    board_size.width = 11;
    board_size.height = 8;
    square_size = 5.0;

    int nimages_mid = 1;
    int nimages_right = 1;
    int nimages_ex = 1; 

    std::string exe_outut_img_path = exe_input_img_path;
    exe_outut_img_path.replace(exe_outut_img_path.size() - 3, 3, "jpg");
    std::string blank = " ";
    std::string exe_path= "dcraw_emu.exe -v -w -T -W -4 -Z " + 
                        exe_outut_img_path + blank + exe_input_img_path;
	int res1 = WinExec(exe_path.c_str(), 0);
    if(res1 < 31){
        std::cout<<"Raw to jpg failed !"<<std::endl;
        return false;
    }

    std::string exe_outut_img_path_2 = exe_input_img_path_2;
    exe_outut_img_path_2.replace(exe_outut_img_path_2.size() - 3, 3, "jpg");
    std::string exe_path_2= "dcraw_emu.exe -v -w -T -W -4 -Z " + 
                        exe_outut_img_path_2 + blank + exe_input_img_path_2;
	int res2 = WinExec(exe_path_2.c_str(), 0);
    if(res2 < 31){
        std::cout<<"Raw to jpg failed !"<<std::endl;
        return false;
    }
    
    Sleep(25000);

    cv::Size image_size;
    cv::Mat img_tmp, img_tmp_resize;
    cv::Mat img_tmp_right, img_tmp_right_resize;

    for(int i = 0; i < nimages_mid; i++){
        img_tmp = cv::imread(exe_outut_img_path , 1);
        cv::resize(img_tmp, img_tmp_resize, cv::Size(3200,2400));
        cv::imwrite(exe_outut_img_path, img_tmp_resize);
    }
    for(int i = 0; i < nimages_right; i++){
        img_tmp_right = cv::imread(exe_outut_img_path_2 , 1);
        cv::resize(img_tmp_right, img_tmp_right_resize, cv::Size(3200,2400));
        cv::imwrite(exe_outut_img_path_2, img_tmp_right_resize);
    }

    image_size = cv::Size(img_tmp_resize.cols, img_tmp_resize.rows);
    if(image_size.width != 3200 || image_size.height != 2400){
        std::cout<<"image size is wrong !"<<std::endl;
        return false;
    }

    //////////////////////////////////////stereo calibration//////////////////////////////////////////////
	
	std::cout << "Running intrinsic stereo calibration ...\n";
    cv::Mat camera_matrix[2], dist_coeffs[2];
    cv::Mat T_out;
    std::vector<std::vector<cv::Point2f>> image_points[2];
    std::vector<std::vector<cv::Point3f>> object_points;
    std::vector<cv::Mat> chessboards_total[2];

    std::string input_path_mid = exe_outut_img_path;
    std::string input_path_right = exe_outut_img_path_2;

    auto focal_height_s = std::to_string(focal_height);
    std::ofstream fout(result_save_path + "/save_" + focal_height_s + ".txt");
    fout.precision(16);
    bool intrinsic_calib_res1 = IntrinsicCalib(nimages_mid, input_path_mid, image_size, camera_matrix[0], dist_coeffs[0], 
				image_points[0], object_points, chessboards_total[0], board_size, square_size, fout);
    bool intrinsic_calib_res2 = IntrinsicCalib(nimages_right, input_path_right, image_size, camera_matrix[1], dist_coeffs[1], 
				image_points[1], object_points, chessboards_total[1], board_size, square_size, fout);

	std::cout << "Running extrinsic stereo calibration ...\n";
    std::string ret_save_path = result_save_path + "/result_" + focal_height_s + ".jpg";
	bool extrinsic_calib_res = ExtrinsicCalib(input_path_mid, input_path_right, ret_save_path, image_size,
                    camera_matrix, dist_coeffs, board_size, square_size, 
                    image_points[0], image_points[1], 
                    chessboards_total[0], chessboards_total[1], fout, T_out);
    fout.close();

    if(!intrinsic_calib_res1 || !intrinsic_calib_res2 || !extrinsic_calib_res){
        std::cout<<"intrinsic_calib_res or  extrinsic_calib_res is wrong !"<<std::endl;
        return false;
    }

    Json::Reader reader;
    Json::Value root;
    std::string json_name = json_path;
	std::fstream fin(json_name, std::ios::out);
    if(!fin.is_open()){
		return false;
    }

    {
		root["mid_mat_k"][0] = camera_matrix[0].at<double>(0, 0);
		root["mid_mat_k"][1] = camera_matrix[0].at<double>(0, 1);
		root["mid_mat_k"][2] = camera_matrix[0].at<double>(0, 2);
		root["mid_mat_k"][3] = camera_matrix[0].at<double>(1, 0);
		root["mid_mat_k"][4] = camera_matrix[0].at<double>(1, 1);
		root["mid_mat_k"][5] = camera_matrix[0].at<double>(1, 2);
		root["mid_mat_k"][6] = camera_matrix[0].at<double>(2, 0);
		root["mid_mat_k"][7] = camera_matrix[0].at<double>(2, 1);
		root["mid_mat_k"][8] = camera_matrix[0].at<double>(2, 2);

		root["mid_mat_d"][0] = dist_coeffs[0].at<double>(0, 0);
		root["mid_mat_d"][1] = dist_coeffs[0].at<double>(0, 1);
		root["mid_mat_d"][2] = dist_coeffs[0].at<double>(0, 2);
		root["mid_mat_d"][3] = dist_coeffs[0].at<double>(0, 3);
		root["mid_mat_d"][4] = dist_coeffs[0].at<double>(0, 4);

		root["side_mat_k"][0] = camera_matrix[1].at<double>(0, 0);
		root["side_mat_k"][1] = camera_matrix[1].at<double>(0, 1);
		root["side_mat_k"][2] = camera_matrix[1].at<double>(0, 2);
		root["side_mat_k"][3] = camera_matrix[1].at<double>(1, 0);
		root["side_mat_k"][4] = camera_matrix[1].at<double>(1, 1);
		root["side_mat_k"][5] = camera_matrix[1].at<double>(1, 2);
		root["side_mat_k"][6] = camera_matrix[1].at<double>(2, 0);
		root["side_mat_k"][7] = camera_matrix[1].at<double>(2, 1);
		root["side_mat_k"][8] = camera_matrix[1].at<double>(2, 2);

		root["side_mat_d"][0] = dist_coeffs[1].at<double>(0, 0);
		root["side_mat_d"][1] = dist_coeffs[1].at<double>(0, 1);
		root["side_mat_d"][2] = dist_coeffs[1].at<double>(0, 2);
		root["side_mat_d"][3] = dist_coeffs[1].at<double>(0, 3);
		root["side_mat_d"][4] = dist_coeffs[1].at<double>(0, 4);

		root["mat_rt"][0] = T_out.at<double>(0, 0);
		root["mat_rt"][1] = T_out.at<double>(0, 1);
		root["mat_rt"][2] = T_out.at<double>(0, 2);
		root["mat_rt"][3] = T_out.at<double>(0, 3);
		root["mat_rt"][4] = T_out.at<double>(1, 0);
		root["mat_rt"][5] = T_out.at<double>(1, 1);
		root["mat_rt"][6] = T_out.at<double>(1, 2);
		root["mat_rt"][7] = T_out.at<double>(1, 3);
		root["mat_rt"][8] = T_out.at<double>(2, 0);
		root["mat_rt"][9] = T_out.at<double>(2, 1);
		root["mat_rt"][10] = T_out.at<double>(2, 2);
		root["mat_rt"][11] = T_out.at<double>(2, 3);
		root["mat_rt"][12] = 0;
		root["mat_rt"][13] = 0;
		root["mat_rt"][14] = 0;
		root["mat_rt"][15] = 1;

		root["matrices_scale"] = 3.64;
    }

    Json::StyledStreamWriter writer; 
    writer.write(fin, root);
    fin.close();

    return true;
}

#if 0
void testFileReadWrite(std::string& json_path, int focal_height)
{
	Json::Reader reader;
	Json::Value root;
	std::string json_name = json_path;
	std::fstream fin(json_name, std::ios::in | std::ios::out);
	while (!fin.is_open()) {
		Sleep(500);
	}

	//fin.seekg(0);
	if (reader.parse(fin, root)) {
		Json::Value& camera_matrices_array = root["camera_matrices"];
		Json::Value& camera_matrice = camera_matrices_array[focal_height];
		root["mid_mat_k"][0] = 0;
		root["mid_mat_k"][1] = 0;
		root["mid_mat_k"][2] = 0;
		root["mid_mat_k"][3] = 0;
		root["mid_mat_k"][4] = 0;
		root["mid_mat_k"][5] = 0;
		root["mid_mat_k"][6] = 0;
		root["mid_mat_k"][7] = 0;
		root["mid_mat_k"][8] = 0;

		root["mid_mat_d"][0] = 0;
		root["mid_mat_d"][1] = 0;
		camera_matrice["mid_mat_d"][2] = 0;
		camera_matrice["mid_mat_d"][3] = 0;
		camera_matrice["mid_mat_d"][4] = 0;

		camera_matrice["side_mat_k"][0] = 0;
		camera_matrice["side_mat_k"][1] = 0;
		camera_matrice["side_mat_k"][2] = 0;
		camera_matrice["side_mat_k"][3] = 0;
		camera_matrice["side_mat_k"][4] = 0;
		camera_matrice["side_mat_k"][5] = 0;
		camera_matrice["side_mat_k"][6] = 0;
		camera_matrice["side_mat_k"][7] = 0;
		camera_matrice["side_mat_k"][8] = 0;

		camera_matrice["side_mat_d"][0] = 0;
		camera_matrice["side_mat_d"][1] = 0;
		camera_matrice["side_mat_d"][2] = 0;
		camera_matrice["side_mat_d"][3] = 0;
		camera_matrice["side_mat_d"][4] = 0;

		camera_matrice["mat_rt"][0] = 0;
		camera_matrice["mat_rt"][1] = 0;
		camera_matrice["mat_rt"][2] = 0;
		camera_matrice["mat_rt"][3] = 0;
		camera_matrice["mat_rt"][4] = 0;
		camera_matrice["mat_rt"][5] = 0;
		root["mat_rt"][6] = 0;
		root["mat_rt"][7] = 0;
		root["mat_rt"][8] = 0;
		root["mat_rt"][9] = 0;
		root["mat_rt"][10] = 0;
		root["mat_rt"][11] = 0;
		root["mat_rt"][12] = 0;
		root["mat_rt"][13] = 0;
		root["mat_rt"][14] = 0;
		root["mat_rt"][15] = 1;

		root["matrices_scale"] = 3.64;
	}

	fin.clear();
	//fin.seekg(0);
	Json::StyledStreamWriter writer;
	writer.write(fin, root);
	fin.close();
}
#endif

/*
* 参数说明：
* 1、左相机路径
* 2、右相机路径
* 3、json文件路径
* 4、高度
* 5、缓存路径
* 
* 反回值：
* 0： 成功
* -1：参数错误
* -2：其它错误
*/
int main(int argc, char* argv[])
{
    int ret = 0;
#if 0
    std::string input_path = "test/input";   //input path : should contain mid && right files !
                                             //      |_ _ _ _ mid
                                             //                |_ _ _ _ focal_height.RAF(0.RAF/1.RAF/2.RAF/3.RAF )
                                             //      |_ _ _ _ right
                                             //                |_ _ _ _ focal_height.RAF(0.RAF/1.RAF/2.RAF/3.RAF )            
    std::string result_path = "result"; //result path : save txt && jpg
    std::string json_path = "cam_120.json";         //json path : change & save json file
    int focal_height = 3;                // f = 0 or 1 or 2 or 3
#else
	if (argc != 6)
        return -1;

    auto left_image_path = std::string(argv[1]);
	auto right_image_path = std::string(argv[2]);
	auto json_path = std::string(argv[3]);
    auto result_path = std::string(argv[5]);
    int focal_height = std::atoi(argv[4]);
	
	if (left_image_path.empty() || right_image_path.empty() || result_path.empty() || 
        json_path.empty() || focal_height < 0 || focal_height > 3)
	{
		return -1;
	}
#endif

	//testFileReadWrite(json_path, focal_height);

    bool calib_result = Recalibration(left_image_path, right_image_path, result_path, json_path, focal_height);

    if(calib_result == true){
        ret = 0;
        std::cout<<" recalibration sucess !"<<std::endl;
    }
    else{
		ret = -2;
		std::cout << " recalibration failed !" << std::endl;
    }
    
    return ret;
}
