cmake_minimum_required( VERSION 2.8 )
project( classInfo )
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

include_directories(tinyMysqlLib)
aux_source_directory(tinyMysqlLib SRC_LIST)

add_executable( classInfo classInfo.cpp ${SRC_LIST})

target_link_libraries( classInfo ${Boost_FILESYSTEM_LIBRARIES}
                                ${Boost_SYSTEM_LIBRARIES}
                                ${Boost_PROGRAM_OPTIONS_LIBRARIES} 
                                -lmysqlclient)