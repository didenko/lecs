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

asios::connection_new on_new_conn = [](
  const asios::connection_ptr,
  const asio::ip::tcp::socket &s
) {
  asio::error_code ec;
  auto remote_ep = s.remote_endpoint(ec);
  if (ec)
  {
    std::cerr
      << "Accepted: "
      << "Remote unknown: "
      << ec.message()
      << std::endl;
    return;
  }
  std::cerr
    << "Accepted: "
    << "Remote: "
    << remote_ep.address() << ":"
    << remote_ep.port()
    << std::endl;
};

//asios::request_handler handle_request = [](const httpl::request &req, httpl::reply &rep) {
//  rep.status = httpl::reply::ok;
//  std::string buf{"Hello world!"};
//  rep.content.append(buf.c_str(), buf.size());
//  rep.headers.resize(2);
//  rep.headers[0].name = "Content-Length";
//  rep.headers[0].value = std::to_string(rep.content.size());
//  rep.headers[1].name = "Content-Type";
//  rep.headers[1].value = "text/plain";
//
//  std::cerr
//    << "Request: \""
//    << req.uri
//    << "\"" << std::endl;
//};

asios::read_handler on_read = [](
  const asios::connection_ptr,
  asio::mutable_buffer b, std::size_t s
) {
  std::cerr << "Got data." << std::endl;
};

//class http_context: public httpl::Context
//{
//  http_context(
//    httpl::connection_new c,
//    httpl::request_handler r
//  ) : httpl::Context(c, r)
//  {};
//
//private:
//  httpl::request req;
//};

int main(int argc, char *argv[])
{
  try
  {
    // Check command line arguments.
    if (argc < 3)
    {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }

    auto context = std::make_shared<asios::Context>(
      on_new_conn,
      on_read
    );
    // Initialise the server.
    asios::server s(argv[1], argv[2], context);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception &e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
