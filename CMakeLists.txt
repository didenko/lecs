cmake_minimum_required(VERSION 3.6)
project(svr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DASIO_STANDALONE -pthread")

include(ExternalProject)

include(dep/asio.cmake)
AddAsio("${CMAKE_SOURCE_DIR}/dep/asio-1-10-8.tgz" 5cdb77d9b66242c04991ad6e9cdfbc1d)

set(SOURCE_FILES
    main.cpp
    asios/connection.cpp
    asios/connection_manager.cpp
    httpl/mime_types.cpp
    httpl/reply.cpp
    httpl/request_handler.cpp
    httpl/request_parser.cpp
    asios/server.cpp
)
add_executable(svr ${SOURCE_FILES})