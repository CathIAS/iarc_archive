#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
/*
visualization_msgs::Marker plainMod(float px,float py,float pz,float ox,float oy,float oz,float ow,float scale)
{
		visualization_msgs::Marker myPlain;
		myPlain.type = visualization_msgs::Marker::MESH_RESOURCE;
		myPlain.mesh_resource = "package://rviz_plugin/meshes/myplain.stl";
		//the frame link
		myPlain.header.frame_id = "/plain_state";
		myPlain.header.stamp = ros::Time::now();
		myPlain.ns = "plainBase";
		myPlain.id = 0;
		myPlain.action = visualization_msgs::Marker::ADD;
		myPlain.pose.position.x = px;
		myPlain.pose.position.y = py;
		myPlain.pose.position.z = pz;
		myPlain.pose.orientation.x = 0.701+ox;
		myPlain.pose.orientation.y = 0.0+oy;
		myPlain.pose.orientation.z = 0.0+oz;
		myPlain.pose.orientation.w = 0.701+ow;
		
		myPlain.scale.x = scale;
		myPlain.scale.y = scale;
		myPlain.scale.z = scale;

		myPlain.color.r = 0.9f;
		myPlain.color.g = 0.8f;
		myPlain.color.b = 0.3f;
		myPlain.color.a = 1.0;
		//never delete
		myPlain.lifetime = ros::Duration();
		return myPlain;
		

}

int main( int argc, char** argv )
{
	ros::init(argc, argv, "myPlain");
	ros::NodeHandle nv;
	ros::Rate r(30);
	ros::Publisher myPlain_pub = nv.advertise<visualization_msgs::Marker>("myPlain", 10);
	while (ros::ok())
	{
		visualization_msgs::Marker myPlain;
		myPlain=plainMod(0,0,0,0,0,0,0,0.001);
		myPlain_pub.publish(myPlain);
		r.sleep();
	}
}
*/

void plainMod(float px,float py,float pz,float ox,float oy,float oz,float ow,float scale,ros::Publisher pub)
{
		visualization_msgs::Marker myPlain;
		myPlain.type = visualization_msgs::Marker::MESH_RESOURCE;
		myPlain.mesh_resource = "package://rviz_plugin/meshes/myplain.stl";
		//the frame link
		myPlain.header.frame_id = "/plain_state";
		myPlain.header.stamp = ros::Time::now();
		myPlain.ns = "plainBase";
		myPlain.id = 0;
		myPlain.action = visualization_msgs::Marker::ADD;
		myPlain.pose.position.x = px;
		myPlain.pose.position.y = py;
		myPlain.pose.position.z = pz;
		myPlain.pose.orientation.x = 0.701+ox;
		myPlain.pose.orientation.y = 0.0+oy;
		myPlain.pose.orientation.z = 0.0+oz;
		myPlain.pose.orientation.w = 0.701+ow;
		
		myPlain.scale.x = scale;
		myPlain.scale.y = scale;
		myPlain.scale.z = scale;

		myPlain.color.r = 0.9f;
		myPlain.color.g = 0.8f;
		myPlain.color.b = 0.3f;
		myPlain.color.a = 1.0;
		//never delete
		myPlain.lifetime = ros::Duration();
		pub.publish(myPlain);
}

int main( int argc, char** argv )
{
	ros::init(argc, argv, "myPlain");
	ros::NodeHandle nv;
	ros::Rate r(30);
	ros::Publisher myPlain_pub = nv.advertise<visualization_msgs::Marker>("myPlain", 10);
	while (ros::ok())
	{
		plainMod(0,0,0,0,0,0,0,0.1,myPlain_pub);
		r.sleep();
	}
}

