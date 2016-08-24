//
// impl/system_executor.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_IMPL_SYSTEM_EXECUTOR_HPP
#define NET_TS_IMPL_SYSTEM_EXECUTOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/executor_op.hpp>
#include <experimental/__net_ts/detail/global.hpp>
#include <experimental/__net_ts/detail/recycling_allocator.hpp>
#include <experimental/__net_ts/detail/type_traits.hpp>
#include <experimental/__net_ts/execution_context.hpp>

#include <experimental/__net_ts/detail/push_options.hpp>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {

inline execution_context& system_executor::context() const NET_TS_NOEXCEPT
{
  return detail::global<context_impl>();
}

template <typename Function, typename Allocator>
void system_executor::dispatch(
    NET_TS_MOVE_ARG(Function) f, const Allocator&) const
{
  typename decay<Function>::type tmp(NET_TS_MOVE_CAST(Function)(f));
  networking_ts_handler_invoke_helpers::invoke(tmp, tmp);
}

template <typename Function, typename Allocator>
void system_executor::post(
    NET_TS_MOVE_ARG(Function) f, const Allocator& a) const
{
  context_impl& ctx = detail::global<context_impl>();

  // Make a local, non-const copy of the function.
  typedef typename decay<Function>::type function_type;
  function_type tmp(NET_TS_MOVE_CAST(Function)(f));

  // Construct an allocator to be used for the operation.
  typedef typename detail::get_recycling_allocator<Allocator>::type alloc_type;
  alloc_type allocator(detail::get_recycling_allocator<Allocator>::get(a));

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, alloc_type> op;
  typename op::ptr p = { allocator, 0, 0 };
  p.v = p.a.allocate(1);
  p.p = new (p.v) op(tmp, allocator);

  NET_TS_HANDLER_CREATION((ctx, *p.p,
        "system_executor", &this->context(), 0, "post"));

  ctx.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Allocator>
void system_executor::defer(
    NET_TS_MOVE_ARG(Function) f, const Allocator& a) const
{
  context_impl& ctx = detail::global<context_impl>();

  // Make a local, non-const copy of the function.
  typedef typename decay<Function>::type function_type;
  function_type tmp(NET_TS_MOVE_CAST(Function)(f));

  // Construct an allocator to be used for the operation.
  typedef typename detail::get_recycling_allocator<Allocator>::type alloc_type;
  alloc_type allocator(detail::get_recycling_allocator<Allocator>::get(a));

  // Allocate and construct an operation to wrap the function.
  typedef detail::executor_op<function_type, alloc_type> op;
  typename op::ptr p = { allocator, 0, 0 };
  p.v = p.a.allocate(1);
  p.p = new (p.v) op(tmp, allocator);

  NET_TS_HANDLER_CREATION((ctx, *p.p,
        "system_executor", &this->context(), 0, "defer"));

  ctx.scheduler_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}

} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#include <experimental/__net_ts/detail/pop_options.hpp>

#endif // NET_TS_IMPL_SYSTEM_EXECUTOR_HPP
