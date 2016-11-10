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

using connection_new = std::function<void(const asio::ip::tcp::socket &)>;
using request_handler = std::function<void(const ses::request &, ses::reply &)>;

class Context
{
public:
  Context(
    connection_new c,
    request_handler r
  ) :
    on_new_conn(c),
    on_request(r)
  {};

  const connection_new on_new_conn;
  const request_handler on_request;
};

using context_ptr = std::shared_ptr<Context>;

} // namespace ses
