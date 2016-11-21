//
// tests.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2016 Vlad Didenko (business at didenko dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <cstring>

#include <gtest/gtest.h>
#include "les_context.hpp"

namespace tests {
namespace les {

struct ContextUtility: public ::testing::Test, public ::les::Context
{
  #define INPUT "123456\nabc\n\nZYXWVUTSRQP"
  ::asios::buffer buf{INPUT};
};

TEST_F(ContextUtility, separate)
{
  ::les::Message m;
  ::les::Cursor
    cur{buf.begin()},
    last{cur + std::strlen(INPUT)};

  ASSERT_TRUE(get_line(m, cur, last));
  ASSERT_EQ(cur, buf.begin() + 7);
  ASSERT_EQ(m, "123456");

  ASSERT_TRUE(get_line(m, cur, last));
  ASSERT_EQ(cur, buf.begin() + 11);
  ASSERT_EQ(m, "abc");

  ASSERT_TRUE(get_line(m, cur, last));
  ASSERT_EQ(cur, buf.begin() + 12);
  ASSERT_EQ(m, "");

  ASSERT_FALSE(get_line(m, cur, last));
  ASSERT_EQ(cur, last);
  ASSERT_EQ(m, "ZYXWVUTSRQP");
}

}
}