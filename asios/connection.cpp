//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection.hpp"
#include "connection_manager.hpp"

#include <iostream>

namespace asios {

connection::connection(
  asio::ip::tcp::socket socket,
  connection_manager &manager,
  context_ptr context
)
  : socket_(std::move(socket)),
  connection_manager_(manager),
  context(context)
{
  context->on_new_conn(shared_from_this(), socket_);
}

void connection::start()
{
  do_read();
}

void connection::stop()
{
  socket_.close();
}

void connection::do_read()
{
  auto self(shared_from_this());
  socket_.async_read_some(
    asio::buffer(buffer_),
    [this, self](std::error_code ec, std::size_t bytes_transferred) {
      if (!ec)
      {
//        collect_response result;
//        std::tie(result, std::ignore) = request_parser_.parse(
//          request_,
//          buffer_.data(),
//          buffer_.data() + bytes_transferred
//        );

        context->on_read(self, asio::buffer(buffer_), bytes_transferred);
        do_read();

//        if (result == decltype(result)::good)
//        {
//          context->on_request(request_, reply_);
//          do_write();
//        }
//        else if (result == decltype(result)::bad)
//        {
//          reply_ = httpl::reply::stock_reply(httpl::reply::bad_request);
//          do_write();
//        }
//        else
//        {
//          do_read();
//        }
      }
      else if (ec != asio::error::operation_aborted)
      {
        connection_manager_.stop(shared_from_this());
      }
    });
}

void connection::do_write()
{
  auto self(shared_from_this());
  asio::async_write(
    socket_, reply_.to_buffers(),
    [this, self](std::error_code ec, std::size_t) {
      if (!ec)
      {
        // Initiate graceful connection closure.
        asio::error_code ignored_ec;
        socket_.shutdown(asio::ip::tcp::socket::shutdown_both,
                         ignored_ec);
      }

      if (ec != asio::error::operation_aborted)
      {
        connection_manager_.stop(shared_from_this());
      }
    });
}

}
