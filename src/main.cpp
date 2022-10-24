#include "CalibProcess.h"
#include "config/json/json.h"

using namespace std;

int main(int argc, char* argv[])
{

    Json::Reader reader;
    Json::Value root;
	//std::string json_name = argv[1];
    std::string json_name = "120_stereo.json";
    std::ifstream fin(json_name);
    if(!fin.is_open()){
        std::cout<<"open json failed !"<<std::endl;
        return -1;
    }
    std::string input_path;
    //std::string undistort_path;
    std::string result_save_path;
    cv::Size board_size;
    float square_size;

    int nimages_mid = 1;
    int nimages_right = 1;
    int nimages_ex = 1; 
    // int nimages_un = 16;

    if(reader.parse(fin,root)){
        input_path = root["path"]["input_path"].asString();
        //undistort_path = root["path"]["undistort_path"].asString();
        result_save_path = root["path"]["result_save_path"].asString();
        board_size.width = root["chessboard"]["board_width"].asInt();
        board_size.height = root["chessboard"]["board_height"].asInt();
        square_size = root["chessboard"]["board_size"].asFloat();
    }

    #ifdef _WIN32
		mkdir(result_save_path.c_str());
	#else
		mkdir(result_save_path.c_str(), 0777);
	#endif
    
    // std::string input_path_mid_jpg = input_path + "/mid/*.*";
    // std::string input_path_right_jpg = input_path + "/right/*.*";
    // std::vector<std::string> str_vec_input_mid;
    // std::vector<std::string> str_vec_input_right;
    // cv::glob(input_path_mid_jpg, str_vec_input_mid, false);
    // cv::glob(input_path_right_jpg, str_vec_input_right, false);
    // if(str_vec_input_mid.size() != nimages_mid || str_vec_input_right.size() != nimages_right){
    //     std::cout<<"image's num is wrong !"<<std::endl;
    //     return -1;
    // }

    std::string exe_input_img_path = input_path + "/mid/0.RAF";
    std::string exe_outut_img_path = input_path + "/mid/0.jpg";
    std::string blank = " ";
    std::string exe_path= "dcraw_emu.exe -v -w -T -W -4 -Z " + 
                        exe_outut_img_path + blank + exe_input_img_path;
	int res1 = WinExec(exe_path.c_str(),0);

    std::string exe_input_img_path_2 = input_path + "/right/0.RAF";
    std::string exe_outut_img_path_2 = input_path + "/right/0.jpg";
    std::string exe_path_2= "dcraw_emu.exe -v -w -T -W -4 -Z " + 
                        exe_outut_img_path_2 + blank + exe_input_img_path_2;
	int res2 = WinExec(exe_path_2.c_str(),0);
    
    Sleep(25000);

    cv::Size image_size;
    cv::Mat img_tmp, img_tmp_resize;
    cv::Mat img_tmp_right, img_tmp_right_resize;

    for(int i = 0; i < nimages_mid; i++){
        //std::string path_in_left = input_path + cv::format("/mid/%d.jpg", i);
        img_tmp = cv::imread(exe_outut_img_path , 1);
        cv::resize(img_tmp, img_tmp_resize, cv::Size(3200,2400));
        cv::imwrite(exe_outut_img_path, img_tmp_resize);
    }
    for(int i = 0; i < nimages_right; i++){
        //std::string path_in_right = input_path + cv::format("/right/%d.jpg", i);
        img_tmp_right = cv::imread(exe_outut_img_path_2 , 1);
        cv::resize(img_tmp_right, img_tmp_right_resize, cv::Size(3200,2400));
        cv::imwrite(exe_outut_img_path_2, img_tmp_right_resize);
    }

    image_size = cv::Size(img_tmp_resize.cols, img_tmp_resize.rows);
    if(image_size.width != 3200 || image_size.height != 2400){
        std::cout<<"image size is wrong !"<<std::endl;
        return -1;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
	
	std::cout << "Running intrinsic stereo calibration ...\n";
    cv::Mat camera_matrix[2], dist_coeffs[2];
    cv::Mat T_out;
    vector<vector<cv::Point2f> > image_points[2];
    vector<vector<cv::Point3f> > object_points;
    std::vector<cv::Mat> chessboards_total[2];

    std::string input_path_mid = input_path + "/mid/";
    std::string input_path_right = input_path + "/right/";
    std::ofstream fout(result_save_path + "/save.txt");
    fout.precision(16);
    IntrinsicCalib(nimages_mid, input_path_mid, image_size, camera_matrix[0], dist_coeffs[0], 
				image_points[0], object_points, chessboards_total[0], board_size, square_size, fout);
    IntrinsicCalib(nimages_right, input_path_right, image_size, camera_matrix[1], dist_coeffs[1], 
				image_points[1], object_points, chessboards_total[1], board_size, square_size, fout);

	std::cout << "Running extrinsic stereo calibration ...\n";
	ExtrinsicCalib(input_path, result_save_path, image_size, camera_matrix, dist_coeffs, board_size, 
                    square_size, image_points[0], image_points[1], 
                    chessboards_total[0], chessboards_total[1], fout, T_out);
    fout.close();

    Json::Value root_out;
	Json::Value friends;
    
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            friends["mid_mat_k"].append(camera_matrix[0].at<double>(i,j));
            friends["side_mat_k"].append(camera_matrix[1].at<double>(i,j));
        }
    }
	for(int i = 0; i < 5; i++){
         friends["mid_mat_d"].append(dist_coeffs[0].at<double>(0,i));
         friends["side_mat_d"].append(dist_coeffs[1].at<double>(0,i));
    }
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            friends["mat_rt"].append(T_out.at<double>(i,j));
        }
    }
    friends["matrices_scale"] = Json::Value(3.64);
 
	root_out["pre_camera_matrices"] = Json::Value(friends);
	Json::StyledWriter sw; 
	std::ofstream json_out(result_save_path + "/save.json");
    json_out.precision(8);
	json_out << sw.write(root_out);
	json_out.close();

	// std::cout << "Running undistort ...\n";
	// UndistortImages(nimages_un, path_undist, camera_matrix, dist_coeffs);

    std::cout<<"calib finished !"<<std::endl;
    return 0;
}
