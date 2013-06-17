#pragma once

#include "session.h"
#include "client.h"

#include <boost/thread.hpp>

namespace gtl
{

namespace svr
{

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service)
		, acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
	{
		boost::shared_ptr<session> new_session(new session(io_service_));
		acceptor_.async_accept(new_session->socket(), boost::bind(
			&server::handle_accept, this, new_session,
			boost::asio::placeholders::error));
	}

	void handle_accept(boost::shared_ptr<session> new_session, const boost::system::error_code& error)
	{
		if (!error)
		{
			TRACE("new session\n");
			new_session->start();

			// 连接成功,保存当前会话.
			gtl::tstr ip(new_session->socket().remote_endpoint().address().to_string());
			boost::shared_ptr<client> clnt(new client(this, ip, _T(""), new_session));
			new_session->set_client(clnt.get());
			m_sessions.push_back(clnt);

			new_session.reset(new session(io_service_));
			acceptor_.async_accept(new_session->socket(), boost::bind(
				&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
		}
	}

	void run()
	{
		io_thread_.reset(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
	}

	void stop()
	{
		io_service_.stop();
		io_thread_->join();
	}

public:
	void handle_login(boost::shared_ptr<client> clnt, const gtl::tstr& name)
	{
		const gtl::tstr& ip = clnt->ip();
		boost::shared_ptr<session> sess = clnt->sess();

		auto first = m_clients.begin();
		auto last = m_clients.end();
		for(; first != last; ++first)
		{
			boost::shared_ptr<client> cln = *first;
			if((cln->ip() + cln->name()) == (ip + name))
			{
				cln->set_session(sess);
				sess->set_client(cln.get());
				remove_session(cln);
				cln->handle_write();

				TRACE("relog, client count: %d\n", m_clients.size());
				return;
			}
		}

		boost::shared_ptr<client> new_clnt(new client(this, ip, name, sess));
		sess->set_client(clnt.get());

		m_clients.push_back(new_clnt);
		TRACE("client count: %d\n", m_clients.size());
	}

public:
	const std::list<boost::shared_ptr<client>>& clients()
	{
		return m_clients;
	}

protected:
	void remove_session(boost::shared_ptr<client> clnt)
	{
		auto first = m_sessions.begin();
		auto last = m_sessions.end();
		for(; first != last; ++first)
		{
			if(clnt.get() == (*first).get())
			{
				m_sessions.erase(first);
				break;
			}
		}
	}

private:
	boost::shared_ptr<boost::thread> io_thread_;
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;

	std::list<boost::shared_ptr<client>> m_sessions;
	std::list<boost::shared_ptr<client>> m_clients;
};

}

}