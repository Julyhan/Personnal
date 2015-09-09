#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<fcntl.h>
#include <opencv2/core/core_c.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <netinet/in.h>

//初始参数设置
#define PORT 6666
#define IPADDRESS "192.168.1.12"
#define WIDTH 640
#define HEIGHT 480
#define CHANNEL 3

int main(int argc, char **argv) {
	int sockfd;
	int err, n;
	struct sockaddr_in addr_ser;

	//build connection
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket error\n");
		return -1;
	}

	//connect
	bzero(&addr_ser, sizeof(addr_ser));
	addr_ser.sin_family = AF_INET;
	addr_ser.sin_addr.s_addr = inet_addr(IPADDRESS);//htonl(INADDR_ANY);
	addr_ser.sin_port = htons(PORT);
	err = connect(sockfd, (struct sockaddr *) &addr_ser, sizeof(addr_ser));
	if (err == -1) {
		printf("connect error\n");
		return -1;
	}
	printf("connect with server...\n");

	cvNamedWindow("Result", CV_WINDOW_AUTOSIZE);
	IplImage *img = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, CHANNEL);
	int pixel = WIDTH * HEIGHT * CHANNEL;
	int recvonce = 0;
	int recvtotal = 0;
	while (1) {
		recvonce = 0;
		recvtotal = 0;
		while (recvtotal < pixel) {
			recvonce = recv(sockfd, img->imageData + recvtotal,
					pixel - recvtotal, 0);
			if (recvonce < 0 || recvonce == 0)
				break;
			recvtotal += recvonce;
		}
		cvShowImage("Result", img);
		cvWaitKey(1);
	}
	cvDestroyWindow("Result");

	return 0;
}
