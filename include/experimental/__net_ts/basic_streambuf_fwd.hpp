//
// basic_streambuf_fwd.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_BASIC_STREAMBUF_FWD_HPP
#define NET_TS_BASIC_STREAMBUF_FWD_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>

#if !defined(NET_TS_NO_IOSTREAM)

#include <memory>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {

template <typename Allocator = std::allocator<char> >
class basic_streambuf;

template <typename Allocator = std::allocator<char> >
class basic_streambuf_ref;

} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#endif // !defined(NET_TS_NO_IOSTREAM)

#endif // NET_TS_BASIC_STREAMBUF_FWD_HPP
