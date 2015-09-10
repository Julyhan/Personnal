#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
int main( int argc, char **argv)
{
	Mat image = imread(argv[1]);
	//定义前景物体的包围盒 x, y, width, height
	Rect rectangle(364, 328, 518, 164);
	
	Mat result;//分割（四种可能的值）
	Mat bgModel, fgModel;//模型（供内部使用）
	//GrabCut分割
	grabCut(image,				//输入图像
	result,								//分割结果
	rectangle,						//包含前景物体的矩形
	bgModel, fgModel,		//模型
	5,										//迭代次数
	GC_INIT_WITH_RECT);	//使用矩形进行初始化
	
	//得到可能为前景的像素
	compare(result, GC_PR_FGD, result, CMP_EQ);
	//生成输出图像
	Mat foreground(image.size(), CV_8UC3, Scalar(255, 255, 255));
	image.copyTo(foreground, //不复制前景像素
							result);
	//按位与核对第1个位
	result = result & 1;//前景时为1
	
	//生成输出图像
	//图像为全白
	//不复制背景图像
	namedWindow("Foreground objects");
	imshow("Foreground objects", foreground);
	waitKey();
	return 0;
}
