//
// detail/null_event.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_DETAIL_NULL_EVENT_HPP
#define NET_TS_DETAIL_NULL_EVENT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>

#if !defined(NET_TS_HAS_THREADS)

#include <experimental/__net_ts/detail/noncopyable.hpp>

#include <experimental/__net_ts/detail/push_options.hpp>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {
namespace detail {

class null_event
  : private noncopyable
{
public:
  // Constructor.
  null_event()
  {
  }

  // Destructor.
  ~null_event()
  {
  }

  // Signal the event. (Retained for backward compatibility.)
  template <typename Lock>
  void signal(Lock&)
  {
  }

  // Signal all waiters.
  template <typename Lock>
  void signal_all(Lock&)
  {
  }

  // Unlock the mutex and signal one waiter.
  template <typename Lock>
  void unlock_and_signal_one(Lock&)
  {
  }

  // If there's a waiter, unlock the mutex and signal it.
  template <typename Lock>
  bool maybe_unlock_and_signal_one(Lock&)
  {
    return false;
  }

  // Reset the event.
  template <typename Lock>
  void clear(Lock&)
  {
  }

  // Wait for the event to become signalled.
  template <typename Lock>
  void wait(Lock&)
  {
  }

  // Timed wait for the event to become signalled.
  template <typename Lock>
  bool wait_for_usec(Lock& lock, long usec)
  {
    return true;
  }
};

} // namespace detail
} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#include <experimental/__net_ts/detail/pop_options.hpp>

#endif // !defined(NET_TS_HAS_THREADS)

#endif // NET_TS_DETAIL_NULL_EVENT_HPP
