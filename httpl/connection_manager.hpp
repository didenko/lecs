//
// connection_manager.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <map>
#include <shared_mutex>
#include "asios/connection.hpp"
#include "client.hpp"

namespace httpl {

// Manages request/reply structures per client connection
class connection_manager
{
public:
  connection_manager(const connection_manager &) = delete;

  connection_manager &operator=(const connection_manager &) = delete;

  connection_manager();

  ~connection_manager();

  void add(asios::connection_ptr);

  void del(asios::connection_ptr);

  const client::ptr at(asios::connection_ptr);

  void shutdown();

private:
  std::map<asios::connection_ptr, client::ptr> clients;
  std::shared_timed_mutex clients_lock;
};

}
