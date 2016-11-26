//
// example.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>

#include <asio.hpp>
#include "asios/node.hpp"
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

    lecs::Context handlers(
      [](asios::connection_ptr conn) {
        std::cerr << "Connection from: " << std::string(conn->endpoint_remote()) << std::endl;
      },
      [](asios::connection_ptr conn) {
        std::cerr << "Disconnected from: " << std::string(conn->endpoint_remote()) << std::endl;
      },
      [&handlers](asios::connection_ptr conn, lecs::Message m) {
        handlers.write(conn, m + "\n");
        std::cout << std::string(conn->endpoint_remote()) << " :> " << m << std::endl;
      }
    );

    using namespace std::placeholders;
    auto context = std::make_shared<asios::Context>(
      std::bind(&lecs::Context::on_connect, &handlers, _1),
      std::bind(&lecs::Context::on_disconnect, &handlers, _1),
      std::bind(&lecs::Context::shutdown, &handlers),
      std::bind(&lecs::Context::on_read, &handlers, _1, _2, _3),
      std::bind(&lecs::Context::on_write, &handlers, _1)
    );
    // Initialise the server.
    asios::Node s(context, argv[1], argv[2]);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
