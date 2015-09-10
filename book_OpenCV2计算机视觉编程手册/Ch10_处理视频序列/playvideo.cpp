#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

//帧处理的接口
//在面向对象的上下文中，更适合使用帧处理类，而非帧处理函数
class FrameProcessor
{
public:
	//处理方法
	virtual void process(Mat &input, Mat &ouput) = 0;
};

class FeatureTracker:public FrameProcessor
{
private:
	//当前灰度图像
	Mat gray;
	//之前灰度图像
	Mat gray_prev;
	//两幅图像之间的特征点 0->1
	vector<Point2f> points[2];
	//跟踪点的初始位置
	vector<Point2f> initial;
	//检测到的特征
	vector<Point2f> features;
	//需要跟踪的最大特征数目
	int max_count;
	//特征检测中的质量等级
	double qlevel;
	//两点之间的最小距离
	double minDist;
	//检测到的特征的状态
	vector<uchar> status;
	//跟踪过程中的错误
	vector<float> err;
public:
	FeatureTracker():max_count(500), qlevel(0.01), minDist(10.){}

	//是否需要添加新的特征点
	bool addNewPoints()
	{
		//如果点的数量太少
		return points[0].size() <= 10;
	}

	//检测特征点
	void detectFeaturePoints()
	{
		//检测特征
		goodFeaturesToTrack(gray,		//图像
				features,								//检测到的特征
				max_count,						//特征的最大数目
				qlevel,									//质量等级
				minDist);								//两个特征之间的最小距离
	}

	//决定哪些点应该跟踪
	bool acceptTrackedPoint(int i)
	{
		return status[i] &&
				//如果它移动了
				((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
	}

	//处理当前跟踪的点
	void handleTrackedPoints(Mat &frame, Mat &output)
	{
		//遍历所有跟踪点
		for (int i = 0; i < points[1].size(); i++)
		{
			//绘制直线与圆
			line(output,
					initial[i],		//初始位置
					points[1][i],//新位置
					Scalar(255, 255, 255));
			circle(output, points[1][i], 3, Scalar(255, 255, 255), -1);
		}
	}
	void process(Mat &frame, Mat &output)
	{
		//转换为灰度图像
		cvtColor(frame, gray, CV_BGR2GRAY);
		frame.copyTo(output);
		//1.如果需要添加新的特征点
		if (addNewPoints())
		{
			//进行检测
			detectFeaturePoints();
			//添加检测到的特征到当前跟踪的特征中
			points[0].insert(points[0].end(), features.begin(), features.end());
			initial.insert(initial.end(), features.begin(), features.end());
		}
		//对于序列中的第一幅图像
		if (gray_prev.empty())
		{
			gray.copyTo(gray_prev);
		}
		//2.跟踪特征点
		calcOpticalFlowPyrLK(
				gray_prev, gray,	//两幅连续图
				points[0],				//图1中的输入点坐标
				points[1],				//图2中的输入点坐标
				status,						//跟踪成功
				err);							//跟踪失误
		//2.遍历所有跟踪的点进行筛选
		int k = 0;
		for (int i = 0; i < points[1].size(); i++)
		{
			//是否需要保留该点？
			if (acceptTrackedPoint(i))
			{
				//进行保留
				initial[k] = initial[i];
				points[1][k++] = points[1][i];
			}
		}
		//去除不成功的点
		points[1].resize(k);
		initial.resize(k);
		//3.处理接受的跟踪点
		handleTrackedPoints(frame, output);
		//4.当前帧的点和图像变为前一帧的点和图像
		swap(points[1], points[0]);
		swap(gray_prev, gray);
	}
};

class BGFGSegmentor:public FrameProcessor
{
private:
	//当前灰度图像
	Mat gray;
	//累积的背景
	Mat background;
	//背景图像
	Mat backImage;
	//前景图像
	Mat foreground;
	//背景累加中的学习率
	double learningRate;
	//前景提取的阈值
	int thresholdvalue;
public:
	BGFGSegmentor():thresholdvalue(10), learningRate(0.01){}
	void setThreshold(int t)
	{
		thresholdvalue = t;
	}
	//处理方法
	void process(Mat& frame, Mat &output)
	{
		//转换为灰度图像
		cvtColor(frame, gray, CV_BGR2GRAY);
		//初始化背景为第一帧
		if (background.empty())
		{
			gray.convertTo(background, CV_32F);
		}
		//转换背景图像为8U格式
		background.convertTo(backImage, CV_8U);
		//计算差值
		absdiff(backImage, gray, foreground);
		//应用阈值化到前景图像
		threshold(foreground, output,
				thresholdvalue, 255, THRESH_BINARY_INV);
		//对背景累加
		accumulateWeighted(gray, background, learningRate, output);
	}
};

class VideoProcessor{
private:
	//OpenCV视频捕捉对象
	VideoCapture capture;
	//每帧调用的回调函数
	void (*process)(Mat &, Mat &);
	//每帧调用FrameProcessor的回调函数
	FrameProcessor *frameProcessor;
	//确定是否调用回调函数的bool变量
	bool callIt;
	//输入窗口的名称
	string windowNameInput;
	//输出窗口的名称
	string windowNameOutput;
	//延迟
	int delay;
	//已处理的帧数
	long fnumber;
	//在该帧数停止
	long frameToStop;
	//是否停止处理
	bool stop;
	//把图像文件名的数组作为输入
	vector<string> images;
	//图像向量的迭代器
	vector<string>::const_iterator itImg;
	//OpenCV的写视频对象
	VideoWriter writer;
	//输出文件名称
	string outputFile;
	//输出图像的当前索引
	int currentIndex;
	//输出图像名称中的数字位数
	 int digits;
	 //输出图像的扩展名
	 string extension;
public:
	VideoProcessor():callIt(true), delay(0), fnumber(0), stop(false), frameToStop(-1){}
	//设置输入的图像向量
	bool setInput(const vector<string> &imgs)
	{
		fnumber = 0;
		capture.release();
		images = imgs;
		itImg = images.begin();
		return true;
	}
	void setFrameProcessor(void(*frameProcessingCallback)(Mat &, Mat &))
	{
		frameProcessor = 0;
		//重新设置回调函数
		process = frameProcessingCallback;
		callProcess();
	}
	void setFrameProcessor(FrameProcessor *frameProcessorPtr)
	{
		//使回调函数无效化
		process = 0;
		//重新设置FrameProcessor实例
		frameProcessor = frameProcessorPtr;
		callProcess();
	}
	//设置视频文件的名称
	bool setInput(string filename)
	{
		fnumber = 0;
		capture.release();
		return capture.open(filename);
	}
	void setDelay()
	{
		delay = 1000./capture.get(CV_CAP_PROP_FPS);
	}
	void setDelay(int delaytime)
	{
		delay = delaytime;
	}
	//设置输出视频文件
	//默认使用与输入视频相同的参数
	bool setOutput(const string &filename, int codec = 0, double framerate = 0.0, bool isColor = false)
	{
		outputFile = filename;
		extension.clear();
		if (framerate == 0.0)
		{
			framerate = capture.get(CV_CAP_PROP_FPS);
		}
		char c[4];
		//使用相同的编码格式
		if (codec == 0)
		{
			codec = getCodec(c);
		}
		Size frameSize;
		frameSize.height = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
		frameSize.width = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_WIDTH));
		//打开输出视频
		return writer.open(outputFile,	//文件名
				codec,										//将使用的编码格式
				framerate,								//帧率
				frameSize,								//尺寸
				isColor);									//是否是彩色视频
	}
	//设置输出视频文件
	//默认使用与输入视频相同的参数
	bool setOutput(const string &filename,	//前缀
			const string &ext,									//后缀
			int numberOfDigits = 3,						//数字位数
			int startIndex = 0)									//开始索引
	{
		//数字位数必须是正的
		if (numberOfDigits < 0)
		{
			return false;
		}
		//文件名及后缀
		outputFile = filename;
		extension = ext;
		//文件名命名中的数字位数
		digits = numberOfDigits;
		//开始索引
		currentIndex = startIndex;
		return true;
	}
	//得到输入视频的编解码
	int getCodec(char codec[4])
	{
		//未指定图像数组
		if (images.size() != 0)//TODO: youwenti
		{
			return -1;
		}
		union
		{
			//4-char编码的数据结果
			int value;
			char code[4];
		}returned;
		returned.value = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
		//得到4个字符
		codec[0] = returned.code[0];
		codec[1] = returned.code[1];
		codec[2] = returned.code[2];
		codec[3] = returned.code[3];
		cout<<"Codec:"<<codec[0]<<codec[1]<<codec[2]<<codec[3]<<endl;
		//返回对应的整数
		return returned.value;
	}
	//创建输入窗口
	void displayInput(string wn)
	{
		windowNameInput = wn;
		namedWindow(windowNameInput);
	}
	//创建输出窗口
	void displayOutput(string wn)
	{
		windowNameOutput = wn;
		namedWindow(windowNameOutput);
	}
	//不再显示处理后的帧
	void dontDisplay()
	{
		destroyWindow(windowNameInput);
		destroyWindow(windowNameOutput);
		windowNameInput.clear();
		windowNameOutput.clear();
	}

	bool isOpened()
	{
		return capture.isOpened() || !images.empty();
	}
	bool isStopped()
	{
		return stop;
	}
	bool readNextFrame(Mat &frame)
	{
		if (images.size() == 0)
		{
			return capture.read(frame);
		}
		else
		{
			if (itImg != images.end())
			{
				frame = imread(*itImg);
				itImg++;
				return frame.data != 0;
			}
			else
			{
				return false;
			}
		}
	}
	//输出帧
	//可能是：视频文件或者图像文件
	void writeNextFrame(Mat &frame)
	{
		if (extension.length())
		//我们输出到图像文件
		{
			stringstream ss;
			//组成输出文件
			ss << outputFile << setfill('0') <<setw(digits) << currentIndex++ << extension;
			imwrite(ss.str(), frame);
		}
		else
		{
			writer.write(frame);
		}
	}
	void stopAtFrameNo(long frame)
	{
		frameToStop = frame;
	}
	void callProcess()
	{
		callIt = true;
	}
	void dontCallProcess()
	{
		callIt = false;
	}
	//返回下一帧的帧数
	long getFrameNumber()
	{
		long fnumber = static_cast<long>(capture.get(CV_CAP_PROP_POS_FRAMES));
		return fnumber;
	}

	void stopIt()
	{
		stop = true;
	}
	//获取并处理序列帧
	void run()
	{
		Mat frame;
		Mat output;
		if (!isOpened())
			return;
		stop = false;
		while (!isStopped())
		{
			//读取下一帧
			if (!readNextFrame(frame))
			{
				break;
			}
			//显示输出帧
			if (windowNameInput.length() != 0)
			{
				imshow(windowNameInput, frame);
			}
			//调用处理函数
			if (callIt)
			{
				//处理当前帧
				if (process)//如果是回调函数
				{
					process(frame, output);
				}
				else if (frameProcessor)//如果是帧处理实例
				{
					frameProcessor->process(frame, output);
				}
				//增加帧数
				fnumber++;
			}
			else
			{
				output = frame;
			}
			//输出图像序列
			if (outputFile.length() != 0)
			{
				writeNextFrame(output);
			}
			//显示输出帧
			if (windowNameOutput.length() != 0)
			{
				imshow(windowNameOutput, output);
			}
			//引入延迟
			if (delay >=0 && waitKey(delay) >= 0)
			{
				stopIt();
			}
			if (frameToStop >= 0 && getFrameNumber() == frameToStop)
			{
				stopIt();
			}
		}
	}
};

void playvideo(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("Usage: playvideo [videoname]\n");
		return;
	}
	//打开视频文件
	VideoCapture capture(argv[1]);
	//检查视频是否成功打开
	if (!capture.isOpened())
		return;
	//获取帧率
	double rate = capture.get(CV_CAP_PROP_FPS);
	bool stop(false);
	Mat frame;	//当前视频帧
	namedWindow("Extract Frame");
	//每一帧之间的延迟
	//与视频的帧率相对应
	int delay = 1000 / rate;
	//遍历每一帧
	while (!stop)
	{
		if (!capture.read(frame))
			break;
		imshow("Extract Frame", frame);
		//引入延迟
		//也可通过按键停止
		if (waitKey(delay) >= 0)
		{
			stop = true;
		}

	}
	//关闭视频文件
	//将由析构函数调用，因此非必须
	capture.release();
}



void canny(Mat &img, Mat &out)
{
	if (img.channels() == 3)
	{
		cvtColor(img, out, CV_BGR2GRAY);
		Canny(out, out, 100, 200);
		threshold(out, out, 128, 255, THRESH_BINARY_INV);
	}
}
int main(int argc, char** argv) {
	//playvideo(argc, argv);

	if (0) //处理视频并将其写入输出文件
	{
		VideoProcessor processor;
		processor.setInput(argv[1]);
		processor.displayInput("Current Frame");
		processor.displayOutput("Output Frame");
		processor.setDelay();
		processor.setFrameProcessor(canny);
		//processor.setOutput("xiaoxiao.avi");
		processor.setOutput("xiaoxiao", ".jpg");
		processor.run();
	}

	if (0)//特征点追踪
	{
		//创建视频处理器实例
		VideoProcessor processor;
		//创建特征跟踪器实例
		FeatureTracker tracker;
		//打开视频文件
		processor.setInput(argv[1]);
		//设置帧处理器对象
		processor.setFrameProcessor(&tracker);
		//声明显示窗口
		processor.displayOutput("Tracked Features");
		//以原始帧率播放视频
		processor.setDelay();
		//开始处理过程
		processor.run();
	}

	if (0)//前景检测
	{
		//创建视频处理器实例
		VideoProcessor processor;
		//创建背景/前景分类器
		BGFGSegmentor segmentor;
		segmentor.setThreshold(25);
		//打开视频文件
		processor.setInput(argv[1]);
		//设置帧处理器对象
		processor.setFrameProcessor(&segmentor);
		//声明显示窗口
		processor.displayOutput("Tracked Features");
		//以原始帧率播放视频
		processor.setDelay();
		//开始处理过程
		processor.run();
	}
	if (1)//混合高斯模型前景检测
	{
		VideoCapture capture(argv[1]);
		//检查视频是否成功打开
		if (!capture.isOpened())
		{
			return 0;
		}
		//当前视频帧
		Mat frame;
		//前景图像
		Mat foreground;
		namedWindow("Extracted Foreground");
		//使用默认参数的Mixture of Gasussian对象
		BackgroundSubtractorMOG mog;
		bool stop(false);
		while (!stop)
		{
			//读取下一帧
			if (!capture.read(frame))
			{
				break;
			}
			//更新背景并返回前景
			mog(frame, foreground, 0.01);
			//对图像取反
			threshold(foreground, foreground, 128, 255, THRESH_BINARY_INV);
			//显示前景
			imshow("Extracted Foreground", foreground);
			//引入延迟
			//也可通过按键停止
			if (waitKey(10) >= 0)
			{
				stop = true;
			}
		}
	}
	return (0);

}

