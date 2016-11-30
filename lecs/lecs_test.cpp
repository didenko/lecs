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

class TestContext: public ::lecs::Context
{
public:
  TestContext() : ::lecs::Context(
    std::bind(&TestContext::onconn, this, _1),
    std::bind(&TestContext::ondisc, this, _1),
    std::bind(&TestContext::intake, this, _1, _2)
  )
  {}

  void onconn(asion::connection_ptr c)
  {
    cursor = messages_flat.begin();
    message_idx = 0;
  }

  void ondisc(asion::connection_ptr c)
  {}

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

protected:
  message_vec::value_type::const_iterator cursor{messages_flat.begin()};
  uint message_idx{0};

};

struct Nodes: public ::testing::Test
{
  TestContext server_context, client_context, cl_ser_context;

  asion::Node
    server{server_context.node_context(), "0.0.0.0", "50001"},
    client{client_context.node_context()},
    cl_ser{cl_ser_context.node_context(), "0.0.0.0", "50002"};

  std::thread
    server_thread,
    client_thread,
    cl_ser_thread;

protected:
  virtual void SetUp()
  {
    server_thread = std::thread(std::bind(&asion::Node::run, &server));
    client_thread = std::thread(std::bind(&asion::Node::run, &client));
    cl_ser_thread = std::thread(std::bind(&asion::Node::run, &cl_ser));
  }

  virtual void TearDown()
  {
    server.shutdown();
    server_thread.join();

    client.shutdown();
    client_thread.join();

    cl_ser.shutdown();
    cl_ser_thread.join();
  }

};

TEST_F(Nodes, connect)
{
//  client.connect({"localhost", "50002"});
  cl_ser.connect({"localhost", "50001"});
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