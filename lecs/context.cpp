//
// context.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include "context.hpp"

namespace lecs {

Context::Context()
  : Context(
  [](asion::connection_ptr) {},
  [](asion::connection_ptr) {},
  [](asion::connection_ptr, lecs::Message m) {}
)
{}

Context::Context(OnConnect c, OnDisconnect d, Intake i) :
  on_conn(c),
  on_disc(d),
  intake(i)
{}

asion::context_ptr Context::node_context()
{
  using namespace std::placeholders;
  return std::make_shared<asion::Context>(
    std::bind(&::lecs::Context::on_connect, this, _1),
    std::bind(&::lecs::Context::on_disconnect, this, _1),
    std::bind(&::lecs::Context::shutdown, this),
    std::bind(&::lecs::Context::on_read, this, _1, _2, _3),
    std::bind(&::lecs::Context::on_write, this, _1)
  );
}

void Context::on_connect(asion::connection_ptr conn)
{
  peers.add(conn);
  on_conn(conn);
}

void Context::on_disconnect(asion::connection_ptr conn)
{
  on_disc(conn);
  conn->stop();
  peers.del(conn);
}

void Context::shutdown(void)
{
  peers.shutdown();
}

void Context::on_read(
  asion::connection_ptr conn,
  const asion::buffer &buf,
  std::size_t sz
)
{
  Cursor
    cursor{buf.begin()},
    last{cursor + sz - 1};

  Message raw_message;

  while (get_line(raw_message, cursor, last))
    intake(conn, std::move(raw_message));
}

std::vector<asio::const_buffer> Context::on_write(asion::connection_ptr)
{
  assert(false);
  std::vector<asio::const_buffer> buffers;
  return buffers;
}

void Context::write(asion::connection_ptr conn, const std::string &messages)
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

std::string Context::endpoint(asion::connection_ptr conn, bool remote)
{
  return remote ? conn->endpoint_remote() : conn->endpoint_local();
}

}