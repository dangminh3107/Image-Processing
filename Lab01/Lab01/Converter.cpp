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

			float _blue = (float)blue / 255;
			float _green = (float)green / 255;
			float _red = (float)red / 255;

			float max, min, delta, H, S, V;
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

			S = (max == 0) ? 0 : delta / max;

			if (delta == 0) {
				H = 0;
			}
			else {
				if (max == _red)
					H = 60 * ((_green - _blue) / delta);
				else if (max == _green)
					H = 60 * ((_blue - _red) / delta + 2);
				else if (max == _blue)
					H = 60 * ((_red - _green) / delta + 4);
			}

			H = (H < 0) ? H + 360 : H;

			pRowDes[0] = uchar(H / 2);
			pRowDes[1] = uchar(S * 255);
			pRowDes[2] = uchar(V * 255);
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
	uchar* pData = (uchar*)sourceImage.data; //pData là con trỏ quản lý vùng nhớ ảnh gốc

											 //Khởi tạo ảnh đích với kích thước như ảnh nguồn
	destinationImage = Mat(height, width, CV_8UC3, Scalar(0));

	uchar* pDataDes = (uchar*)destinationImage.data;  //pData là con trỏ quản lý vùng nhớ ảnh đích

	float C, X, M, _H, H, S, V, red, green, blue;

	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

	for (int y = 0; y < height; y++, pData += widthStep, pDataDes += widthStep) {
		uchar * pRow = pData; //lấy con trỏ đầu mỗi dòng của ảnh gốc
		uchar * pRowDes = pDataDes; //lấy con trỏ đầu mỗi dòng của ảnh đích
		for (int x = 0; x < width; x++, pRow += nChannels, pRowDes += nChannels) {

			H = pRow[0] * 2.0;
			S = pRow[1] * BYTE_TO_FLOAT;
			V = pRow[2] * BYTE_TO_FLOAT;

			/*
			CÔNG THỨC CHUYỂN ĐỔI
			C = V * S
			_H = (H/ 60) % 6
			X = C * (1 - |(_H % 2) - 1|)
			M = V -C

			{ (C,X,0) nếu 0 <= _H < 1
			{ (X,C,0) nếu _H < 2
			(red, green, blue) = { (0,C,X) nếu _H < 3
			{ (0,X,C) nếu _H < 4
			{ (X,0,C) nếu _H < 5
			{ (C,0,X) nếu _H < 6
			{ (0,0,0) còn lại

			(red, green blue) = (red * 255, green * 255, blue * 255)
			*/

			C = S * V;
			_H = fmod(H / 60.0, 6);
			X = C * (1 - fabs(fmod(_H, 2) - 1));
			M = V - C;

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

			pRowDes[0] = uchar(blue * FLOAT_TO_BYTE);
			pRowDes[1] = uchar(green * FLOAT_TO_BYTE);
			pRowDes[2] = uchar(red * FLOAT_TO_BYTE);
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