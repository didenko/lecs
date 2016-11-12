//
// context.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <asio.hpp>

namespace asios {

class connection;

typedef std::shared_ptr<connection> connection_ptr;

using connection_new = std::function<void(connection_ptr, const asio::ip::tcp::socket &)>;
using read_handler = std::function<void(connection_ptr, asio::mutable_buffers_1, std::size_t)>;

struct Context
{
public:
  Context(
    connection_new c,
    read_handler rh
  ) :
    on_new_conn(c),
    on_read(rh)
  {};

  const connection_new on_new_conn;
  const read_handler on_read;
};

using context_ptr = std::shared_ptr<Context>;

} // namespace ses
