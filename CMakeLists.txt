cmake_minimum_required(VERSION 3.10)
project(DistributedKeyValueStore)

# Specify the version of Boost to find
set(BOOST_ROOT "C:/Users/91702/Desktop/LORS/boost")
set(BOOST_LIBRARYDIR "C:/Users/91702/Desktop/LORS/boost/stage/lib")

find_package(Boost REQUIRED COMPONENTS system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(DistributedKeyValueStore server.cpp)
    target_link_libraries(DistributedKeyValueStore ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find Boost libraries")
endif()
