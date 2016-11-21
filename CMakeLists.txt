cmake_minimum_required(VERSION 3.6)
project(svr)

include(ExternalProject)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -DASIO_STANDALONE -pthread")

include(dep/asio.cmake)
AddAsio("${CMAKE_SOURCE_DIR}/dep/asio-1-10-8.tgz" 5cdb77d9b66242c04991ad6e9cdfbc1d)

include_directories(
    "${CMAKE_SOURCE_DIR}"
)

add_subdirectory(asios)
add_subdirectory(httpl)

add_executable(http_example http_example.cpp)
target_link_libraries(http_example asios httpl)
