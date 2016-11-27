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

  void on_connect(asion::connection_ptr);

  void on_disconnect(asion::connection_ptr);

  void shutdown(void);

  asion::next on_read(asion::connection_ptr, const asion::buffer &, std::size_t);

  std::vector<asio::const_buffer> on_write(asion::connection_ptr);

private:
  connection_manager connections;
  request_parser parser;
  request_handler handler;

  std::string endpoint(asion::connection_ptr, bool remote = true);
};

}
