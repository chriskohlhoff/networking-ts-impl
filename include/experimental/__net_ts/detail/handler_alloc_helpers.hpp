//
// detail/handler_alloc_helpers.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_DETAIL_HANDLER_ALLOC_HELPERS_HPP
#define NET_TS_DETAIL_HANDLER_ALLOC_HELPERS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>
#include <experimental/__net_ts/detail/memory.hpp>
#include <experimental/__net_ts/detail/noncopyable.hpp>
#include <experimental/__net_ts/associated_allocator.hpp>
#include <experimental/__net_ts/handler_alloc_hook.hpp>

#include <experimental/__net_ts/detail/push_options.hpp>

// Calls to networking_ts_handler_allocate and networking_ts_handler_deallocate must be made from
// a namespace that does not contain any overloads of these functions. The
// networking_ts_handler_alloc_helpers namespace is defined here for that purpose.
namespace networking_ts_handler_alloc_helpers {

template <typename Handler>
inline void* allocate(std::size_t s, Handler& h)
{
#if !defined(NET_TS_HAS_HANDLER_HOOKS)
  return ::operator new(s);
#else
  using std::experimental::net::networking_ts_handler_allocate;
  return networking_ts_handler_allocate(s, std::experimental::net::detail::addressof(h));
#endif
}

template <typename Handler>
inline void deallocate(void* p, std::size_t s, Handler& h)
{
#if !defined(NET_TS_HAS_HANDLER_HOOKS)
  ::operator delete(p);
#else
  using std::experimental::net::networking_ts_handler_deallocate;
  networking_ts_handler_deallocate(p, s, std::experimental::net::detail::addressof(h));
#endif
}

} // namespace networking_ts_handler_alloc_helpers

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {
namespace detail {

template <typename Handler, typename T>
class hook_allocator
{
public:
  typedef T value_type;

  template <typename U>
  struct rebind
  {
    typedef hook_allocator<Handler, U> other;
  };

  explicit hook_allocator(Handler& h)
    : handler_(h)
  {
  }

  template <typename U>
  hook_allocator(const hook_allocator<Handler, U>& a)
    : handler_(a.handler_)
  {
  }

  T* allocate(std::size_t n)
  {
    return static_cast<T*>(
        networking_ts_handler_alloc_helpers::allocate(sizeof(T) * n, handler_));
  }

  void deallocate(T* p, std::size_t n)
  {
    networking_ts_handler_alloc_helpers::deallocate(p, sizeof(T) * n, handler_);
  }

//private:
  Handler& handler_;
};

template <typename Handler>
class hook_allocator<Handler, void>
{
public:
  typedef void value_type;

  template <typename U>
  struct rebind
  {
    typedef hook_allocator<Handler, U> other;
  };

  explicit hook_allocator(Handler& h)
    : handler_(h)
  {
  }

  template <typename U>
  hook_allocator(const hook_allocator<Handler, U>& a)
    : handler_(a.handler_)
  {
  }

//private:
  Handler& handler_;
};

} // namespace detail
} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#define NET_TS_DEFINE_HANDLER_PTR(op) \
  struct ptr \
  { \
    typedef typename ::std::experimental::net::associated_allocator<Handler, \
      ::std::experimental::net::detail::hook_allocator<Handler, \
        void> >::type associated_allocator_type; \
    typedef NET_TS_REBIND_ALLOC( \
      associated_allocator_type, op) allocator_type; \
    Handler* h; \
    op* v; \
    op* p; \
    ~ptr() \
    { \
      reset(); \
    } \
    static op* allocate(Handler& handler) \
    { \
      allocator_type a(::std::experimental::net::associated_allocator<Handler, \
        ::std::experimental::net::detail::hook_allocator<Handler, void> >::get(handler, \
          ::std::experimental::net::detail::hook_allocator<Handler, void>(handler))); \
      return a.allocate(1); \
    } \
    void reset() \
    { \
      allocator_type a(::std::experimental::net::associated_allocator<Handler, \
        ::std::experimental::net::detail::hook_allocator<Handler, void> >::get(*h, \
          ::std::experimental::net::detail::hook_allocator<Handler, void>(*h))); \
      if (p) \
      { \
        p->~op(); \
        p = 0; \
      } \
      if (v) \
      { \
        a.deallocate(static_cast<op*>(v), 1); \
        v = 0; \
      } \
    } \
  } \
  /**/

#define NET_TS_DEFINE_HANDLER_ALLOCATOR_PTR(op, alloc) \
  struct ptr \
  { \
    NET_TS_REBIND_ALLOC(alloc, op) a; \
    void* v; \
    op* p; \
    ~ptr() \
    { \
      reset(); \
    } \
    void reset() \
    { \
      if (p) \
      { \
        p->~op(); \
        p = 0; \
      } \
      if (v) \
      { \
        a.deallocate(static_cast<op*>(v), 1); \
        v = 0; \
      } \
    } \
  } \
  /**/

#include <experimental/__net_ts/detail/pop_options.hpp>

#endif // NET_TS_DETAIL_HANDLER_ALLOC_HELPERS_HPP
