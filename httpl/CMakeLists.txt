set(
    HTTPL_SOURCE_FILES

    connection_manager.cpp
    mime_types.cpp
    reply.cpp
    request_handler.cpp
    request_parser.cpp
    http_context.cpp
)

add_library(
    httpl
    STATIC
    ${HTTPL_SOURCE_FILES}
)

add_dependencies(httpl asios)
