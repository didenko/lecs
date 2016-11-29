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

namespace asion {

connection::endpoint_address::operator std::string() const
{
  if (err) return "unknown: " + err.message();
  return addr.to_string() + ":" + std::to_string(port);
}

connection::connection(
  asio::ip::tcp::socket socket,
  context_ptr context
)
  : socket_(std::move(socket)),
  context(context)
{
  endpoint_local();
  endpoint_remote();
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  // Initiate graceful connection closure.
  asio::error_code ignored_ec;
  socket_.shutdown(
    asio::ip::tcp::socket::shutdown_both,
    ignored_ec
  );
}

const connection::endpoint_address &connection::endpoint_local()
{
  if (!local.err) return local;
  auto ep = socket_.local_endpoint(local.err);
  if (!local.err)
  {
    local.addr = ep.address();
    local.port = ep.port();
  }
  return local;
};

const connection::endpoint_address &connection::endpoint_remote()
{
  if (!remote.err) return remote;
  auto ep = socket_.remote_endpoint(remote.err);
  if (!remote.err)
  {
    remote.addr = ep.address();
    remote.port = ep.port();
  }
  return remote;
};

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
          case next::disconnect:
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

void connection::do_write(const std::vector<asio::const_buffer> &buffers)
{
  auto self(shared_from_this());
  asio::async_write(
    socket_,
    buffers,
    [this, self](std::error_code ec, std::size_t) {
      if (ec)
      {
        std::cerr << "post-write to " << std::string(remote) << ": " << ec.message() << std::endl;
        context->on_disconnect(shared_from_this());
      }
    }
  );
}

void connection::do_write()
{
  auto self(shared_from_this());
  do_write(context->on_write(self));
}

}
