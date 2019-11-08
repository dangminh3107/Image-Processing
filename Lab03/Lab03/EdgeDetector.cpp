#include "stdafx.h"
#include "EdgeDetector.h"

int EdgeDetector::DetectEdge(const Mat& sourceImage, Mat& destinationImage, int kWidth, int kHeight, int method) {
	if (sourceImage.empty())
		return 0;

	switch (method)
	{
	case 1: {
		//Lọc Sobel
		Convolution Convo;
		vector<float> Kernelx, Kernely;

		int height = sourceImage.rows;
		int width = sourceImage.cols;
		//Tạo ma trận đích có loại và kích thước giống ma trận nguồn
		destinationImage.create(height, width, sourceImage.type());

		float Wx[9] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };
		//		[1  0 -1]
		//	Wx =[2  0 -2] 
		//		[1  0 -1]
		float Wy[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
		//		[-1  -2  -1]
		//	Wx =[ 0   0   0] 
		//		[ 1   2   1]
		//tạo 2 bộ lọc kernel
		for (int i = 0; i < 9; i++) {
			Wx[i] = Wx[i] / 4;
			Wy[i] = Wy[i] / 4;
			Kernelx.push_back(Wx[i]);
			Kernely.push_back(Wy[i]);
		}

		Mat fx, fy;
		//Tính tích chập ma trận fx với Kernelx
		Convo.SetKernel(Kernelx, 3, 3);
		Convo.DoConvolution(sourceImage, fx);
		//Tính tích chập ma trận fy với Kernely
		Convo.SetKernel(Kernely, 3, 3);
		Convo.DoConvolution(sourceImage, fy);

		//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
		int widthStep = sourceImage.step[0];
		//Số kênh màu
		int nChannels = sourceImage.channels();

		//Con trỏ lưu dữ liệu ảnh
		uchar* pDes = (uchar*)destinationImage.data;
		//Con trỏ lưu dữ liệu 2 ma trận fx, fy
		uchar* pX = (uchar*)fx.data;
		uchar* pY = (uchar*)fy.data;
		float sum = 0;
		for (int i = 0; i < height; i++, pDes += widthStep, pX += widthStep, pY += widthStep) {
			//Con trỏ đầu mỗi dòng
			uchar* pdRow = pDes;
			uchar* pXRow = pX;
			uchar* pYRow = pY;
			float f2x, f2y;
			for (int j = 0; j < width; j++, pdRow += nChannels, pXRow += nChannels, pYRow += nChannels) {
				f2x = pXRow[0] * pXRow[0];
				f2y = pYRow[0] * pYRow[0];
				sum = sqrt(f2x + f2y);
				pdRow[0] = saturate_cast<uchar>(sum);
			}
		}
	}
		break;
	case 2: {
		//Lọc Prewitt
		Convolution Convo;
		vector<float> Kernelx, Kernely;

		int height = sourceImage.rows;
		int width = sourceImage.cols;
		//Tạo ma trận đích có loại và kích thước giống ma trận nguồn
		destinationImage.create(height, width, sourceImage.type());

		float Wx[9] = { 1, 0, -1, 1, 0, -1, 1, 0, -1 };
		//		[1  0 -1]
		//	Wx =[1  0 -1] 
		//		[1  0 -1]
		float Wy[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
		//		[-1 -1 -1]
		//	Wy =[ 0  0  0]
		//		[ 1  1  1]
		//tạo 2 bộ lọc kernel
		for (int i = 0; i < 9; i++) {
			Wx[i] = Wx[i] / 3;
			Wy[i] = Wy[i] / 3;
			Kernelx.push_back(Wx[i]);
			Kernely.push_back(Wy[i]);
		}

		Mat fx, fy;
		//Tính tích chập ma trận fx với Kernelx
		Convo.SetKernel(Kernelx, 3, 3);
		Convo.DoConvolution(sourceImage, fx);
		//Tính tích chập ma trận fy với Kernely
		Convo.SetKernel(Kernely, 3, 3);
		Convo.DoConvolution(sourceImage, fy);

		//widthStep là khoảng cách tính byte giữa 2 pixel cùng cột trên 2 dòng liên tiếp
		int widthStep = sourceImage.step[0];
		//Số kênh màu
		int nChannels = sourceImage.channels();

		//Con trỏ lưu dữ liệu ảnh
		uchar* pDes = (uchar*)destinationImage.data;
		//Con trỏ lưu dữ liệu 2 ma trận fx, fy
		uchar* pX = (uchar*)fx.data;
		uchar* pY = (uchar*)fy.data;
		float sum = 0;
		for (int i = 0; i < height; i++, pDes += widthStep, pX += widthStep, pY += widthStep) {
			//Con trỏ đầu mỗi dòng
			uchar* pdRow = pDes;
			uchar* pXRow = pX;
			uchar* pYRow = pY;
			float f2x, f2y;
			for (int j = 0; j < width; j++, pdRow += nChannels, pXRow += nChannels, pYRow += nChannels) {
				f2x = pXRow[0] * pXRow[0];
				f2y = pYRow[0] * pYRow[0];
				sum = sqrt(f2x + f2y);
				pdRow[0] = saturate_cast<uchar>(sum);
			}
		}
	}
		break;
	case 3: {
		//Lọc Laplace
		float Laplace[9] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
		//			  [1  1  1]
		//	Laplace = [1 -8  1]
		//			  [1  1  1]
		//Tạo Kernel
		vector<float> Kernel;
		for (int i = 0; i < 9; i++)
			Kernel.push_back(Laplace[i]);

		//Tính tích chập của ảnh với ma trận Kernel
		Convolution Convo;
		Convo.SetKernel(Kernel, 3, 3);
		Convo.DoConvolution(sourceImage, destinationImage);
	}
		break;
	default:
		break;
	}
	return 1;
}

EdgeDetector::EdgeDetector() {}
EdgeDetector::~EdgeDetector() {}