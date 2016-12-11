cmake_minimum_required(VERSION 3.6)
project(svr)

include(ExternalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DASIO_STANDALONE -pthread")

include(dep/asio.cmake)
AddAsio("${CMAKE_SOURCE_DIR}/dep/asio-1-10-8.tgz" 5cdb77d9b66242c04991ad6e9cdfbc1d)

include("${CMAKE_SOURCE_DIR}/dep/gtest.cmake")
AddGtest("${CMAKE_SOURCE_DIR}/dep/gtest-1.8.0.tgz" cc2b6bf4780012be6575f32a584606cf)
include_directories("${GTEST_INCLUDES}")

include_directories(
    "${CMAKE_SOURCE_DIR}"
)

add_subdirectory(asion)
add_subdirectory(lecs)
