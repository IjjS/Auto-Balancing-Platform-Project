#include <opencv2/opencv.hpp>
#include <iostream>
#include "serialcomm.h"
#include <chrono>

using namespace std::chrono;

/* HSV filter variables
const int max_value_H = 360 / 2;
const int max_value = 255;
const std::string window_detection_name = "Object Detection";
int low_H = 0, low_S = 0, low_V = 0;
int high_H = max_value_H, high_S = max_value, high_V = max_value;


static void on_low_H_thresh_trackbar(int, void*)
{
	low_H = min(high_H - 1, low_H);
	setTrackbarPos("Low H", window_detection_name, low_H);
}
static void on_high_H_thresh_trackbar(int, void*)
{
	high_H = max(high_H, low_H + 1);
	setTrackbarPos("High H", window_detection_name, high_H);
}
static void on_low_S_thresh_trackbar(int, void*)
{
	low_S = min(high_S - 1, low_S);
	setTrackbarPos("Low S", window_detection_name, low_S);
}
static void on_high_S_thresh_trackbar(int, void*)
{
	high_S = max(high_S, low_S + 1);
	setTrackbarPos("High S", window_detection_name, high_S);
}
static void on_low_V_thresh_trackbar(int, void*)
{
	low_V = min(high_V - 1, low_V);
	setTrackbarPos("Low V", window_detection_name, low_V);
}
static void on_high_V_thresh_trackbar(int, void*)
{
	high_V = max(high_V, low_V + 1);
	setTrackbarPos("High V", window_detection_name, high_V);
}
*/

CSerialComm serial_comm;

bool validate_camera(bool isOpened) {
	if (!isOpened) {
		std::cout << "Cannot connect to the camera." << std::endl;
		std::cout << "Please press any key to quit." << std::endl;
		std::cin.get();
		return false;
	}

	return true;
}

int main(int ac, char** av) {
	// connect to communication port first
	char comm_port_init[] = "COM7";
	char* comm_port = comm_port_init;

	if (!serial_comm.connect(comm_port)) {
		std::cout << "Serial Communication Failed" << std::endl;
		return -1;
	}
	else {
		std::cout << "Serial Communication Connected to " << comm_port << std::endl;
		std::cout << "Please press any key to continue" << std::endl;
		std::cin.get();
	}
	
	//data collection
	//std::vector<int> x_positions, y_positions, time;

	//start camera
	cv::VideoCapture cap(1);
	uint64_t start_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

	if (!validate_camera(cap.isOpened())) {
		return -1;
	}

	/* Trackbars for HSV filter
	namedWindow(window_detection_name);
	createTrackbar("Low H", window_detection_name, &low_H, max_value_H, on_low_H_thresh_trackbar);
	createTrackbar("High H", window_detection_name, &high_H, max_value_H, on_high_H_thresh_trackbar);
	createTrackbar("Low S", window_detection_name, &low_S, max_value, on_low_S_thresh_trackbar);
	createTrackbar("High S", window_detection_name, &high_S, max_value, on_high_S_thresh_trackbar);
	createTrackbar("Low V", window_detection_name, &low_V, max_value, on_low_V_thresh_trackbar);
	createTrackbar("High V", window_detection_name, &high_V, max_value, on_high_V_thresh_trackbar);
	*/

	const cv::Scalar lower_bound = cv::Scalar(21, 0, 154);
	const cv::Scalar upper_bound = cv::Scalar(119, 104, 255);
	const int rotation_angle = 45;

	std::string original_window_name = "Original Frame";
	std::string rotated_window_name = "HSV Frame";
	std::string binary_window_name = "Binary Frame";

	while (true) {
		cv::Mat frame;
		cv::Mat binary_frame;
		cv::Mat rotated_frame;
		cv::Mat hsv;
		bool b_success = cap.read(frame);

		if (!b_success) {
			std::cout << "Cannot read a frame from your camera" << std::endl;
			std::cin.get();
			return -1;
		}

		//cut the frame to sqaure 480x480
		frame = frame(cv::Range(22, 432), cv::Range(128, 538));

		//rotate the frame
		cv::Point rotation_center = cv::Point(frame.cols / 2, frame.rows / 2);
		cv::Mat rotation_matrix = getRotationMatrix2D(rotation_center, rotation_angle, 0.7);
		warpAffine(frame, rotated_frame, rotation_matrix, frame.size());

		//find circule from hsv converted frame
		cvtColor(rotated_frame, hsv, cv::COLOR_BGR2HSV);

		//inRange(hsv, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), binary_frame);
		inRange(hsv, lower_bound, upper_bound, binary_frame);

		//reduce noise
		blur(binary_frame, binary_frame, cv::Size(2, 6));
		erode(binary_frame, binary_frame, cv::Mat(), cv::Point(-1, -1), 5);
		dilate(binary_frame, binary_frame, cv::Mat(), cv::Point(-1, -1), 5);

		//clone to keep original binary_frame
		cv::Mat contour_input = binary_frame.clone();
		std::vector<std::vector<cv::Point>> contour_output;

		findContours(contour_input, contour_output, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		cv::Point2f center;
		float radius;

		for (size_t index=0; index < contour_output.size(); index++) {
			double area = contourArea(contour_output[index]);
			
			if (area > 700) {
				minEnclosingCircle(contour_output[index], center, radius);
				circle(rotated_frame, center, 3, cv::Scalar(0, 0, 255), cv::FILLED);
				circle(binary_frame, center, radius, cv::Scalar(255, 255, 255), cv::FILLED);

				uint64_t detection_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
				std::cout << "Position detected after " << detection_time - start_time << " milliseconds \nfrom the start of the application." << std::endl;


				//string for transimitting to Arduino Mega
				std::string x = std::to_string((int)center.x - 210) + 'x';
				std::string y = std::to_string(210 - (int)center.y) + 'y';

				std::cout << "Center: x = " << x << " ";
				std::cout << " y = " << y << std::endl;

				std::string center_data = x + y + '\n'; //new line character as a terminator

				//data transmission
				for (int i = 0; i < center_data.size(); i++) {
					if (!serial_comm.sendCommand(center_data[i])) {
						std::cout << "Failed to send " << center_data[i] << std::endl;
					}
					if (center_data[i] == 'x') {
						std::cout << "x position " << x << " is successfully sent to " << comm_port << std::endl;
					}
					else if (center_data[i] == 'y') {
						std::cout << "y position " << y << " is successfully sent to " << comm_port << std::endl << std::endl;
					}
				}

				/* data collection
				x_positions.push_back((int)center.x - 210);
				y_positions.push_back(210 - (int)center.y);
				time.push_back(detection_time - start_time);
				*/
			}
		}

		imshow(original_window_name, rotated_frame);
		imshow(rotated_window_name, hsv);
		imshow(binary_window_name, binary_frame);

		if (cv::waitKey(30) == 27) {
			std::cout << "Close";
			break;
		}
	}

	/* print data transmitted
	std::cout << "x positions -----" << std::endl;
	for (int i = 0; i < x_positions.size(); i++) {
		std::cout << x_positions[i] << std::endl;
	}

	std::cout << "y positions -----" << std::endl;
	for (int i = 0; i < y_positions.size(); i++) {
		std::cout << y_positions[i] << std::endl;
	}

	std::cout << "time -------------" << std::endl;
	for (int i = 0; i < time.size(); i++) {
		std::cout << time[i] << std::endl;
	}
	*/
}