#include "stdafx.h"
#include "ColorTransformer.h"

int ColorTransformer::ChangeBrighness(const Mat& sourceImage, Mat& destinationImage, short b) {
	//xuất ra màn hình "Can't open image" nếu không có ảnh đầu vào
	if (sourceImage.data == NULL) {
		//cout << "Can't open image";
		return 0;
	}

	int width = sourceImage.cols; //số cột của ảnh gốc --> chiều rộng
	int height = sourceImage.rows; //số dòng của ảnh gốc --> chiều dài
	int nChannels = sourceImage.channels(); //số kênh màu
	int widthStep = sourceImage.step[0]; //khoảng cách tính theo byte giữa 2 pixel cùng cột trên 2 dòng kế tiếp
	uchar* pData = (uchar*)sourceImage.data; //pData là con trỏ quản lý vùng nhớ ảnh

											 //tạo ảnh đích có kích thước như ảnh gốc, một điểm ảnh sẽ có 3 channel, mỗi channel dùng 8 bit không dấu để biểu diễn.
	destinationImage = Mat(height, width, CV_8UC3, Scalar(0));

	uchar* pDataDes = (uchar*)destinationImage.data;  //pData là con trỏ quản lý vùng nhớ ảnh

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++)
			for (int n = 0; n < nChannels; n++)
				destinationImage.at<Vec3b>(y, x)[n] = saturate_cast<uchar>(sourceImage.at<Vec3b>(y, x)[n] + b);
	}
	return 1;
}

int ColorTransformer::ChangeContrast(const Mat& sourceImage, Mat& destinationImage, float c) {
	if (sourceImage.empty())
		return 0;
	uchar lookup[256];
	for (int i = 0; i < 256; i++)
		lookup[i] = saturate_cast<uchar>(i * c);
	//Khởi tạo ảnh đích có kích thước và type giống ảnh nguồn
	destinationImage.create(sourceImage.rows, sourceImage.cols, sourceImage.type());
	int width = sourceImage.cols;
	int height = sourceImage.rows;
	//số kênh màu
	int nChannels = sourceImage.channels();
	//widthStep là khoảng cách tính theo byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
	int widthStep = sourceImage.step[0];
	//p là con trỏ quản lý vùng nhớ ảnh
	uchar* pSource = (uchar*)sourceImage.data;
	uchar* pDest = (uchar*)destinationImage.data;
	for (int i = 0; i < height; i++, pSource += widthStep, pDest += widthStep)
	{
		uchar* pSRow = pSource;
		uchar* pDRow = pDest;
		for (int j = 0; j < width; j++, pDRow += nChannels, pSRow += nChannels) {
			for (int k = 0; k < nChannels; k++)
				pDRow[k] = lookup[(int)pSRow[k]];
		}
	}
	if (destinationImage.empty())
		return 0;
	return 1;
}

int ColorTransformer::CalcHistogram(const Mat& sourceImage, Mat& histMatrix)
{
	if (sourceImage.empty())
		return 0;
	//nếu ảnh chỉ có 1 kênh màu
	if (sourceImage.channels() == 1)
		histMatrix = Mat::zeros(1, 256, CV_32S);
	else
		//ảnh 3 kênh màu
		histMatrix = Mat::zeros(3, 256, CV_32S);
	//gán kích thước ảnh
	int width = sourceImage.cols;
	int height = sourceImage.rows;
	//số kênh màu
	int nChannels = sourceImage.channels();
	int widthStep = sourceImage.step[0];
	//con trỏ lưu dữ liệu ảnh
	uchar* p = (uchar*)sourceImage.data;
	for (int i = 0; i < height; i++) {
		p += widthStep;
		uchar* pRow = p;
		for (int j = 0; j < width; j++) {
			pRow += nChannels;
			//nếu chỉ 1 kênh màu
			if (nChannels==1)
				histMatrix.at<signed>(0, pRow[0])++;
			//ngược lại
			else {
				histMatrix.at<signed>(0, pRow[0])++;
				histMatrix.at<signed>(1, pRow[1])++;
				histMatrix.at<signed>(2, pRow[2])++;
			}
		}
	}
	return 1;
}

int ColorTransformer::HistogramEqualization(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.empty()) {
		return 0;
	}

	Mat srcImg, HSV, H, T;
	Converter convert;
	srcImg = sourceImage.clone(); //Copy dữ liệu ảnh nguồn
	int rows = sourceImage.rows;
	int cols = sourceImage.cols;

	//Nếu ảnh nguồn là ảnh màu thì chuyển sang HSV rồi tính Histogram
	if (srcImg.channels() != 1) {
		convert.Convert(srcImg, HSV, 2);
		CalcHistogram(HSV, H);
	}
	//Nếu ảnh nguồn là ảnh xám thì không cần chuyển
	else {
		CalcHistogram(srcImg, H);
	}

	//T[p] = T[p-1] + H[p]
	//H là lược đồ xám
	//T là lược đồ xám tích lũy

	//Nếu là ảnh xám
	if (srcImg.channels() == 1) {
		T = H.clone();
		T.at<signed>(0, 0) = H.at<signed>(0, 0);
		for (int i = 1; i < H.cols; i++) 
			T.at<signed>(0, i) = T.at<signed>(0, i - 1) + H.at<signed>(0, i);
		
		//Chuẩn hóa T về đoạn [0-255]
		for (int i = 0; i < H.cols; i++)
			T.at<signed>(0, i) = (255.0 / (rows*cols))*T.at<signed>(0, i) + 0.5;
	}
	//Nếu là ảnh màu thì xử lý giá trị V
	else {
		T = H.clone();
		T.at<signed>(2, 0) = H.at<signed>(2, 0);
		for (int i = 1; i < H.cols; i++)
			T.at<signed>(2, i) = T.at<signed>(2, i - 1) + H.at<signed>(2, i);

		//Chuẩn hóa T về đoạn [0-255]
		for (int i = 0; i < H.cols; i++)
			T.at<signed>(2, i) = (255.0 / (rows*cols))*T.at<signed>(2, i) + 0.5;
	}

	for (int i = 0; i< srcImg.rows;i++)
		for (int j = 0; j < srcImg.cols; j++) {
			//Nếu là ảnh xám
			if (srcImg.channels() == 1)
				//Thay giá trị màu của f(x,y) thành giá trị đã được cân bằng
				srcImg.at<uchar>(i, j) = T.at<signed>(0, srcImg.at<uchar>(i, j));
			//Nếu là ảnh màu
			else {
				//Thay giá trị màu của f(x,y) kênh V thành giá trị đã được cân bằng
				HSV.at<Vec3b>(i, j)[2] = T.at<signed>(2, srcImg.at<Vec3b>(i, j)[2]);
			}
		}
	if (srcImg.channels() != 1)
		//Nếu là ảnh màu thì chuyển lại RGB
		convert.Convert(HSV, destinationImage, 3);
	else
		destinationImage = srcImg.clone();
	return 1;
}
ColorTransformer::ColorTransformer() {}
ColorTransformer::~ColorTransformer() {}