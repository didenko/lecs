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

#include "connection.hpp"

namespace asion {

connection::endpoint_address::operator std::string() const
{
  if (err) return "unknown: " + err.message();
  return addr.to_string() + ":" + std::to_string(port);
}

connection::connection(
  asio::io_service &ios,
  context_ptr context
) :
  socket_{ios},
  context(context)
{
  endpoint_local();
  endpoint_remote();
}

connection_ptr connection::ptr()
{
  return this->shared_from_this();
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  if (socket_.is_open())
  {
    asio::error_code ignored_ec;
    socket_.close(ignored_ec);
  }
}

asio::ip::tcp::socket &connection::socket()
{
  return socket_;
}

const connection::endpoint_address &connection::endpoint_local() const
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

const connection::endpoint_address &connection::endpoint_remote() const
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
  context->admit_read(
    self,
    [this, self](std::error_code ec, std::size_t bytes_transferred) {
      if (!ec)
      {
        context->on_read(self, bytes_transferred);
        do_read();
      }
      else
      {
        context->on_disconnect(self, ec.message());
      }
    }
  );
}

void connection::do_write(const std::vector<asio::const_buffer> &buffers)
{
  auto self(this->shared_from_this());
  asio::async_write(
    socket_,
    buffers,
    [this, self](std::error_code ec, std::size_t) {
      if (ec)
      {
        context->on_disconnect(
          self,
          "error after writing to " +
            std::string{remote} +
            ": " + ec.message());
      }
    }
  );
}

}
