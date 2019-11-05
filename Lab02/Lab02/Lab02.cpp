#include "stdafx.h"
#include <iostream>
#include "GeometricTransformer.h"
using namespace std;

int main(int argc, char *argv[]) {

	if (argc > 6) {
		cout << "Too much argument!" << endl;
		return 0;
	}

	Mat srcImage, desImage;
	string cmd = string(argv[1]);
	string Interpolate = string(argv[2]);
	srcImage = imread(argv[3], -1);

	if (srcImage.data == NULL) {
		cout << "Can't open image" << endl;
		return 0;
	}


	PixelInterpolate *interpolator;
	GeometricTransformer gt;

	// Kiểm tra pp gán giá trị màu
	if (Interpolate == "--nn")
		interpolator = new NearestNeighborInterpolate;
	else if (Interpolate == "--bl")
		interpolator = new BilinearInterpolate;
	else {
		cout << Interpolate << endl;
		cout << "Interpolation error " << endl;
		return 0;
	}

	if (cmd == "--zoom") {
		// Lấy 2 đối số
		gt.Scale(srcImage, desImage, atof(argv[4]), atof(argv[5]), interpolator);
	}
	else if (cmd == "--rotate") {
		gt.RotateKeepImage(srcImage, desImage, atof(argv[4]), interpolator);
	}
	else if (cmd == "--rotateN") {
		gt.RotateUnkeepImage(srcImage, desImage, atof(argv[4]), interpolator);
	}
	else if (cmd == "--flip") {
		gt.RotateUnkeepImage(srcImage, desImage, atof(argv[4]), interpolator);
	}

	// Xóa biến interpolator
	delete interpolator;

	imwrite("output.jpg", desImage);
	namedWindow("Input Image");
	namedWindow("Output Image");

	imshow("Input Image", srcImage);
	imshow("Output Image", desImage);
	waitKey(0);

	return 0;
}
