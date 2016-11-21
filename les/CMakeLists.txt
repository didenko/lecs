include(ExternalProject)

include("${CMAKE_SOURCE_DIR}/dep/gtest.cmake")
AddGtest("${CMAKE_SOURCE_DIR}/dep/gtest-1.8.0.tgz" cc2b6bf4780012be6575f32a584606cf)

set(
    LES_SOURCE_FILES
    les_context.cpp
    les_context.cpp
    les_peers.cpp
    tests.cpp)

include_directories(
    "${GTEST_INCLUDES}"
)

add_library(
    les
    STATIC
    ${LES_SOURCE_FILES}
)

add_dependencies(les asios)

add_executable(les_example les_example.cpp)
target_link_libraries(les_example les asios)

add_executable(les_tests tests.cpp)
target_link_libraries(les_tests ${GTEST_LIBS} les asios)

enable_testing()

add_test(
    NAME les
    COMMAND les_tests
)
