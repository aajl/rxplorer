#pragma once

#include "client.h"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace gtl
{

namespace svr
{

class session
	: public boost::enable_shared_from_this<session>
{
public:
	session(boost::asio::io_service& io_service)
		: m_socket(io_service)
		, m_client(NULL)
	{
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return m_socket;
	}

	void set_client(client* clnt)
	{
		m_client = clnt;
	}

	void start()
	{
		m_socket.async_read_some(boost::asio::buffer(m_recv_buff, max_length),
			boost::bind(&session::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
	{
		if (!error)
		{
			m_recv_buff[bytes_transferred] = 0;
			TRACE("recv: %s\n", m_recv_buff);

			if(m_client != NULL)
				m_client->handle_recv(m_recv_buff, bytes_transferred);

			m_socket.async_read_some(boost::asio::buffer(m_recv_buff, max_length),
				boost::bind(&session::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			if(m_client != NULL)
				m_client->handle_close();

			TRACE("read closed\n");
		}
	}

	void handle_write(const boost::system::error_code& error, int id)
	{
		if (!error)
		{
			TRACE("send completed\n");
			if(m_client != NULL)
				m_client->handle_write(id);
		}
		else
		{
			TRACE("write closed\n");
			if(m_client != NULL)
				m_client->handle_close();
		}
	}

public:
	void send(const gtl::str& data, int id = 0)
	{
		boost::asio::async_write(m_socket, boost::asio::buffer(data.c_str(), data.capacity()),
			boost::bind(&session::handle_write, this, boost::asio::placeholders::error, id));
	}

private:
	enum
	{
		max_length = 8192
	};

	client* m_client;
	char m_recv_buff[max_length];
	boost::asio::ip::tcp::socket m_socket;
};

} // end of namespace svr

} // end of namespace gtl