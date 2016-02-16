#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <irobot_tracker/trackerDebug.h>
#include "irobot_tracker/PlainBase.h"

#define SCALE 10
ros::Publisher myPlain_pub;

geometry_msgs::Pose est;
geometry_msgs::Pose mes;
geometry_msgs::Vector3 mySpeed;
geometry_msgs::Vector3 myAccelerate;
PlainBase myPlain;

#define GNUPLOT_ENABLE_PTY
#define MAX_DATE 12
#include "gnuplot-iostream.h"

using namespace std;
ofstream myfile;
string filename = "Tracker_Log_";
std::string get_date(void)
{
   time_t now;
   char the_date[MAX_DATE];

   the_date[0] = '\0';

   now = time(NULL);

   if (now != -1)
   {
      strftime(the_date, MAX_DATE, "%H_%M_%m_%d", gmtime(&now));
   }

   return std::string(the_date);
}

Gnuplot gp;
std::vector<boost::tuple<double, double, double> > measure_record;
std::vector<boost::tuple<double, double, double> > estimate_record;

void debug_plot(const irobot_tracker::trackerDebug::ConstPtr& msg)
{
    measure_record.push_back(boost::make_tuple(msg->vision_pos.x, msg->vision_pos.y, msg->vision_pos.z));
    estimate_record.push_back(boost::make_tuple(msg->filter_pos.x, msg->filter_pos.y, msg->filter_pos.z));

    gp << "set xlabel 'tracker frame X'\n";
    gp << "set ylabel 'tracker frame Y'\n";
    gp << "splot ";
    gp << gp.binFile1d(measure_record, "record") << "with lines title 'measured process positions'";
    gp << ",";
    gp << gp.binFile1d(estimate_record, "record") << "with lines title 'estimated process positions'";
    gp << std::endl;
}

void debug_record(const irobot_tracker::trackerDebug::ConstPtr& msg)
{
	myfile.open (filename.c_str(),ios::out | ios::app);
	if (msg -> is_tracking == 1)
		myfile << 1 << '\t';
	else
		myfile << 0 << '\t';

	myfile << msg -> imu_ort.w << '\t';
	myfile << msg -> imu_ort.x << '\t';
	myfile << msg -> imu_ort.y << '\t';
	myfile << msg -> imu_ort.z << '\t';

	myfile << msg -> imu_linear_a.x << '\t';
	myfile << msg -> imu_linear_a.y << '\t';
	myfile << msg -> imu_linear_a.z << '\t';

	myfile << msg -> imu_angular_v.x << '\t';
	myfile << msg -> imu_angular_v.y << '\t';
	myfile << msg -> imu_angular_v.z << '\t';

	
	myfile << msg -> filter_ort.w << '\t';
	myfile << msg -> filter_ort.x << '\t';
	myfile << msg -> filter_ort.y << '\t';
	myfile << msg -> filter_ort.z << '\t';

	myfile << msg -> vision_pos.x << '\t';
	myfile << msg -> vision_pos.y << '\t';
	myfile << msg -> vision_pos.z << '\t';
	myfile << endl;

	myfile.close();
}

void debug_rviz(const irobot_tracker::trackerDebug::ConstPtr& msg)
{
	est.position.x = (msg -> filter_pos.x+100)/100.0f;
	est.position.y = msg -> filter_pos.y/100.0f;
	est.position.z = msg -> filter_pos.z/100.0f;
	est.orientation.x= msg -> filter_ort.x;
	est.orientation.y= msg -> filter_ort.y;
	est.orientation.z= msg -> filter_ort.z;
	est.orientation.w= msg -> filter_ort.w;

	mes.position.x = msg -> vision_pos.x/100.0f;
	mes.position.y = msg -> vision_pos.y/100.0f;
	mes.position.z = msg -> vision_pos.z/100.0f;
	mes.orientation.x=msg -> imu_ort.x;
	mes.orientation.y=msg -> imu_ort.y;
	mes.orientation.z=msg -> imu_ort.z;
	mes.orientation.w=msg -> imu_ort.w;

	mySpeed.x = msg -> filter_speed.x/100;
	mySpeed.y = msg -> filter_speed.y/100;
	mySpeed.z = msg -> filter_speed.z/100;

	myAccelerate.x = msg -> imu_linear_a.x/100;
	myAccelerate.y = msg -> imu_linear_a.y/100;
	myAccelerate.z = msg -> imu_linear_a.z/100;
	
	myPlain.update(est,mes,mySpeed,myAccelerate);
}
int main (int argc, char** argv)
{
	filename.append(get_date());
    filename.append(".txt");
    for(int i = 0; i<filename.length(); ++i){
        if (filename[i] == '/' || filename[i] == ':')
            filename[i] = '-';
    }
    ros::init(argc, argv, "Tracker_Debug");
    ros::NodeHandle nh;
    ros::Subscriber sub;
	myPlain_pub = nh.advertise<visualization_msgs::Marker>("myPlain", 100);
	myPlain.setPub(myPlain_pub);
	//sub = nh.subscribe("/tracker_debug", 5, debug_plot);
	//sub = nh.subscribe("/tracker_debug", 5, debug_record);
	sub = nh.subscribe("/tracker_debug", 2, debug_rviz);
    ros::spin();
}
