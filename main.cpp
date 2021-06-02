#include "opencv2/opencv.hpp"
#include <fstream>
#include <filesystem>

using namespace std::filesystem;
using namespace cv;
using namespace std;

// ��� ����
const string INPUT_DIR = "input";
const string OUTPUT_DIR = "output";
const string TEXT_DIR = "/text";
const string IMAGE_DIR = "/image";

Mat calcGrayHist(const Mat& img);
void setPotition(char(*map)[19], Point point, bool is_white);
void start(path filepath);

int main()
{

	//���� �غ�
	// 
	// input ������ ������ ����
	if (!exists(INPUT_DIR)) {
		cout << "\"input\" folder not found!" << endl;
		return -1;
	}
	// output ���� ������ ���������� ��������
	if (exists(OUTPUT_DIR)) {
		remove_all(OUTPUT_DIR);
	}
	// ���� ������ �ٽ� ����
	path dir(OUTPUT_DIR);
	create_directory(dir);
	dir = path(OUTPUT_DIR + TEXT_DIR);
	create_directory(dir);
	dir = path(OUTPUT_DIR + IMAGE_DIR);
	create_directory(dir);

	directory_iterator input_files(INPUT_DIR);

	// input ������ ���ϵ� �ҷ��� ó�� ����
	for (const directory_entry & a_file : input_files) {
		cout << a_file << endl;
		start(a_file.path());
	}

	return 0;
}

void start(path filepath) {
	// ���� �ҷ�����
	Mat src = imread(filepath.string(), IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	char map[19][19] = { 0, };	// ������ ������ 2���� �迭 ����(0���� ����)
	fill(&map[0][0], &map[18][18] + 1, '.');	// .���� �ʱ�ȭ

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
		Rect centerpoint(Rect(center.x - 5, center.y - 5, 10, 10));
		Mat hist = calcGrayHist(bin(centerpoint));
		// �������� �� ������(�������̸�)
		if (hist.at<float>(0) > hist.at<float>(255)) {
			circle(dst, center, 3, Scalar(0, 0, 255), -1);	// ���� �׸�
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

	// �̹��� ����
	imwrite(OUTPUT_DIR + IMAGE_DIR + "/" + filepath.filename().string(), dst);
	// �ؽ�Ʈ ����
	ofstream fout(OUTPUT_DIR + TEXT_DIR + "/" + filepath.stem().string()+".txt");
	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) {
			fout.put(map[i][j]);
		}
		fout.put('\n');
	}

	waitKey();
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

// ���� ��ġ�� map�� ��ǥȭ
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

