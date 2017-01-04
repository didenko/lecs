//
// lecs_test.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <condition_variable>

#include <gtest/gtest.h>
#include "context.hpp"
#include "node.hpp"

namespace tests {
namespace asion {

using namespace std::placeholders;

class AsionTestContext
{
public:
  AsionTestContext(
    std::string &&tag
  ) :
    tag(std::move(tag)),
    ctx(std::make_shared<::asion::Context>(
      std::bind(&AsionTestContext::onconn, this, _1),
      [](::asion::connection_ptr c) {},
      std::bind(&AsionTestContext::dcon, this),
      [](::asion::connection_ptr c, const ::asion::buffer &b, std::size_t s) {},
      [](::asion::connection_ptr) { return std::vector<asio::const_buffer>(); }
    ))
  {}

  ~AsionTestContext()
  {
  }

  const ::asion::context_ptr &node_context()
  {
    return ctx;
  }

  void onconn(::asion::connection_ptr c)
  {
    conn = c;
    connected_confirmation = true;
    connected_flag.notify_one();
  }

  void dcon()
  {
    if (conn) conn->stop();
  }

protected:
  const std::string &tag;
  ::asion::context_ptr ctx;

public:
  ::asion::connection_ptr conn;
  std::condition_variable connected_flag;
  std::mutex connected_flag_mutex;
  bool connected_confirmation{false};
};

struct AsionNodes: public ::testing::Test
{
  constexpr static uint numruns{10000};
  std::string host{"localhost"};
  std::string port{"50001"};
  std::chrono::seconds timeout{5};
};

TEST_F(AsionNodes, connect_no_wait)
{
  for (uint i = 0; i < numruns; ++i)
  {
    AsionTestContext
      server_ctx{"server"},
      client_ctx{"client"};

    ::asion::Node
      server{server_ctx.node_context(), host, port},
      client{client_ctx.node_context()};

    auto ec = client.connect({host, port});
    ASSERT_FALSE(ec) << ec.message();
  }
}

TEST_F(AsionNodes, connect_destruct)
{
  for (uint i = 0; i < numruns; ++i)
  {
    AsionTestContext
      server_ctx{"server"},
      client_ctx{"client"};

    ::asion::Node
      server{server_ctx.node_context(), host, port},
      client{client_ctx.node_context()};

    auto ec = client.connect({host, port});
    ASSERT_FALSE(ec) << ec.message();

    {
      std::unique_lock<std::mutex> connection_flag_lock(client_ctx.connected_flag_mutex);
      EXPECT_TRUE(client_ctx.connected_flag.wait_for(
        connection_flag_lock,
        timeout,
        [&client_ctx]() {
          auto old = client_ctx.connected_confirmation;
          client_ctx.connected_confirmation = false;
          return old;
        }
      )) << "Connection from client to server timed out on iteration " << i;
    }
  }
}

TEST_F(AsionNodes, connect_disconnect)
{
  for (uint i = 0; i < numruns; ++i)
  {
    AsionTestContext
      server_ctx{"server"},
      client_ctx{"client"};

    ::asion::Node
      server{server_ctx.node_context(), host, port},
      client{client_ctx.node_context()};

    auto ec = client.connect({host, port});
    ASSERT_FALSE(ec) << ec.message();

    {
      std::unique_lock<std::mutex> connection_flag_lock(client_ctx.connected_flag_mutex);
      EXPECT_TRUE(client_ctx.connected_flag.wait_for(
        connection_flag_lock,
        timeout,
        [&client_ctx]() {
          auto old = client_ctx.connected_confirmation;
          client_ctx.connected_confirmation = false;
          return old;
        }
      )) << "Connection from client to server timed out on iteration " << i;
    }

    client_ctx.conn.reset();
    {
      std::unique_lock<std::mutex> connection_flag_lock(client_ctx.connected_flag_mutex);
      EXPECT_TRUE(client_ctx.connected_flag.wait_for(
        connection_flag_lock,
        timeout,
        [&client_ctx]() {
          auto old = client_ctx.connected_confirmation;
          client_ctx.connected_confirmation = true;
          return !old;
        }
      )) << "Disconnection to server timed out on iteration " << i;
    }
  }
}

}
}
