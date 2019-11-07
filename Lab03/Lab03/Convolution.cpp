#include "stdafx.h"
#include "Convolution.h"

vector<float> Convolution::GetKernel() {
	return this->_kernel;
}

void Convolution::SetKernel(vector<float> kernel, int kWidth, int kHeight) {
	this->_kernel = kernel;
	this->_kernelHeight = kHeight;
	this->_kernelWidth = kWidth;
}

int Convolution::DoConvolution(const Mat& sourceImage, Mat& destinationImage) {
	if (sourceImage.empty()) {
		return 0;
	}

	int height = sourceImage.rows;
	int width = sourceImage.cols;
	//số kênh màu
	int nChannels = sourceImage.channels();
	//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
	int widthStep = sourceImage.step[0];

	//Tạo ảnh đích với kích thước ảnh nguồn và type là ảnh grayscale
	destinationImage.create(height, width, CV_8UC1);

	//con trỏ quản lý vùng nhớ ảnh
	uchar* pDes = (uchar*)destinationImage.data;
	uchar* pSrc = (uchar*)sourceImage.data;
	
	//_kernelWidth, _kernelHeight là kích thước vùng lân cận
	int kHalfWidth = this->_kernelWidth >> 1;
	int kHalfHeight = this->_kernelHeight >> 1;
	vector<int> offset;
	//Tạo bảng offset
	for (int y = -kHalfHeight; y <= kHalfHeight; y++)
		for (int x = -kHalfWidth; x <= kHalfWidth; x++)
			offset.push_back(y*widthStep + x);

	float sum;
	for (int i = 0; i < height; i++, pDes += widthStep, pSrc += widthStep) {
		uchar* psRow = pSrc;
		uchar* pdRow = pDes;
		for (int j = 0; j < width; j++, psRow += nChannels, pdRow += nChannels) {
			sum = 0.0;
			//Tích chập
			for (int k = 0; k < offset.size(); k++)
				sum += *(psRow + offset[k]) * this->_kernel[offset.size() - 1 - k];
			pdRow[0] = saturate_cast<uchar>(sum);
		}
	}
	return 1;
}

Convolution::Convolution() {}
Convolution::~Convolution() {}