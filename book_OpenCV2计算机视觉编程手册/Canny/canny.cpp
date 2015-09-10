/*简单的二值滤波有两大缺陷：
 * 一：检测到的边缘过粗，难以实现物体的精确定位
 * 二： 难以找到合适的阈值， 足够低以检测重要边缘，足够高避免包含过多次要边缘
 */

#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <iostream>

#define PI 3.1415926
using namespace cv;
using namespace std;

//概率霍夫变换 Probabilistic Hough Transform
class LineFinder{
private:
	//原图
	Mat img;
	//向量中包含检测到的直线的断点
	vector<Vec4i> lines;
	//累加器的分辨率
	double deltaRho;
	double deltaTheta;
	//直线被接受时所需的最小投票数
	int minVote;
	//直线的最小长度
	double minLength;
	//沿着直线方向的最大缺口
	double maxGap;
public:
	//默认的累加器的分辨率为单个像素 及 1度
	//不设置缺口及最小长度的值
	LineFinder():deltaRho(1), deltaTheta(PI / 180), minVote(10), minLength(0.), maxGap(0.){}
	//设置累加器的分辨率
	void setAccResolution(double dRho, double dTheta)
	{
		deltaRho = dRho;
		deltaTheta = dTheta;
	}
	//设置最小投票数
	void setMinvote(int minv)
	{
		minVote = minv;
	}
	//设置缺口及最小长度
	void setLineLengthAndGap(double length, double gap)
	{
		minLength = length;
		maxGap = gap;
	}
	//使用概率霍夫变换
	vector<Vec4i> findLines(Mat &binary)
	{
		lines.clear();
		HoughLinesP(binary, lines, deltaRho, deltaTheta, minVote, minLength, maxGap);
		return lines;
	}
	void drawDetectedLines(Mat &image, Scalar color = Scalar(255, 255, 255))
	{
		//画线
		vector<Vec4i>::const_iterator it2 = lines.begin();
		while (it2 != lines.end())
		{
			Point pt1((*it2)[0], (*it2)[1]);
			Point pt2((*it2)[2], (*it2)[3]);
			line(image, pt1, pt2, color);
			++it2;
		}
	}
};

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("Please input the image name.\n");
		return 1;
	}
	Mat image = imread(argv[1]);
	if (!image.data)
	{
		printf("Image doesn't exist.\n");
		return 1;
	}
	//应用Canny算法
	Mat contours;
	Canny(image,			//灰度图
			contours,		//输出轮廓
			125,					//低阈值
			350);				//高阈值
	Mat contoursInv;	//反转后的图像
	threshold(contours, contoursInv, 128, 255, THRESH_BINARY_INV);
	namedWindow("Canny contours");
	imshow("Canny contours", contoursInv);

	/*
	//Hough变换检测直线
	vector<Vec2f> lines;
	//检测到的是直线，而非线段
	HoughLines(contours, lines,
			1, PI/180,	//步进尺寸：以1为间隔搜索所有可能的半径，以pi/180为间隔搜索所有可能的角度
			80);				//最小投票数
	vector<Vec2f>::const_iterator it = lines.begin();
	while (it != lines.end())
	{
		float rho = (*it)[0];
		float theta = (*it)[1];
		if (theta < PI/4. || theta > 3.*PI/4.)//垂直线
		{
			//线与第一行的交点
			Point pt1(rho / cos(theta), 0);
			//线与最后一行的交点
			Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
			//绘制白线
			line(image, pt1, pt2, Scalar(255), 1);
		}
		else //水平线
		{
			//线与第一列的交点
			Point pt1(0, rho / sin(theta));
			//线与最后一列的交点
			Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
			//绘制白线
			line(image, pt1, pt2, Scalar(255), 1);
		}
		++it;
	}
	namedWindow("Detected Lines with Hough");
	imshow("Detected Lines with Hough", image);
	*/

	LineFinder finder;
	finder.setLineLengthAndGap(100, 20);
	finder.setMinvote(80);
	vector<Vec4i> lines = finder.findLines(contours);
	finder.drawDetectedLines(image);
	namedWindow("Detected Lines with HoughP");
	imshow("Detected Lines with HoughP", image);

	int n = 0;
	//黑色图像
	Mat oneline(contours.size(), CV_8U, Scalar(0));
	//白色直线
	line(oneline, Point(lines[n][0], lines[n][1]), Point(lines[n][2], lines[n][3]), Scalar(255), 5);
	//轮廓与白线进行AND操作
	bitwise_and(contours, oneline, oneline);
	namedWindow("One line");
	imshow("One line", oneline);

	vector<Point> points;
	for (int y = 0; y < oneline.rows; y++)
	{
		uchar *rowPtr = oneline.ptr<uchar>(y);
		for (int x = 0; x < oneline.cols; x++)
		{
			if (rowPtr[x])
			{
				points.push_back(Point(x, y));
			}
		}
	}

	Vec4f linel;
	fitLine(Mat(points), linel, CV_DIST_L2, 0, 0.01, 0.01);
	int x0 = linel[2];
	int y0 = linel[3];
	int x1 = x0 - 200 * linel[0];
	int y1 = y0 - 200 * linel[1];
	image = imread(argv[1], 0);
	line(image, Point(x0, y0), Point(x1, y1), Scalar(0), 3);
	namedWindow("Estimated line");
	imshow("Estimated line", image);

	waitKey();

	return 0;
}
