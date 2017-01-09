//
// example.cpp
// ~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>

#include <asio.hpp>
#include "asion/node.hpp"
#include "lecs/context.hpp"

int main(int argc, char *argv[])
{
  try
  {
    // Check command line arguments.
    if (argc < 3)
    {
      std::cerr << "Usage: http_server <address> <port>>" << std::endl;
      return 1;
    }

    lecs::Context handlers{
      [](asion::connection_ptr conn) {
        std::cerr << "Connection from: " << std::string{conn->endpoint_remote()} << std::endl;
      },
      [](asion::connection_ptr conn, const std::string & diag) {
        std::cerr << "Disconnected from: " << std::string(conn->endpoint_remote()) << ", reason: " << diag << std::endl;
      },
      [&handlers](asion::connection_ptr conn, lecs::Message m) {
        handlers.write(conn, m + "\n");
        std::cout << std::string(conn->endpoint_remote()) << " :> " << m << std::endl;
      }
    };

    auto context = handlers.node_context();

    asion::Node s(context, argv[1], argv[2]);

    // Wait for the server to stop.
    s.wait();
  }
  catch (std::exception &e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
