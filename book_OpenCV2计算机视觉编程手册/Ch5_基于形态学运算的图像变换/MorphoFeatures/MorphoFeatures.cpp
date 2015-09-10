#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class MorphoFeatures{
	private:
		//用于生成二值图像的阈值
		int threshold1;
		//角点检测中用到的结构元素
		Mat cross;
		Mat diamond;
		Mat square;
		Mat x;
		
		void applyThreshold(Mat& result)
		{
			if (threshold1 > 0)
			{
				threshold(result, result, threshold1, 255, THRESH_BINARY);
			}
		}

	public:
	MorphoFeatures():threshold1(-1), cross(5, 5, CV_8U, Scalar(0)), diamond(5, 5, CV_8U, Scalar(1)), square(5, 5, CV_8U, Scalar(1)), x(5, 5, CV_8U, Scalar(0))
		{
			//创建十字形元素
			for (int i = 0; i< 5; i++)
			{
				cross.at<uchar>(2, i) = 1;
				cross.at<uchar>(i, 2) = 1;
			}
			//创建菱形元素
			diamond.at<uchar>(0, 0) = 0;
			diamond.at<uchar>(0, 1) = 0;
			diamond.at<uchar>(1, 0) = 0;
			diamond.at<uchar>(4, 4) = 0;
			diamond.at<uchar>(3, 4) = 0;
			diamond.at<uchar>(4, 3) = 0;
			diamond.at<uchar>(4, 0) = 0;
			diamond.at<uchar>(4, 1) = 0;
			diamond.at<uchar>(3, 0) = 0;
			diamond.at<uchar>(0, 4) = 0;
			diamond.at<uchar>(0, 3) = 0;
			diamond.at<uchar>(1, 4) = 0;
			//创建X形元素
			for (int i = 0; i< 5; i++)
			{
				x.at<uchar>(i, i) = 1;
				x.at<uchar>(4 - i, i) = 1;
			}
		}

		void setThreshold(int tempThreshold)
		{
			threshold1 = tempThreshold;
		}

		Mat getEdges(const Mat &image)
		{
			Mat result;
			morphologyEx(image, result, MORPH_GRADIENT, Mat());
			applyThreshold(result);
			return result;
		}

		Mat getCorners(const Mat &image)
		{
			Mat result;
			//十字形膨胀
			dilate(image, result, cross);
			//菱形腐蚀
			erode(result, result, diamond);

			Mat result2;
			//X形膨胀
			dilate(image, result2, x);
			//方行腐蚀
			erode(result2, result2, square);

			//通过对两张图像做差值得到角点图像
			absdiff(result2, result, result);
			//阈值化以得到二值图像
			applyThreshold(result);
			return result;
		}

		void drawOnImage(const Mat& binary, Mat& image)
		{
			Mat_<uchar>::const_iterator it = binary.begin<uchar>();
			Mat_<uchar>::const_iterator itend = binary.end<uchar>();
			//遍历每个像素
			for (int i = 0; it != itend; ++it, ++i)
			{
				if (!*it)
				{
					circle(image, Point(i % image.step, i/image.step), 5, Scalar(255, 0, 0));
				}
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
	Mat image = imread(argv[1], 0);
	if (image.rows == 0 && image.cols == 0)
	{
		printf("Image doesn't exist.\n");
		return 1;
	}
	MorphoFeatures morpho;
	morpho.setThreshold(40);
	Mat edges;
	edges = morpho.getEdges(image);
	namedWindow("morphoedges");
	imshow("morphoedges", edges);
	Mat corners;
	corners = morpho.getCorners(image);
	morpho.drawOnImage(corners, image);
	namedWindow("morphocorners");
	imshow("morphocorners", corners);
	waitKey();
	return 0;
}

