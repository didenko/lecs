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
{
  using namespace std::placeholders;
  basic_context = std::make_shared<asion::Context>(
    std::bind(&::lecs::Context::on_connect, this, _1),
    std::bind(&::lecs::Context::on_disconnect, this, _1),
    std::bind(&::lecs::Context::shutdown, this),
    std::bind(&::lecs::Context::admit_read, this, _1, _2),
    std::bind(&::lecs::Context::on_read, this, _1, _2),
    std::bind(&::lecs::Context::on_write, this, _1)
  );
}

asion::context_ptr Context::node_context()
{
  return basic_context;
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

void Context::admit_read(asion::connection_ptr conn, std::function<void(std::error_code, std::size_t)> handler)
{
  asio::async_read_until(
    conn->socket(),
    *(peers.at(conn)),
    eol,
    handler
  );
}

void Context::on_read(
  asion::connection_ptr conn,
  std::size_t sz
)
{
  std::istream in_stream(peers.at(conn).get());
  Message raw_message;
  std::getline(in_stream, raw_message);
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

std::string Context::endpoint(asion::connection_ptr conn, bool remote)
{
  return remote ? conn->endpoint_remote() : conn->endpoint_local();
}

}