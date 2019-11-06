// Lab01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ColorTransformer.h"
#include "Converter.h"
#include <string>
#include <iostream>
using namespace std;

//int main()
//{
//	Mat source_Image, dest_Image;
//	ColorTransformer colorTransf;
//	Converter convert;
//
//	string cmd = "--equalhist";
//	source_Image = imread("lena_color.png", -1);
//
//	int result = -1;
//
//	/*if (cmd == "--contrast") {
//		float c = atof(argv[3]);
//		result = colorTransf.ChangeContrast(source_Image, dest_Image, c);
//	}*/
//	if (cmd == "--hist") {
//		result = colorTransf.CalcHistogram(source_Image, dest_Image);
//	}
//	if (cmd == "--gray2rgb") {
//		result = convert.Convert(source_Image, dest_Image, 1);
//	}
//	if (cmd == "--equalhist") {
//		result = colorTransf.HistogramEqualization(source_Image, dest_Image);
//	}
//	if (result)
//	{
//		imshow("Source Image", source_Image);
//		imshow("Destination Image", dest_Image);
//	}
//	else
//		printf("Could not open or find the image");
//	waitKey();
//	return 0;
//}
int main(int argc, char* argv[])
{
	Mat source_Image, dest_Image;
	ColorTransformer colorTransf;
	Converter convert;

	string cmd = string(argv[1]);
	source_Image = imread(argv[2], -1);

	int result = -1;
	
	if (cmd == "bright") {
		
		short b = atoi(argv[3]);
		result = colorTransf.ChangeBrighness(source_Image, dest_Image, b);
	}
	if (cmd == "--contrast") {
		float c = atof(argv[3]);
		result = colorTransf.ChangeContrast(source_Image, dest_Image, c);
	}
	if (cmd == "--hist") {
		result = colorTransf.CalcHistogram(source_Image, dest_Image);
	}
	if (cmd == "--gray2rgb") {
		result = convert.Convert(source_Image, dest_Image, 1);
	}
	if (cmd == "--equalhist") {
		result = colorTransf.HistogramEqualization(source_Image, dest_Image);
	}
	if (result)
	{
		imshow("Source Image", source_Image);
		imshow("Destination Image", dest_Image);
	}
	else
		printf("Could not open or find the image");
	waitKey();
	return 0;
}

