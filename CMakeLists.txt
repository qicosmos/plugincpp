cmake_minimum_required(VERSION 3.1)
project(plugin_app)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
add_definitions(-w)

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories(
        "/usr/local/include"
        "/usr/local/include/msgpack/include"
        "/usr/local/cinatra/include"
        "/usr/local/rest_rpc/include"
        )

add_library(custom SHARED plugin/custom_plugin.cc)
add_library(dummy SHARED plugin/dummy_plugin.cc)

add_executable(plugin_app plugin_app/main.cpp)
target_link_libraries(plugin_app ${Boost_LIBRARIES})

add_executable(client client/app_client.cpp)
