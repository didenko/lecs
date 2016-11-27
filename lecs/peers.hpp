//
// peers.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <set>
#include <shared_mutex>
#include "asion/connection.hpp"

namespace lecs {

class Peers
{
public:
  Peers(const Peers &) = delete;

  Peers &operator=(const Peers &) = delete;

  Peers();

  ~Peers();

  void add(asion::connection_ptr);

  void del(asion::connection_ptr);

  void shutdown();

private:
  std::set<asion::connection_ptr> conns;
  std::shared_timed_mutex peers_lock;
};

}
