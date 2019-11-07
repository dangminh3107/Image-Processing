// Lab03.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Convolution.h"
#include "Blur.h"
#include "EdgeDetector.h"

int main(int argc, char* argv[])
{
	Blur blur;
	Mat srcImage, desImage;
	srcImage = imread(argv[2], IMREAD_GRAYSCALE);
	string cmd = string(argv[1]);
	int result = 0;
	if (cmd == "--mean") {
		int kHeight, kWidth;
		kHeight = atoi(argv[4]);
		kWidth = atoi(argv[3]);
		result = blur.BlurImage(srcImage, desImage, kWidth, kHeight, 0);
	}
	if (result)
	{
		imshow("Source Image", srcImage);
		imshow("Destination Image", desImage);
	}
	else
		printf("Could not open or find the image");
	waitKey(0);
    return 0;
}

//int main(int argc, char* argv[])
//{
//	Blur blur;
//	Mat srcImage, desImage;
//
//	srcImage = imread("Lena.png", IMREAD_GRAYSCALE);
//	string cmd = "--mean";
//	int result = 0;
//	if (cmd == "--mean") {
//		int kHeight, kWidth;
//		kHeight = 3;
//		kWidth = 3;
//		result = blur.BlurImage(srcImage, desImage, kWidth, kHeight, 0);
//	}
//	if (result)
//	{
//		imshow("Source Image", srcImage);
//		imshow("Destination Image", desImage);
//	}
//	else
//		printf("Could not open or find the image");
//	waitKey(0);
//	return 0;
//}
