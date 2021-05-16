#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/aruco.hpp>
#include <vector>

int main(void) {

	//The code to generate the aruco markers.You can uncomment these codes and comment all the other codes to generate.
	/*cv::Mat markerImage;
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
	cv::aruco::drawMarker(dictionary, 138, 200, markerImage, 1);//the second number is the ID of the marker in the dictionary
	cv::imwrite("marker138.png", markerImage);*/


	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	//read in the calibration results
	cv::Mat cameraMatrix, distCoeffs;
	cv::FileStorage file_storage("out_camera_data.xml", cv::FileStorage::READ);
	file_storage["camera_matrix"] >> cameraMatrix;
	std::cout << cameraMatrix << std::endl;
	file_storage["distortion_coefficients"] >> distCoeffs;
	std::cout << distCoeffs << std::endl;
	file_storage.release();

	cv::VideoCapture inputVideo;
	inputVideo.open(0);
	cv::namedWindow("detection_result", CV_WINDOW_NORMAL);

	while (inputVideo.grab()) {
		cv::Mat image, imageCopy;
		inputVideo.retrieve(image);
		image.copyTo(imageCopy);
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f> > corners;

		cv::aruco::detectMarkers(image, dictionary, corners, ids);
		// if at least one marker detected
		if (ids.size() > 0)
			cv::aruco::drawDetectedMarkers(imageCopy, corners, ids, cv::Scalar(0, 255, 255));

		std::vector<cv::Vec3d> rvecs, tvecs;
		cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);
		// draw axis for each marker
		for (int i = 0; i < ids.size(); i++)
			cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);

		cv::imshow("detection_result", imageCopy);
		char key = (char)cv::waitKey(50);
		if (key == 27)
			break;
	}


	return 0;
}