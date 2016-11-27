//
// http_context.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "http_context.hpp"
#include <iostream>

namespace httpl {

http_context::http_context(
  const std::string &doc_root
)
  : handler(doc_root)
{};

void http_context::on_connect(asion::connection_ptr conn)
{
  connections.add(conn);
  std::cerr << "Accepted: " << endpoint(conn) << std::endl;
}

void http_context::on_disconnect(asion::connection_ptr conn)
{
  auto ep = endpoint(conn);
  connections.del(conn);
  std::cerr << "Disconnected: " << ep << std::endl;
}

void http_context::shutdown(void)
{
  std::cerr << "Shutting down" << std::endl;
  connections.shutdown();
}

asion::next http_context::on_read(asion::connection_ptr conn, const asion::buffer &buffer, std::size_t sz)
{
  request_parser::result_type result;

  auto client = connections.at(conn); //TODO: handle std::out_of_range

  std::tie(result, std::ignore) = parser.parse(
    client->request,
    buffer.data(),
    buffer.data() + sz
  );

  switch (result)
  {
    case decltype(result)::good:
      handler.handle_request(client->request, client->reply);
      conn->do_write();
      return asion::next::read;

    case decltype(result)::bad:
      client->reply = httpl::reply::stock_reply(httpl::reply::bad_request);
      return asion::next::write;

    case decltype(result)::indeterminate:
      return asion::next::read;
  }
}

std::vector<asio::const_buffer> http_context::on_write(asion::connection_ptr conn)
{
  auto client = connections.at(conn); //TODO: handle std::out_of_range
  return client->reply.to_buffers();
};

std::string http_context::endpoint(asion::connection_ptr conn, bool remote)
{
  return remote ? conn->endpoint_remote() : conn->endpoint_local();
}

}
