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

enum collect_response
{
  good, bad, indeterminate
};
typedef std::function<collect_response(asio::mutable_buffers_1, std::size_t)> request_collect;

struct Context
{
public:
  Context(
    connection_new c,
    request_collect rc,
    request_handler rh
  ) :
    on_new_conn(c),
    do_collect(rc),
    on_request(rh)
  {};

  const connection_new on_new_conn;
  const request_collect do_collect;
  const request_handler on_request;
};

using context_ptr = std::shared_ptr<Context>;

} // namespace ses
