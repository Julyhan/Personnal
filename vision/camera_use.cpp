#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main()
{
    Mat image, image_3c;
    image.create(Size(256, 256), CV_8UC1);
    image_3c.create(Size(256, 256), CV_8UC3);   //3通道的图像
    image.setTo(0);
    image_3c.setTo(0);

    //image.at<uchar>(10, 200) = 255; //使用at函数的地方，用的是10,200
    Point point(20, 100);
    image.at<uchar>(20, 100) = 250;//使用at函数的地方，用的是Point(10,200)

    image_3c.at<uchar>(10, 300) = 255;
    image_3c.at<uchar>(10, 302) = 254;
    Point point_3c(20, 200);
    image_3c.at<uchar>(point_3c) = 250;

    double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
    Point maxLoc;
    minMaxLoc(image, NULL, &maxVal, NULL, &maxLoc);
    cout << "单通道图像最大值： " << maxVal << endl;
    double min_3c, max_3c;
    //注意多通道在使用minMaxLoc()函数是不能给出其最大最小值坐标的，因为每个像素点其实有多个坐标，所以是不会给出的
    minMaxLoc(image_3c, &min_3c, &max_3c, NULL, NULL);
    cout << "3通道图像最大值: " << max_3c << endl;

    imshow("image", image);
    //imshow("image_3c", image_3c);
    waitKey(0);
    return 0;
}
