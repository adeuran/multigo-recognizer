#include "opencv2/opencv.hpp"
#include <fstream>
#include <filesystem>

using namespace std::filesystem;
using namespace cv;
using namespace std;

// 상수 정의
const string INPUT_DIR = "input";
const string OUTPUT_DIR = "output";
const string TEXT_DIR = "/text";
const string IMAGE_DIR = "/image";

Mat calcGrayHist(const Mat& img);
void setPotition(char(*map)[19], Point point, bool is_white);
void start(path filepath);

int main()
{

	//폴더 준비
	// 
	// input 폴더가 없으면 오류
	if (!exists(INPUT_DIR)) {
		cout << "\"input\" folder not found!" << endl;
		return -1;
	}
	// output 관련 폴더가 남아있으면 날려버림
	if (exists(OUTPUT_DIR)) {
		remove_all(OUTPUT_DIR);
	}
	// 날린 폴더를 다시 만듦
	path dir(OUTPUT_DIR);
	create_directory(dir);
	dir = path(OUTPUT_DIR + TEXT_DIR);
	create_directory(dir);
	dir = path(OUTPUT_DIR + IMAGE_DIR);
	create_directory(dir);

	directory_iterator input_files(INPUT_DIR);

	// input 폴더의 파일들 불러서 처리 시작
	for (const directory_entry & a_file : input_files) {
		cout << a_file << endl;
		start(a_file.path());
	}

	return 0;
}

void start(path filepath) {
	// 영상 불러오기
	Mat src = imread(filepath.string(), IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	char map[19][19] = { 0, };	// 돌들을 저장할 2차원 배열 생성(0으로 저장)
	fill(&map[0][0], &map[18][18] + 1, '.');	// .으로 초기화

	// 이진화된 영상 생성
	Mat bin;
	threshold(src, bin, 100, 255, THRESH_BINARY);

	// 돌 찾기
	Mat dst;
	vector<Vec3f> circles;
	HoughCircles(src, circles, HOUGH_GRADIENT, 1, 5, 180, 30, 10, 30);
	cvtColor(src, dst, COLOR_GRAY2BGR);
	for (Vec3f c : circles) {
		Point center(cvRound(c[0]), cvRound(c[1]));
		Rect centerpoint(Rect(center.x - 5, center.y - 5, 10, 10));
		Mat hist = calcGrayHist(bin(centerpoint));
		// 검은색이 더 많으면(검은돌이면)
		if (hist.at<float>(0) > hist.at<float>(255)) {
			circle(dst, center, 3, Scalar(0, 0, 255), -1);	// 원을 그림
			setPotition(map, center, false);	// 좌표에 표시
		}
		// 흰돌이 더 많으면(흰돌이면)
		else {
			circle(dst, center, 3, Scalar(255, 0, 0), -1);
			setPotition(map, center, true);
		}
		int radius = cvRound(c[2]);
		circle(dst, center, radius, Scalar(0, 255, 0), 2, LINE_AA);
	}

	// 이미지 저장
	imwrite(OUTPUT_DIR + IMAGE_DIR + "/" + filepath.filename().string(), dst);
	// 텍스트 저장
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
	CV_Assert(img.type() == CV_8UC1);   // 이미지가 CV_8UC1 속성이어야 함.
	Mat hist;   // 히스토그램을 구해서 저장할 행렬 생성
	int channels[] = { 0 }; // 히스토그램을 구할 채널 배열(흑백이므로 0번만)
	int dims = 1;   // 히스토그램의 결과가 몇차원 행렬으로 나올 지(흑백이니 1차원)
	const int histSize[] = { 256 }; // 첫 번째 채널(0번배열)의 값이 256빈으로 구성됨
	float graylevel[] = { 0, 256 }; // 최솟값 0 최댓값 256
	const float* ranges[] = { graylevel };
	calcHist(&img, 1, channels, noArray(), hist, dims, histSize, ranges);   // 히스토그램 연산
	return hist;
}

// 돌의 위치를 map에 좌표화
void setPotition(char(*map)[19], Point point, bool is_white) {
	int least[] = { 52, 76, 100, 124, 148, 172, 196, 220, 244, 268, 292, 316, 340, 364, 388, 412, 436, 460, 500 };
	int pointarr[] = { point.x, point.y };	// 바둑알 center의 위치
	int xy[] = { 0,0 };	// 바둑알 좌표를 저장할 곳 (x, y 순서)
	for (int time = 0; time < 2; time++) {	//x, y순서대로 찾음
		for (int i = 0; i < 19; i++) {
			if (least[i] > pointarr[time]) {	// least보다 작으면 i가 현재 좌표
				xy[time] = i;
				break;
			}
		}
	}
	//	좌표에 색상을 기록
	char color;
	if (is_white)
		color = 'w';
	else
		color = 'b';

	map[xy[0]][xy[1]] = color;
	return;
}

