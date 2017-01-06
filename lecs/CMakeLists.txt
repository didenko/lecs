set(
    LECS_SOURCE_FILES
    context.cpp
    context.cpp
    peers.cpp
)

add_library(
    lecs
    STATIC
    ${LECS_SOURCE_FILES}
)

add_dependencies(lecs asion)

add_executable(lecs_node example.cpp)
target_link_libraries(lecs_node lecs asion)

add_executable(
    lecs_tests
    lecs_test.cpp
)
target_link_libraries(lecs_tests ${GTEST_LIBS} lecs asion)

enable_testing()

add_test(
    NAME lecs_tests
    COMMAND lecs_tests
)
