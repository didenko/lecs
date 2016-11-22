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
  : Context([](asios::connection_ptr, les::Message m) {})
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
    intake(conn, std::move(raw_message));

  return asios::next::read;
}

std::vector<asio::const_buffer> Context::on_write(asios::connection_ptr)
{
  assert(false);
  std::vector<asio::const_buffer> buffers;
  return buffers;
}

void Context::write(asios::connection_ptr conn, const std::string &messages)
{
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(asio::buffer(messages));
  conn->do_write(buffers);
}

bool Context::get_line(Message &msg, Cursor &current, const Cursor &last)
{
  if (current == last) return false;

  Cursor first{current};

  while (current != last && *current != eol) ++current;

  msg.assign(first, current);

  if (current != last) ++current;
  return true;
}

std::string Context::endpoint(asios::connection_ptr conn, bool remote)
{
  return remote ? conn->endpoint_remote() : conn->endpoint_local();
}

}