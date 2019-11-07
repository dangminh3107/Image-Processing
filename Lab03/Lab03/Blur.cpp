#include "stdafx.h"
#include "Blur.h"
#include "Convolution.h"

int Blur::BlurImage(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
	if (sourceImage.empty())
		return 0;
	Convolution Convo;
	vector<float> kernel;
	int n;
	switch (method)
	{
	case 0:
		//Lọc trung bình
		n = kWidth*kHeight;
		for (int i = 0; i < n; i++)
			kernel.push_back(1.0 / n);
		Convo.SetKernel(kernel, kWidth, kHeight);
		return Convo.DoConvolution(sourceImage, destinationImage);
		break;
	default:
		break;
	}
}

Blur::Blur() {}
Blur::~Blur() {}