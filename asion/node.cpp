//
// node.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <utility>
#include <iostream>

#include "node.hpp"

namespace asion {

Node::Node(
  context_ptr context,
  const std::string &address,
  const std::string &port
) :
  context(context),
  resolver_{io_service_},
  acceptor_{io_service_}
{
  runner = std::thread(
    std::bind(
      &Node::run,
      this,
      [](const std::string &s) { std::cerr << s << std::endl; }
    )
  );

  if (address != "") start_accept(address, port);
}

Node::~Node()
{
  shutdown();
  if (runner.joinable()) runner.join();
}

void Node::start_accept(const std::string &address, const std::string &port)
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::error_code ec;
  try
  {
    auto endpoints = resolver_.resolve({address, port}, ec);
    if (ec)
    {
      std::cerr << "Failed to resolve the listen address: " << ec.message() << std::endl;
      std::exit(1);
    };

    asio::ip::tcp::endpoint endpoint{*endpoints};
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
    std::exit(1);
  }

  acceptor_.listen();

  do_accept();
}

void Node::run(std::function<void(const std::string &)> exception_handler)
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming clients.

  asio::io_service::work work(io_service_);
  for (;;)
  {
    try
    {
      io_service_.run();
      break;
    }
    catch (std::exception &e)
    {
      exception_handler(e.what());
    }
  };
}

asio::error_code Node::connect(const asio::ip::tcp::resolver::query &remote)
{
  asio::error_code ec;
  auto endpoints = resolver_.resolve(remote, ec);
  if (ec) return ec;

  auto conn = std::make_shared<connection>(io_service_, context);

  asio::async_connect(
    conn->socket(),
    endpoints,
    [this, conn](const std::error_code &ecc, asio::ip::tcp::resolver::iterator i) {
      if (!ecc)
      {
        context->on_connect(conn);
      }
    }
  );
  return ec;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "InfiniteRecursion"

void Node::do_accept()
{

  auto conn = std::make_shared<connection>(io_service_, context);

  acceptor_.async_accept(
    conn->socket(),
    [this, conn](const std::error_code &ec) {
      if (!acceptor_.is_open()) return;

      if (!ec)
      {
        context->on_connect(conn);
      }

      do_accept();
    });
}

void Node::shutdown()
{
//  asio::io_service::work work(io_service_);

  context->on_shutdown();
  acceptor_.close();
  io_service_.stop(); // TODO: This should not have to be done - but the thread hangs without it.
//  std::this_thread::sleep_for(std::chrono::microseconds(100));
}

}
