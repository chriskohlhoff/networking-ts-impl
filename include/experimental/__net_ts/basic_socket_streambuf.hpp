//
// basic_socket_streambuf.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NET_TS_BASIC_SOCKET_STREAMBUF_HPP
#define NET_TS_BASIC_SOCKET_STREAMBUF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <experimental/__net_ts/detail/config.hpp>

#if !defined(NET_TS_NO_IOSTREAM)

#include <streambuf>
#include <experimental/__net_ts/basic_socket.hpp>
#include <experimental/__net_ts/detail/array.hpp>
#include <experimental/__net_ts/detail/deadline_timer_service.hpp>
#include <experimental/__net_ts/detail/throw_error.hpp>
#include <experimental/__net_ts/io_context.hpp>
#include <experimental/__net_ts/stream_socket_service.hpp>

#if defined(NET_TS_HAS_BOOST_DATE_TIME)
# include <experimental/__net_ts/deadline_timer_service.hpp>
#else
# include <experimental/__net_ts/steady_timer.hpp>
#endif

#if !defined(NET_TS_HAS_VARIADIC_TEMPLATES)

# include <experimental/__net_ts/detail/variadic_templates.hpp>

// A macro that should expand to:
//   template <typename T1, ..., typename Tn>
//   basic_socket_streambuf* connect(T1 x1, ..., Tn xn)
//   {
//     init_buffers();
//     this->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec_);
//     typedef typename Protocol::resolver resolver_type;
//     resolver_type resolver(detail::socket_streambuf_base::io_context_);
//     connect_to_endpoints(
//         resolver.resolve(x1, ..., xn, ec_));
//     return !ec_ ? this : 0;
//   }
// This macro should only persist within this file.

# define NET_TS_PRIVATE_CONNECT_DEF(n) \
  template <NET_TS_VARIADIC_TPARAMS(n)> \
  basic_socket_streambuf* connect(NET_TS_VARIADIC_BYVAL_PARAMS(n)) \
  { \
    init_buffers(); \
    this->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec_); \
    typedef typename Protocol::resolver resolver_type; \
    resolver_type resolver(detail::socket_streambuf_base::io_context_); \
    connect_to_endpoints( \
        resolver.resolve(NET_TS_VARIADIC_BYVAL_ARGS(n), ec_)); \
    return !ec_ ? this : 0; \
  } \
  /**/

#endif // !defined(NET_TS_HAS_VARIADIC_TEMPLATES)

#if !defined(NET_TS_ENABLE_OLD_SERVICES)
# define NET_TS_SVC_T1 detail::deadline_timer_service<traits_helper>
#endif // !defined(NET_TS_ENABLE_OLD_SERVICES)

#include <experimental/__net_ts/detail/push_options.hpp>

namespace std {
namespace experimental {
namespace net {
inline namespace v1 {
namespace detail {

// A separate base class is used to ensure that the io_context is initialised
// prior to the basic_socket_streambuf's basic_socket base class.
class socket_streambuf_base
{
protected:
  io_context io_context_;
};

} // namespace detail

#if !defined(NET_TS_BASIC_SOCKET_STREAMBUF_FWD_DECL)
#define NET_TS_BASIC_SOCKET_STREAMBUF_FWD_DECL

// Forward declaration with defaulted arguments.
template <typename Protocol
    NET_TS_SVC_TPARAM_DEF1(= stream_socket_service<Protocol>),
#if defined(NET_TS_HAS_BOOST_DATE_TIME)
    typename Time = boost::posix_time::ptime,
    typename TimeTraits = time_traits<Time>
    NET_TS_SVC_TPARAM1_DEF2(= deadline_timer_service<Time, TimeTraits>)>
#else
    typename Time = chrono::steady_clock,
    typename TimeTraits = wait_traits<Time>
    NET_TS_SVC_TPARAM1_DEF1(= steady_timer::service_type)>
#endif
class basic_socket_streambuf;

#endif // !defined(NET_TS_BASIC_SOCKET_STREAMBUF_FWD_DECL)

/// Iostream streambuf for a socket.
#if defined(GENERATING_DOCUMENTATION)
template <typename Protocol,
    typename Time = chrono::steady_clock,
    typename TimeTraits = wait_traits<Time> >
#else // defined(GENERATING_DOCUMENTATION)
template <typename Protocol NET_TS_SVC_TPARAM,
    typename Time, typename TimeTraits NET_TS_SVC_TPARAM1>
#endif // defined(GENERATING_DOCUMENTATION)
class basic_socket_streambuf
  : public std::streambuf,
    private detail::socket_streambuf_base,
#if defined(NET_TS_NO_DEPRECATED) || defined(GENERATING_DOCUMENTATION)
    private basic_socket<Protocol NET_TS_SVC_TARG>
#else // defined(NET_TS_NO_DEPRECATED) || defined(GENERATING_DOCUMENTATION)
    public basic_socket<Protocol NET_TS_SVC_TARG>
#endif // defined(NET_TS_NO_DEPRECATED) || defined(GENERATING_DOCUMENTATION)
{
private:
  // These typedefs are intended keep this class's implementation independent
  // of whether it's using Boost.DateTime, Boost.Chrono or std::chrono.
#if defined(NET_TS_HAS_BOOST_DATE_TIME)
  typedef TimeTraits traits_helper;
#else
  typedef detail::chrono_time_traits<Time, TimeTraits> traits_helper;
#endif

public:
  /// The endpoint type.
  typedef typename Protocol::endpoint endpoint_type;

#if defined(GENERATING_DOCUMENTATION)
  /// (Deprecated: Use time_point.) The time type.
  typedef typename TimeTraits::time_type time_type;

  /// The time type.
  typedef typename TimeTraits::time_point time_point;

  /// (Deprecated: Use duration.) The duration type.
  typedef typename TimeTraits::duration_type duration_type;

  /// The duration type.
  typedef typename TimeTraits::duration duration;
#else
# if !defined(NET_TS_NO_DEPRECATED)
  typedef typename traits_helper::time_type time_type;
  typedef typename traits_helper::duration_type duration_type;
# endif // !defined(NET_TS_NO_DEPRECATED)
  typedef typename traits_helper::time_type time_point;
  typedef typename traits_helper::duration_type duration;
#endif

  /// Construct a basic_socket_streambuf without establishing a connection.
  basic_socket_streambuf()
    : basic_socket<Protocol NET_TS_SVC_TARG>(
        this->detail::socket_streambuf_base::io_context_),
      unbuffered_(false),
      timer_service_(0),
      timer_state_(no_timer)
  {
    init_buffers();
  }

  /// Destructor flushes buffered data.
  virtual ~basic_socket_streambuf()
  {
    if (pptr() != pbase())
      overflow(traits_type::eof());

    destroy_timer();
  }

  /// Establish a connection.
  /**
   * This function establishes a connection to the specified endpoint.
   *
   * @return \c this if a connection was successfully established, a null
   * pointer otherwise.
   */
  basic_socket_streambuf* connect(const endpoint_type& endpoint)
  {
    init_buffers();

    this->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec_);

    if (timer_state_ == timer_has_expired)
    {
      ec_ = std::experimental::net::error::operation_aborted;
      return 0;
    }

    io_handler handler = { this };
    this->basic_socket<Protocol NET_TS_SVC_TARG>::async_connect(
        endpoint, handler);

    ec_ = std::experimental::net::error::would_block;
    this->get_service().get_io_context().restart();
    do this->get_service().get_io_context().run_one();
    while (ec_ == std::experimental::net::error::would_block);

    return !ec_ ? this : 0;
  }

#if defined(GENERATING_DOCUMENTATION)
  /// Establish a connection.
  /**
   * This function automatically establishes a connection based on the supplied
   * resolver query parameters. The arguments are used to construct a resolver
   * query object.
   *
   * @return \c this if a connection was successfully established, a null
   * pointer otherwise.
   */
  template <typename T1, ..., typename TN>
  basic_socket_streambuf* connect(T1 t1, ..., TN tn);
#elif defined(NET_TS_HAS_VARIADIC_TEMPLATES)
  template <typename... T>
  basic_socket_streambuf* connect(T... x)
  {
    init_buffers();
    this->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec_);
    typedef typename Protocol::resolver resolver_type;
    resolver_type resolver(detail::socket_streambuf_base::io_context_);
    connect_to_endpoints(resolver.resolve(x..., ec_));
    return !ec_ ? this : 0;
  }
#else
  NET_TS_VARIADIC_GENERATE(NET_TS_PRIVATE_CONNECT_DEF)
#endif

  /// Close the connection.
  /**
   * @return \c this if a connection was successfully established, a null
   * pointer otherwise.
   */
  basic_socket_streambuf* close()
  {
    sync();
    this->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec_);
    if (!ec_)
      init_buffers();
    return !ec_ ? this : 0;
  }

  /// Get a reference to the underlying socket.
  basic_socket<Protocol NET_TS_SVC_TARG>& socket()
  {
    return *this;
  }

  /// Get the last error associated with the stream buffer.
  /**
   * @return An \c error_code corresponding to the last error from the stream
   * buffer.
   */
  const std::error_code& error() const
  {
    return ec_;
  }

#if !defined(NET_TS_NO_DEPRECATED)
  /// (Deprecated: Use error().) Get the last error associated with the stream
  /// buffer.
  /**
   * @return An \c error_code corresponding to the last error from the stream
   * buffer.
   */
  const std::error_code& puberror() const
  {
    return error();
  }

  /// (Deprecated: Use expiry().) Get the stream buffer's expiry time as an
  /// absolute time.
  /**
   * @return An absolute time value representing the stream buffer's expiry
   * time.
   */
  time_point expires_at() const
  {
    return timer_service_
      ? timer_service_->expires_at(timer_implementation_)
      : time_point();
  }
#endif // !defined(NET_TS_NO_DEPRECATED)

  /// Get the stream buffer's expiry time as an absolute time.
  /**
   * @return An absolute time value representing the stream buffer's expiry
   * time.
   */
  time_point expiry() const
  {
    return timer_service_
#if defined(NET_TS_HAS_BOOST_DATE_TIME)
      ? timer_service_->expires_at(timer_implementation_)
#else // defined(NET_TS_HAS_BOOST_DATE_TIME)
      ? timer_service_->expiry(timer_implementation_)
#endif // defined(NET_TS_HAS_BOOST_DATE_TIME)
      : time_point();
  }

  /// Set the stream buffer's expiry time as an absolute time.
  /**
   * This function sets the expiry time associated with the stream. Stream
   * operations performed after this time (where the operations cannot be
   * completed using the internal buffers) will fail with the error
   * std::experimental::net::error::operation_aborted.
   *
   * @param expiry_time The expiry time to be used for the stream.
   */
  void expires_at(const time_point& expiry_time)
  {
    construct_timer();

    std::error_code ec;
    timer_service_->expires_at(timer_implementation_, expiry_time, ec);
    std::experimental::net::detail::throw_error(ec, "expires_at");

    start_timer();
  }

  /// Set the stream buffer's expiry time relative to now.
  /**
   * This function sets the expiry time associated with the stream. Stream
   * operations performed after this time (where the operations cannot be
   * completed using the internal buffers) will fail with the error
   * std::experimental::net::error::operation_aborted.
   *
   * @param expiry_time The expiry time to be used for the timer.
   */
  void expires_after(const duration& expiry_time)
  {
    construct_timer();

    std::error_code ec;
#if defined(NET_TS_HAS_BOOST_DATE_TIME)
    timer_service_->expires_from_now(timer_implementation_, expiry_time, ec);
#else // defined(NET_TS_HAS_BOOST_DATE_TIME)
    timer_service_->expires_after(timer_implementation_, expiry_time, ec);
#endif // defined(NET_TS_HAS_BOOST_DATE_TIME)
    std::experimental::net::detail::throw_error(ec, "expires_after");

    start_timer();
  }

#if !defined(NET_TS_NO_DEPRECATED)
  /// (Deprecated: Use expiry().) Get the stream buffer's expiry time relative
  /// to now.
  /**
   * @return A relative time value representing the stream buffer's expiry time.
   */
  duration expires_from_now() const
  {
    return traits_helper::subtract(expires_at(), traits_helper::now());
  }

  /// (Deprecated: Use expires_after().) Set the stream buffer's expiry time
  /// relative to now.
  /**
   * This function sets the expiry time associated with the stream. Stream
   * operations performed after this time (where the operations cannot be
   * completed using the internal buffers) will fail with the error
   * std::experimental::net::error::operation_aborted.
   *
   * @param expiry_time The expiry time to be used for the timer.
   */
  void expires_from_now(const duration& expiry_time)
  {
    construct_timer();

    std::error_code ec;
    timer_service_->expires_from_now(timer_implementation_, expiry_time, ec);
    std::experimental::net::detail::throw_error(ec, "expires_from_now");

    start_timer();
  }
#endif // !defined(NET_TS_NO_DEPRECATED)

protected:
  int_type underflow()
  {
    if (gptr() == egptr())
    {
      if (timer_state_ == timer_has_expired)
      {
        ec_ = std::experimental::net::error::operation_aborted;
        return traits_type::eof();
      }

      io_handler handler = { this };
      this->get_service().async_receive(this->get_implementation(),
          std::experimental::net::buffer(std::experimental::net::buffer(get_buffer_) + putback_max),
          0, handler);

      ec_ = std::experimental::net::error::would_block;
      this->get_service().get_io_context().restart();
      do this->get_service().get_io_context().run_one();
      while (ec_ == std::experimental::net::error::would_block);
      if (ec_)
        return traits_type::eof();

      setg(&get_buffer_[0], &get_buffer_[0] + putback_max,
          &get_buffer_[0] + putback_max + bytes_transferred_);
      return traits_type::to_int_type(*gptr());
    }
    else
    {
      return traits_type::eof();
    }
  }

  int_type overflow(int_type c)
  {
    if (unbuffered_)
    {
      if (traits_type::eq_int_type(c, traits_type::eof()))
      {
        // Nothing to do.
        return traits_type::not_eof(c);
      }
      else
      {
        if (timer_state_ == timer_has_expired)
        {
          ec_ = std::experimental::net::error::operation_aborted;
          return traits_type::eof();
        }

        // Send the single character immediately.
        char_type ch = traits_type::to_char_type(c);
        io_handler handler = { this };
        this->get_service().async_send(this->get_implementation(),
            std::experimental::net::buffer(&ch, sizeof(char_type)), 0, handler);

        ec_ = std::experimental::net::error::would_block;
        this->get_service().get_io_context().restart();
        do this->get_service().get_io_context().run_one();
        while (ec_ == std::experimental::net::error::would_block);
        if (ec_)
          return traits_type::eof();

        return c;
      }
    }
    else
    {
      // Send all data in the output buffer.
      std::experimental::net::const_buffer buffer =
        std::experimental::net::buffer(pbase(), pptr() - pbase());
      while (buffer.size() > 0)
      {
        if (timer_state_ == timer_has_expired)
        {
          ec_ = std::experimental::net::error::operation_aborted;
          return traits_type::eof();
        }

        io_handler handler = { this };
        this->get_service().async_send(this->get_implementation(),
            std::experimental::net::buffer(buffer), 0, handler);

        ec_ = std::experimental::net::error::would_block;
        this->get_service().get_io_context().restart();
        do this->get_service().get_io_context().run_one();
        while (ec_ == std::experimental::net::error::would_block);
        if (ec_)
          return traits_type::eof();

        buffer = buffer + bytes_transferred_;
      }
      setp(&put_buffer_[0], &put_buffer_[0] + put_buffer_.size());

      // If the new character is eof then our work here is done.
      if (traits_type::eq_int_type(c, traits_type::eof()))
        return traits_type::not_eof(c);

      // Add the new character to the output buffer.
      *pptr() = traits_type::to_char_type(c);
      pbump(1);
      return c;
    }
  }

  int sync()
  {
    return overflow(traits_type::eof());
  }

  std::streambuf* setbuf(char_type* s, std::streamsize n)
  {
    if (pptr() == pbase() && s == 0 && n == 0)
    {
      unbuffered_ = true;
      setp(0, 0);
      return this;
    }

    return 0;
  }

private:
  void init_buffers()
  {
    setg(&get_buffer_[0],
        &get_buffer_[0] + putback_max,
        &get_buffer_[0] + putback_max);
    if (unbuffered_)
      setp(0, 0);
    else
      setp(&put_buffer_[0], &put_buffer_[0] + put_buffer_.size());
  }

  template <typename EndpointSequence>
  void connect_to_endpoints(const EndpointSequence& endpoints)
  {
    if (!ec_)
    {
      typename EndpointSequence::iterator i = endpoints.begin();
      typename EndpointSequence::iterator end = endpoints.end();
      ec_ = std::experimental::net::error::host_not_found;
      while (ec_ && i != end)
      {
        this->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec_);

        if (timer_state_ == timer_has_expired)
        {
          ec_ = std::experimental::net::error::operation_aborted;
          return;
        }

        io_handler handler = { this };
        this->basic_socket<Protocol NET_TS_SVC_TARG>::async_connect(
            *i, handler);

        ec_ = std::experimental::net::error::would_block;
        this->get_service().get_io_context().restart();
        do this->get_service().get_io_context().run_one();
        while (ec_ == std::experimental::net::error::would_block);

        ++i;
      }
    }
  }

  struct io_handler;
  friend struct io_handler;
  struct io_handler
  {
    basic_socket_streambuf* this_;

    void operator()(const std::error_code& ec,
        std::size_t bytes_transferred = 0)
    {
      this_->ec_ = ec;
      this_->bytes_transferred_ = bytes_transferred;
    }
  };

  struct timer_handler;
  friend struct timer_handler;
  struct timer_handler
  {
    basic_socket_streambuf* this_;

    void operator()(const std::error_code&)
    {
      time_point now = traits_helper::now();

#if defined(NET_TS_HAS_BOOST_DATE_TIME)
      time_point expiry_time = this_->timer_service_->expires_at(
            this_->timer_implementation_);
#else // defined(NET_TS_HAS_BOOST_DATE_TIME)
      time_point expiry_time = this_->timer_service_->expiry(
            this_->timer_implementation_);
#endif // defined(NET_TS_HAS_BOOST_DATE_TIME)

      if (traits_helper::less_than(now, expiry_time))
      {
        this_->timer_state_ = timer_is_pending;
        this_->timer_service_->async_wait(this_->timer_implementation_, *this);
      }
      else
      {
        this_->timer_state_ = timer_has_expired;
        std::error_code ec;
        this_->basic_socket<Protocol NET_TS_SVC_TARG>::close(ec);
      }
    }
  };

  void construct_timer()
  {
    if (timer_service_ == 0)
    {
      NET_TS_SVC_T1& timer_service = use_service<NET_TS_SVC_T1>(
          detail::socket_streambuf_base::io_context_);
      timer_service.construct(timer_implementation_);
      timer_service_ = &timer_service;
    }
  }

  void destroy_timer()
  {
    if (timer_service_)
      timer_service_->destroy(timer_implementation_);
  }

  void start_timer()
  {
    if (timer_state_ != timer_is_pending)
    {
      timer_handler handler = { this };
      handler(std::error_code());
    }
  }

  enum { putback_max = 8 };
  enum { buffer_size = 512 };
  std::experimental::net::detail::array<char, buffer_size> get_buffer_;
  std::experimental::net::detail::array<char, buffer_size> put_buffer_;
  bool unbuffered_;
  std::error_code ec_;
  std::size_t bytes_transferred_;
  NET_TS_SVC_T1* timer_service_;
  typename NET_TS_SVC_T1::implementation_type timer_implementation_;
  enum state { no_timer, timer_is_pending, timer_has_expired } timer_state_;
};

} // inline namespace v1
} // namespace net
} // namespace experimental
} // namespace std

#include <experimental/__net_ts/detail/pop_options.hpp>

#if !defined(NET_TS_ENABLE_OLD_SERVICES)
# undef NET_TS_SVC_T1
#endif // !defined(NET_TS_ENABLE_OLD_SERVICES)

#if !defined(NET_TS_HAS_VARIADIC_TEMPLATES)
# undef NET_TS_PRIVATE_CONNECT_DEF
#endif // !defined(NET_TS_HAS_VARIADIC_TEMPLATES)

#endif // !defined(NET_TS_NO_IOSTREAM)

#endif // NET_TS_BASIC_SOCKET_STREAMBUF_HPP
