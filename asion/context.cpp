//
// context.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "context.hpp"

namespace asion {

Context::Context(
  connected c,
  disconnected d,
  shutdown s,
  admitter a,
  reader r,
  writer w
) :
  on_connect(c),
  on_disconnect(d),
  on_shutdown(s),
  admit_read(a),
  on_read(r),
  on_write(w)
{};

}
