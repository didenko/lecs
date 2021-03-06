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

#include <functional>
#include "asion/node.hpp"
#include "peers.hpp"

namespace lecs {

constexpr auto eol = '\n';

using Message = std::string;
using OnConnect = std::function<void(asion::connection_ptr)>;
using OnDisconnect = std::function<void(asion::connection_ptr, const std::string &)>;
using Intake= std::function<void(asion::connection_ptr, Message &&)>;

class Context
{
public:
  Context();

  Context(OnConnect, OnDisconnect, Intake);

  Context(const Context &) = delete;

  Context &operator=(const Context &) = delete;

  asion::context_ptr node_context();

  void on_connect(asion::connection_ptr);

  void on_disconnect(asion::connection_ptr, const std::string &);

  void shutdown(void);

  void admit_read(asion::connection_ptr, std::function<void(std::error_code, std::size_t)>);

  void on_read(asion::connection_ptr, std::size_t);

  std::vector<asio::const_buffer> on_write(asion::connection_ptr);

  void write(asion::connection_ptr, const std::string &);

private:
  const OnConnect on_conn;
  const OnDisconnect on_disc;
  const Intake intake;
  Peers peers;

  std::shared_ptr<asion::Context> basic_context;

protected:
  std::string endpoint(asion::connection_ptr, bool remote = true);
};

}

