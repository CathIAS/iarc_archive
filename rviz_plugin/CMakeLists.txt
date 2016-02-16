cmake_minimum_required(VERSION 2.8.3)
project(rviz_plugin)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  visualization_msgs
  message_generation
)

add_message_files(
  FILES
  trackInfo.msg
  UAV.msg
)

generate_messages(
  DEPENDENCIES
  geometry_msgs
)

catkin_package(
	INCLUDE_DIRS include
	CATKIN_DEPENDS roscpp visualization_msgs message_runtime
	DEPENDS system_lib
)

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(plain1 src/plain1.cpp)
target_link_libraries(plain1 ${catkin_LIBRARIES})

add_executable(plainmod src/plainmod.cpp)
target_link_libraries(plainmod ${catkin_LIBRARIES})

add_executable(plainmod2 src/plainmod2.cpp)
target_link_libraries(plainmod2 ${catkin_LIBRARIES})

SET(SRC_Rviz_plugin_test1
./src/Rviz_plugin_test1.cpp
./src/PlainBase.cpp
)
add_executable(Rviz_plugin_test1 ${SRC_Rviz_plugin_test1})
target_link_libraries(Rviz_plugin_test1 ${catkin_LIBRARIES})
add_dependencies(Rviz_plugin_test1 ${catkin_EXPORTED_TARGETS})

SET(SRC_Rviz_plugin_test2
./src/Rviz_plugin_test2.cpp
./src/PlainBase.cpp
)
add_executable(Rviz_plugin_test2 ${SRC_Rviz_plugin_test2})
target_link_libraries(Rviz_plugin_test2 ${catkin_LIBRARIES})
add_dependencies(Rviz_plugin_test2 ${catkin_EXPORTED_TARGETS})

