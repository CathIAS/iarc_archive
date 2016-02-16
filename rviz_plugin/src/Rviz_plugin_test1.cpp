#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "rviz_plugin/PlainBase.h"

int main( int argc, char** argv )
{
	ros::init(argc, argv, "Rviz_plugin_test1");
	ros::NodeHandle nv;
	ros::Rate r(30);
	ros::Publisher myPlain_pub = nv.advertise<visualization_msgs::Marker>("myPlain", 100);

	geometry_msgs::Pose est;
	geometry_msgs::Pose mes;
	geometry_msgs::Vector3 mySpeed;
	geometry_msgs::Vector3 myAccelerate;

	PlainBase myPlain;
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

	mySpeed.x=0.1;
	mySpeed.y=0.1;
	mySpeed.z=0.1;

	myAccelerate.x=0;
	myAccelerate.y=0;
	myAccelerate.z=-0.1;
	float angle=0;
	const double degree = M_PI/180;

	while (ros::ok())
	{
		myPlain.update(est,mes,mySpeed,myAccelerate);

		r.sleep();

		est.position.x-=0.01;
		est.position.y-=0.01;
		est.position.z+=0.01;

		mes.position.x+=0.01;
		mes.position.y+=0.01;
		mes.position.z+=0.01;

		if(mes.position.x>1)
		{
			float x=0,y=0;
			while(angle<360)
			{	
				x=1.4*sin(angle*degree);
				y=1.4*cos(angle*degree);
				mes.position.x=x;
				est.position.x=-x;
				mes.position.y=y;
				est.position.y=-y;
				angle+=1;
				myPlain.update(est,mes,mySpeed,myAccelerate);
				r.sleep();
			}
			angle=0;
			est.position.x=0;
			est.position.y=0;
			est.position.z=0;
			mes.position.x=0;
			mes.position.y=0;
			mes.position.z=0;
		}

	}
	
}
