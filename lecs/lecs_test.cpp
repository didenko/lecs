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

#include <gtest/gtest.h>
#include "context.hpp"

namespace tests {
namespace lecs {

struct Nodes: public ::testing::Test
{
  ::lecs::Context handlers{
    [](::asios::connection_ptr conn) {
      std::cerr << "Connection from: " << std::string(conn->endpoint_remote()) << std::endl;
    },
    [](::asios::connection_ptr conn) {
      std::cerr << "Disconnected from: " << std::string(conn->endpoint_remote()) << std::endl;
    },
    [this](::asios::connection_ptr conn, ::lecs::Message m) {
      handlers.write(conn, m + "\n");
      std::cout << std::string(conn->endpoint_remote()) << " :> " << m << std::endl;
    }
  };

  std::shared_ptr<asios::Context> context = std::make_shared<asios::Context>(
    std::bind(&::lecs::Context::on_connect, &handlers, std::placeholders::_1),
    std::bind(&::lecs::Context::on_disconnect, &handlers, std::placeholders::_1),
    std::bind(&::lecs::Context::shutdown, &handlers),
    std::bind(&::lecs::Context::on_read,
              &handlers,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3),
    std::bind(&::lecs::Context::on_write, &handlers, std::placeholders::_1)
  );

  asios::Node
    server{context},
    client{context},
    cliser{context};
};

TEST_F(Nodes, connect)
{
}

}
}