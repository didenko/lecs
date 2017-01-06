//
// peers.cpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <thread>

#include "peers.hpp"

namespace lecs {

Peers::Peers()
{}

Peers::~Peers()
{
  shutdown();
}

void Peers::add(asion::connection_ptr c)
{
  {
    std::lock_guard<std::shared_timed_mutex> lock(peers_lock);
    conns.emplace(c, std::make_shared<buffer>());
  }
  c->start();
}

void Peers::del(asion::connection_ptr c)
{
  {
    std::lock_guard<std::shared_timed_mutex> lock(peers_lock);
    conns.erase(c);
  }
  c->stop();
}

buffer_ptr Peers::at(asion::connection_ptr conn)
{
  std::lock_guard<std::shared_timed_mutex> lock(peers_lock);
  return conns.at(conn);
}

void Peers::shutdown()
{
  std::lock_guard<std::shared_timed_mutex> lock(peers_lock);

  for (auto& conn_buf: conns) conn_buf.first->stop();
  conns.clear();
}

}
