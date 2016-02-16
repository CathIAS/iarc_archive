#include "ros/ros.h"
#include "std_msgs/String.h"
#include <gazebo/transport/transport.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/gazebo.hh>

#include <iostream>
#include <sstream>

ros::Publisher chatter_pub;
int count = 0;
/////////////////////////////////////////////////
// Function is called everytime a message is received.
void cb(ConstPosesStampedPtr &_msg)
{
  // Dump the message contents to stdout.
  //std::cout << _msg->DebugString();
  std::cout << _msg->pose(0).name()<<std::endl;
  std::cout << _msg->pose(0).position().x()<<std::endl;
  std::cout << _msg->pose(0).position().y()<<std::endl;
  std::cout << _msg->pose(0).position().z()<<std::endl;

	std_msgs::String msg;

	std::stringstream ss;
	ss << "hello world " << count;
	msg.data = ss.str();

	ROS_INFO("%s", msg.data.c_str());

	chatter_pub.publish(msg);
	count++;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "talker");

  ros::NodeHandle n;
  chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);

	gazebo::load(argc, argv);
  gazebo::run();
	gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();
	gazebo::transport::SubscriberPtr sub = node->Subscribe("/gazebo/default/pose/info", cb);
	//ros::spinOnce();

  while (true)
    gazebo::common::Time::MSleep(1000);


	gazebo::transport::fini();
  return 0;
}
