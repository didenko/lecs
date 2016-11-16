#
# asio.cmake
# ~~~~~~~~~~
#
# Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#

macro(AddAsio FileName Md5)
    ExternalProject_Add(
        asio
        URL "${FileName}"
        URL_MD5 ${Md5}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(asio install_dir)
    set(ASIO_ROOT "${install_dir}/src/asio/asio" CACHE INTERNAL "")

    set(ASIO_INCLUDE_DIR "${ASIO_ROOT}/include")

    include_directories("${ASIO_INCLUDE_DIR}")
endmacro()
