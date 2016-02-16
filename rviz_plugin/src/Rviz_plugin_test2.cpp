#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <rviz_plugin/UAV.h>
#include <rviz_plugin/trackInfo.h>
#include "rviz_plugin/PlainBase.h"

#define SCALE 10
ros::Publisher myPlain_pub;

geometry_msgs::Pose est;
geometry_msgs::Pose mes;
geometry_msgs::Vector3 mySpeed;
geometry_msgs::Vector3 myAccelerate;
PlainBase myPlain;

void callposition(const rviz_plugin::trackInfo& position);
void callimu(const rviz_plugin::UAV& imu);

int main( int argc, char** argv )
{
	ros::init(argc, argv, "Rviz_plugin_test1");
	ros::NodeHandle nv;
	myPlain_pub = nv.advertise<visualization_msgs::Marker>("myPlain", 100);
	myPlain.setPub(myPlain_pub);

	est.position.x=0;
	est.position.y=0;
	est.position.z=0;
	est.orientation.x=0;
	est.orientation.y=0;
	est.orientation.z=0;
	est.orientation.w=0;

	mes.position.x=0;
	mes.position.y=0;
	mes.position.z=0;
	mes.orientation.x=0;
	mes.orientation.y=0;
	mes.orientation.z=0;
	mes.orientation.w=0;

	mySpeed.x=0;
	mySpeed.y=0;
	mySpeed.z=0;

	myAccelerate.x=0;
	myAccelerate.y=0;
	myAccelerate.z=0;
	
	ros::Subscriber board_sub = nv.subscribe("board_pose", 1000, callposition);
	ros::Subscriber imu_sub = nv.subscribe("uav_imu",1000,callimu);
	ros::spin();
 
     	return 0;
}

void callimu(const rviz_plugin::UAV& imu)
{

	float x=imu.orientation.x;
	float y=imu.orientation.y;
	float z=imu.orientation.z;
	float w=imu.orientation.w;

	float ax=imu.linear_a.x;
	float ay=imu.linear_a.y;
	float az=imu.linear_a.z;
	
	printf("Get_imu.orientation:\nox=%f,oy=%f,oz=%f,ow=%f \n",x,y,z,w);
	printf("Get_imu.linear_a:\nax=%f,ay=%f,az=%f \n",ax,ay,az);
	printf("Get_imu.angular_v: \nvx=%f,vy=%f,vz=%f \n",imu.angular_v.x,imu.angular_v.y,imu.angular_v.z);
	printf("Get_imu.height: h=%f \n",imu.height);
	
	mySpeed=imu.angular_v;
	myAccelerate=imu.linear_a;
	est.orientation=imu.orientation;
	est.position.z=imu.height+3;
	myPlain.update(est,mes,mySpeed,myAccelerate);
}

void callposition(const rviz_plugin::trackInfo& position)
{

	float x=position.pose.position.x*SCALE;
	float y=position.pose.position.y*SCALE;
	float z=position.pose.position.z*SCALE;
	
	//ROS_INFO("Get_trackInfo: x=%f,y=%f,z=%f",x,y,z);
	
	mes=position.pose;
	mes.position.x=x;
	mes.position.y=y;
	mes.position.z=z;

	myPlain.update(est,mes,mySpeed,myAccelerate);
}





