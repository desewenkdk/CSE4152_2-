#include <iostream>
#include <string>
#include <time.h>// 시간 측정 위해서
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>

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
		cout << "Kernel size cannot be smaller than 0 kernel size will be 1" <<endl;
		value = 1;
	}

	if (value % 2 == 0) {
		value -= 1;
	}
	Size ksize = Size(value, value);

	start_time = clock();
	switch (flag) {
	case 1:
		blur(input_im, output_dst, ksize, Point(-1, -1), BORDER_CONSTANT);
		break;
	case 2:
		userdefined_blur(input_im, output_dst, ksize, Point(-1, -1), BORDER_DEFAULT);
		break;
	default:
		cout << "flag exception : this message should not be shown" << endl;
		exit(1);
		break;
	}
	end_time = clock();
	cout << "kernel value : " << value << ", Exec Time : " << (double)(end_time - start_time) << " (msec)" << endl;

	imshow(win_name, output_dst);
}

void userdefined_blur(InputArray src, OutputArray dst, Size ksize, Point anchor, int BorderType){
	double kernelConst = 1.0 / ((ksize.height) * (ksize.width));
	//Mat kernel = Mat(ksize.width,ksize.height,CV_8UC1);

	int ki = 0;

	/*
	for (int i = 0; i < input_im.rows; i++) { //(i,j):커널의 중심 좌표.
		for (int j = 0; j < input_im.cols; j++) {
			double sum = 0;

			//brute force
			for (ki = i - (ksize.height / 2); ki <= i + (ksize.height / 2); ki++) {
				for (int kj = j - (ksize.width / 2); kj <= j + (ksize.width / 2); kj++) {
					if (ki < 0 || ki > input_im.rows - 1 || kj < 0 || kj > input_im.cols-1) {
						sum += 0;
						continue;
					}
					sum += input_im.at<uchar>(ki, kj);
				}
			}
			output_dst.at<uchar>(i, j) = sum * kernelConst;

		}
	}
	*/
	
	for (int i = 0; i < input_im.rows; i++) { //(i,j):커널의 중심 좌표.
		int lboarder, rboarder, tboarder, bboarder;

		tboarder = (i - (ksize.height / 2) < 0 ? 0 : i - (ksize.height / 2));
		bboarder = (i + (ksize.height / 2) >= input_im.rows ? input_im.rows-1 : i + (ksize.height / 2));
		double prevSum = 0;
		for (int j = 0; j < input_im.cols; j++) {
			double sum = 0;
			double colSum = 0;
			lboarder = (j - (ksize.width / 2) < 0 ? 0 : j - (ksize.width / 2));
			rboarder = (j + (ksize.width / 2) >= input_im.cols ? input_im.cols-1 : j + (ksize.width / 2));

			//brute force
			if (j == 0) {
				for (ki = lboarder; ki <= rboarder; ki++) {
					for (int kj = tboarder; kj <= bboarder; kj++) {
						sum += input_im.at<uchar>(ki, kj);
					}
				}
				 prevSum = sum * kernelConst;
				 output_dst.at<uchar>(i, 0) = prevSum;
			}

			else {
				if (j - (ksize.width / 2) <= 0) {//왼쪽 열 합을 빼줄 필요가 없는 경우.
					for (int h = tboarder; h <= bboarder; h++){
						colSum += input_im.at<uchar>(h, rboarder);
						
					}
				}
				else if ((j - (ksize.width / 2) > 0) && (j + (ksize.width / 2) < input_im.cols)) {
					for (int h = tboarder; h <= bboarder; h++) {
						colSum = colSum + input_im.at<uchar>(h, rboarder) - input_im.at<uchar>(h,lboarder-1);
						
					}
				}
				else if(j + (ksize.width / 2) >= input_im.cols) {
					for (int h = tboarder; h <= bboarder; h++) {
						colSum += (-1)*(input_im.at<uchar>(h, lboarder - 1));
						
					}
				}

				prevSum = prevSum + colSum;

				output_dst.at<uchar>(i, j) = (prevSum) * kernelConst;
				//prevSum = output_dst.at<uchar>(i, j);
			}	
		}
	}
	
	//filter2D(src,dst, CV_8UC1,kernel,Point(-1,-1),BorderType);

}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		cout << "[프로그램 사용법]" << endl;
		cout << "명령문 : ~.exe lena.bpm flag<ent>\nflag : 1 - opencv function 사용\nflag : 2 - loop로 brute force 사용" << endl;
		return 0;
	}
	//파일 주소는 argv[1]로 들어옴
	input_im = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
	if (input_im.empty())
	{
		cout << "File open Error!" << endl;
		return -1;
	}
	output_dst = Mat(input_im.rows,input_im.cols, CV_8UC1);
	cout << "Image size :" <<	input_im.size() << ", Type:" << type2str(input_im.type()) << endl;

	string window_name = "Image Average filter Window";
	namedWindow(window_name);

	//Create track bar to change kernel size
	int start_value = 3;
	int max_value = 15;

	//flag setting
	flag = atoi(argv[2]);
	if (flag == 1 || flag == 2) {
		createTrackbar("Average Filtering Window", window_name, &start_value, max_value, averageFiltering_opencvCommand, static_cast<void*>(&window_name));
	}
	else {
		cout << "Wrong flag!!" << endl;
	}
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