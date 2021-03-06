#include "rviz_plugin/PlainBase.h"


PlainBase::PlainBase(geometry_msgs::Pose estp,geometry_msgs::Pose mesp,
		geometry_msgs::Vector3 speedv,geometry_msgs::Vector3 acceleratev,ros::Publisher puber)
{
	est=estp;
	mes=mesp;
	speed=speedv;
	accelerate=acceleratev;
	pub=puber;
	plainMod=initPlainMod();
	estTrack=initTrack();
	mesTrack=initTrack();
	visualSpeed=initVisualSpeed();
	visualAccelerate=initVisualAccelerate();

}

PlainBase::PlainBase()
{
	plainMod=initPlainMod();
	estTrack=initTrack();
	mesTrack=initTrack();
	visualSpeed=initVisualSpeed();
	visualAccelerate=initVisualAccelerate();
}

PlainBase::~PlainBase()
{
}


visualization_msgs::Marker PlainBase::initPlainMod()
{
	visualization_msgs::Marker myPlain;
	myPlain.type = visualization_msgs::Marker::MESH_RESOURCE;
	myPlain.mesh_resource = MOD_SRC;

	//the frame link
	myPlain.header.frame_id = FRAME_ID;

	myPlain.scale.x = PLAIN_SCALE;
	myPlain.scale.y = PLAIN_SCALE;
	myPlain.scale.z = PLAIN_SCALE;

	myPlain.color.r = 0.9f;
	myPlain.color.g = 0.8f;
	myPlain.color.b = 0.3f;
	myPlain.color.a = 1.0;
	//never delete
	myPlain.lifetime = ros::Duration();
	return myPlain;
}

visualization_msgs::Marker PlainBase::initTrack()
{
	visualization_msgs::Marker myTrack;
	myTrack.type = visualization_msgs::Marker::LINE_STRIP;

	//the frame link
	myTrack.header.frame_id = FRAME_ID;

	myTrack.scale.x = TRACK_SCALE;
	myTrack.scale.y = TRACK_SCALE;
	myTrack.scale.z = TRACK_SCALE;

	myTrack.pose.position.x=0;
	myTrack.pose.position.y=0;
	myTrack.pose.position.z=0;

	myTrack.color.r = 0.9f;
	myTrack.color.g = 0.8f;
	myTrack.color.b = 0.3f;
	myTrack.color.a = 1.0;
	//never delete
	myTrack.lifetime = ros::Duration();
	return myTrack;
}

visualization_msgs::Marker PlainBase::initVisualSpeed()
{
	visualization_msgs::Marker mySpeed;
	mySpeed.type = visualization_msgs::Marker::ARROW;

	//the frame link
	mySpeed.header.frame_id = FRAME_ID;

	mySpeed.scale.x = SPEED_SCALE-0.5*SPEED_SCALE;
	mySpeed.scale.y = SPEED_SCALE;
	mySpeed.scale.z = SPEED_SCALE;

	mySpeed.pose.position.x=0;
	mySpeed.pose.position.y=0;
	mySpeed.pose.position.z=0;

	mySpeed.color.r = 0.9f;
	mySpeed.color.g = 0.8f;
	mySpeed.color.b = 0.3f;
	mySpeed.color.a = 1.0;
	//never delete
	mySpeed.lifetime = ros::Duration();
	return mySpeed;
}

//initialize accelerate
visualization_msgs::Marker PlainBase::initVisualAccelerate()
{
	visualization_msgs::Marker myAccelerate;
	myAccelerate.type = visualization_msgs::Marker::ARROW;

	//the frame link
	myAccelerate.header.frame_id = FRAME_ID;

	myAccelerate.scale.x = ACCEL_SCALE -0.5*ACCEL_SCALE ;
	myAccelerate.scale.y = ACCEL_SCALE ;
	myAccelerate.scale.z = ACCEL_SCALE ;

	myAccelerate.pose.position.x=0;
	myAccelerate.pose.position.y=0;
	myAccelerate.pose.position.z=0;

	myAccelerate.color.r = 0.9f;
	myAccelerate.color.g = 0.8f;
	myAccelerate.color.b = 0.3f;
	myAccelerate.color.a = 1.0;
	//never delete
	myAccelerate.lifetime = ros::Duration();
	return myAccelerate;
}

void PlainBase::setPub(ros::Publisher puber)
{
	pub=puber;
}

/*
void PlainBase::updateTrack(std::vector<geometry_msgs::Point>& points, geometry_msgs::Pose newPose, std::string ns, float r, float g, float b)
{
	track.header.stamp = ros::Time::now();
	track.ns = ns;
	track.id = 0;
	track.action = visualization_msgs::Marker::ADD;

	track.color.r = r;
	track.color.g = g;
	track.color.b = b;
	track.color.a = 1.0;		

	track.pose.orientation.w=1.0;
	geometry_msgs::Point p;
	p.x = newPose.position.x;
	p.y = newPose.position.y;
	p.z = newPose.position.z;

	points.push_back(p);

}
*/
void PlainBase::updateTrack(visualization_msgs::Marker & mytrack, geometry_msgs::Pose newPose, std::string ns, float r, float g, float b)
{
	mytrack.header.stamp = ros::Time::now();
	mytrack.ns = ns;
	mytrack.id = 0;
	mytrack.action = visualization_msgs::Marker::ADD;

	mytrack.color.r = r;
	mytrack.color.g = g;
	mytrack.color.b = b;
	mytrack.color.a = 1.0;		

	mytrack.pose.orientation.w=1.0;
	geometry_msgs::Point p;
	p.x = newPose.position.x;
	p.y = newPose.position.y;
	p.z = newPose.position.z;

	mytrack.points.push_back(p);

}

void PlainBase::updatePlainMod(geometry_msgs::Pose newPose, std::string ns, float r, float g, float b)
{
	plainMod.header.stamp = ros::Time::now();
	plainMod.ns = ns;
	plainMod.id = 0;
	plainMod.action = visualization_msgs::Marker::ADD;

	plainMod.color.r = r;
	plainMod.color.g = g;
	plainMod.color.b = b;
	plainMod.color.a = 1.0;	

	plainMod.pose=newPose;

	plainMod.pose.position=newPose.position;
	float tmpQ[4];
	tmpQ[0]=newPose.orientation.w;
	tmpQ[1]=newPose.orientation.x;
	tmpQ[2]=newPose.orientation.y;
	tmpQ[3]=newPose.orientation.z;
	//printf("new\n");
	//printf("x=%f,y=%f,z=%f,w=%f\n",tmpQ[1],tmpQ[2],tmpQ[3],tmpQ[0]);

	//set the offset of the plainMod  
	float tmpYaw,tmpPitch,tmpRoll;
	Quaternoin_to_Eular(tmpQ, &tmpYaw, &tmpPitch, &tmpRoll);

	//geometry_msgs::Quaternion Q;
	//printf("yaw=%f,pithc=%f,roll=%f \n",tmpYaw,tmpPitch,tmpRoll);
	//tmpRoll+=1.5707;
	//tmpRoll+=90;
	tmpPitch+=90;
	//printf("yaw=%f,pithc=%f,roll=%f \n",tmpYaw,tmpPitch,tmpRoll);	
	Eular_to_Quaternion(tmpQ, tmpYaw, tmpPitch, tmpRoll);

	plainMod.pose.orientation.x = tmpQ[1];
	plainMod.pose.orientation.y = tmpQ[2];
	plainMod.pose.orientation.z = tmpQ[3];
	plainMod.pose.orientation.w = tmpQ[0];
	//printf("x=%f,y=%f,z=%f,w=%f\n",tmpQ[1],tmpQ[2],tmpQ[3],tmpQ[0]);
}


void PlainBase::updateSpeed(geometry_msgs::Vector3 newSpeed,geometry_msgs::Pose newPose, std::string ns, float r, float g, float b)
{
	visualSpeed.points.clear();
	visualSpeed.header.stamp = ros::Time::now();
	visualSpeed.ns = ns;
	visualSpeed.id = 0;
	visualSpeed.action = visualization_msgs::Marker::ADD;

	visualSpeed.color.r = r;
	visualSpeed.color.g = g;
	visualSpeed.color.b = b;
	visualSpeed.color.a = 1.0;		

	visualSpeed.pose.orientation.w=1.0;
	geometry_msgs::Point p;
	p.x = newPose.position.x;
	p.y = newPose.position.y;
	p.z = newPose.position.z;

	visualSpeed.points.push_back(p);

	p.x += newSpeed.x;
	p.y += newSpeed.y;
	p.z += newSpeed.z;
	visualSpeed.points.push_back(p);

	}

	void PlainBase::updateAccelerate(geometry_msgs::Vector3 newAccelerate,geometry_msgs::Pose newPose, std::string ns, float r, float g, float b)
	{
	visualAccelerate.points.clear();
	visualAccelerate.header.stamp = ros::Time::now();
	visualAccelerate.ns = ns;
	visualAccelerate.id = 0;
	visualAccelerate.action = visualization_msgs::Marker::ADD;

	visualAccelerate.color.r = r;
	visualAccelerate.color.g = g;
	visualAccelerate.color.b = b;
	visualAccelerate.color.a = 1.0;		

	visualAccelerate.pose.orientation.w=1.0;
	geometry_msgs::Point p;
	p.x = newPose.position.x;
	p.y = newPose.position.y;
	p.z = newPose.position.z;

	visualAccelerate.points.push_back(p);

	p.x += newAccelerate.x;
	p.y += newAccelerate.y;
	p.z += newAccelerate.z;
	visualAccelerate.points.push_back(p);
	
}


void PlainBase::update(geometry_msgs::Pose est,geometry_msgs::Pose mes,
		geometry_msgs::Vector3 speed,geometry_msgs::Vector3 accelerate)
{
	updatePlainMod(est,"plainEstimate",0.9,0.8,0.3);
	pub.publish(plainMod);
	updatePlainMod(mes,"plainMeasure",0.3,0.9,0.8);
	pub.publish(plainMod);
	pub.publish(plainMod);
	updateTrack(estTrack,est,"trackEstimate",0.9,0.1,0.1);
	pub.publish(estTrack);
	updateTrack(mesTrack,mes,"trackMeasure",0.1,0.9,0.1);
	pub.publish(mesTrack);
	updateSpeed(speed,est,"speed",0.1,0.4,0.9);
	pub.publish(visualSpeed);
	updateAccelerate(accelerate,est,"accelerate",0.1,0.8,0.8);
	pub.publish(visualAccelerate);
	if(mesTrack.points.size()>5000)mesTrack.points.clear();	
	if(estTrack.points.size()>5000)estTrack.points.clear();
}

void PlainBase::update(geometry_msgs::Pose est,geometry_msgs::Pose mes)
{
	updatePlainMod(est,"plainEstimate",0.9,0.8,0.3);
	pub.publish(plainMod);
	updatePlainMod(mes,"plainMeasure",0.3,0.9,0.8);
	pub.publish(plainMod);
	updateTrack(estTrack,est,"trackEstimate",0.9,0.1,0.1);
	pub.publish(estTrack);
	updateTrack(mesTrack,mes,"trackMeasure",0.1,0.9,0.1);
	pub.publish(mesTrack);
}


void PlainBase::Eular_to_Quaternion(float Q[4], float yaw, float pitch, float roll)
{   
	float cos_half_yaw, sin_half_yaw, cos_half_pitch, sin_half_pitch, cos_half_roll, sin_half_roll;
	float rad=M_PI/180.0f;
	cos_half_yaw = cos(yaw*rad/2.0f);
	sin_half_yaw = sin(yaw*rad/2.0f);
	cos_half_pitch = cos(pitch*rad/2.0f);
	sin_half_pitch = sin(pitch*rad/2.0f);
	cos_half_roll = cos(roll*rad/2.0f);
	sin_half_roll = sin(roll*rad/2.0f);

	Q[0] = cos_half_yaw * cos_half_pitch * cos_half_roll + sin_half_yaw * sin_half_pitch * sin_half_roll;
	Q[1] = cos_half_yaw * sin_half_pitch * cos_half_roll - sin_half_yaw * cos_half_pitch * sin_half_roll;
	Q[2] = cos_half_yaw * cos_half_pitch * sin_half_roll + sin_half_yaw * sin_half_pitch * cos_half_roll;
	Q[3] = sin_half_yaw * cos_half_pitch * cos_half_roll - cos_half_yaw * sin_half_pitch * sin_half_roll;
}

void PlainBase::Quaternoin_to_Eular(const float Q[4], float *yaw, float *pitch, float *roll)
{
	*yaw  = -atan2(2 * Q[1] * Q[2] + 2 * Q[0]* Q[3], -2 * Q[2]*Q[2] - 2 * Q[3] * Q[3] + 1)* 180.0f/M_PI;
	*pitch = -asin(-2 * Q[1] * Q[3] + 2 * Q[0] * Q[2])* 180.0f/M_PI;
	*roll = atan2(2 * Q[2] * Q[3] + 2 * Q[0] * Q[1], -2 * Q[1] * Q[1] - 2 * Q[2] * Q[2] + 1)* 180.0f/M_PI;
}
