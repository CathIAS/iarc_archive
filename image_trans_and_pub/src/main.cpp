#include "Decoder.h"
#include "imagetransfer.h"
#include "usb.h"
#include "config.h"
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <cstdlib>
#include <signal.h>

using namespace cv;

void cleanUp()
{
	remove("./log.txt");
	remove("./index.txt");
	//	system("sudo rm -rf output_*");
}

void trap(int sig){
	cout << "got signal!" << endl;
	(void) signal(SIGINT, SIG_DFL);
	cleanUp();
	exit(0);
}

int main( int argc, char **argv )
{
	//node "cam_video_publisher" with four publishers
	ros::init(argc, argv, "cam_video_publisher");
	ros::NodeHandle nh;
	image_transport::ImageTransport it_1(nh);
	image_transport::Publisher pub_1 = it_1.advertise("cam_video_publisher_1", 1);
	image_transport::ImageTransport it_2(nh);
	image_transport::Publisher pub_2 = it_2.advertise("cam_video_publisher_2", 1);
	image_transport::ImageTransport it_3(nh);
	image_transport::Publisher pub_3 = it_3.advertise("cam_video_publisher_3", 1);
	image_transport::ImageTransport it_4(nh);
	image_transport::Publisher pub_4 = it_4.advertise("cam_video_publisher_4", 1);

	if ( 2 == argc )
	{
		char *param = argv[1];
		if ( 'l' == param[0] || 'l' == param[1] )
		{ 
			//查看usb列表
			ShowUsbList();
			return 0;
		}
	}

	bool bInitSuccess = InitImageTransfer();
	if ( !bInitSuccess )
	{
		cleanUp();
		return 0;
	}

	IplImage *g_imSrc = cvCreateImage(cvSize(WIDTH, HEIGHT), 8, 3);
	int nBMPIndex = 0;
	for ( ; ; )
	{
		char key;
		if ( 1 == GetConfig("realtimeshow") )
		{
			//loop for each cam
			(void) signal(SIGINT, trap);
			for ( unsigned int uCameraIndex = 0; uCameraIndex < g_uCameraNum; ++uCameraIndex )
			{
				//match camera and uCameraIndex
				GetImage( (ECAMERAINDEX)uCameraIndex, g_imSrc->imageData );
				const char *wndname[CAMERANUM] = { "imagetransfer1", "imagetransfer2", "imagetransfer3", "imagetransfer4" };
				Mat a = Mat(g_imSrc, false);

				//				imshow(wndname[uCameraIndex], a);

				cv_bridge::CvImage cvi;
				a.copyTo(cvi.image);
				//cvi.header.stamp = time;
				cvi.header.frame_id = "camera";
				cvi.encoding = "bgr8";

				sensor_msgs::Image msg;
				cvi.toImageMsg(msg);

				switch (uCameraIndex) {
					case 0:                     // front cam
						pub_1.publish(msg);
						ros::spinOnce();
						break;
					case 1:                     // back cam
						pub_2.publish(msg);
						ros::spinOnce();
						break;
					case 2:                     // left cam
						pub_3.publish(msg);
						ros::spinOnce();
						break;
					case 3:                     // right cam
						pub_4.publish(msg);
						ros::spinOnce();
						break;
					default:
						cout << "wrong uCameraIndex!" << endl;
				}

				//  	              char filename[32] = {0};
				//     	              snprintf( filename, sizeof(filename), "./%d_img/%d.jpg", (int)uCameraIndex, nBMPIndex++ );
				//     	              printf( "%s\n", filename );
				//     	              imwrite( filename, a );
				//cvWaitKey(3);
				key = waitKey(3);  // press while the image window is selected
				if (key == 'q'|| key == 'Q'|| key == 27)
				{
					cleanUp();
					return 0;
				}								
			}
		}
		usleep(1000);
	}
	cleanUp();
	return 0;
}

