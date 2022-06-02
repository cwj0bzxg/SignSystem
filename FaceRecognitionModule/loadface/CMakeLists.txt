cmake_minimum_required(VERSION 2.8)
project( trainFaceModel )

# 链接opencv
set(OpenCV_DIR ../opencv/build/)  # 重新设置opencvcv库的路径
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( trainFaceModel loadface.cpp )

# 动态链接库

target_link_libraries( trainFaceModel  ${OpenCV_LIBS})