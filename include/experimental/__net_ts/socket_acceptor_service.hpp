//
// socket_acceptor_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_SOCKET_ACCEPTOR_SERVICE_HPP
#define NET_TS_SOCKET_ACCEPTOR_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>

#if defined(NET_TS_ENABLE_OLD_SERVICES)

#include <experimental/__net_ts/basic_socket.hpp>
#include <experimental/__net_ts/detail/type_traits.hpp>
#include <experimental/__net_ts/error.hpp>
#include <experimental/__net_ts/io_context.hpp>

#if defined(NET_TS_WINDOWS_RUNTIME)
# include <experimental/__net_ts/detail/null_socket_service.hpp>
#elif defined(NET_TS_HAS_IOCP)
# include <experimental/__net_ts/detail/win_iocp_socket_service.hpp>
#else
# include <experimental/__net_ts/detail/reactive_socket_service.hpp>
#endif

#include <experimental/__net_ts/detail/push_options.hpp>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {

/// Default service implementation for a socket acceptor.
template <typename Protocol>
class socket_acceptor_service
#if defined(GENERATING_DOCUMENTATION)
  : public std::experimental::net::io_context::service
#else
  : public std::experimental::net::detail::service_base<socket_acceptor_service<Protocol> >
#endif
{
public:
#if defined(GENERATING_DOCUMENTATION)
  /// The unique service identifier.
  static std::experimental::net::io_context::id id;
#endif

  /// The protocol type.
  typedef Protocol protocol_type;

  /// The endpoint type.
  typedef typename protocol_type::endpoint endpoint_type;

private:
  // The type of the platform-specific implementation.
#if defined(NET_TS_WINDOWS_RUNTIME)
  typedef detail::null_socket_service<Protocol> service_impl_type;
#elif defined(NET_TS_HAS_IOCP)
  typedef detail::win_iocp_socket_service<Protocol> service_impl_type;
#else
  typedef detail::reactive_socket_service<Protocol> service_impl_type;
#endif

public:
  /// The native type of the socket acceptor.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef typename service_impl_type::implementation_type implementation_type;
#endif

  /// The native acceptor type.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_handle_type;
#else
  typedef typename service_impl_type::native_handle_type native_handle_type;
#endif

  /// Construct a new socket acceptor service for the specified io_context.
  explicit socket_acceptor_service(std::experimental::net::io_context& io_context)
    : std::experimental::net::detail::service_base<
        socket_acceptor_service<Protocol> >(io_context),
      service_impl_(io_context)
  {
  }

  /// Construct a new socket acceptor implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

#if defined(NET_TS_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct a new socket acceptor implementation.
  void move_construct(implementation_type& impl,
      implementation_type& other_impl)
  {
    service_impl_.move_construct(impl, other_impl);
  }

  /// Move-assign from another socket acceptor implementation.
  void move_assign(implementation_type& impl,
      socket_acceptor_service& other_service,
      implementation_type& other_impl)
  {
    service_impl_.move_assign(impl, other_service.service_impl_, other_impl);
  }

  /// Move-construct a new socket acceptor implementation from another protocol
  /// type.
  template <typename Protocol1>
  void converting_move_construct(implementation_type& impl,
      typename socket_acceptor_service<
        Protocol1>::implementation_type& other_impl,
      typename enable_if<is_convertible<
        Protocol1, Protocol>::value>::type* = 0)
  {
    service_impl_.template converting_move_construct<Protocol1>(
        impl, other_impl);
  }
#endif // defined(NET_TS_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

  /// Destroy a socket acceptor implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  /// Open a new socket acceptor implementation.
  NET_TS_SYNC_OP_VOID open(implementation_type& impl,
      const protocol_type& protocol, std::error_code& ec)
  {
    service_impl_.open(impl, protocol, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Assign an existing native acceptor to a socket acceptor.
  NET_TS_SYNC_OP_VOID assign(implementation_type& impl,
      const protocol_type& protocol, const native_handle_type& native_acceptor,
      std::error_code& ec)
  {
    service_impl_.assign(impl, protocol, native_acceptor, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the acceptor is open.
  bool is_open(const implementation_type& impl) const
  {
    return service_impl_.is_open(impl);
  }

  /// Cancel all asynchronous operations associated with the acceptor.
  NET_TS_SYNC_OP_VOID cancel(implementation_type& impl,
      std::error_code& ec)
  {
    service_impl_.cancel(impl, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Bind the socket acceptor to the specified local endpoint.
  NET_TS_SYNC_OP_VOID bind(implementation_type& impl,
      const endpoint_type& endpoint, std::error_code& ec)
  {
    service_impl_.bind(impl, endpoint, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Place the socket acceptor into the state where it will listen for new
  /// connections.
  NET_TS_SYNC_OP_VOID listen(implementation_type& impl, int backlog,
      std::error_code& ec)
  {
    service_impl_.listen(impl, backlog, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Close a socket acceptor implementation.
  NET_TS_SYNC_OP_VOID close(implementation_type& impl,
      std::error_code& ec)
  {
    service_impl_.close(impl, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get the native acceptor implementation.
  native_handle_type native_handle(implementation_type& impl)
  {
    return service_impl_.native_handle(impl);
  }

  /// Set a socket option.
  template <typename SettableSocketOption>
  NET_TS_SYNC_OP_VOID set_option(implementation_type& impl,
      const SettableSocketOption& option, std::error_code& ec)
  {
    service_impl_.set_option(impl, option, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get a socket option.
  template <typename GettableSocketOption>
  NET_TS_SYNC_OP_VOID get_option(const implementation_type& impl,
      GettableSocketOption& option, std::error_code& ec) const
  {
    service_impl_.get_option(impl, option, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Perform an IO control command on the socket.
  template <typename IoControlCommand>
  NET_TS_SYNC_OP_VOID io_control(implementation_type& impl,
      IoControlCommand& command, std::error_code& ec)
  {
    service_impl_.io_control(impl, command, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Gets the non-blocking mode of the acceptor.
  bool non_blocking(const implementation_type& impl) const
  {
    return service_impl_.non_blocking(impl);
  }

  /// Sets the non-blocking mode of the acceptor.
  NET_TS_SYNC_OP_VOID non_blocking(implementation_type& impl,
      bool mode, std::error_code& ec)
  {
    service_impl_.non_blocking(impl, mode, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Gets the non-blocking mode of the native acceptor implementation.
  bool native_non_blocking(const implementation_type& impl) const
  {
    return service_impl_.native_non_blocking(impl);
  }

  /// Sets the non-blocking mode of the native acceptor implementation.
  NET_TS_SYNC_OP_VOID native_non_blocking(implementation_type& impl,
      bool mode, std::error_code& ec)
  {
    service_impl_.native_non_blocking(impl, mode, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get the local endpoint.
  endpoint_type local_endpoint(const implementation_type& impl,
      std::error_code& ec) const
  {
    return service_impl_.local_endpoint(impl, ec);
  }

  /// Wait for the acceptor to become ready to read, ready to write, or to have
  /// pending error conditions.
  NET_TS_SYNC_OP_VOID wait(implementation_type& impl,
      socket_base::wait_type w, std::error_code& ec)
  {
    service_impl_.wait(impl, w, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Asynchronously wait for the acceptor to become ready to read, ready to
  /// write, or to have pending error conditions.
  template <typename WaitHandler>
  NET_TS_INITFN_RESULT_TYPE(WaitHandler,
      void (std::error_code))
  async_wait(implementation_type& impl, socket_base::wait_type w,
      NET_TS_MOVE_ARG(WaitHandler) handler)
  {
    async_completion<WaitHandler,
      void (std::error_code)> init(handler);

    service_impl_.async_wait(impl, w, init.completion_handler);

    return init.result.get();
  }

  /// Accept a new connection.
  template <typename Protocol1, typename SocketService>
  NET_TS_SYNC_OP_VOID accept(implementation_type& impl,
      basic_socket<Protocol1, SocketService>& peer,
      endpoint_type* peer_endpoint, std::error_code& ec,
      typename enable_if<is_convertible<Protocol, Protocol1>::value>::type* = 0)
  {
    service_impl_.accept(impl, peer, peer_endpoint, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

#if defined(NET_TS_HAS_MOVE)
  /// Accept a new connection.
  typename Protocol::socket accept(implementation_type& impl,
      io_context* peer_io_context, endpoint_type* peer_endpoint,
      std::error_code& ec)
  {
    return service_impl_.accept(impl, peer_io_context, peer_endpoint, ec);
  }
#endif // defined(NET_TS_HAS_MOVE)

  /// Start an asynchronous accept.
  template <typename Protocol1, typename SocketService, typename AcceptHandler>
  NET_TS_INITFN_RESULT_TYPE(AcceptHandler,
      void (std::error_code))
  async_accept(implementation_type& impl,
      basic_socket<Protocol1, SocketService>& peer,
      endpoint_type* peer_endpoint,
      NET_TS_MOVE_ARG(AcceptHandler) handler,
      typename enable_if<is_convertible<Protocol, Protocol1>::value>::type* = 0)
  {
    async_completion<AcceptHandler,
      void (std::error_code)> init(handler);

    service_impl_.async_accept(impl,
        peer, peer_endpoint, init.completion_handler);

    return init.result.get();
  }

#if defined(NET_TS_HAS_MOVE)
  /// Start an asynchronous accept.
  template <typename MoveAcceptHandler>
  NET_TS_INITFN_RESULT_TYPE(MoveAcceptHandler,
      void (std::error_code, typename Protocol::socket))
  async_accept(implementation_type& impl,
      std::experimental::net::io_context* peer_io_context, endpoint_type* peer_endpoint,
      NET_TS_MOVE_ARG(MoveAcceptHandler) handler)
  {
    async_completion<MoveAcceptHandler,
      void (std::error_code,
        typename Protocol::socket)> init(handler);

    service_impl_.async_accept(impl,
        peer_io_context, peer_endpoint, init.completion_handler);

    return init.result.get();
  }
#endif // defined(NET_TS_HAS_MOVE)

private:
  // Destroy all user-defined handler objects owned by the service.
  void shutdown()
  {
    service_impl_.shutdown();
  }

  // The platform-specific implementation.
  service_impl_type service_impl_;
};

} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#include <experimental/__net_ts/detail/pop_options.hpp>

#endif // defined(NET_TS_ENABLE_OLD_SERVICES)

#endif // NET_TS_SOCKET_ACCEPTOR_SERVICE_HPP
