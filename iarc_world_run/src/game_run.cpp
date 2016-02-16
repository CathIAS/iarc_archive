#include "ros/ros.h"
#include "std_srvs/Empty.h"
#include "gazebo_msgs/SetModelState.h"
#include "gazebo_msgs/GetModelState.h"
#include "gazebo_msgs/GetPhysicsProperties.h"
#include "gazebo_msgs/GetWorldProperties.h"
#include "gazebo_msgs/ModelState.h"

#include "main.h"

int main(int argc, char **argv)
{
	ros::init(argc, argv, "game_controller");

	ros::NodeHandle n;

	system("rosservice call /gazebo/reset_world");
	system("rosservice call /gazebo/reset_simulation");


	gpp_c = n.serviceClient<gazebo_msgs::GetPhysicsProperties>("/gazebo/get_physics_properties");
	gwp_c = n.serviceClient<gazebo_msgs::GetWorldProperties>("/gazebo/get_world_properties");
	gms_c = n.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
	sms_c = n.serviceClient<gazebo_msgs::SetModelState>("/gazebo/set_model_state");

	init_game_field();

	ros::Subscriber sub = n.subscribe("/gazebo/model_states", 1000, move_robots);
	ros::spin();
	
	return 0;
}
