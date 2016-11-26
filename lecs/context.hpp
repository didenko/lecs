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

#include "asios/node.hpp"
#include "peers.hpp"

namespace lecs {

constexpr asios::buffer::value_type eol{'\n'};

using Message = std::string;
using Cursor = asios::buffer::const_iterator;
using Intake= std::function<void(asios::connection_ptr, Message &&)>;

class Context
{
public:
  Context();

  Context(Intake);

  void on_connect(asios::connection_ptr);

  void on_disconnect(asios::connection_ptr);

  void shutdown(void);

  asios::next on_read(asios::connection_ptr, const asios::buffer &, std::size_t);

  std::vector<asio::const_buffer> on_write(asios::connection_ptr);

  void write(asios::connection_ptr, const std::string &);

private:
  const Intake intake;
  Peers peers;

protected:
  std::string endpoint(asios::connection_ptr, bool remote = true);

  static bool get_line(Message &msg, Cursor &first, const Cursor &last);
};

}

