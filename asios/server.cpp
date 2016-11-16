//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <signal.h>
#include <utility>
#include <iostream>

#include "server.hpp"

namespace asios {

server::server(
  const std::string &address,
  const std::string &port,
  context_ptr context
) :
  io_service_(),
  signals_(io_service_),
  acceptor_(io_service_),
  socket_(io_service_),
  context(context)
{
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  signals_.add(SIGINT);
  signals_.add(SIGTERM);

  #if defined(SIGQUIT)
  signals_.add(SIGQUIT);
  #endif // defined(SIGQUIT)

  do_await_stop();

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  asio::ip::tcp::resolver resolver{io_service_};
  asio::error_code ec;
  auto ep_iter = resolver.resolve({address, port}, ec);
  if (ec)
  {
    std::cerr << "Failed to resolve the listen address: " << ec.message() << std::endl;
    raise(SIGTERM);
  };

  asio::ip::tcp::endpoint endpoint{*ep_iter};
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  do_accept();
}

void server::run()
{
  // The io_service::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming clients.
  io_service_.run();
}

void server::do_accept()
{
  acceptor_.async_accept(
    socket_,
    [this](std::error_code ec) {
      // Check whether the server was stopped by a signal before this
      // completion handler had a chance to run.
      if (!acceptor_.is_open()) return;

      if (!ec)
      {
        context->on_connect(
          std::make_shared<connection>(
            std::move(socket_),
            context
          )
        );
      }

      do_accept();
    });
}

void server::do_await_stop()
{
  signals_.async_wait(
    [this](std::error_code /*ec*/, int /*signo*/) {
      // The server is stopped by cancelling all outstanding asynchronous
      // operations. Once all operations have finished the io_service::run()
      // call will exit.
      acceptor_.close();
      context->on_shutdown();
    });
}

}
