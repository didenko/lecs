set(
    ASIOS_SOURCE_FILES

    connection.cpp
    context.cpp
    node.cpp
)

add_library(
    asios
    STATIC
    ${ASIOS_SOURCE_FILES}
)

add_dependencies(asios asio)
