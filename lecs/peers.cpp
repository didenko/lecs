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

#include "peers.hpp"

namespace lecs {

Peers::Peers()
{}

Peers::~Peers()
{
  shutdown();
}

void Peers::add(asios::connection_ptr c)
{
  {
    std::lock_guard <std::shared_timed_mutex> lock(peers_lock);
    conns.insert(c);
  }
  c->start();
}

void Peers::del(asios::connection_ptr c)
{
  {
    std::lock_guard <std::shared_timed_mutex> lock(peers_lock);
    conns.erase(c);
  }
  c->stop();
}

void Peers::shutdown()
{
  for (auto c: conns)
    c->stop();

  std::lock_guard <std::shared_timed_mutex> lock(peers_lock);
  conns.clear();
}

}