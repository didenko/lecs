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
    [](::asion::connection_ptr conn) {
      std::cerr << "Connection from: " << std::string(conn->endpoint_remote()) << std::endl;
    },
    [](::asion::connection_ptr conn) {
      std::cerr << "Disconnected from: " << std::string(conn->endpoint_remote()) << std::endl;
    },
    [this](::asion::connection_ptr conn, ::lecs::Message m) {
      handlers.write(conn, m + "\n");
      std::cout << std::string(conn->endpoint_remote()) << " :> " << m << std::endl;
    }
  };

  asion::context_ptr context = handlers.node_context();

  asion::Node
    server{context},
    client{context},
    cliser{context};
};

TEST_F(Nodes, connect)
{
}

}
}