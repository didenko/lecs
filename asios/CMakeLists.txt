set(
    ASIOS_SOURCE_FILES

    connection.cpp
    context.cpp
    server.cpp
)

add_library(
    asios
    STATIC
    ${ASIOS_SOURCE_FILES}
)

add_dependencies(asios asio)
