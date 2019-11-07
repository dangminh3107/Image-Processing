#include "stdafx.h"
#include <math.h>
#include "Blur.h"
#include "Convolution.h"
#define pi 3.141592653589793238462643383279502884L /* pi */ 

void Sort(uchar* I)
{
	int n = sizeof(I);
	for (int i=0;i<n-1;i++)
		for (int j = i + 1; j < n; j++)
		{
			int temp;
			if (I[i] > I[j])
			{
				temp = I[i];
				I[i] = I[j];
				I[j] = temp;
			}
		}
}

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
	if (sourceImage.empty())
		return 0;
	Convolution Convo;
	vector<float> kernel;
	vector<int> offset;
	switch (method)
	{
	case 0: {
		//Lọc trung bình
		int n = kWidth*kHeight;
		for (int i = 0; i < n; i++)
			kernel.push_back(1 / n);
		Convo.SetKernel(kernel, kWidth, kHeight);
		return Convo.DoConvolution(sourceImage, destinationImage);
	}
		break;
	case 1: {
		//Lọc trung vị
		int height = sourceImage.cols;
		int width = sourceImage.rows;
		destinationImage.create(height, width, sourceImage.type());
		//Số kênh màu
		int nChannels = sourceImage.channels();
		//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
		int widthStep = sourceImage.step[0];

		//Con trỏ  lưu dữ liệu vùng nhớ ảnh
		uchar* pDes = (uchar*)destinationImage.data;
		uchar* pSrc = (uchar*)sourceImage.data;

		//Tạo offset table (hay bộ lọc mask)
		int kHalfWidth = kWidth >> 1;
		int kHalfHeight = kHeight >> 1;
		for (int y = -kHalfHeight; y <= kHalfHeight; y++)
			for (int x = -kHalfWidth; x <= kHalfWidth; x++)
				offset.push_back(y*widthStep + x);

		int n = offset.size();
		uchar* I = new uchar[n];
		for (int i = 0; i < height; i++, pSrc += widthStep, pDes += widthStep) {
			uchar* psRow = pSrc;
			uchar* pdRow = pDes;
			for (int j = 0; j < width; j++, psRow += nChannels, pdRow += nChannels)
			{
				for (int k = 0; k < n; k++)
					I[k] = psRow[offset[k]];
				Sort(I);
				pdRow[0] = I[n / 2];
			}
		}
	}
		break;
	case 2: {
		float sigma = 1.0; //phương sai
		float inverse_sqrt2pi_sigma = 1 / (sqrt(2 * pi)*sigma), inverse_2_sigmasquare = 1 / (2 * sigma*sigma);
		int n = kHeight*kWidth;

		//Tạo kernel (bộ lọc h)
		int kHalfHeight = kHeight >> 1;
		int kHalfWidth = kWidth >> 1;
		float h, sum = 0.0;
		for (int y=-kHalfHeight;y<=kHalfHeight;y++)
			for (int x = -kHalfWidth; x <= kHalfWidth; x++) {
				h = inverse_sqrt2pi_sigma*exp(-(y*y + x*x)*inverse_2_sigmasquare);
				sum += h;
				kernel.push_back(h);
			}

		//Chuẩn hóa kernel
		for (int i = 0; i < n; i++)
			kernel[i] /= sum;

		//nhân chập ảnh với kernel
		Convo.SetKernel(kernel, kWidth, kHeight);
		return Convo.DoConvolution(sourceImage, destinationImage);
		break;
	}
	default:
		break;
	}
}

Blur::Blur() {}
Blur::~Blur() {}