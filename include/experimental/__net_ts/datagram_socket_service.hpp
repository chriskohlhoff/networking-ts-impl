//
// datagram_socket_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_DATAGRAM_SOCKET_SERVICE_HPP
#define NET_TS_DATAGRAM_SOCKET_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>

#if defined(NET_TS_ENABLE_OLD_SERVICES)

#include <cstddef>
#include <experimental/__net_ts/async_result.hpp>
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

/// Default service implementation for a datagram socket.
template <typename Protocol>
class datagram_socket_service
#if defined(GENERATING_DOCUMENTATION)
  : public std::experimental::net::io_context::service
#else
  : public std::experimental::net::detail::service_base<datagram_socket_service<Protocol> >
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
  typedef typename Protocol::endpoint endpoint_type;

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
  /// The type of a datagram socket.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined implementation_type;
#else
  typedef typename service_impl_type::implementation_type implementation_type;
#endif

  /// The native socket type.
#if defined(GENERATING_DOCUMENTATION)
  typedef implementation_defined native_handle_type;
#else
  typedef typename service_impl_type::native_handle_type native_handle_type;
#endif

  /// Construct a new datagram socket service for the specified io_context.
  explicit datagram_socket_service(std::experimental::net::io_context& io_context)
    : std::experimental::net::detail::service_base<
        datagram_socket_service<Protocol> >(io_context),
      service_impl_(io_context)
  {
  }

  /// Construct a new datagram socket implementation.
  void construct(implementation_type& impl)
  {
    service_impl_.construct(impl);
  }

#if defined(NET_TS_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)
  /// Move-construct a new datagram socket implementation.
  void move_construct(implementation_type& impl,
      implementation_type& other_impl)
  {
    service_impl_.move_construct(impl, other_impl);
  }

  /// Move-assign from another datagram socket implementation.
  void move_assign(implementation_type& impl,
      datagram_socket_service& other_service,
      implementation_type& other_impl)
  {
    service_impl_.move_assign(impl, other_service.service_impl_, other_impl);
  }

  /// Move-construct a new datagram socket implementation from another protocol
  /// type.
  template <typename Protocol1>
  void converting_move_construct(implementation_type& impl,
      typename datagram_socket_service<
        Protocol1>::implementation_type& other_impl,
      typename enable_if<is_convertible<
        Protocol1, Protocol>::value>::type* = 0)
  {
    service_impl_.template converting_move_construct<Protocol1>(
        impl, other_impl);
  }
#endif // defined(NET_TS_HAS_MOVE) || defined(GENERATING_DOCUMENTATION)

  /// Destroy a datagram socket implementation.
  void destroy(implementation_type& impl)
  {
    service_impl_.destroy(impl);
  }

  // Open a new datagram socket implementation.
  NET_TS_SYNC_OP_VOID open(implementation_type& impl,
      const protocol_type& protocol, std::error_code& ec)
  {
    if (protocol.type() == NET_TS_OS_DEF(SOCK_DGRAM))
      service_impl_.open(impl, protocol, ec);
    else
      ec = std::experimental::net::error::invalid_argument;
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Assign an existing native socket to a datagram socket.
  NET_TS_SYNC_OP_VOID assign(implementation_type& impl,
      const protocol_type& protocol, const native_handle_type& native_socket,
      std::error_code& ec)
  {
    service_impl_.assign(impl, protocol, native_socket, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the socket is open.
  bool is_open(const implementation_type& impl) const
  {
    return service_impl_.is_open(impl);
  }

  /// Close a datagram socket implementation.
  NET_TS_SYNC_OP_VOID close(implementation_type& impl,
      std::error_code& ec)
  {
    service_impl_.close(impl, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Get the native socket implementation.
  native_handle_type native_handle(implementation_type& impl)
  {
    return service_impl_.native_handle(impl);
  }

  /// Cancel all asynchronous operations associated with the socket.
  NET_TS_SYNC_OP_VOID cancel(implementation_type& impl,
      std::error_code& ec)
  {
    service_impl_.cancel(impl, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Determine whether the socket is at the out-of-band data mark.
  bool at_mark(const implementation_type& impl,
      std::error_code& ec) const
  {
    return service_impl_.at_mark(impl, ec);
  }

  /// Determine the number of bytes available for reading.
  std::size_t available(const implementation_type& impl,
      std::error_code& ec) const
  {
    return service_impl_.available(impl, ec);
  }

  // Bind the datagram socket to the specified local endpoint.
  NET_TS_SYNC_OP_VOID bind(implementation_type& impl,
      const endpoint_type& endpoint, std::error_code& ec)
  {
    service_impl_.bind(impl, endpoint, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Connect the datagram socket to the specified endpoint.
  NET_TS_SYNC_OP_VOID connect(implementation_type& impl,
      const endpoint_type& peer_endpoint, std::error_code& ec)
  {
    service_impl_.connect(impl, peer_endpoint, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Start an asynchronous connect.
  template <typename ConnectHandler>
  NET_TS_INITFN_RESULT_TYPE(ConnectHandler,
      void (std::error_code))
  async_connect(implementation_type& impl,
      const endpoint_type& peer_endpoint,
      NET_TS_MOVE_ARG(ConnectHandler) handler)
  {
    async_completion<ConnectHandler,
      void (std::error_code)> init(handler);

    service_impl_.async_connect(impl, peer_endpoint, init.completion_handler);

    return init.result.get();
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

  /// Gets the non-blocking mode of the socket.
  bool non_blocking(const implementation_type& impl) const
  {
    return service_impl_.non_blocking(impl);
  }

  /// Sets the non-blocking mode of the socket.
  NET_TS_SYNC_OP_VOID non_blocking(implementation_type& impl,
      bool mode, std::error_code& ec)
  {
    service_impl_.non_blocking(impl, mode, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Gets the non-blocking mode of the native socket implementation.
  bool native_non_blocking(const implementation_type& impl) const
  {
    return service_impl_.native_non_blocking(impl);
  }

  /// Sets the non-blocking mode of the native socket implementation.
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

  /// Get the remote endpoint.
  endpoint_type remote_endpoint(const implementation_type& impl,
      std::error_code& ec) const
  {
    return service_impl_.remote_endpoint(impl, ec);
  }

  /// Disable sends or receives on the socket.
  NET_TS_SYNC_OP_VOID shutdown(implementation_type& impl,
      socket_base::shutdown_type what, std::error_code& ec)
  {
    service_impl_.shutdown(impl, what, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Wait for the socket to become ready to read, ready to write, or to have
  /// pending error conditions.
  NET_TS_SYNC_OP_VOID wait(implementation_type& impl,
      socket_base::wait_type w, std::error_code& ec)
  {
    service_impl_.wait(impl, w, ec);
    NET_TS_SYNC_OP_VOID_RETURN(ec);
  }

  /// Asynchronously wait for the socket to become ready to read, ready to
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

  /// Send the given data to the peer.
  template <typename ConstBufferSequence>
  std::size_t send(implementation_type& impl,
      const ConstBufferSequence& buffers,
      socket_base::message_flags flags, std::error_code& ec)
  {
    return service_impl_.send(impl, buffers, flags, ec);
  }

  /// Start an asynchronous send.
  template <typename ConstBufferSequence, typename WriteHandler>
  NET_TS_INITFN_RESULT_TYPE(WriteHandler,
      void (std::error_code, std::size_t))
  async_send(implementation_type& impl, const ConstBufferSequence& buffers,
      socket_base::message_flags flags,
      NET_TS_MOVE_ARG(WriteHandler) handler)
  {
    async_completion<WriteHandler,
      void (std::error_code, std::size_t)> init(handler);

    service_impl_.async_send(impl, buffers, flags, init.completion_handler);

    return init.result.get();
  }

  /// Send a datagram to the specified endpoint.
  template <typename ConstBufferSequence>
  std::size_t send_to(implementation_type& impl,
      const ConstBufferSequence& buffers, const endpoint_type& destination,
      socket_base::message_flags flags, std::error_code& ec)
  {
    return service_impl_.send_to(impl, buffers, destination, flags, ec);
  }

  /// Start an asynchronous send.
  template <typename ConstBufferSequence, typename WriteHandler>
  NET_TS_INITFN_RESULT_TYPE(WriteHandler,
      void (std::error_code, std::size_t))
  async_send_to(implementation_type& impl,
      const ConstBufferSequence& buffers, const endpoint_type& destination,
      socket_base::message_flags flags,
      NET_TS_MOVE_ARG(WriteHandler) handler)
  {
    async_completion<WriteHandler,
      void (std::error_code, std::size_t)> init(handler);

    service_impl_.async_send_to(impl, buffers,
        destination, flags, init.completion_handler);

    return init.result.get();
  }

  /// Receive some data from the peer.
  template <typename MutableBufferSequence>
  std::size_t receive(implementation_type& impl,
      const MutableBufferSequence& buffers,
      socket_base::message_flags flags, std::error_code& ec)
  {
    return service_impl_.receive(impl, buffers, flags, ec);
  }

  /// Start an asynchronous receive.
  template <typename MutableBufferSequence, typename ReadHandler>
  NET_TS_INITFN_RESULT_TYPE(ReadHandler,
      void (std::error_code, std::size_t))
  async_receive(implementation_type& impl,
      const MutableBufferSequence& buffers,
      socket_base::message_flags flags,
      NET_TS_MOVE_ARG(ReadHandler) handler)
  {
    async_completion<ReadHandler,
      void (std::error_code, std::size_t)> init(handler);

    service_impl_.async_receive(impl, buffers, flags, init.completion_handler);

    return init.result.get();
  }

  /// Receive a datagram with the endpoint of the sender.
  template <typename MutableBufferSequence>
  std::size_t receive_from(implementation_type& impl,
      const MutableBufferSequence& buffers, endpoint_type& sender_endpoint,
      socket_base::message_flags flags, std::error_code& ec)
  {
    return service_impl_.receive_from(impl, buffers, sender_endpoint, flags,
        ec);
  }

  /// Start an asynchronous receive that will get the endpoint of the sender.
  template <typename MutableBufferSequence, typename ReadHandler>
  NET_TS_INITFN_RESULT_TYPE(ReadHandler,
      void (std::error_code, std::size_t))
  async_receive_from(implementation_type& impl,
      const MutableBufferSequence& buffers, endpoint_type& sender_endpoint,
      socket_base::message_flags flags,
      NET_TS_MOVE_ARG(ReadHandler) handler)
  {
    async_completion<ReadHandler,
      void (std::error_code, std::size_t)> init(handler);

    service_impl_.async_receive_from(impl, buffers,
        sender_endpoint, flags, init.completion_handler);

    return init.result.get();
  }

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

#endif // NET_TS_DATAGRAM_SOCKET_SERVICE_HPP
