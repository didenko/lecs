//
// connection_manager.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "connection_manager.hpp"

namespace httpl {

connection_manager::connection_manager()
{}

connection_manager::~connection_manager()
{
  shutdown();
}

void connection_manager::add(asion::connection_ptr c)
{
  {
    std::lock_guard<std::shared_timed_mutex> lock(clients_lock);
    clients[c] = std::make_shared<client>();
  }
  c->start();
}

void connection_manager::del(asion::connection_ptr c)
{
  {
    std::lock_guard<std::shared_timed_mutex> lock(clients_lock);
    clients.erase(c);
  }
  c->stop();
}

const client::ptr connection_manager::at(asion::connection_ptr conn)
{
  std::shared_lock<std::shared_timed_mutex> lock(clients_lock);
  return clients.at(conn);
}

void connection_manager::shutdown()
{
  for (auto client_pair: clients)
    client_pair.first->stop();

  std::lock_guard<std::shared_timed_mutex> lock(clients_lock);
  clients.clear();
}

}
