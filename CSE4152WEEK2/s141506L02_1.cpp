#include <iostream>
#include <string>
#include <time.h>// 시간 측정 위해서
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;
void averageFiltering_opencvCommand(int value, void *userData);//flag에 따라 cv함수, user-defined함수 중 선택해서 실행해야 하는데 그걸 콜백에서 처리? 아니면 서로 다른 콜백?
void userdefined_blur(InputArray src, OutputArray dst, Size ksize, Point anchor,int BorderType);

Mat input_im,output_dst;
clock_t start_time, end_time;
string type2str(int type);
int flag;

void averageFiltering_opencvCommand(int value, void *userData) {

	string &win_name = *(static_cast<string*>(userData));

	if (value <= 0) {
		cout << "Kernel size cannot be smaller than 0" <<endl;
		return;
	}

	Size ksize = Size(value, value);

	start_time = clock();
	if (flag == 1)
		blur(input_im, output_dst, ksize,Point(-1,-1), BORDER_CONSTANT);

	else if (flag == 2) {
		userdefined_blur(input_im, output_dst, ksize, Point(-1,-1), BORDER_CONSTANT);
	}
	end_time = clock();
	cout << "kernel value : " << value << ", Exec Time : " << (double)(end_time - start_time) << " (msec)" << endl;

	imshow(win_name, output_dst);
}

void userdefined_blur(InputArray src, OutputArray dst, Size ksize, Point anchor, int BorderType){
	double kernelConst = 1 / (ksize.height) * (ksize.width);
	Mat kernel = Mat(ksize.height,ksize.width,CV_8UC1,Scalar(kernelConst));
	for (int i = 0; i < ksize.height; i++) {
		for (int j = 0; j < ksize.width; j++) {
			
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		cout << "[프로그램 사용법]" << endl;
		cout << "명령문 : ~.exe image_file flag<ent>" << endl;
		return 0;
	}
	//파일 주소는 argv[1]로 들어옴
	input_im = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	if (input_im.empty())
	{
		cout << "File open Error!" << endl;
		return -1;
	}
	cout << "Image size :" << input_im.size() << ", Type:" << type2str(input_im.type()) << endl;

	string window_name = "Image Average filter Window";
	namedWindow(window_name);

	//Create track bar to change kernel size
	int start_value = 3;
	int max_value = 15;

	//flag setting
	flag = atoi(argv[2]);

	createTrackbar("AverageFilter", window_name, &start_value, max_value, averageFiltering_opencvCommand, static_cast<void*>(&window_name));

	imshow(window_name, input_im);
	waitKey(0);

	return 0;
}

string type2str(int type) {
	// https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}