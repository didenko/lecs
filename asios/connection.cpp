//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <signal.h>

#include "connection.hpp"

namespace asios {

connection::connection(
  asio::ip::tcp::socket socket,
  context_ptr context
)
  : socket_(std::move(socket)),
  context(context)
{}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  socket_.close();
}

std::tuple<asio::ip::tcp::endpoint, asio::error_code> connection::endpoint_local() const
{
  asio::error_code ec;
  auto ep = socket_.local_endpoint(ec);
  return std::make_tuple(
    std::move(ep),
    std::move(ec)
  );
};

std::tuple<asio::ip::tcp::endpoint, asio::error_code> connection::endpoint_remote() const
{
  asio::error_code ec;
  auto ep = socket_.remote_endpoint(ec);
  return std::make_tuple(
    std::move(ep),
    std::move(ec)
  );
};

std::tuple<asio::ip::tcp::endpoint, asio::error_code> connection::endpoint_remote() const;

void connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(
    asio::buffer(buffer_),
    [this, self](std::error_code ec, std::size_t bytes_transferred) {
      if (!ec)
      {
        switch (context->on_read(self, buffer_, bytes_transferred))
        {
          case next::read:
            do_read();
            break;
          case next::write:
            do_write();
            break;
          case next::diconnect:
            context->on_disconnect(self);
            raise(SIGTERM);
            break;
        };
      }
      else if (ec != asio::error::operation_aborted)
      {
        context->on_disconnect(self);
      }
    });
}

void connection::do_write()
{
  auto self(shared_from_this());
  asio::async_write(
    socket_,
    context->on_write(self),
    [this, self](std::error_code ec, std::size_t) {
      if (!ec)
      {
        // Initiate graceful connection closure.
        asio::error_code ignored_ec;
        socket_.shutdown(
          asio::ip::tcp::socket::shutdown_both,
          ignored_ec
        );
      }

      if (ec != asio::error::operation_aborted)
      {
        context->on_disconnect(shared_from_this());
      }
    });
}

}
