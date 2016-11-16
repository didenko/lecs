//
// http_context.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "connection_manager.hpp"
#include "request_parser.hpp"
#include "request_handler.hpp"

namespace httpl {

class http_context
{
public:
  http_context(const std::string &doc_root);

  void on_connect(asios::connection_ptr);

  void on_disconnect(asios::connection_ptr);

  void shutdown(void);

  asios::next on_read(asios::connection_ptr, const asios::buffer &, std::size_t);

  std::vector<asio::const_buffer> on_write(asios::connection_ptr);

private:
  connection_manager connections;
  request_parser parser;
  request_handler handler;

  std::string endpoint(asios::connection_ptr, bool remote = true);
};

}
