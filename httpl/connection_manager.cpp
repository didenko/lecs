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

void connection_manager::add(asios::connection_ptr c)
{
  clients[c] = std::make_shared<client>();
  c->start();
}

void connection_manager::del(asios::connection_ptr c)
{
  clients.erase(c);
  c->stop();
}

const client::ptr connection_manager::at(asios::connection_ptr conn)
{
  return clients.at(conn);
}

void connection_manager::shutdown()
{
  for (auto client_pair: clients)
    client_pair.first->stop();
  clients.clear();
}

}
