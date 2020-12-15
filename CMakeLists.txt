cmake_minimum_required(VERSION 3.1)
project(plugin_app)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++14")

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(
        "/usr/local/include"
        "/usr/local/include/msgpack/include"
        )

add_library(custom SHARED plugin/custom_plugin.cc)

add_executable(plugin_app plugin_app/main.cpp)
target_link_libraries(plugin_app ${Boost_LIBRARIES})
