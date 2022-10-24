#include "CalibProcess.h"

//using namespace cv;
using namespace std;

void SearchNearstChessBoard(std::vector<cv::Point2f>& image_points_left_saved, 
                            vector<vector<cv::Point2f> >& image_points_ex2,
			                vector<vector<cv::Point2f> >& image_points_ex2_match)
{
	for(int i = 0; i < image_points_left_saved.size(); i++){
		cv::Point2f first_point = image_points_left_saved[i];
		float dist_x = std::pow(first_point.x - image_points_ex2[0][0].x, 2);
		float dist_y = std::pow(first_point.y - image_points_ex2[0][0].y, 2);
		float dist = std::sqrt(dist_x + dist_y);
		int predict_num = 0;
		for(int j = 1; j < image_points_ex2.size(); j++){
			float dist_x_tmp = std::pow(first_point.x - image_points_ex2[j][0].x, 2);
			float dist_y_tmp = std::pow(first_point.y - image_points_ex2[j][0].y, 2);
			float dist_tmp = std::sqrt(dist_x_tmp + dist_y_tmp);
			if(dist_tmp < dist){
				dist = dist_tmp;
				predict_num = j;
			}
		}
		int saved_num = predict_num;
		image_points_ex2_match.push_back(image_points_ex2[saved_num]);
	}

}


bool cmp(pair<float, vector<cv::Point2f>>& central_point1, pair<float, vector<cv::Point2f>>& central_point2)
{
	return central_point1.first < central_point2.first;
}


void ChessboardCornersMatch(string& result_save_path, std::vector<cv::Mat>& chessboards_total_mid, std::vector<cv::Mat>& chessboards_total_right,
						vector<vector<cv::Point2f> >& image_points_ex1, vector<vector<cv::Point2f> >& image_points_ex2,
						cv::Mat img1, cv::Mat img2, cv::Size board_size)
{
	int chessboards_left_num = image_points_ex1.size();
	int chessboards_right_num = image_points_ex2.size();
	if( chessboards_left_num != chessboards_right_num){
		std::cout<<" chessboards num not match !"<<std::endl;
		return;
	}

	std::vector<cv::Point2f> board_left;
	std::vector<cv::Point2f> board_right;
	std::vector<cv::Point2f> image_points_left;
	std::vector<cv::Point2f> image_points_right;
	std::vector<cv::Point2f> image_points_left_saved;
	std::vector<cv::Point2f> image_points_right_saved;
	for(int i = 0; i < 4; i++){
		board_left.push_back(image_points_ex1[0][0]);
		board_right.push_back(image_points_ex2[0][0]);

		board_left.push_back(image_points_ex1[0][10]);
		board_right.push_back(image_points_ex2[0][10]);

		board_left.push_back(image_points_ex1[0][77]);
		board_right.push_back(image_points_ex2[0][77]);

		board_left.push_back(image_points_ex1[0][87]);
		board_right.push_back(image_points_ex2[0][87]);
	}

	for(int i = 0; i < image_points_ex1.size();i++){
		image_points_left.push_back(image_points_ex1[i][0]);
	}
	for(int i = 0; i < image_points_ex2.size();i++){
		image_points_right.push_back(image_points_ex2[i][0]);
	}

	cv::Mat H12 = cv::findHomography(board_left, board_right);     

	cv::perspectiveTransform(image_points_left, image_points_left_saved, H12);

	std::vector<std::vector<cv::Point2f> > image_points_ex2_match;
	SearchNearstChessBoard(image_points_left_saved, image_points_ex2, image_points_ex2_match);

	int disp_width = img1.cols + img2.cols;
	int disp_height = img1.rows;
	cv::Mat disp = cv::Mat(disp_height, disp_width, CV_8UC1, cv::Scalar::all(0));
	cv::Mat roi_1 = disp(cv::Rect(0, 0, img1.cols, img1.rows));
	cv::Mat roi_2 = disp(cv::Rect(img1.cols, 0, img2.cols, img1.rows));
	img1.copyTo(roi_1);
	img2.copyTo(roi_2);

	if (disp.channels() < 3)
		cv::cvtColor(disp, disp, cv::COLOR_GRAY2BGR);

	float scale = 0.3;
	cv::Scalar s1(0, 0, 255);
	cv::Mat SmallMat;

	int cnt = 0;
	std::vector<cv::Point2f> image_points_left_tmp;
	std::vector<cv::Point2f> image_points_right_tmp;
	
	for (int i = 0; i < image_points_ex1.size(); i++)
	{
		for(int j = 0; j < image_points_ex1[0].size(); j++)
		{
			cv::line(disp, cv::Point2f(image_points_ex1[i][j].x , image_points_ex1[i][j].y ), 
				cv::Point2f(image_points_ex2_match[i][j].x + img1.cols , image_points_ex2_match[i][j].y ),
				s1, 8,	8);
			cv::resize(disp, SmallMat, cv::Size(), scale, scale);
			// cv::namedWindow("title");
			// cv::imshow("title", SmallMat);
			// cv::waitKey(0);
			if( cnt == board_size.width * board_size.height -1)
			{
				cnt = 0;
				s1 = cv::Scalar(rand()&255, rand()&255, rand()&255);
				continue;
			}
			cnt++;
		}
	}
	cv::imwrite(result_save_path + "/result.jpg", SmallMat);
	image_points_ex2.clear();
	for (int i = 0; i < image_points_ex2_match.size(); i++)
	{
		image_points_ex2.push_back(image_points_ex2_match[i]);
	}

	std::cout<<" chessboard corners match finished !"<<std::endl;
	
}

void ReorderImagePoints(ofstream& log_record, vector<vector<cv::Point2f>>& image_points_ex, 
						vector<vector<cv::Point2f>>& image_points_ex_reorder, 
						std::vector<cv::Mat>& chessboards)
{
	
	vector<pair<float, vector<cv::Point2f>>> image_points_ex_reorder_one_pic_vec;
	for(int num = 0; num < image_points_ex.size(); num++)
	{
		int width = chessboards[num].cols;
		int height = chessboards[num].rows;
	
		pair<float, vector<cv::Point2f>> image_points_ex_reorder_one_pic;

		cv::Point2f bound_point1, bound_point2, bound_point3, bound_point4, central_point;
		bound_point1=cv::Point2f(image_points_ex[num][0].x, image_points_ex[num][0].y);
		bound_point2=cv::Point2f(image_points_ex[num][width-1].x, image_points_ex[num][width-1].y);
		bound_point3=cv::Point2f(image_points_ex[num][(height-1)*width].x,
								image_points_ex[num][(height-1)*width].y);
		bound_point4=cv::Point2f(image_points_ex[num][image_points_ex[num].size()-1].x,
								image_points_ex[num][image_points_ex[num].size()-1].y);
		central_point = cv::Point2f((bound_point1.x + bound_point2.x + bound_point3.x + bound_point4.x)/4,
								(bound_point1.y + bound_point2.y + bound_point3.y + bound_point4.y)/4);
		image_points_ex_reorder_one_pic.first = central_point.x + central_point.y;

		double dist1, dist2, dist3, dist4;
		dist1 = (bound_point1.x - 0.0) * (bound_point1.x - 0.0) + (bound_point1.y - 0.0) * (bound_point1.y - 0.0);
		dist2 = (bound_point2.x - 0.0) * (bound_point2.x - 0.0) + (bound_point2.y - 0.0) * (bound_point2.y - 0.0);
		dist3 = (bound_point3.x - 0.0) * (bound_point3.x - 0.0) + (bound_point3.y - 0.0) * (bound_point3.y - 0.0);
		dist4 = (bound_point4.x - 0.0) * (bound_point4.x - 0.0) + (bound_point4.y - 0.0) * (bound_point4.y - 0.0);
		double init_dist1 = dist1 < dist2 ? dist1 : dist2;
		double init_dist2 = dist3 < dist4 ? dist3 : dist4;
		double init_dist = init_dist1 < init_dist2 ? init_dist1 : init_dist2;
		
		double candidateDirection, candidateDirection1, candidateDirection2;
		if(init_dist == dist1)
		{
			candidateDirection1 = abs(bound_point2.y - bound_point1.y);
			candidateDirection2 = abs(bound_point3.y - bound_point1.y);
			candidateDirection = candidateDirection1 < candidateDirection2 ? candidateDirection1 : candidateDirection2;
			if(candidateDirection == candidateDirection1)
			{
				for(int i = 0; i < height; i++){
					for(int j = 0; j < width; j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x =image_points_ex[num][i*width+j].x;
						point_tmp.y =image_points_ex[num][i*width+j].y;
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
					}
					log_record<<endl;
				}
					
			}
			else
			{
				for(int i = 0; i < width; i++){
					for(int j = 0; j < height; j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][j*width+i].x;
						point_tmp.y = image_points_ex[num][j*width+i].y;
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
					}
					log_record<<endl;
				}
				 
			}
		}
		else if(init_dist == dist2)
		{
			candidateDirection1 = abs(bound_point1.y - bound_point2.y);
			candidateDirection2 = abs(bound_point3.y - bound_point2.y);
			candidateDirection = candidateDirection1 < candidateDirection2 ? candidateDirection1 : candidateDirection2;
			if(candidateDirection == candidateDirection1)
			{
				for(int i = 0;i < height;i++){
					for(int j = 0;j < width;j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][i*width+height-1-j].x;
						point_tmp.y = image_points_ex[num][i*width+height-1-j].y;
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
					}
					log_record<<endl; 
				}
				 
			}
			else
			{
				for(int i = 0;i < width; i++){
					for(int j = 0;j < height; j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][(width-i-1)+j*width].x;
						point_tmp.y = image_points_ex[num][(width-i-1)+j*width].y;
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
					}
					log_record<<endl;
				}
				
			}
		}
		else if(init_dist == dist3)
		{
			candidateDirection1 = abs(bound_point1.y-bound_point3.y);
			candidateDirection2 = abs(bound_point4.y-bound_point3.y);
			candidateDirection = candidateDirection1 < candidateDirection2 ? candidateDirection1 : candidateDirection2;
			if(candidateDirection == candidateDirection1)
			{
				for(int i=0;i<width;i++){
					for(int j=0;j<height;j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][(height-j-1)*width+i].x; 
						point_tmp.y = image_points_ex[num][(height-j-1)*width+i].y;
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
					}
					log_record<<endl;
				}
				
			}
			else
			{
				for(int i=0;i<height;i++){
					for(int j=0;j<width;j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][(height-i-1)*width+j].x;
						point_tmp.y = image_points_ex[num][(height-i-1)*width+j].y;
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
					}
					log_record<<endl;
				}
				
			}  
		}
		else
		{
			candidateDirection1=abs(bound_point2.y-bound_point4.y);
			candidateDirection2=abs(bound_point3.y-bound_point4.y);
			candidateDirection=candidateDirection1<candidateDirection2?candidateDirection1:candidateDirection2;
			if(candidateDirection==candidateDirection1)
			{
				for(int i=0;i<width;i++){
					for(int j=0;j<height;j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][(height-1-j)*width+width-1-i].x;
						point_tmp.y = image_points_ex[num][(height-1-j)*width+width-1-i].y;
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
					}
					log_record<<endl;
				}
				
			}
			else
			{
				for(int i=0;i<height;i++){
					for(int j=0;j<width;j++)
					{
						cv::Point2f point_tmp;
						point_tmp.x = image_points_ex[num][(height-1-i)*width+width-1-j].x;
						point_tmp.y = image_points_ex[num][(height-1-i)*width+width-1-j].y;
						log_record<<"( "<<point_tmp.x<<", "<<point_tmp.y<<" )"<<" ";
						image_points_ex_reorder_one_pic.second.push_back(point_tmp);
					}
					log_record<<endl;
				}
				
			}   
		}
		log_record<<endl<<endl;
		image_points_ex_reorder_one_pic_vec.push_back(image_points_ex_reorder_one_pic);
	}
    sort(image_points_ex_reorder_one_pic_vec.begin(), image_points_ex_reorder_one_pic_vec.end(), cmp);

	for(int i = 0; i < image_points_ex_reorder_one_pic_vec.size(); i++){
		image_points_ex_reorder.push_back(image_points_ex_reorder_one_pic_vec[i].second);
	}

}

void IntrinsicCalib(int nimages, string& img_path, cv::Size image_size, cv::Mat& camera_matrix, cv::Mat& dist_coeffs,
                    vector<vector<cv::Point2f> >& image_points, vector<vector<cv::Point3f> >& object_points,
					std::vector<cv::Mat>& chessboards_total, cv::Size board_size, float square_size, std::ofstream& fout)
{	
	image_points.clear();
	object_points.clear();
	std::vector<cv::Mat> chessboards_one_pic;

	for( int i = 0; i < nimages; i++ ){
		chessboards_one_pic.clear();

		string filename;
		filename = img_path + std::to_string(i)+".jpg";
		cv::Mat img = cv::imread(filename, 0);
		if(img.empty()){
			filename = img_path + std::to_string(i)+".JPG";
			img = cv::imread(filename, 0);
        }
		cout << "start calib " << i<<" images"<<endl;
		double start_time = (double)cv::getCPUTickCount();
		
		CornerDetAC corner_detector(img);
		ChessboradStruct chessboardstruct;

		Corners corners_s;
		corner_detector.detectCorners(img, corners_s, 0.01);

		double cost_time = ((double)cv::getCPUTickCount() - start_time) / cv::getTickFrequency();
		std::cout << "find corner time cost :" << cost_time << std::endl;
		
		chessboardstruct.chessboardsFromCorners(corners_s, chessboards_one_pic, 0.6);
		chessboardstruct.drawchessboard(img, corners_s, image_points, chessboards_one_pic);

		for(int num = 0; num < chessboards_one_pic.size(); num++){
			chessboards_total.push_back(chessboards_one_pic[num]);
		}
	}

    for (int t = 0; t < chessboards_total.size(); t++)
    {
        vector<cv::Point3f> object_points_tmp;
        for (int i = 0; i < chessboards_total[t].rows; i++)
        {
            for (int j = 0; j < chessboards_total[t].cols; j++)
            {
                cv::Point3f real_point;
               
                real_point.x = i * square_size;
                real_point.y = j * square_size;
                real_point.z = 0;
                object_points_tmp.push_back(real_point);
            }
        }
        object_points.push_back(object_points_tmp);
    }

	int flags = 0;
	//flags |= CALIB_FIX_K3;
    flags |= cv::CALIB_FIX_K4; 
    flags |= cv::CALIB_FIX_K5;
	vector<cv::Mat> rvecsMat;                                         
	vector<cv::Mat> tvecsMat;                                         

	double err_first = calibrateCamera(object_points, image_points, image_size, camera_matrix, dist_coeffs, 
							rvecsMat, tvecsMat, flags);
	fout << "reprojection error: " << err_first << " pixel" << endl << endl;
	cout << "save intrinsic calib result ..."<<endl<< endl << endl;
	fout << "mat_k:" << endl;
	fout << camera_matrix.at<double>(0, 0)<<", "<< camera_matrix.at<double>(0, 1)<<", "<<camera_matrix.at<double>(0, 2)<<","<<endl
		<< camera_matrix.at<double>(1, 0)<<", "<< camera_matrix.at<double>(1, 1)<<", "<<camera_matrix.at<double>(1, 2)<<","<<endl
		<< camera_matrix.at<double>(2, 0)<<", "<< camera_matrix.at<double>(2, 1)<<", "<<camera_matrix.at<double>(2, 2)<<endl;
	fout << "mat_d:"<<endl;
	fout << dist_coeffs.at<double>(0, 0)<<", "<<dist_coeffs.at<double>(0, 1)<<", "<<dist_coeffs.at<double>(0, 2) <<", "
		<< dist_coeffs.at<double>(0, 3)<<", "<<dist_coeffs.at<double>(0, 4)<< endl<< endl;

}

void ExtrinsicCalib(string& path_mixed, string& result_save_path, cv::Size image_size, cv::Mat *camera_matrix, cv::Mat *dist_coeffs,
				cv::Size board_size, float square_size, vector<vector<cv::Point2f> >& image_points_mid,
				vector<vector<cv::Point2f> >& image_points_right, std::vector<cv::Mat>& chessboards_total_mid, 
				std::vector<cv::Mat>& chessboards_total_right, std::ofstream& fout, cv::Mat& T_out)
{
	vector<vector<cv::Point2f> > image_points_ex_reorder[2];
	vector<vector<cv::Point3f> > object_points_ex;

	ofstream log_record_1("log_record_1.txt");
	ofstream log_record_2("log_record_2.txt");
	ReorderImagePoints(log_record_1, image_points_mid, image_points_ex_reorder[0], chessboards_total_mid);
	ReorderImagePoints(log_record_2, image_points_right, image_points_ex_reorder[1], chessboards_total_right);
	log_record_1.close();
	log_record_2.close();

	cv::Mat img_s1 = cv::imread(path_mixed + "/mid/0.jpg",0);
	if(img_s1.empty()){
			img_s1 = cv::imread(path_mixed + "/mid/0.JPG",0);
    }
	cv::Mat img_s2 = cv::imread(path_mixed + "/right/0.jpg",0);
	if(img_s2.empty()){
			img_s2 = cv::imread(path_mixed + "/right/0.JPG",0);
    }

	ChessboardCornersMatch(result_save_path, chessboards_total_mid, chessboards_total_right, 
						  image_points_ex_reorder[0], image_points_ex_reorder[1], img_s1, img_s2, board_size);

	for (int t = 0; t < chessboards_total_mid.size(); t++)
    {
        vector<cv::Point3f> object_points;
        for (int i = 0; i < board_size.height; i++)
        {
            for (int j = 0; j <board_size.width; j++)
            {
                cv::Point3f real_point;
                real_point.x = i * square_size;
                real_point.y = j * square_size;
                real_point.z = 0;
                object_points.push_back(real_point);
            }
        }
        object_points_ex.push_back(object_points);
    }

	cv::Mat R;
	cv::Mat T;
	cv::Mat E, F;
	double rms = stereoCalibrate(object_points_ex, image_points_ex_reorder[0], image_points_ex_reorder[1],
                    camera_matrix[0], dist_coeffs[0],
                    camera_matrix[1], dist_coeffs[1],
                    image_size, R, T, E, F,
                    // CALIB_FIX_ASPECT_RATIO +
                    // CALIB_ZERO_TANGENT_DIST +
                    //CALIB_USE_INTRINSIC_GUESS + 
					//CALIB_USE_EXTRINSIC_GUESS+
                    // CALIB_SAME_FOCAL_LENGTH +
                    // CALIB_RATIONAL_MODEL+
                    cv::CALIB_FIX_K4 + cv::CALIB_FIX_K5+
                    cv::CALIB_FIX_INTRINSIC,
                    cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 100, 1e-5) 
                    );
    cout << "done with RMS error=" << rms << endl;
	if(rms > 5){
		cout<< endl<< endl;
		cout << "wrong !!!!!!!!!!!!" <<endl;
		return;
	}
	fout << "mat_rt:" << endl;
	T_out = (cv::Mat_<double>(4,4)<< R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), T.at<double>(0, 0),
				R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), T.at<double>(1, 0),
				R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), T.at<double>(2, 0),
				0, 0, 0, 1);
	fout << R.at<double>(0, 0)<<", "<<R.at<double>(0, 1)<<", "<<R.at<double>(0, 2)<<", "<<T.at<double>(0, 0)<<","<< endl
		<< R.at<double>(1, 0)<<", "<<R.at<double>(1, 1)<<", "<<R.at<double>(1, 2)<<", "<<T.at<double>(1, 0)<<","<< endl
		<< R.at<double>(2, 0)<<", "<<R.at<double>(2, 1)<<", "<<R.at<double>(2, 2)<<", "<<T.at<double>(2, 0)<<","<< endl
		<< 0 <<", "<< 0 <<", "<< 0 <<", "<< 1 <<endl;
	
}

void UndistortImages(int nimages_un, string& path_undist, cv::Mat *camera_matrix, cv::Mat *dist_coeffs)
{
	for(int i = 0; i < nimages_un; i++ ){
        for(int k = 0; k < 2; k++ ){
            std::cout<<"undistort "<<i<<" image"<<std::endl;
            cv::Mat img_un;
            if(k == 0){
                img_un = cv::imread(path_undist +"/mid/"+std::to_string(i)+".jpg", 1);
                cv::Mat undist;
                undistort(img_un, undist, camera_matrix[0], dist_coeffs[0]);
                imwrite(path_undist + "/mid/" + "undist_mid_"+std::to_string(i)+".jpg", undist); 
            }
            else{
                img_un = cv::imread(path_undist + "/right/"+std::to_string(i)+".jpg", 1);
                cv::Mat undist;
                undistort(img_un, undist, camera_matrix[1], dist_coeffs[1]);
                imwrite(path_undist + "/right/" + "undist_right_"+std::to_string(i)+".jpg", undist);
            }
        }
    }
}