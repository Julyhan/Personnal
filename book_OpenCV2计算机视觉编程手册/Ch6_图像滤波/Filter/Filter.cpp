#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//不存在指定导数阶数的参数，都是二阶导数
class LaplacianZC{
private:
	Mat img;
	Mat laplace;
	int aperture;
public:
	LaplacianZC():aperture(3){}
	//设置卷积核的大小
	void setAperture(int a)
	{
		aperture = a;
	}
	//计算浮点数Laplacian
	Mat computeLaplacian(const Mat &image)
	{
		Laplacian(image, laplace, CV_32F, aperture);
		img = image.clone();
		return laplace;
	}
	//你必须在调用它之前调用computeLaplacian
	Mat getLaplacianImage(double scale = -1.0)
	{
		if (scale < 0)
		{
			double lapmin, lapmax;
			minMaxLoc(laplace, &lapmin, &lapmax);
		}
		Mat laplaceImage;
		laplace.convertTo(laplaceImage, CV_8U, scale, 128);
		return laplaceImage;
	}
	//生产零交叉的二值图像
	Mat getZeroCrossing(float threshold = 1.0)
	{
		Mat_<float>::const_iterator it = laplace.begin<float>() + laplace.step1();
		Mat_<float>::const_iterator itend = laplace.end<float>();
		Mat_<float>::const_iterator itup = laplace.begin<float>();
		//初始化为白色的二值图像
		Mat binary(laplace.size(), CV_8U, Scalar(255));
		Mat_<uchar>::iterator itout = binary.begin<uchar>() + binary.step1();
		//对输入阈值取反
		threshold *= -1.0;
		for (; it != itend; ++it, ++itup, ++itout)
		{
			//如果相邻像素的乘积为负数，那么符号发生改变
			if (*it * *(it - 1) < threshold)
			{
				*itout = 0;
			}
			else if (*it * *itup < threshold)
			{
				*itout = 0;
			}
		}
		return binary;
	}
};

void SobelEdgeDetection(Mat &image)
{
	/*
	Sobel(image,				//输入
				sobel,				//输出
				image_depth,//图像类型
				xorder,yorder,//核的阶数
				kernel_size,	//方形核的大小
				alpha, beta);	//缩放值及偏移量
	Sobel(image, sobelX, CV_8U, 1, 0, 3, 0.4, 128);
	*/
	Mat sobelX, sobelY;
	Sobel(image, sobelX, CV_16S, 1, 0);
	Sobel(image, sobelY, CV_16S, 0, 1);
	Mat sobel;
	sobel = abs(sobelX) + abs(sobelY);

	//搜寻Sobel极大值
	double sobmin, sobmax;
	minMaxLoc(sobel, &sobmin, &sobmax);

	//变换为8位图像
	//sobelImage = -alpha * sobel + 255;
	Mat sobelImage;
	sobel.convertTo(sobelImage, CV_8U, -255./sobmax, 255);
	namedWindow("Sobel image");
	imshow("Sobel image", sobelImage);

	Mat sobelThresholded;
	threshold(sobelImage, sobelThresholded, 100, 255, THRESH_BINARY);
	namedWindow("Binary Sobel Image");
	imshow("Binary Sobel Image", sobelThresholded);
}





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

	Mat result1;
	blur(image, result1, Size(5, 5));
	namedWindow("Mean filtered image");
	imshow("Mean filtered image", result1);

	Mat result2;
	GaussianBlur(image, result2, Size(5, 5), 1.5);
	namedWindow("Gaussian filtered image");
	imshow("Gaussian filtered image", result2);

	//类似resize的函数
	//将图像尺寸减半
	Mat reducedImage;
	pyrDown(image, reducedImage); //pyrUp
	namedWindow("Reduced image");
	imshow("Reduced image", reducedImage);
	Mat resizedImage;
	resize(image, resizedImage, Size(image.cols/3, image.rows/3));
	namedWindow("Resized image");
	imshow("Resized image", resizedImage);

	//中值滤波器对于去除椒盐噪声尤其有用
	//medianBlur(image, result, 5);

	//计算Sobel范式
	Mat sobelX, sobelY;
	Sobel(image, sobelX, CV_16S, 1, 0);
	Sobel(image, sobelY, CV_16S, 0, 1);
	//计算L1范式
	Mat sobel = abs(sobelX) + abs(sobelY);

	SobelEdgeDetection(image);

	/*                      3 * 3                                *
	 *            0            1           0                     *
	 *            1            -4          1                     *
	 *            0            1           0                     */
	 //倾向于使用更大的核
	LaplacianZC laplacian;
	laplacian.setAperture(7);
	Mat flap = laplacian.computeLaplacian(image);
	Mat laplace = laplacian.getLaplacianImage();
	namedWindow("Laplacian image (7*7)");
	imshow("Laplacian image (7*7)", laplace);
	Mat zeroLaplace = laplacian.getZeroCrossing();
	namedWindow("Zero crossing");
	imshow("Zero crossing", zeroLaplace);
	waitKey();
	return 0;
}
