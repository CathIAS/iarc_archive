cmake_minimum_required(VERSION 2.8.3)
project(image_trans_and_pub)

find_package(OpenCV REQUIRED)
find_package(catkin REQUIRED  COMPONENTS
		cv_bridge
		image_transport
		sensor_msgs
)

catkin_package(
	CATKIN_DEPENDS cv_bridge image_transport sensor_msgs
)	

################### find libusb-1.0 library
find_path(LIBUSB_1_INCLUDE_DIRS libusb.h
	/usr/include/libusb-1.0
)
message(STATUS "Found libusb.h: ${LIBUSB_1_INCLUDE_DIRS}")

find_library(LIBUSB_1_LIBRARY NAMES usb-1.0
	/usr/lib/x86_64-linux-gnu
)
message(STATUS "Found libusb-1.0 library: ${LIBUSB_1_LIBRARY}")

if (LIBUSB_1_LIBRARY AND LIBUSB_1_INCLUDE_DIRS)
	set(LIBUSB_1_FOUND TRUE)
	set(LIBUSB_1_LIBRARIES ${LIBUSB_1_LIBRARY})
else (LIBUSB_1_LIBRARY AND LIBUSB_1_INCLUDE_DIRS)
	set(LIBUSB_1_FOUND FALSE)
endif (LIBUSB_1_LIBRARY AND LIBUSB_1_INCLUDE_DIRS)

if (LIBUSB_1_FOUND)
	message(STATUS "Found library LIBUSB_!: ${LIBUSB_1_LIBRARIES}")
else (LIBUSB_1_FOUND)
	message(STATUS "Cannot find library LIBUSB_1")
endif (LIBUSB_1_FOUND)

# MARK_AS_ADVANCED(LIBUSB_1_INCLUDE_DIRS LIBUSB_1_LIBRARIES LIBUSB_1_LIBRARY)
####################
include_directories(
	${OpenCV_INCLUDE_DIRS}
	${LIBUSB_1_INCLUDE_DIRS}
	${catkin_INCLUDE_DIRS}
	./include/
)

add_executable(image_trans_and_pub src/main.cpp src/Decoder.cpp src/usb.cpp src/imagetransfer.cpp src/config.cpp src/common.cpp src/log.cpp)

target_link_libraries( image_trans_and_pub
	${OpenCV_LIBRARIES}
	${LIBUSB_1_LIBRARIES}
	${catkin_LIBRARIES}
)

