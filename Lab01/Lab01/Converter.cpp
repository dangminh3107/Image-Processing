#include "stdafx.h"
#include "Converter.h"

int Converter::RGB2GrayScale(const Mat& sourceImage, Mat& destinationImage) {
	//xuất ra màn hình "can't open image nếu không có ảnh đầu vào
	if (sourceImage.data == NULL) {
		//cout << "Can't open image";
		return 0;
	}

	//tạo ảnh đích có kích thước như ảnh gốc, 1 điểm ảnh sẽ có 1 channel 8 bit không dấu để biểu diễn
	destinationImage = Mat(sourceImage.size(), CV_8UC1);

	int width = sourceImage.cols; //số cột của ảnh gốc --> chiều rộng
	int height = sourceImage.rows; //số dòng của ảnh gốc --> chiều dài

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			Vec3b color = sourceImage.at<Vec3b>(y, x); //truy cập dữ liệu của ảnh gốc

			double blue = color.val[0];  //truy xuất kênh màu blue của ảnh gốc
			double green = color.val[1]; //truy xuất kênh màu green của ảnh gốc
			double red = color.val[2]; //truy xuất kênh màu red của ảnh gốc

									   //gán các giá trị blue, green, red sau khi biến đổi vào ảnh đích
			destinationImage.at<uchar>(y, x) = blue * 0.144 + green * 0.587 + red * 0.299;
		}
	}
	return 1;
}

int Converter::GrayScale2RGB(const Mat& sourceImage, Mat& destinationImage)
{
	if (sourceImage.empty()) {
		return 0;
	}
	//lấy ra cao, rộng từ ảnh nguồn
	int width = sourceImage.cols;
	int height = sourceImage.rows;
	//khoảng cách giữa 2 điểm ảnh cùng cột thuộc 2 dòng liên tiếp
	int widthStep = sourceImage.step[0];
	//số kênh màu
	int nChannels = sourceImage.channels();
	//tạo ảnh đích với kích thước của ảnh nguồn và 3 kênh màu, mỗi phần tử unsigned char, ảnh màu RGB 
	destinationImage.create(height, width, CV_8UC3);
	
	//con trỏ quản lý vùng nhớ ảnh
	uchar* pDest = (uchar*)destinationImage.data;
	uchar* pSource = (uchar*)sourceImage.data;

	for (int i = 0; i < height; i++, pSource += widthStep, pDest += destinationImage.step[0]) {
		uchar * pRow = pDest;
		uchar * psRow = pSource;
		for (int j = 0; j < width; j++, psRow += nChannels, pRow += 3) {
			pRow[0] = psRow[0];
			pRow[1] = psRow[0];
			pRow[2] = psRow[0];
		}
	}

	return 1;
}


int Converter::RGB2HSV(const Mat& sourceImage, Mat& destinationImage) {
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

											 //tạo ảnh đích có kích thước như ảnh gốc, một điểm ảnh sẽ có 3 channel, mỗi channel dùng 8 bit không dấu để biểu diễn
	destinationImage = Mat(height, width, CV_8UC3, Scalar(0));

	uchar* pDataDes = (uchar*)destinationImage.data;  //pData là con trỏ quản lý vùng nhớ ảnh

	for (int y = 0; y < height; y++, pData += widthStep, pDataDes += widthStep) {
		uchar* pRow = pData; //lấy con trỏ đầu mỗi dòng của ảnh gốc
		uchar* pRowDes = pDataDes; //lấy con trỏ đầu mỗi dòng của ảnh đích
		for (int x = 0; x < width; x++, pRow += nChannels, pRowDes += nChannels) {
			uchar blue = pRow[0]; //truy xuất pixel (x,y) channel thứ 0, sau đó gán giá trị cho biến blue
			uchar green = pRow[1]; //truy xuất pixel (x,y) channel thứ 1, sau đó gán giá trị cho biến green
			uchar red = pRow[2]; //truy xuất pixel (x,y) channel thứ 2, sau đó gán giá trị cho biến red

			double _blue = (double)blue / 255;
			double _green = (double)green / 255;
			double _red = (double)red / 255;

			double max, min, delta, H, S, V;
			/*max: giá trị lớn nhất của 3 kênh màu red, green, blue
			min: giá trị nhỏ nhất của 3 kênh màu red, green, blue
			delta: hiệu số của max và min
			H, S, V: 3 kênh màu trong không gian màu HSV
			*/

			//tìm ra giá trị lớn nhất của 3 giá trị red, green, blue
			max = _red > _green ? _red : _green;
			max = max > _blue ? max : _blue;

			//tìm ra già trị nhỏ nhất của 3 giá trị red, green, blue
			min = _red < _green ? _red : _green;
			min = min < _blue ? min : _blue;

			delta = max - min;


			/*---------CÔNG THỨC CHUYỂN ĐỔI----------
			{0	nếu delta = 0
			H=	{60 * (((_green - _blue)/ delta) mod 6) nếu max = _red
			{60 * (((_blue - _red)/ delta) + 2) nếu max = _green
			{60 * (((_red - _green)/ delta) + 4) nếu max = _blue

			{0			nếu max = 0
			S=	{delta/max	nếu max != 0

			V = max
			*/

			V = max;

			if (max == 0) {
				S = 0;
			}
			else {
				S = delta / max;
			}

			if (delta == 0) {
				H = 0;
			}
			else {
				if (max = _red)
					H = 60 * (fmod(((_green - _blue) / delta), 6));
				else if (max = _green)
					H = 60 * (((_blue - _red) / delta) + 2);
				else if (max = _blue)
					H = 60 * (((_red - _green) / delta) + 4);
			}
			if (H < 0)
				H += 360;

			uchar _H = uchar(H / 2);
			uchar _S = uchar(S * 255);
			uchar _V = uchar(V * 255);

			//gán lần lượt các giá trị H, S, V channel thứ 0, 1, 2 của ảnh đích
			pRowDes[0] = _H;
			pRowDes[1] = _S;
			pRowDes[2] = _V;
		}
	}
	return 1;
}

int Converter::HSV2RGB(const Mat& sourceImage, Mat& destinationImage) {
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

											 //tạo ảnh đích có kích thước như ảnh gốc, một điểm ảnh sẽ có 3 channel, mỗi channel dùng 8 bit không dấu để biểu diễn
	destinationImage = Mat(height, width, CV_8UC3, Scalar(0));

	uchar* pDataDes = (uchar*)destinationImage.data;  //pData là con trỏ quản lý vùng nhớ ảnh

	for (int y = 0; y < height; y++, pData += widthStep, pDataDes += widthStep) {
		uchar* pRow = pData; //lấy con trỏ đầu mỗi dòng của ảnh gốc
		uchar* pRowDes = pDataDes; //lấy con trỏ đầu mỗi dòng của ảnh đích
		for (int x = 0; x < width; x++, pRow += nChannels, pRowDes += nChannels) {

			//CÔNG THỨC CHUYỂN ĐỔI
			/*
			0 <= H, S, V <= 1
			C = S * V
			X = C(1 - |(H/60)%2-1|)
			m = V - C
			{ (C, X, 0) nếu 0 <= H < 1
			{ (X, C, 0) nếu 1 <= H < 2
			{ (0, C, X) nếu 2 <= H < 3
			(R', G', B') =	{ (0, X, C) nếu 3 <= H < 4
			{ (X, 0, C) nếu 4 <= H < 5
			{ (C, 0, X) nếu 5 <= H < 6

			--> (R, G, B) = ((R'+m)×255, (G'+m)×255, (B'+m)×255)
			*/

			//chuyển đổi từ không gian HSV sang RGB
			double H = double(pRow[0] * 2);
			double S = double(pRow[1]);
			double V = double(pRow[2]);

			double C = S * V;
			double _H = fmod(H / 60, 6);
			double X = C * (1 - fabs(fmod(_H, 2) - 1));
			double M = V - C;

			double red, green, blue;

			if (0 <= _H && _H < 1) {
				red = C; green = X; blue = 0;
			}
			else if (_H < 2) {
				red = X; green = C; blue = 0;
			}
			else if (_H < 3) {
				red = 0; green = C; blue = X;
			}
			else if (_H < 4) {
				red = 0; green = X; blue = C;
			}
			else if (_H < 5) {
				red = X; green = 0; blue = C;
			}
			else if (_H < 6) {
				red = C; green = 0; blue = X;
			}
			else {
				red = 0; green = 0; blue = 0;
			}
			red += M;
			green += M;
			blue += M;

			pRowDes[0] = saturate_cast<uchar>(blue);
			pRowDes[1] = saturate_cast<uchar>(green);
			pRowDes[2] = saturate_cast<uchar>(red);
		}
	}
	return 1;
}

int Converter::Convert(Mat& sourceImage, Mat& destinationImage, int type) {
	switch (type) {
	case 0:
		return RGB2GrayScale(sourceImage, destinationImage);
		break;
	case 1:
		return GrayScale2RGB(sourceImage, destinationImage);
		break;
	case 2:
		return RGB2HSV(sourceImage, destinationImage);
		break;
	case 3:
		return HSV2RGB(sourceImage, destinationImage);
		break;
	}
	return 0;
}

int Converter::Convert(IplImage* sourceImage, IplImage* destinationImage, int type) {
	return 0;
}

Converter::Converter() {
}

Converter::~Converter() {
}