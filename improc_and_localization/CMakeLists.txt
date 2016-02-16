cmake_minimum_required(VERSION 2.8.3)
project(improc_and_localization)

find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED  COMPONENTS
        cv_bridge
        image_transport
        sensor_msgs
		message_filters
)

catkin_package(
    CATKIN_DEPENDS cv_bridge image_transport sensor_msgs message_filters
)   

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${catkin_INCLUDE_DIRS}
)

add_executable(improc_and_localization src/main.cpp)

target_link_libraries( improc_and_localization
    ${OpenCV_LIBRARIES}
    ${catkin_LIBRARIES}
)

