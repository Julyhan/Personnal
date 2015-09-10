#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Please input the image name.\n");
		return 1;
	}
	Mat image = imread(argv[1], 0);
	if (!image.data)
	{
		printf("Image doesn't exist.\n");
		return 1;
	}

	vector<vector<Point> > contours;
	findContours(image,
			contours,				//轮廓的数组
			CV_RETR_EXTERNAL,	//获取外轮廓
			CV_CHAIN_APPROX_NONE);	//获取每个轮廓的每个像素
	waitKey();

	//在白色图像上绘制轮廓
	Mat result(image.size(), CV_8U, Scalar(255));
	drawContours(result, contours,
			-1,		//绘制所有轮廓
			Scalar(0),	//颜色为黑色
			2);				//轮廓线的绘制宽度为2
	namedWindow("Contours");
	imshow("Contours", result);

	//移除过长或过短的轮廓
	int cmin = 400;
	int cmax = 2000;
	vector<vector<Point> >::iterator itc = contours.begin();
	while (itc != contours.end())
	{
		if (itc->size() < cmin || itc->size() > cmax)
		{
			itc = contours.erase(itc);
			printf("erase!\n");
		}
		else
		{
			++itc;
		}
	}
	
	Mat result2(image.size(), CV_8U, Scalar(255));
	drawContours(result2, contours,
			-1,		//绘制所有轮廓
			Scalar(0),	//颜色为黑色
			2);				//轮廓线的绘制宽度为2
	namedWindow("Contours on Animals");
	imshow("Contours on Animals", result2);

	waitKey(0);
	return 0;
}
