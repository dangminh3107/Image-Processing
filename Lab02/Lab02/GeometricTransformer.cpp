#include "stdafx.h"
#include "GeometricTransformer.h"
#define pi 3.14159265358979323846

uchar PixelInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int channel, uchar* pDes) {
	int nChannels = (channel > 0) ? 3 : 1;
	int x = round(tx);
	int y = round(ty);

	return pSrc[y * srcWidthStep + x * nChannels + ((channel > 0) ? channel - 1 : 0)];
}

PixelInterpolate::PixelInterpolate(){}

PixelInterpolate::~PixelInterpolate(){}

//----------------------------------------------

uchar BilinearInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels, uchar* pDes) {
	//tọa độ của 4 điểm gần nhất của (tx, ty)
	int neighbor[4][2] = { 
						   {floor(tx),floor(ty)},
						   {floor(tx), ceil(ty)},
						   {ceil(tx), floor(ty)},
						   {ceil(tx), ceil(ty)} 
						 };
	
	int color_value[4][3]; //giá trị màu r, g, b của 4 điểm lân cận

	uchar* point = pSrc;
	for (int neightbor_coo = 0; neightbor_coo <= 3; neightbor_coo++) {
		//point = pSrc;
		point += neighbor[neightbor_coo][1] * srcWidthStep + neighbor[neightbor_coo][0] * nChannels;

		color_value[neightbor_coo][0] = uchar(point[0]); //blue
		color_value[neightbor_coo][1] = uchar(point[1]); //green
		color_value[neightbor_coo][2] = uchar(point[2]); //red
	}
	
	//khoảng cách 
	float a = tx - neighbor[0][0];
	float b = ty - neighbor[0][1];

	float blue = b * a * color_value[0][0] + b * (1 - a) * color_value[1][0] + (1 - b) * (1 - a) * color_value[2][0] + (1 - b) * a * color_value[3][0];
	float green = b * a * color_value[0][1] + b * (1 - a) * color_value[1][1] + (1 - b) * (1 - a) * color_value[2][1] + (1 - b) * a * color_value[3][1];
	float red = b * a * color_value[0][2] + b * (1 - a) * color_value[1][2] + (1 - b) * (1 - a) * color_value[2][2] + (1 - b) * a * color_value[3][2];

	pDes[0] = floor(blue);
	pDes[1] = floor(green);
	pDes[2] = floor(red);

	return 0;
}

BilinearInterpolate::BilinearInterpolate(){}

BilinearInterpolate::~BilinearInterpolate(){}

//------------------------------------------------------
float Distance(float x1, float y1, float x2, float y2) {
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

uchar NearestNeighborInterpolate::Interpolate(float tx, float ty, uchar* pSrc, int srcWidthStep, int nChannels, uchar* pDes) {
	//tọa độ của 4 điểm gần nhất của (tx, ty)
	int neighbor[4][2] = {
						   {floor(tx),floor(ty)},
						   {floor(tx), ceil(ty)},
						   {ceil(tx), floor(ty)},
						   {ceil(tx), ceil(ty)}
						 };

	uchar* point = pSrc;
	int get_index; //biến dùng để lấy tọa độ gần nhất
	float min_distance = 1000000000;
	for (int neightbor_coo = 0; neightbor_coo <= 3; neightbor_coo++) {
		if (neighbor[neightbor_coo][0] >= 0 && neighbor[neightbor_coo][1] >= 0) {
			float distance = Distance(tx, ty, neighbor[neightbor_coo][0], neighbor[neightbor_coo][1]);
			if (distance < min_distance) {
				min_distance = distance;
				get_index = neightbor_coo;
			}
		}
	}
	if (min_distance == 1000000000) {
		pDes[0] = 0;
		pDes[1] = 0;
		pDes[2] = 0;
	}
	else {	
		point += neighbor[get_index][1] * srcWidthStep + neighbor[get_index][0] * nChannels;
		pDes[0] = point[0];
		pDes[1] = point[1];
		pDes[2] = point[2];
	}

	return 0;
}

NearestNeighborInterpolate::NearestNeighborInterpolate(){}

NearestNeighborInterpolate::~NearestNeighborInterpolate(){}

//--------------------------------------------------
void AffineTransform::Translate(float dx, float dy) {
	float data[9] = { 1,0,dx,0,1,dy,0,0,1 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i * 3 + j];
		}
	}
}
void AffineTransform::Rotate(float angle) {
	float data[9] = { cos(angle), -sin(angle), 0, sin(angle), cos(angle), 0.0, 0.0, 0.0, 1.0 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i * 3 + j];
		}
	}
}
void AffineTransform::Scale(float sx, float sy) {
	float data[9] = { sx, 0.0, 0.0, 0.0, sy, 0.0, 0.0, 0.0, 1.0 };
	_matrixTransform = Mat(3, 3, CV_32F);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			_matrixTransform.at<float>(i, j) = data[i * 3 + j];
		}
	}
}
void AffineTransform::TransformPoint(float &x, float &y) {
	float v, w;
	v = x * _matrixTransform.at<float>(0, 0) + y * _matrixTransform.at<float>(0, 1) + _matrixTransform.at<float>(0, 2);
	w = x * _matrixTransform.at<float>(1, 0) + y * _matrixTransform.at<float>(1, 1) + _matrixTransform.at<float>(1, 2);
	x = v;
	y = w;
}

AffineTransform::AffineTransform(){}

AffineTransform::~AffineTransform(){}
//--------------------------------------------------
int GeometricTransformer::Transform(const Mat &beforeImage,
									Mat &afterImage,
									AffineTransform* transformer,
									PixelInterpolate* interpolator) {
	uchar* pSrc = (uchar*)beforeImage.data;
	uchar* pDes = (uchar*)afterImage.data;
	int width = afterImage.cols; //số cột của ảnh --> chiều rộng
	int height = afterImage.rows; //số hàng của ảnh --> chiều cao
	int nChannels = afterImage.channels(); //số kênh màu của ảnh

	//khoảng cách tính theo byte giữa 2 pixel cùng cột trên 2 dòng kế tiếp của ảnh trước và sau
	int before_widthStep = beforeImage.step[0]; 
	int after_widthStep = afterImage.step[0];
	
	for (int yDes = 0; yDes < height; yDes++, pDes += after_widthStep) {
		uchar* pRowDes = pDes; //lấy con trỏ đầu mỗi dòng của ảnh đích
		uchar* point;
		for (int xDes = 0; xDes < width; xDes++, pRowDes += nChannels) {
			float xSrc = xDes, ySrc = yDes;

			transformer->TransformPoint(xSrc, ySrc);
			point = pSrc;

			if (int(xSrc) == xSrc && int(ySrc) == ySrc) {
				point += int(ySrc)*before_widthStep + int(xSrc)*nChannels;
				pRowDes[0] = point[0];
				pRowDes[1] = point[1];
				pRowDes[2] = point[2];
			}

			else
				interpolator->Interpolate(xSrc, ySrc, point, before_widthStep, nChannels, pRowDes);
		}
	}

	return 0;
}

int GeometricTransformer::RotateKeepImage(const Mat &srcImage, Mat &dstImage, 
										  float angle, PixelInterpolate* interpolator) {
	int width = srcImage.cols;
	int height = srcImage.rows;
	int new_width = width * cos(angle*pi / 180) + height * sin(angle*pi / 180);
	int	new_height = height * cos(angle*pi / 180) + width * sin(angle*pi / 180);

	//dstImage = Mat(new_width, new_height, CV_8UC3, Scalar(0));
	AffineTransform* trans = new AffineTransform;
	trans->Rotate(angle);

	int xoffset = (new_width - width) / 2;
	int yoffset = (new_height - height) / 2;

	//dstImage = Mat(new_width, new_height, srcImage.type());
	dstImage = Mat(new_width, new_height, CV_8UC3, Scalar(0));
	Mat frame= Mat(new_width, new_height, srcImage.type());

	//chỉnh ảnh về giữa màn hình
	srcImage.copyTo(frame.colRange(xoffset, xoffset + width).rowRange(yoffset, yoffset + height));

	uchar* pData = (uchar*)frame.data;
	uchar* pDst = (uchar*)dstImage.data;
	int nchannels = srcImage.channels();
	int frame_widthStep = frame.step[0];
	int dst_widthStep = dstImage.step[0];

	// Lấy dòng và cột ảnh dstImage
	int dstHeight = dstImage.rows;
	int dstWidth = dstImage.cols;

	// Lấy nửa dòng và cột ảnh dstImage
	int rowHalf = dstHeight / 2;
	int colHalf = dstWidth / 2;
	for (int y2 = -rowHalf; y2 < rowHalf; y2++, pDst += dst_widthStep)
	{
		uchar* pRow2 = pDst;
		uchar* pPoint;
		float x1, y1;
		for (int x2 = -colHalf; x2 < colHalf; x2++, pRow2 += nchannels){
			x1 = x2;
			y1 = y2;
			trans->TransformPoint(x1, y1);
			x1 += colHalf;
			y1 += rowHalf;
			if (x1 < 0 || x1 >= dstWidth || y1 < 0 || y1 >= dstHeight)
				continue;
			pPoint = pData;
			if (int(x1) == x1 && int(y1) == y1){
				pPoint += int(y1) *frame_widthStep + int(x1) *nchannels;
				pRow2[0] = pPoint[0];
				pRow2[1] = pPoint[1];
				pRow2[2] = pPoint[2];
			}
			else
				interpolator->Interpolate(x1, y1, pPoint, frame_widthStep, nchannels, pRow2);
		}
	}
	delete trans;
	return 1;
}

int GeometricTransformer::RotateUnkeepImage(
	const Mat &srcImage, Mat &dstImage, float angle, PixelInterpolate* interpolator) {

	int width = srcImage.cols;
	int height = srcImage.rows;
	dstImage = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	AffineTransform* transformer = new AffineTransform;
	transformer->Rotate(angle);

	Mat frame = srcImage;

	uchar* pData1 = (uchar*)frame.data;
	uchar* pData2 = (uchar*)dstImage.data;
	int nchannels = srcImage.channels();
	int widthStep1 = frame.step[0];
	int widthStep2 = dstImage.step[0];

	// Lấy dòng và cột ảnh dstImage
	int rows = dstImage.rows;
	int cols = dstImage.cols;

	// Lấy nửa dòng và cột ảnh dstImage
	int rowHalf = rows / 2;
	int colHalf = cols / 2;

	for (int y2 = -rowHalf; y2 < rowHalf; y2++, pData2 += widthStep2){
		uchar* pRow2 = pData2;
		uchar* pPoint;
		float x1, y1;
		for (int x2 = -colHalf; x2 < colHalf; x2++, pRow2 += nchannels){
			x1 = x2;
			y1 = y2;
			transformer->TransformPoint(x1, y1);
			x1 += colHalf;
			y1 += rowHalf;
			if (x1 < 0 || x1 > cols || y1 < 0 || y1 > rows)
				continue;
			pPoint = pData1;
			if (int(x1) == x1 && int(y1) == y1){
				pPoint += int(y1) *widthStep1 + int(x1) *nchannels;
				pRow2[0] = pPoint[0];
				pRow2[1] = pPoint[1];
				pRow2[2] = pPoint[2];
			}
			else
				interpolator->Interpolate(x1, y1, pPoint, widthStep1, nchannels, pRow2);
		}
	}

	delete transformer;
	return 1;
}


int GeometricTransformer::Scale(
	const Mat &srcImage,
	Mat &dstImage,
	float sx, float sy,
	PixelInterpolate* interpolator) {

	int width = srcImage.cols;
	int height = srcImage.rows;
	dstImage = Mat(height*sy, width*sx, CV_8UC3, Scalar(0, 0, 0));
	AffineTransform *trans =  new AffineTransform;
	trans->Scale(1 / sx, 1 / sy);
	Transform(srcImage, dstImage, trans, interpolator);
	delete trans;
	return 1;
}

//void changePixelHorizontally(int rows, int cols) {
//	Mat srcImage, dstImage;
//	int width = srcImage.cols;
//	int height = srcImage.rows;
//
//	//lật ảnh theo chiều ngang, trục đối xứng là cols / 2
//	int colsHalf = width / 2;
//	for (int x = 0; x < colsHalf; x++)
//		for (int y = 0; y < height; y++)
//			dstImage.at<uchar>(y, width - 1 - x) = srcImage.at<uchar>(y, x);
//
//	for (int x = colsHalf; x < width; x++)
//		for (int y = 0; y < height; y++)
//			dstImage.at<uchar>(y, width - 1 - x) = srcImage.at<uchar>(y, x);
//}
//		
//
//void changePixelVertically(int rows, int cols) {
//	Mat srcImage, dstImage;
//	int width = srcImage.cols;
//	int height = srcImage.rows;
//	//lật ảnh theo chiều dọc, trụ đối xứng là rows / 2
//	int rowsHalf = height / 2;
//	for (int y = 0; y < rowsHalf; y++) 
//		for (int x = 0; x < width; x++) 
//			dstImage.at<uchar>(height - 1 - y, x) = srcImage.at<uchar>(y, x);
//	
//	for (int y = rowsHalf; y < height; y++) 
//		for (int x = 0; x < width; x++) 
//			dstImage.at<uchar>(height - 1 - y, x) = srcImage.at<uchar>(y, x);
//}

int GeometricTransformer::Flip(const Mat &srcImage, Mat &dstImage,
	bool direction, PixelInterpolate* interpolator) {
	int width = srcImage.cols;
	int height = srcImage.rows;
	dstImage = Mat(height, width, CV_8UC3, Scalar(0));

	//AffineTransform *trans = new AffineTransform;

	//lật ảnh theo chiều ngang, trục đối xứng là cols / 2
	if (direction == 1) {
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++) {
				Vec3b color2 = dstImage.at<Vec3b>(Point(x, y));
				Vec3b color1 = srcImage.at<Vec3b>(Point((width - 1 - x), (height - 1 - y)));
				color2.val[0] = color1.val[0];
				color2.val[1] = color1.val[1];
				color2.val[2] = color1.val[2];
				dstImage.at<Vec3b>(Point(x, y)) = color1;
			}
	}

	//lật ảnh theo chiều dọc, trụ đối xứng là rows / 2
	if (direction == 0) {
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++) {
				Vec3b color2 = dstImage.at<Vec3b>(Point(x, y));
				Vec3b color1 = srcImage.at<Vec3b>(Point((width - 1 - x), y));
				color2.val[0] = color1.val[0];
				color2.val[1] = color1.val[1];
				color2.val[2] = color1.val[2];
				dstImage.at<Vec3b>(Point(x, y)) = color1;
			}
	}
	return 0;
}

//int GeometricTransformer::Flip(const Mat &srcImage, Mat &dstImage,
//							   bool direction, PixelInterpolate* interpolator) {
//	int width = srcImage.cols;
//	int height = srcImage.rows;
//	dstImage = Mat(height, width, CV_8UC3, Scalar(0));
//
//	//AffineTransform *trans = new AffineTransform;
//	
//	//lật ảnh theo chiều ngang, trục đối xứng là cols / 2
//	if (direction == 1) {
//		int colsHalf = width / 2;
//		for (int x = 0; x < colsHalf; x++)
//			for (int y = 0; y < height; y++)
//				dstImage.at<uchar>(y, width - 1 - x) = srcImage.at<uchar>(y, x);
//
//		for (int x = colsHalf; x < width; x++)
//			for (int y = 0; y < height; y++)
//				dstImage.at<uchar>(y, width - 1 - x) = srcImage.at<uchar>(y, x);
//	}
//
//	//lật ảnh theo chiều dọc, trụ đối xứng là rows / 2
//	if (direction == 0) {
//		int rowsHalf = height / 2;
//		for (int y = 0; y < rowsHalf; y++)
//			for (int x = 0; x < width; x++)
//				dstImage.at<uchar>(height - 1 - y, x) = srcImage.at<uchar>(y, x);
//
//		for (int y = rowsHalf; y < height; y++)
//			for (int x = 0; x < width; x++)
//				dstImage.at<uchar>(height - 1 - y, x) = srcImage.at<uchar>(y, x);
//	}
//	
//	return 0;
//}

GeometricTransformer::GeometricTransformer(){}

GeometricTransformer::~GeometricTransformer(){}