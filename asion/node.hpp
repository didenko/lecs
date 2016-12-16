//
// node.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <asio.hpp>
#include <string>
#include <thread>
#include "connection.hpp"
#include "context.hpp"

namespace asion {

class Node
{
public:
  Node(const Node &) = delete;

  Node &operator=(const Node &) = delete;

  explicit Node(
    context_ptr context,
    const std::string &address = "",
    const std::string &port = ""
  );

  ~Node();

  void run(std::function<void(const std::string &)>);

  void shutdown();

  asio::error_code connect(const asio::ip::tcp::resolver::query &remote);

private:

  void start_accept(const std::string &address, const std::string &port);

  void do_accept();

  context_ptr context;
  asio::io_service io_service_{};
  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::resolver resolver_;
  std::thread runner;
};
}
