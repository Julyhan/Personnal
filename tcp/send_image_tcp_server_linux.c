#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#define PORT 6666
#define IPADDRESS "127.0.0.3"

#include <opencv2/core/core_c.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
 
int main(int argc,char **argv)
{
	int ser_sockfd,cli_sockfd;
	int err,n;
	unsigned int addlen;
	struct sockaddr_in ser_addr;
	struct sockaddr_in cli_addr;
	char recvline[200], *sendline;

	ser_sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(ser_sockfd==-1)
	{
		printf("socket error:%s\n",strerror(errno));
		return -1;
	}

	//bind
	bzero(&ser_addr,sizeof(ser_addr));
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(IPADDRESS);
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	ser_addr.sin_port=htons(PORT);
	err=bind(ser_sockfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
	if(err==-1)
	{
		printf("bind error:%s\n",strerror(errno));
		return -1;
	}

	//listen
	err=listen(ser_sockfd,5);
	if(err==-1)
	{
		printf("listen error\n");
		return -1;
	}
	printf("listen the port:\n");

	//accept
	addlen=sizeof(struct sockaddr);
	cli_sockfd=accept(ser_sockfd,(struct sockaddr *)&cli_addr,&addlen);
	if(cli_sockfd==-1)
	{
		printf("accept error\n");
	}

	IplImage *img;
	while(1)
	{
		img = cvLoadImage("julie.bmp", 1);
		sendline = img->imageData;
		/*
		IplImage *img2 = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
		int step = 640 * 3;
		cvSetData( img2 , sendline , step);
		cvNamedWindow("Server", CV_WINDOW_AUTOSIZE);
		cvShowImage("Server", img2);
		cvWaitKey(0);*/
		send(cli_sockfd, sendline, 640*480*3, 0);
	}
	CvReleaseImage(&img);
	close(cli_sockfd);

	close(ser_sockfd);

	return 0;
}
