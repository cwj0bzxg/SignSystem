# cmake_minimum_required(VERSION 2.8)
# project( faceSignIn )
# set(OpenCV_DIR ../opencv/build/)  # 重新设置opencvcv库的路径
# find_package( OpenCV REQUIRED )
# include_directories( ${OpenCV_INCLUDE_DIRS} )
# #include_directories( ${Boost_INCLUDE_DIRS} )
# add_executable( faceSignIn faceSignIn.cpp )
# # 动态链接库
# target_link_libraries( faceSignIn ${OpenCV_LIBS} )

cmake_minimum_required(VERSION 2.8)
project( faceSignIn )
# 链接boost
set(Boost_DIR ../boost/)
find_package(Boost 1.55.0 REQUIRED system filesystem)
# set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
# set(Boost_USE_STATIC_RUNTIME    OFF)
if(Boost_FOUND)
    message(STATUS "boost include path is : ${Boost_INCLUDE_DIRS}")
    message(STATUS "boost library path is : ${Boost_LIBRARY_DIRS}")
    message(STATUS "boost libraries is : ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    message(WARNING "boost not found.")
endif()
# 链接opencv
set(OpenCV_DIR ../opencv/build/)  # 重新设置opencvcv库的路径
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( faceSignIn faceSignIn.cpp )

# 动态链接库
#target_link_libraries( trainFaceModel  )
target_link_libraries( faceSignIn ${Boost_FILESYSTEM_LIBRARIES}
                                ${Boost_SYSTEM_LIBRARIES}
                                ${Boost_PROGRAM_OPTIONS_LIBRARIES} 
                                ${OpenCV_LIBS})