#include <iostream>
#include <sstream>
#include <string>
#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>

using namespace std;
using namespace cv;
using namespace sensor_msgs;
using namespace message_filters;
using namespace cv_bridge;

// canny low thresh, canny high thresh, hough thresh, min line length, max line gap, alpha, beta, alpha1, beta1
int trackbar_1 = 100, trackbar_2 = 500, trackbar_3 = 150, trackbar_4 = 50, trackbar_5 = 30, trackbar_6 = 1, trackbar_7 = 0, trackbar_8 = 2, trackbar_9 = 0;

int pickLine(const vector<Vec4i> p_lines)
{
	int theLine = -1;
	int currLowest = -1;
	int n = p_lines.size();
	for(int i=0;i<n;i++)
	{
		Vec4i line = p_lines[i];
		float x1 = line[0], y1 = line[1];
		float x2 = line[2], y2 = line[3];
		// line: y-y0 = ((y1-y2)/(x1-x2)) * (x-x0)
		//pick middle point in the picture to compare
		double k = (y1-y2)/(x1-x2);
		float y = cvRound(k*(368-x1)+y1);
		if ((y<0)||(y>576))
			continue;
		if ((k<-1)||(k>1))
			continue;
		if (y<currLowest || currLowest == -1)
			currLowest = i;		
	}
	theLine = currLowest;
	return theLine;
}

void img_proc(const CvImageConstPtr& cv_ptr_1, const CvImageConstPtr& cv_ptr_2, const CvImageConstPtr& cv_ptr_3, const CvImageConstPtr& cv_ptr_4)
{
	//	ROS_INFO("Image processing...");
	boost::shared_ptr<const cv_bridge::CvImage> cv_ptr[4];
	cv_ptr[0] = cv_ptr_1;
	cv_ptr[1] = cv_ptr_2;
	cv_ptr[2] = cv_ptr_3;
	cv_ptr[3] = cv_ptr_4;

	Mat img[4], img_gray[4], img_cvt[4], img_cvt_1[4], img_edge[4];
	for (int i=0; i<4; i++)
	{
		//assign original image
		img[i] = cv_ptr[i]->image;
		//convert color image to grayscale image
		cvtColor(img[i], img_gray[i], COLOR_RGB2GRAY );
		//adjust contrast and brightness
		img_gray[i].convertTo(img_cvt[i], -1, trackbar_6, trackbar_7-50); //alpha(contrast), beta(brightness)
		//adjust second time
		img_cvt[i].convertTo(img_cvt_1[i], -1, trackbar_8, trackbar_9-50);
		//apply canny filter
		Canny(img_cvt_1[i], img_edge[i], trackbar_1, trackbar_2, 3);
		//apply probablistic hough transform
		vector<Vec4i> p_lines;
		HoughLinesP(img_edge[i], p_lines, 1, CV_PI/180, trackbar_3, trackbar_4, trackbar_5);
		//add selection
		int lineNum = pickLine(p_lines);
		if (lineNum == -1)
			cout << "No Available Line Found!" << endl;
		else 
		{	
			Vec4i l = p_lines[lineNum];
			line(img[i], Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 2, 8);
		}
		//apply standard hough transform
		//		vector<Vec2f> lines;
		//		HoughLines(img[i], lines, 1, CV_PI/180, 100);
		//		for(size_t j=0; j < lines.size(); j++)
		//		{
		//			float rho = lines[j][0];
		//			float theta = lines[j][1];
		//			double a = cos(theta), b = sin(theta);
		//			double x0 = a*rho, y0 = b*rho;
		//			Point pt1(cvRound(x0 + 1000*(-b)), cvRound(y0 + 1000*(a)));
		//			Point pt2(cvRound(x0 - 1000*(-b)), cvRound(y0 - 1000*(a)));
		//			line(img[i], pt1, pt2, Scalar(0,0,255), 3, 8);
		//		}

		ostringstream ostr;
		ostr << i;
		imshow(("Sync_Image"+ostr.str()).c_str(), img[i]);
		waitKey(3);
	}
//		imshow("Window", img[1]);
//		imshow("Window edge", img_edge[1]);
//		imshow("Window cvt", img_cvt[1]);
//		imshow("Window cvt 1", img_cvt_1[1]);
//		waitKey(3);	
}

void callback(const ImageConstPtr& msg_1, const ImageConstPtr& msg_2, const ImageConstPtr& msg_3, const ImageConstPtr& msg_4)
{
	//	ROS_INFO("Receiving messages... Converting into images for process...");
	cv_bridge::CvImagePtr cv_ptr_1, cv_ptr_2, cv_ptr_3, cv_ptr_4;
	try
	{
		cv_ptr_1 = cv_bridge::toCvCopy(msg_1, sensor_msgs::image_encodings::BGR8);
		cv_ptr_2 = cv_bridge::toCvCopy(msg_2, sensor_msgs::image_encodings::BGR8);
		cv_ptr_3 = cv_bridge::toCvCopy(msg_3, sensor_msgs::image_encodings::BGR8);
		cv_ptr_4 = cv_bridge::toCvCopy(msg_4, sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}

	//	imshow("Sync_Image_1", cv_ptr_1->image);
	//	imshow("Sync_Image_2", cv_ptr_2->image);
	//	imshow("Sync_Image_3", cv_ptr_3->image);
	//	imshow("Syrc_Image_4", cv_ptr_4->image);
	//	waitKey(10);

	img_proc(cv_ptr_1, cv_ptr_2, cv_ptr_3, cv_ptr_4);
}

int main(int argc, char** argv)
{
	namedWindow("Trackbars", CV_WINDOW_AUTOSIZE);
	createTrackbar("CannyLowThresh", "Trackbars", &trackbar_1, 100);
	createTrackbar("CannyHighThresh", "Trackbars", &trackbar_2, 500);
	createTrackbar("HoughThresh", "Trackbars", &trackbar_3, 300);
	createTrackbar("MinLineLength", "Trackbars", &trackbar_4, 100);
	createTrackbar("MaxLineGap", "Trackbars", &trackbar_5, 50);
	createTrackbar("Contrast", "Trackbars", &trackbar_6, 5);
	createTrackbar("Brightness", "Trackbars", &trackbar_7, 100);
	createTrackbar("Contrast1", "Trackbars", &trackbar_8, 5);
	createTrackbar("Brightness1", "Trackbars", &trackbar_9, 100);


	ros::init(argc, argv, "improc_and_localization");
	ros::NodeHandle nh("~");

	message_filters::Subscriber<Image>* image1_sub;
	message_filters::Subscriber<Image>* image2_sub;
	message_filters::Subscriber<Image>* image3_sub;
	message_filters::Subscriber<Image>* image4_sub;

	string source;
	nh.getParam("source", source);
	ROS_INFO("Image source set: %s", source.c_str());

	if (source == "gazebo")
	{
		image1_sub = new Subscriber<Image>(nh, "/iarc_uav/camera1/image_raw", 3);
		image2_sub = new Subscriber<Image>(nh, "/iarc_uav/camera2/image_raw", 3);
		image3_sub = new Subscriber<Image>(nh, "/iarc_uav/camera3/image_raw", 3);
		image4_sub = new Subscriber<Image>(nh, "/iarc_uav/camera4/image_raw", 3);
	}
	else if (source == "camera")
	{
		image1_sub = new Subscriber<Image>(nh, "/cam_video_publisher_1", 3);
		image2_sub = new Subscriber<Image>(nh, "/cam_video_publisher_2", 3);
		image3_sub = new Subscriber<Image>(nh, "/cam_video_publisher_3", 3);
		image4_sub = new Subscriber<Image>(nh, "/cam_video_publisher_4", 3);
	}
	else 
		cout << "Source not specified! Please specify image source by adding \"_source:=value\", where value is either \"gazebo\" or \"camera\"" << endl;

	typedef sync_policies::ApproximateTime<Image, Image, Image, Image> MySyncPolicy;

	Synchronizer<MySyncPolicy> sync(MySyncPolicy(30), *image1_sub, *image2_sub, *image3_sub, *image4_sub);
	sync.registerCallback(boost::bind(&callback, _1, _2, _3, _4));

	ros::spin();

	return 0;
}

