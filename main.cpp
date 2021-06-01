#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

Mat calcGrayHist(const Mat& img);
void setPotition(char(*map)[19], Point point, bool is_white);

int main()
{

	// ���� �ҷ�����
	Mat src = imread("./input/test06.jpg", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return -1;
	}
	
	char map[19][19] = {0, };	// ������ ������ 2���� �迭 ����(0���� ����)
	fill(&map[0][0], &map[18][18]+1, '.');	// .���� �ʱ�ȭ

	// ����ȭ�� ���� ����
	Mat bin;
	threshold(src, bin, 100, 255, THRESH_BINARY);

	// �� ã��
	Mat dst;
	vector<Vec3f> circles;
	HoughCircles(src, circles, HOUGH_GRADIENT, 1, 5, 180, 30, 10, 30);
	cvtColor(src, dst, COLOR_GRAY2BGR);
	for (Vec3f c : circles) {
		Point center(cvRound(c[0]), cvRound(c[1]));
		Rect centerpoint (Rect(center.x-5,center.y-5,10, 10));
		Mat hist = calcGrayHist(bin(centerpoint));
		// �������� �� ������(�������̸�)
		if (hist.at<float>(0) > hist.at<float>(255)){
			circle(dst, center, 3,Scalar(0, 0, 255), -1);	// ���� �׸�
			setPotition(map, center, false);	// ��ǥ�� ǥ��
		}
		// ���� �� ������(���̸�)
		else {
			circle(dst, center, 3, Scalar(255, 0, 0), -1);
			setPotition(map, center, true);
		}
		int radius = cvRound(c[2]);
		circle(dst, center, radius, Scalar(0, 255, 0), 2, LINE_AA);
	}

	// ���
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			cout << map[j][i];
		}
		cout << endl;
	}

	imshow("src", src);
	imshow("bin", bin);
	imshow("dst", dst);

	waitKey();

	return 0;
}

Mat calcGrayHist(const Mat& img) {
	CV_Assert(img.type() == CV_8UC1);   // �̹����� CV_8UC1 �Ӽ��̾�� ��.
	Mat hist;   // ������׷��� ���ؼ� ������ ��� ����
	int channels[] = { 0 }; // ������׷��� ���� ä�� �迭(����̹Ƿ� 0����)
	int dims = 1;   // ������׷��� ����� ������ ������� ���� ��(����̴� 1����)
	const int histSize[] = { 256 }; // ù ��° ä��(0���迭)�� ���� 256������ ������
	float graylevel[] = { 0, 256 }; // �ּڰ� 0 �ִ� 256
	const float* ranges[] = { graylevel };
	calcHist(&img, 1, channels, noArray(), hist, dims, histSize, ranges);   // ������׷� ����
	return hist;
}

void setPotition(char(*map)[19], Point point, bool is_white) {
	int least[] = { 52, 76, 100, 124, 148, 172, 196, 220, 244, 268, 292, 316, 340, 364, 388, 412, 436, 460, 500 };
	int pointarr[] = { point.x, point.y };	// �ٵϾ� center�� ��ġ
	int xy[] = { 0,0 };	// �ٵϾ� ��ǥ�� ������ �� (x, y ����)
	for (int time = 0; time < 2; time++) {	//x, y������� ã��
		for (int i = 0; i < 19; i++) {
			if (least[i] > pointarr[time]) {	// least���� ������ i�� ���� ��ǥ
				xy[time] = i;
				break;
			}
		}
	}
	//	��ǥ�� ������ ���
	char color;
	if (is_white)
		color = 'w';
	else
		color = 'b';

	map[xy[0]][xy[1]] = color;
	return;
}