include(ExternalProject)

include("${CMAKE_SOURCE_DIR}/dep/gtest.cmake")
AddGtest("${CMAKE_SOURCE_DIR}/dep/gtest-1.8.0.tgz" cc2b6bf4780012be6575f32a584606cf)

set(
    LECS_SOURCE_FILES
    context.cpp
    context.cpp
    peers.cpp
    context_test.cpp)

include_directories(
    "${GTEST_INCLUDES}"
)

add_library(
    lecs
    STATIC
    ${LECS_SOURCE_FILES}
)

add_dependencies(lecs asios)

add_executable(lecs_node example.cpp)
target_link_libraries(lecs_node lecs asios)

add_executable(
    lecs_tests
    context_test.cpp
    lecs_test.cpp
)
target_link_libraries(lecs_tests ${GTEST_LIBS} lecs asios)

enable_testing()

add_test(
    NAME lecs_tests
    COMMAND lecs_tests
)
