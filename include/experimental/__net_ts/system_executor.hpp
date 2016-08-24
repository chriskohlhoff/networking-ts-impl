//
// system_executor.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_SYSTEM_EXECUTOR_HPP
#define NET_TS_SYSTEM_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>
#include <experimental/__net_ts/detail/scheduler.hpp>
#include <experimental/__net_ts/detail/thread_group.hpp>
#include <experimental/__net_ts/execution_context.hpp>

#include <experimental/__net_ts/detail/push_options.hpp>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {

/// An executor that uses arbitrary threads.
/**
 * The system executor represents an execution context where functions are
 * permitted to run on arbitrary threads. The post() and defer() functions
 * schedule the function to run on an unspecified system thread pool, and
 * dispatch() invokes the function immediately.
 */
class system_executor
{
public:
  /// Obtain the underlying execution context.
  execution_context& context() const NET_TS_NOEXCEPT;

  /// Inform the executor that it has some outstanding work to do.
  /**
   * For the system executor, this is a no-op.
   */
  void on_work_started() const NET_TS_NOEXCEPT
  {
  }

  /// Inform the executor that some work is no longer outstanding.
  /**
   * For the system executor, this is a no-op.
   */
  void on_work_finished() const NET_TS_NOEXCEPT
  {
  }

  /// Request the system executor to invoke the given function object.
  /**
   * This function is used to ask the executor to execute the given function
   * object. The function object will always be executed inside this function.
   *
   * @param f The function object to be called. The executor will make
   * a copy of the handler object as required. The function signature of the
   * function object must be: @code void function(); @endcode
   *
   * @param a An allocator that may be used by the executor to allocate the
   * internal storage needed for function invocation.
   */
  template <typename Function, typename Allocator>
  void dispatch(NET_TS_MOVE_ARG(Function) f, const Allocator& a) const;

  /// Request the system executor to invoke the given function object.
  /**
   * This function is used to ask the executor to execute the given function
   * object. The function object will never be executed inside this function.
   * Instead, it will be scheduled to run on an unspecified system thread pool.
   *
   * @param f The function object to be called. The executor will make
   * a copy of the handler object as required. The function signature of the
   * function object must be: @code void function(); @endcode
   *
   * @param a An allocator that may be used by the executor to allocate the
   * internal storage needed for function invocation.
   */
  template <typename Function, typename Allocator>
  void post(NET_TS_MOVE_ARG(Function) f, const Allocator& a) const;

  /// Request the system executor to invoke the given function object.
  /**
   * This function is used to ask the executor to execute the given function
   * object. The function object will never be executed inside this function.
   * Instead, it will be scheduled to run on an unspecified system thread pool.
   *
   * @param f The function object to be called. The executor will make
   * a copy of the handler object as required. The function signature of the
   * function object must be: @code void function(); @endcode
   *
   * @param a An allocator that may be used by the executor to allocate the
   * internal storage needed for function invocation.
   */
  template <typename Function, typename Allocator>
  void defer(NET_TS_MOVE_ARG(Function) f, const Allocator& a) const;

  /// Compare two executors for equality.
  /**
   * System executors always compare equal.
   */
  friend bool operator==(const system_executor&,
      const system_executor&) NET_TS_NOEXCEPT
  {
    return true;
  }

  /// Compare two executors for inequality.
  /**
   * System executors always compare equal.
   */
  friend bool operator!=(const system_executor&,
      const system_executor&) NET_TS_NOEXCEPT
  {
    return false;
  }

private:
  struct thread_function;

  // Hidden implementation of the system execution context.
  struct context_impl
    : public execution_context
  {
    // Constructor creates all threads in the system thread pool.
    NET_TS_DECL context_impl();

    // Destructor shuts down all threads in the system thread pool.
    NET_TS_DECL ~context_impl();

    // The underlying scheduler.
    detail::scheduler& scheduler_;

    // The threads in the system thread pool.
    detail::thread_group threads_;
  };
};

} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#include <experimental/__net_ts/detail/pop_options.hpp>

#include <experimental/__net_ts/impl/system_executor.hpp>
#if defined(NET_TS_HEADER_ONLY)
# include <experimental/__net_ts/impl/system_executor.ipp>
#endif // defined(NET_TS_HEADER_ONLY)

#endif // NET_TS_SYSTEM_EXECUTOR_HPP
