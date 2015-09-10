#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class WatershedSegmenter
{
	private:
		Mat markers;
	public:
		void setMarkers(const Mat& marketImage)
		{
			marketImage.convertTo(markers, CV_32S);
		}
		Mat process(const Mat &image)
		{
			watershed(image, markers);
			//转换:每个像素都为255p+255(p为像素值)
			markers.convertTo(markers,CV_8U, 255, 255);
			return markers;
		}
};

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Please input the image name.\n");
		return 1;
	}
	Mat imageOrigin = imread(argv[1]);
	if (imageOrigin.rows == 0 && imageOrigin.cols == 0)
	{
		printf("Image doesn't exist.\n");
		return 1;
	}

	Mat image;
	cvtColor(imageOrigin, image, CV_RGB2GRAY);
	printf("image_channels:%d\n", image.channels());
	namedWindow("image");
	imshow("image", image);

	Mat binary;
	threshold(image, binary, 100, 255, THRESH_BINARY);

	//移除噪点与微小物体
	Mat fg;
	erode(binary, fg, Mat(), Point(-1, -1), 6);
	namedWindow("Foreground image");
	imshow("Foreground image", fg);

	//识别不包含物体的像素
	Mat bg;
	dilate(binary, bg, Mat(), Point(-1, -1), 6);
	threshold(bg, bg, 1, 128, THRESH_BINARY_INV);
	namedWindow("Background image");
	imshow("Background image", bg);
	imwrite("cowCounter.jpg", bg);

	//创建标记图像
	Mat markers(binary.size(), CV_8U, Scalar(0));
	markers = fg + bg;
	namedWindow("Markers");
	imshow("Markers", markers);
	markers.convertTo(markers, CV_32S);
	
	imageOrigin.convertTo(imageOrigin, CV_8UC3);
	
	//获取分割结果
	Mat result;
	WatershedSegmenter segmenter;
	segmenter.setMarkers(markers);
	result = segmenter.process(imageOrigin);
	namedWindow("Segmentation");
	imshow("Segmentation", result);
	
	threshold(result, result, 0, 255, THRESH_BINARY);
	namedWindow("Watersheds");
	imshow("Watersheds", result);
	waitKey();
	return 0;
}
