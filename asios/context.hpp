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

using buffer = std::array<char, 8192>;

enum class next
{
  read, write, disconnect
};

class connection;

using connection_ptr = std::shared_ptr<connection>;

using connected = std::function<void(connection_ptr)>;
using disconnected = std::function<void(connection_ptr)>;
using shutdown = std::function<void(void)>;
using reader = std::function<next(connection_ptr, const buffer &, std::size_t)>;
using writer = std::function<std::vector<asio::const_buffer>(connection_ptr)>;

struct Context
{
public:
  Context(
    connected c,
    disconnected d,
    shutdown s,
    reader r,
    writer w
  );

  const connected on_connect;
  const disconnected on_disconnect;
  const shutdown on_shutdown;
  const reader on_read;
  const writer on_write;
};

using context_ptr = std::shared_ptr<Context>;

} // namespace ses
