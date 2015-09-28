//=================================================
// Project: siftImages
// Coded by: Julie HAN
//=================================================

// Includes
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include <iostream>

// Namespaces
using namespace cv;
using namespace std;

inline int min(int a, int b)
{
	return a < b ? a : b;
}

inline int max(int a, int b)
{
	return b < a ? a : b;
}
//@ param bool: whether or not to show  the hint information, true:show, false: not show
bool hint = true;

//----------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//@ brief  Rescale images to sizes comparable to VGA size (480x600) to reduce the computation(the aspect ratio of the images should be kept)
//@ param src: input image
//@ param dest: ouput image
//@ param newSize: VGA size (width, height)
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-------------------------------------
void rescale_image_fixed_aspect_ratio(Mat& src, Mat& dest, Size newSize) 
{	

	float aspect_ratios[2] = { (float)newSize.width / src.cols, (float)newSize.height / src.rows };

	Size new_image_size(src.cols, src.rows);

	if (aspect_ratios[0] < aspect_ratios[1]) {
		new_image_size.width = round(new_image_size.width * aspect_ratios[0]);
		new_image_size.height = round(new_image_size.height * aspect_ratios[0]);
	}
	else {
		new_image_size.width = round(new_image_size.width * aspect_ratios[1]);
		new_image_size.height = round(new_image_size.height * aspect_ratios[1]);
	}

	resize(src, dest, new_image_size, 0, 0, 3); 
}


//----------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//@ brief  extrace Y component of one image
//@ param src: input image
//@ param img_y: ouput image(y-componenet of the input image)
//@ param img_yyy: ouput image(all-componenet are y-component of  the input image)
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-------------------------------------
void extrace_y_component(Mat& src, Mat& img_y, Mat& img_yyy)
{
	Mat imgYUV;
	cvtColor(src, imgYUV, CV_BGR2YCrCb);

	vector<Mat> imgYUVset;	
	split(imgYUV, imgYUVset);

	img_y = imgYUVset[0];
	imgYUVset[1] = img_y;
	imgYUVset[2] = img_y;

	merge(imgYUVset, img_yyy);
}


//----------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//@ brief   For each detected keypoint, draw a cross “+” at the location of the key point and a circle around the keypoint whose radius is proportional to the scale of the keypoint
//@ param key_points: input key points
//@ param img: where the key_points draw on
//@ param color: the color scale
//@ param length: the half size of the cross
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-------------------------------------
void points_draw(vector<KeyPoint> &key_points, Mat& img, Scalar &color, unsigned int size) {

	Point2f begin_point, end_point;

	for (size_t i = 0; i < key_points.size(); ++i) {

		// Draw Cross at the location of the keypoint
		begin_point.x = key_points[i].pt.x - size < 0 ? 0 : key_points[i].pt.x;
		end_point.x = key_points[i].pt.x + size < img.cols - 1 ? key_points[i].pt.x : (img.cols - 1);
		begin_point.y = key_points[i].pt.y;
		end_point.y = key_points[i].pt.y;
		line(img, begin_point, end_point, color);

		begin_point.x = key_points[i].pt.x;
		end_point.x = key_points[i].pt.x;
		begin_point.y = key_points[i].pt.y - size < 0 ? 0 : key_points[i].pt.y;
		end_point.y = key_points[i].pt.y + size < img.rows - 1 ? key_points[i].pt.y : (img.rows - 1);
		line(img, begin_point, end_point, color);

		// Draw Circles around keypoint whose radius is proportional to scale of the keypoint
		circle(img, key_points[i].pt, key_points[i].size * .5, color);
	}
}
//----------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// --- MAIN FUNCTION ---
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<-------------------------------------
int main(int argc, char* argv[]) {

	if (argc == 1)
	{
		cout << "No image." << endl <<
			"-Usage: siftImages imagefile1 [, imagefile2, imagefile3,…]." << endl;
		return 0;
	}


	// --- SINGLE INPUT IMAGE ---
	if (argc == 2) {
		Mat img_read, img_resize, img_yyy, img_y, img_sift_keypoints;
		img_read = imread(argv[1], CV_LOAD_IMAGE_UNCHANGED);	
		if (!img_read.data) {
			cout << "Please make sure " << argv[1] << " exists." << endl;
			return -1;
		}

		// --- 1. Rescale the image to a size comparable to VGA size (480(rows) x 600(columns)) to reduce the computation. Note that the aspect ratio of the image should be kept when the image is rescaled ---
		rescale_image_fixed_aspect_ratio(img_read, img_resize, Size(600, 480));
		if (hint)
		{
			cout << "Original Image = " << img_read.rows << "x" << img_read.cols << "," << img_read.channels() << "|" << img_read.type();
			cout << " ---> ";
			cout << "Resized Image = " << img_resize.rows << "x" << img_resize.cols << "," << img_resize.channels() << "|" << img_resize.type() << endl;
		}
		Mat img_merge(img_resize.rows, img_resize.cols * 2, img_resize.type());//Mat doubleImage(leftImage.rows, leftImage.cols * 2, leftImage.type());
		img_resize.copyTo(img_merge(Rect(0, 0, img_resize.cols, img_resize.rows)));


		/// --- 2. Extract SIFT keypoints from the Y component of the image ---
		extrace_y_component(img_resize, img_y, img_yyy);
		SIFT sift;
		vector<KeyPoint> key_points;
		Mat descriptors;
		sift(img_y, Mat(), key_points, descriptors);


		/// --- 3. For each detected keypoint, draw a cross “+” at the location of the key point and a circle around the keypoint whose radius is proportional to the scale of the keypoint ---
		points_draw(key_points, img_yyy, Scalar(0, 255, 255), 3);


		//// --- 5. In the command window, the program should output the number of detected ---
		cout << "No. of detected keypoints = " << key_points.size() << endl;

		img_yyy.copyTo(img_merge(Rect(img_yyy.cols, 0, img_yyy.cols, img_yyy.rows)));


		/// --- 4. Both the original image and the image with highlighted keypoints should be displayed in a window as follow --- 
		namedWindow("Single Image", CV_WINDOW_AUTOSIZE);
		imshow("Single Image", img_merge);
		waitKey();

	}

	// --- MULTIPLE INPUT IMAGE ---
	else //argc>2
	{
		vector<Mat> img_read(argc - 1);

			//, img_resize, img_yyy, img_y, img_sift_keypoints;
		for (size_t i = 1; i < argc; i++)
		{
			img_read[i - 1] = imread(argv[i], CV_LOAD_IMAGE_UNCHANGED);
			if (!img_read[i - 1].data) {
				cout << "Please make sure " << argv[i] << " exists." << endl;
				return -1;
			}
		}
	}
	return 0;
}
