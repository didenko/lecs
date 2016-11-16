//
// client.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <array>
#include <memory>

#include "request.hpp"
#include "reply.hpp"

namespace httpl {

struct client
{
  using ptr = std::shared_ptr<client>;

  client()
  {};

  client(const client &) = delete;

  client &operator=(const client &) = delete;

  httpl::request request;
  httpl::reply reply;
};

}
