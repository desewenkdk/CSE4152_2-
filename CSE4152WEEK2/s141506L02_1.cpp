#include <iostream>
#include <string>
#include <time.h>// �ð� ���� ���ؼ�
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>

using namespace std;
using namespace cv;
void averageFiltering_opencvCommand(int value, void *userData);//flag�� ���� cv�Լ�, user-defined�Լ� �� �����ؼ� �����ؾ� �ϴµ� �װ� �ݹ鿡�� ó��? �ƴϸ� ���� �ٸ� �ݹ�?
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
	for (int i = 0; i < input_im.rows; i++) {
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

	ki = 0;

	//�� ť�� ���Ҵ� �ش� Ŀ���� �����ټ� / ������ �� ��ŭ�� �����ϵ��� �Ѵ�.
	queue<double> verQ;
	queue<double> horQ;

	//Ŀ���� �߽��� ��¶�� �̹��� ���� ���� �� ���� �ؾ��ϴϱ� �⺻ 2�߹ݺ���.
	for (int i = 0; i < input_im.rows; i++) {
		int kt = i - (ksize.height / 2); int kb = i + (ksize.height / 2);
		//��-��� ������ �̵��� �Ŀ���, Ŀ���� �Ʒ��� �̵�. ��, �Ʒ��� ���鼭 �Ʒ��� �ϳ� �߰�, �� �� �� ����.=> ����ť
		for (int j = 0; j < input_im.cols; j++) {
			//col������ �� �� �� �� ���ؼ� ť�� ��������. -> ����ť
			for()
		}
	}
	//filter2D(src,dst, CV_8UC1,kernel,Point(-1,-1),BorderType);
}

int main(int argc, char *argv[])
{
	if (argc != 3) {
		cout << "[���α׷� ����]" << endl;
		cout << "��ɹ� : ~.exe lena.bpm flag<ent>\nflag : 1 - opencv function ���\nflag : 2 - loop�� brute force ���" << endl;
		return 0;
	}
	//���� �ּҴ� argv[1]�� ����
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