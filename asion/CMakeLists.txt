set(
    ASION_SOURCE_FILES

    connection.cpp
    context.cpp
    node.cpp
)

add_library(
    asion
    STATIC
    ${ASION_SOURCE_FILES}
)

add_dependencies(asion asio)

add_executable(
    asion_tests
    asion_test.cpp
)
target_link_libraries(asion_tests ${GTEST_LIBS} asion)

enable_testing()

add_test(
    NAME asion_tests
    COMMAND asion_tests
)
