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
		userdefined_blur(input_im, output_dst, ksize, Point(-1, -1), BORDER_CONSTANT);
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

	/**** Moving Average ****/
	ki = 0;

	//큐의 원소는 커널의 row?col수만큼 존재한다.
	queue<double> verQ;
	//queue<uchar> horQ;
	double totalSum;


	//커널의 중심이 어쨋든 이미지 위를 전부 다 돌긴 해야하니까 기본 2중반복문.
	for (int i = 0; i < input_im.rows; i++) {
		double tmp_colSum = 0;
		
		//횡으로 다 돌고 돌아오면 colQ초기화 후 다시계산.
		queue<double> emptyQ;
		swap(verQ, emptyQ);

		for (int j = 0; j < input_im.cols; j++) {
			totalSum = 0;
			tmp_colSum = 0;


			if (j == 0) {
				//좌-우로 끝까지 이동한 후에는, 커널이 아래로 이동. 즉, 아래로 한 칸 이동
				for (int kj = -(ksize.width / 2); kj <= (ksize.width / 2); kj++) {//커널이 한방향으로 다 돌고 다시 돌아왔을 때 첫 커널연산값
					if (kj < 0 || kj > input_im.cols-1) {
						verQ.push(tmp_colSum);
						continue;
					}
					else {
						for (int ki = 0; ki <= i + (ksize.height / 2); ki++) {
							tmp_colSum += input_im.at<uchar>(ki, kj);
						}
						verQ.push(tmp_colSum);
					}
				}

	
			}
			//col단위로 한 줄 한 줄 더해서 큐로 관리하자. -> 세로큐
			else {
				int kj = j + (ksize.width / 2);
				for (int ki = i - (ksize.height / 2); ki <= i + (ksize.height / 2); ki++) {
					if (ki < 0 || ki > input_im.rows - 1) {
						tmp_colSum += 0;
						continue;
					}
					else {
						tmp_colSum += input_im.at<uchar>(kj, ki);
					}
				}
				verQ.push(tmp_colSum);//새로 계산된col의 합을 넣고
				verQ.pop();//기존 col중 가장 width값이 작은 col의 합은 제거.
			}

			//queue에 저장된 col합들을 가지고 커널연산.
			queue<double> tmpQ;
			while (!verQ.empty()) {
				totalSum += verQ.front();
				tmpQ.push(verQ.front());
				verQ.pop();
			}
			output_dst.at<uchar>(i, j) = totalSum * kernelConst;
			swap(verQ, tmpQ);
			swap(tmpQ, emptyQ);
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