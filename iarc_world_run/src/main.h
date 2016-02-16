#ifndef MAIN_H
#define MAIN_H
#include "ros/ros.h"
#include "gazebo_msgs/SetModelState.h"
#include "gazebo_msgs/GetModelState.h"
#include "gazebo_msgs/GetPhysicsProperties.h"
#include "gazebo_msgs/GetWorldProperties.h"
#include "gazebo_msgs/ModelState.h"
#include "gazebo_msgs/ModelStates.h"
#include <cstring>
#include <cmath>
#include <cstdlib>
#define PI 3.14159265

/********* Global Variable Definition *********/
int rob_ctl_rate = 1000;
int uav_ctl_rate = 60;
static const int rob_num = 10;
static const int o_rob_num = 4;
double init_r1 = 1.0;
double init_r2 = 5.0;

double rob_speed_o = 0.333333; 
double o_rob_period = PI*init_r2*2/rob_speed_o;
double o_rob_ang_speed = 360.0/o_rob_period;
double rob_speed = 0.333333; 

std::string uav_name = "iarc_uav";
std::string rob_name_list[rob_num] = {"create", "create_0", 
									  "create_1", "create_2",
									  "create_3", "create_4",
									  "create_5", "create_6",
									  "create_7", "create_8"};
std::string o_rob_name_list[o_rob_num] = 
		{"iarc_obstacle_robot", "iarc_obstacle_robot_0", 
		 "iarc_obstacle_robot_1", "iarc_obstacle_robot_2"};
double o_rob_ang_list[o_rob_num] = {135,225,315,45};
double rob_ang_list[rob_num];

double curr_time,step_time;

// this time only update when sim_time passes certain amount, 
// it is used to solve the problem of robot orientation changes
int my_time = 1;

//a trajectory profile
double profile_x[500];
double profile_y[500];
double profile_z[500];
int period = 500;
int count = 0;

ros::ServiceClient rst_c;
ros::ServiceClient gwp_c;
ros::ServiceClient gpp_c;
ros::ServiceClient gms_c;
ros::ServiceClient sms_c;

void init_trajectory_profile()
{
	for (int i = 0; i < 100; i++)
	{
		profile_x[i] = 0;
		profile_y[i] = 2.50-i*0.05;
		profile_z[i] = 1.70-i*0.012;
	}
	for (int i = 100; i < 250; i++)
	{
		profile_x[i] = 3-3*cos(1.2*(i-99)/180*PI);
		profile_y[i] = -3.55-3*sin(1.2*(i-99)/180*PI);
		profile_z[i] = 0.504+(i-100)*0.008;
	}
	for (int i = 250; i < 350; i++)
	{
		profile_x[i] = 6;
		profile_y[i] = -3.55+(i-250)*0.05;
		profile_z[i] = 1.70-(i-250)*0.012;
	}
	for (int i = 350; i < 500; i++)
	{
		profile_x[i] = 3+3*cos(1.2*(i-349)/180*PI);
		profile_y[i] = 2.55+3*sin(1.2*(i-349)/180*PI);
		profile_z[i] = 0.504+(i-350)*0.008;
	}
}

void init_game_field()
{
	gazebo_msgs::SetModelState setmodelstate;
	gazebo_msgs::ModelState modelstate;
	gazebo_msgs::GetPhysicsProperties pprop;
	geometry_msgs::Twist twist;
	geometry_msgs::Pose pose;

	//place ordinary iRobots
	for (int i = 0; i < rob_num; i++)
	{
		twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
		twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

		rob_ang_list[i] = i*36;
		pose.position.x = init_r1*cos(i*36*PI/180);
		pose.position.y = init_r1*sin(i*36*PI/180);
		pose.position.z = 0;
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = sin(i*18*PI/180);
		pose.orientation.w = cos(i*18*PI/180);

		modelstate.model_name =rob_name_list[i];
		modelstate.pose = pose;
		modelstate.twist = twist;

		setmodelstate.request.model_state=modelstate;         

 		sms_c.call(setmodelstate);
	}

	//place obstacle iRobots
	for (int i = 0; i < o_rob_num; i++)
	{
		twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
		twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

		pose.position.x = init_r2*cos(o_rob_ang_list[i]*PI/180);
		pose.position.y = init_r2*sin(o_rob_ang_list[i]*PI/180);
		pose.position.z = 0;
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = sin(o_rob_ang_list[i]/2*PI/180);
		pose.orientation.w = cos(o_rob_ang_list[i]/2*PI/180);

		modelstate.model_name = o_rob_name_list[i];
		modelstate.pose = pose;
		modelstate.twist = twist;
		setmodelstate.request.model_state=modelstate;         

 		sms_c.call(setmodelstate);
	}

	//place UAV
	twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
	twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

	pose.position.x = 10; 
	pose.position.y = 0;
	pose.position.z = 0;
	pose.orientation.x = 0.0;
	pose.orientation.y = 0.0;
	pose.orientation.z = 0.0;
	pose.orientation.w = 0.0;

	modelstate.model_name = uav_name;
	modelstate.pose = pose;
	modelstate.twist = twist;

	setmodelstate.request.model_state=modelstate;         

	sms_c.call(setmodelstate);

	init_trajectory_profile();

	gpp_c.call(pprop);
	step_time = pprop.response.time_step;
}

void move_robots(const gazebo_msgs::ModelStates::ConstPtr& msg)
{
	int length = 16; //by observing world file	
	/*
	  0	iarc-ground
	  1	create
	  2	create_0
	  3	create_1
	  4	create_2
	  5	create_3
	  6	create_4
	  7	create_5
	  8	create_6
	  9	create_7
	 10	create_8
	 11	iarc_obstacle_robot
	 12	iarc_obstacle_robot_0
	 13	iarc_obstacle_robot_1
	 14	iarc_obstacle_robot_2
	 15	iarc_uav
	*/

	
	gazebo_msgs::GetWorldProperties wprop;
	gwp_c.call(wprop);
	curr_time = wprop.response.sim_time;

	//std::cout<<step_time<<std::endl;
	geometry_msgs::Twist twist;
	geometry_msgs::Pose pose;
	gazebo_msgs::SetModelState setmodelstate;
	gazebo_msgs::GetModelState getmodelstate;
	gazebo_msgs::ModelState modelstate;

	/*
	for (int i = 11; i < 11+o_rob_num; i++)
	{
		twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
		twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

		double ang = o_rob_ang_list[i-11]/180*PI;
		o_rob_ang_list[i-11] += o_rob_ang_speed*step_time;
		if (o_rob_ang_list[i-11] > 360)
			o_rob_ang_list[i-11] -= 360;
		//std::cout<<i<<" "<<o_rob_ang_list[i]<<std::endl;
		ang = o_rob_ang_list[i-11]/180*PI;
		//std::cout<<i<<" x "<<getmodelstate.response.pose.position.x<<std::endl;
		//std::cout<<i<<" y "<<getmodelstate.response.pose.position.y<<std::endl;
		//std::cout<<i<<" dx "<<rob_speed*cos(ang)<<std::endl;
		//std::cout<<i<<" dy "<<rob_speed*sin(ang)<<std::endl;
		pose.position.x = 
			msg->pose[i].position.x + 
			rob_speed*cos(ang+PI/2)*step_time;
		//pose.position.x = init_r2*cos(ang);
		pose.position.y = 
			msg->pose[i].position.y + 
			rob_speed*sin(ang+PI/2)*step_time;
		//pose.position.y = init_r2*sin(ang);
		pose.position.z = msg->pose[i].position.z;

	
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = sin(ang/2);
		pose.orientation.w = cos(ang/2);

		modelstate.model_name = o_rob_name_list[i-11];
		modelstate.pose = pose;
		modelstate.twist = twist;
		setmodelstate.request.model_state=modelstate;         

		sms_c.call(setmodelstate);
	}

	if (curr_time>my_time+4)
	{
		my_time =(int)curr_time;
		std::cout<<"MY time is "<<my_time<<std::endl;
	}

	//move ordinary robots
	//TODO: here we assume immediate rotation (in reality not) 
	for (int i = 1; i < 1+rob_num; i++)
	{
		//occasionally malfunction, due to the simtime cannot be obtained exactly
		
		if (curr_time>my_time && my_time % 20 ==0)
		{
			rob_ang_list[i-1] += 180;
			if (i-1==rob_num-1)
				my_time += 1;
		}
		else if (curr_time>my_time && my_time % 5 ==0)
		{
			rob_ang_list[i-1] += rand()%20;
			if (i-1==rob_num-1)
				my_time += 1;
		}

		if (rob_ang_list[i-1] > 360)
			rob_ang_list[i-1] -= 360;


		twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
		twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

		double ang = rob_ang_list[i-1]/180*PI;
		if (i==1)
		{
			//std::cout<<i<<" "<<rob_name_list[i]<<std::endl;
			//std::cout<<i<<" x "<<getmodelstate.response.pose.position.x<<std::endl;
			//std::cout<<i<<" y "<<getmodelstate.response.pose.position.y<<std::endl;
			//std::cout<<i<<" ang "<<rob_ang_list[i]<<std::endl;
		}
		//std::cout<<i<<" dy "<<rob_speed*sin(ang)<<std::endl;
		pose.position.x = 
			msg->pose[i].position.x + 
			rob_speed*cos(ang)*step_time;
		//std::cout<<" dy "<<rob_speed*cos(ang)*step_time<<std::endl;
		//pose.position.x = init_r2*cos(ang);
		pose.position.y = 
			msg->pose[i].position.y + 
			rob_speed*sin(ang)*step_time;
		//pose.position.y = init_r2*sin(ang);
		pose.position.z = msg->pose[i].position.z;

	
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = sin(ang/2);
		pose.orientation.w = cos(ang/2);

		modelstate.model_name = rob_name_list[i-1];
		modelstate.pose = pose;
		modelstate.twist = twist;
		setmodelstate.request.model_state=modelstate;         

		sms_c.call(setmodelstate);
	}
	*/
	////std::cout<<length<<std::endl;
	//geometry_msgs::Twist twist;
	//geometry_msgs::Pose pose;
	//gazebo_msgs::SetModelState setmodelstate;
	//gazebo_msgs::ModelState modelstate;
	std::cout<<"Sim time: "<<step_time<<std::endl;
	//move obstacle robots first (easy to move)
	for (int i = 0; i < o_rob_num; i++)
	{
		getmodelstate.request.model_name = o_rob_name_list[i];
		gms_c.call(getmodelstate);

		twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
		twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

		double ang = o_rob_ang_list[i]/180*PI;
		o_rob_ang_list[i] += o_rob_ang_speed*step_time;
		if (o_rob_ang_list[i] > 360)
			o_rob_ang_list[i] -= 360;
		//std::cout<<i<<" "<<o_rob_ang_list[i]<<std::endl;
		ang = o_rob_ang_list[i]/180*PI;
		//std::cout<<i<<" x "<<getmodelstate.response.pose.position.x<<std::endl;
		//std::cout<<i<<" y "<<getmodelstate.response.pose.position.y<<std::endl;
		//std::cout<<i<<" dx "<<rob_speed*cos(ang)<<std::endl;
		//std::cout<<i<<" dy "<<rob_speed*sin(ang)<<std::endl;
		pose.position.x = 
			getmodelstate.response.pose.position.x + 
			rob_speed*cos(ang+PI/2)*step_time;
		//pose.position.x = init_r2*cos(ang);
		pose.position.y = 
			getmodelstate.response.pose.position.y + 
			rob_speed*sin(ang+PI/2)*step_time;
		//pose.position.y = init_r2*sin(ang);
		pose.position.z = getmodelstate.response.pose.position.z;

	
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = sin(ang/2);
		pose.orientation.w = cos(ang/2);

		modelstate.model_name = o_rob_name_list[i];
		modelstate.pose = pose;
		modelstate.twist = twist;
		setmodelstate.request.model_state=modelstate;         

		sms_c.call(setmodelstate);
	}
	
  /*
	if (time>my_time+4)
	{
		my_time =(int)time;
		std::cout<<"MY time is "<<my_time<<std::endl;
	}
	//move ordinary robots
	//TODO: here we assume immediate rotation (in reality not) 
	for (int i = 0; i < rob_num; i++)
	{
		//occasionally malfunction, due to the simtime cannot be obtained exactly
		
		if (time>my_time && my_time % 20 ==0)
		{
			rob_ang_list[i] += 180;
			if (i==rob_num-1)
				my_time += 1;
		}
		else if (time>my_time && my_time % 5 ==0)
		{
			rob_ang_list[i] += rand()%20;
			if (i==rob_num-1)
				my_time += 1;
		}

		if (rob_ang_list[i] > 360)
			rob_ang_list[i] -= 360;

			
		getmodelstate.request.model_name = rob_name_list[i];
		gms_c.call(getmodelstate);

		twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
		twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

		double ang = rob_ang_list[i]/180*PI;
		if (i==1)
		{
			//std::cout<<i<<" "<<rob_name_list[i]<<std::endl;
			//std::cout<<i<<" x "<<getmodelstate.response.pose.position.x<<std::endl;
			//std::cout<<i<<" y "<<getmodelstate.response.pose.position.y<<std::endl;
			//std::cout<<i<<" ang "<<rob_ang_list[i]<<std::endl;
		}
		//std::cout<<i<<" dy "<<rob_speed*sin(ang)<<std::endl;
		pose.position.x = 
			getmodelstate.response.pose.position.x + 
			rob_speed*cos(ang)*step_time;
		std::cout<<" dy "<<rob_speed*cos(ang)*step_time<<std::endl;
		//pose.position.x = init_r2*cos(ang);
		pose.position.y = 
			getmodelstate.response.pose.position.y + 
			rob_speed*sin(ang)*step_time;
		//pose.position.y = init_r2*sin(ang);
		pose.position.z = getmodelstate.response.pose.position.z;

	
		pose.orientation.x = 0.0;
		pose.orientation.y = 0.0;
		pose.orientation.z = sin(ang/2);
		pose.orientation.w = cos(ang/2);

		modelstate.model_name = rob_name_list[i];
		modelstate.pose = pose;
		modelstate.twist = twist;
		setmodelstate.request.model_state=modelstate;         

		sms_c.call(setmodelstate);
	}
	
	*/
	//move uav
	twist.linear.x = 0.0; twist.linear.y = 0.0; twist.linear.z = 0.0;
	twist.angular.x = 0.0; twist.angular.y = 0.0; twist.angular.z = 0.0;

	pose.position.x = profile_x[count]; 
	pose.position.y = profile_y[count]; 
	pose.position.z = profile_z[count]; 

	pose.orientation.x = 0.0;
	pose.orientation.y = 0.0;
	pose.orientation.z = 0.0;
	pose.orientation.w = 1;

	modelstate.model_name = uav_name;
	modelstate.pose = pose;
	modelstate.twist = twist;
	setmodelstate.request.model_state = modelstate;         

	sms_c.call(setmodelstate);
	
	count++;
	count = count %500;
}


#endif
