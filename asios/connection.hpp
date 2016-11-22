//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <array>
#include <memory>
#include <asio.hpp>
#include "context.hpp"

namespace asios {

class connection_manager;

/// Represents a single connection from a client.
class connection
  : public std::enable_shared_from_this<connection>
{
public:

  struct endpoint_address
  {
    asio::ip::address addr{};
    unsigned short port{};
    asio::error_code err{asio::error::invalid_argument};

    operator std::string() const;
  };

  /// Construct a connection with the given socket.
  explicit connection(
    asio::ip::tcp::socket socket,
    context_ptr context
  );

  connection(const connection &) = delete;

  connection &operator=(const connection &) = delete;

  /// Start the first asynchronous operation for the connection.
  void start();

  /// Stop all asynchronous operations associated with the connection.
  void stop();

  const endpoint_address &endpoint_local();

  const endpoint_address &endpoint_remote();

  /// Perform an asynchronous write operation.
  void do_write();

  void do_write(const std::vector<asio::const_buffer> &);

private:
  /// Perform an asynchronous read operation.
  void do_read();

  /// Socket for the connection.
  asio::ip::tcp::socket socket_;

  /// Cached connection addresses
  endpoint_address local, remote;

  /// The context with nesessary handlers.
  context_ptr context;

  /// Buffer for incoming data.
  buffer buffer_;
};

}
