//
// context.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "asion/node.hpp"
#include "peers.hpp"

namespace lecs {

constexpr asion::buffer::value_type eol{'\n'};

using Message = std::string;
using Cursor = asion::buffer::const_iterator;
using OnConnect = std::function<void(asion::connection_ptr)>;
using OnDisconnect = std::function<void(asion::connection_ptr)>;
using Intake= std::function<void(asion::connection_ptr, Message && )>;

class Context
{
public:
  Context();

  Context(OnConnect, OnDisconnect, Intake);

  void on_connect(asion::connection_ptr);

  void on_disconnect(asion::connection_ptr);

  void shutdown(void);

  asion::next on_read(asion::connection_ptr, const asion::buffer &, std::size_t);

  std::vector<asio::const_buffer> on_write(asion::connection_ptr);

  void write(asion::connection_ptr, const std::string &);

private:
  const OnConnect on_conn;
  const OnDisconnect on_disc;
  const Intake intake;
  Peers peers;

protected:
  std::string endpoint(asion::connection_ptr, bool remote = true);

  static bool get_line(Message &msg, Cursor &first, const Cursor &last);
};

}

