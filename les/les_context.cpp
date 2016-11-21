//
// les_context.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include "les_context.hpp"

namespace les {

Context::Context()
  : Context([](les::Message m) {})
{}

Context::Context(Intake i)
  : intake(i)
{}

void Context::on_connect(asios::connection_ptr conn)
{
  peers.add(conn);
  std::cerr << "Connection from: " << endpoint(conn) << std::endl;
}

void Context::on_disconnect(asios::connection_ptr conn)
{
  conn->stop();
  peers.del(conn);
  std::cerr << "Disconnected from: " << endpoint(conn) << std::endl;
}

void Context::shutdown(void)
{
  peers.shutdown();
  std::cerr << "The server is shutting down." << std::endl;
}

asios::next Context::on_read(
  asios::connection_ptr conn,
  const asios::buffer &buf,
  std::size_t sz
)
{
  Cursor
    cursor{buf.begin()},
    last{cursor + sz - 1};

  Message raw_message;
  while (get_line(raw_message, cursor, last))
    intake(std::move(raw_message));

  return asios::next::read;
}

std::vector<asio::const_buffer> Context::on_write(asios::connection_ptr)
{
  std::vector<asio::const_buffer> buffers;
  return buffers;
}

bool Context::get_line(Message &msg, Cursor &current, const Cursor &last)
{
  Cursor first{current};

  while (current != last && *current != eol) ++current;

  if (current == last)
  {
    msg.assign(first, current);
    return false;
  }

  msg.assign(first, current);
  ++current;
  return true;
}

std::string Context::endpoint(asios::connection_ptr conn, bool remote)
{
  return remote ? conn->endpoint_remote() : conn->endpoint_local();
}

}