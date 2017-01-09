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
#include <vector>
#include <thread>
#include <condition_variable>

#include <gtest/gtest.h>
#include "context.hpp"

namespace tests {
namespace lecs {

using namespace std::placeholders;

template<class outer_type>
const typename outer_type::value_type flatten(const outer_type &outer)
{
  typename outer_type::value_type flat;
  for (auto &inner: outer)
  {
    flat.insert(flat.end(), inner.begin(), inner.end());
  }
  return flat;
};

using message_vec = std::vector<std::vector<std::string>>;

const message_vec messages{
  {""},
  {"", ""},
  {"abc", "1234567890", "ABCDEFGHIJ"},
  {"", "XYZ", ""}
};

const message_vec::value_type messages_flat{flatten(messages)};

class TestContext
{
public:
  TestContext() : ctx(
    std::bind(&TestContext::onconn, this, _1),
    std::bind(&TestContext::ondisc, this, _1),
    std::bind(&TestContext::intake, this, _1, _2)
  )
  {}

  asion::context_ptr node_context()
  {
    return ctx.node_context();
  }

  void onconn(asion::connection_ptr c)
  {
    cursor = messages_flat.begin();
    message_idx = 0;
    connected_flag.notify_one();
    conn = c;
  }

  void ondisc(asion::connection_ptr c)
  {}

  void write(asion::connection_ptr c, const std::string &str)
  {
    ctx.write(c, str);
  }

  void replay()
  {
    for (auto &msg: messages)
    {
      std::string str;
      for (auto &part: msg)
      {
        str += part + ::lecs::eol;
      }
      write(conn, str);
    }
  }

  void intake(asion::connection_ptr c, ::lecs::Message &&received)
  {
    ASSERT_NE(cursor, messages_flat.end()) << "Received message is unexpected: " << received;
    ASSERT_EQ(*cursor, received) << "Received message differs from the expected, idx " << message_idx;
    ++cursor;
    ++message_idx;
  }

  void finished()
  {
    ASSERT_EQ(cursor, messages_flat.end()) << "Did not receive all expected messages";
  }

  void disconnect()
  {
    conn->stop();
    conn.reset();
  }

  std::condition_variable connected_flag, finished_flag;
  uint message_idx{10};
protected:
  ::lecs::Context ctx;
  asion::connection_ptr conn;
  message_vec::value_type::const_iterator cursor{messages_flat.begin()};
};

struct Nodes: public ::testing::Test
{
  std::string
    host{"127.0.0.1"},
    port{"50001"};
};

TEST_F(Nodes, connect_with_replay)
{
  std::mutex connected_flag_mutex, finished_flag_mutex;

  TestContext
    server_context,
    client_context;

  asion::Node
    server{server_context.node_context(), host, port},
    client{client_context.node_context()};

  client.connect({host, port});
  {
    std::unique_lock<std::mutex> connection_flag_lock(connected_flag_mutex);
    ASSERT_TRUE(client_context.connected_flag.wait_for(
      connection_flag_lock,
      std::chrono::seconds(1),
      [&client_context]() { return client_context.message_idx == 0; }
    )) << "Connection from client to server timed out";
  }

  client_context.replay();

  client_context.disconnect();
}

TEST(Flatten, strings)
{
  std::vector<std::vector<std::string>> in = {
    {""},
    {"", ""},
    {"abc", "1234567890", "ABCDEFGHIJ"},
    {"", "XYZ", ""}
  };

  std::vector<std::string> out = {"", "", "", "abc", "1234567890", "ABCDEFGHIJ", "", "XYZ", ""};

  ASSERT_EQ(out, flatten(in));
};

}
}