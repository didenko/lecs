//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>

#include <asio.hpp>
#include "asios/server.hpp"
#include "httpl/http_context.hpp"

int main(int argc, char *argv[])
{
  try
  {
    // Check command line arguments.
    if (argc < 4)
    {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }

    httpl::http_context handlers{argv[3]};

    using namespace std::placeholders;
    auto context = std::make_shared<asios::Context>(
      std::bind(&httpl::http_context::on_connect, &handlers, _1),
      std::bind(&httpl::http_context::on_disconnect, &handlers, _1),
      std::bind(&httpl::http_context::shutdown, &handlers),
      std::bind(&httpl::http_context::on_read, &handlers, _1, _2, _3),
      std::bind(&httpl::http_context::on_write, &handlers, _1)
    );
    // Initialise the server.
    asios::server s(context, argv[1], argv[2]);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
