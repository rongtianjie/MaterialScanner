#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <algorithm>
#include <math.h>

#ifdef _WIN32
#include "Windows.h"
#include <shellapi.h>
#include <direct.h>
#include <io.h>
#else  // Linux/Unix
#include <sys/io.h>
#include <unistd.h>
#include <sys/stat.h>  // mkdir
#include <sys/types.h>
#include <dirent.h>
#endif

#include "opencv2/opencv.hpp"

#include "CornerDetAC.h"
#include "ChessboradStruct.h"

using namespace std;

void SearchNearstChessBoard(std::vector<cv::Point2f>& image_points_left_saved, 
                            std::vector<std::vector<cv::Point2f> >& image_points_ex2,
			    std::vector<std::vector<cv::Point2f> >& image_points_ex2_match);

bool cmp(std::pair<float, std::vector<cv::Point2f>>& central_point1, std::pair<float, std::vector<cv::Point2f>>& central_point2);

void ChessboardCornersMatch(string& result_save_path,
                            std::vector<cv::Mat>& chessboards_total_left, 
                            std::vector<cv::Mat>& chessboards_total_right,
			    std::vector<std::vector<cv::Point2f> >& image_points_ex1, 
                            std::vector<std::vector<cv::Point2f> >& image_points_ex2,
			    cv::Mat img1, cv::Mat img2, cv::Size board_size);

void ReorderImagePoints(std::ofstream& log_record, std::vector<std::vector<cv::Point2f>>& image_points_ex, 
		        std::vector<std::vector<cv::Point2f>>& image_points_ex_reorder, 
			std::vector<cv::Mat>& chessboards);

void IntrinsicCalib(int nimages, string& img_path, cv::Size image_size, cv::Mat& camera_matrix, cv::Mat& dist_coeffs,
                    vector<vector<cv::Point2f> >& image_points, vector<vector<cv::Point3f> >& object_points,
                    std::vector<cv::Mat>& chessboards_total, cv::Size board_size, float square_size, std::ofstream& fout);

void ExtrinsicCalib(string& path_mixed, string& result_save_path, cv::Size image_size, cv::Mat *camera_matrix, cv::Mat *dist_coeffs,
                        cv::Size board_size, float square_size, vector<vector<cv::Point2f> >& image_points_mid,
                        vector<vector<cv::Point2f> >& image_points_right, std::vector<cv::Mat>& chessboards_total_mid, 
                        std::vector<cv::Mat>& chessboards_total_right, std::ofstream& fout, cv::Mat& T_out);

void UndistortImages(int nimages_un, std::string& path_undist, cv::Mat *camera_matrix, cv::Mat *dist_coeffs);


