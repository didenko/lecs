//
// server.hpp
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
#include "connection.hpp"
#include "context.hpp"

namespace asios {

/// The top-level class of the HTTP server.
class server
{
public:
  server(const server &) = delete;

  server &operator=(const server &) = delete;

  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(
    context_ptr context,
    const std::string &address = "",
    const std::string &port = ""
  );

  /// Run the server's io_service loop.
  void run();

  asio::error_code connect(const asio::ip::tcp::resolver::query &remote);

private:

  /// Resolve listening address and start the acceptor loop
  void start_accept(const std::string &address, const std::string &port);

  /// Perform an asynchronous accept operation.
  void do_accept();

  /// Wait for a request to stop the server.
  void do_await_stop();

  /// The io_service used to perform asynchronous operations.
  asio::io_service io_service_;

  asio::ip::tcp::resolver resolver_;

  /// The signal_set is used to register for process termination notifications.
  asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  asio::ip::tcp::acceptor acceptor_;

  // TODO: Is it OK (in this code) to share the socket object between acceptors and connectors?

  /// The next socket to be accepted or connected to
  asio::ip::tcp::socket socket_;

  /// The handler for all incoming requests.
//  request_handler request_handler_;
  context_ptr context;
};

}
