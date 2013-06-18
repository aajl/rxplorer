#pragma once

#include <gtl/io/file.h>
#include <gtl/string/str.h>
#include <gtl/type/inner.h>
#include <gtl/crypto/sha.hpp>
#include <gtl/container/circle_queue.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

namespace gtl
{

namespace svr
{

enum cmd_e
{
	cmd_report_name = 1,
	cmd_text,
	cmd_image,
	cmd_video,
};

enum flag_e
{
	flag_ack = 1,
};

enum type_e
{
	type_text,
	type_image,
	type_video,
};

#pragma pack(push, 1)
struct packet
{
	packet()
	{
		identify = 0;
		id = 0;
		cmd = 0;
		flag = 0;
		size = 0;
		offset = 0;
		len = 0;
		data = 0;
		memset(hash, 0, sizeof(hash) / sizeof(hash[0]));
	}

	packet(uint8 cmd, uint32 size = 0, uint32 offset = 0, uint8 flag = 0)
	{
		this->identify = 0xFF;
		this->id = 0;
		this->cmd = cmd;
		this->flag = flag;
		this->size = size;
		this->offset = offset;
		this->len = 0;
		this->data = NULL;
		memset(hash, 0, sizeof(hash) / sizeof(hash[0]));
	}

	uint8 identify; // 0xFF
	uint16 id;
	uint8 cmd;
	uint8 flag;
	uint32 size;
	uint32 offset;
	uint16 len;
	char hash[40];
	char* data;

	void reset()
	{
		m_buff.clear();
	}

	void set_hash(const char* hash, uint16 len)
	{
		if(hash == NULL || len != sizeof(this->hash) / sizeof(this->hash[0]))
			return;

		memcpy(this->hash, hash, len);
	}

	void set_data(const char* data, uint16 len)
	{
		if(data == NULL)
			return;

		this->data = (char*)realloc(this->data, len);
		memcpy(this->data, data, len);
	}

	const gtl::str& buff(bool fill = true, bool refill = false)
	{
		if(fill && (refill || m_buff.capacity() == 0))
		{
			m_buff.resize(header_len() + len);
			m_buff.mcpy(0, (const char*)this, header_len());
			if(data != NULL && len > 0)
				m_buff.mcpy(header_len(), data, len);
		}

		return m_buff;
	}

	static size_t header_len()
	{
		return sizeof(packet) - sizeof(char*) - sizeof(gtl::str);
	}

private:
	gtl::str m_buff;
};
#pragma pack(pop)

namespace detail
{

template<class Server, class Session, class Sha>
class client : public boost::enable_shared_from_this<client<Server, Session, Sha>>
{
public:
	client(Server* svr, const gtl::tstr& ip, const gtl::tstr& name, boost::shared_ptr<Session> session)
	{
		m_id = 0;
		m_ip = ip;
		m_name = name;
		m_session = session;
		m_svr = svr;
	}

public:
	const gtl::tstr& ip()
	{
		return m_ip;
	}

	const gtl::tstr& name()
	{
		return m_name;
	}

	boost::shared_ptr<Session> sess()
	{
		return m_session;
	}

	void set_session(boost::shared_ptr<Session> sess)
	{
		m_session = sess;
	}

public:
	void send(const gtl::tstr& data, cmd_e cmd)
	{
		if(m_session == NULL)
			return;

		if(cmd == cmd_text)
		{
			gtl::str txt = data.utf8();
			m_packets.push_back(packet(cmd_text));
			m_packets.back().id = get_id();
			m_packets.back().set_data(txt, txt.length());

			if(m_packets.size() == 1)
				m_session->send(m_packets.front().buff());
		}
		else if(cmd == cmd_image || cmd == cmd_video)
		{
			boost::shared_ptr<file> fl(new file());
			if(!fl->open(data, _T("rb")))
				return;

			fl->cmd = cmd;
			Sha::get(fl.get(), fl->hash, sizeof(fl->hash) / sizeof(fl->hash[0]));
			m_send_files.push_back(fl);
			if(m_send_files.size() == 1 && m_packets.empty())
			{
				int size = (int)fl->size();
				int offset = (int)fl->tell();
				if(size < 0 || offset < 0)
				{
					m_send_files.pop_back();
					return;
				}

				char data[4096] = {0};
				size_t len = fl->read(data, sizeof(data) / sizeof(data[0]));

				m_packets.push_back(packet(cmd, size, offset));
				m_packets.back().id = get_id();
				m_packets.back().set_data(data, len);
				m_packets.back().set_hash(fl->hash, sizeof(fl->hash) / sizeof(fl->hash[0]));
				m_session->send(m_packets.front().buff());
			}
		}
	}

public:
	void handle_recv(const char* data, size_t len)
	{
		packet pkt;
		bool alloc_data = false;
		const size_t header_len = packet::header_len();
		if(!m_recv_queue.empty())
		{
			m_recv_queue.push(data, len);
			if(m_recv_queue[m_recv_queue.front()] != 0xFF)
			{
				handle_close();
				return;
			}

			if(m_recv_queue.length() < (int)header_len)
				return;

			m_recv_queue.get((char*)&pkt, header_len);
			if(m_recv_queue.length() < (int)header_len + pkt.len)
				return;

			alloc_data = true;
			m_recv_queue.skip(header_len);
			pkt.data = m_recv_queue.pop(pkt.len);
		}
		else if(len < header_len)
		{
			m_recv_queue.push(data, len);
			return;
		}
		else
		{
			packet* pckt = (packet*)data;
			if(len < (size_t)header_len + pckt->len)
			{
				m_recv_queue.push(data, len);
				return;
			}
			else if(len > (size_t)header_len + pckt->len)
			{
				m_recv_queue.push(data + header_len + pckt->len, len - header_len - pckt->len);
			}

			pkt = *pckt;
			pkt.data = (char*)(data + header_len);
		}

		switch(pkt.cmd)
		{
		case cmd_report_name:
			{
				gtl::str name;
				name.resize(pkt.len + 1);
				memcpy(name.data(), pkt.data, pkt.len);
				handle_login(gtl::tstr(name));
			}
			break;

		case cmd_text:
			break;

		case cmd_image:
		case cmd_video:
			break;
		}

		if(pkt.flag == flag_ack)
		{
			auto first = m_packets.begin();
			auto last = m_packets.end();
			for(; first != last; ++first)
			{
				if(first->id == pkt.id)
				{
					m_packets.erase(first);
					break;
				}
			}
		}

		if(alloc_data)
			delete[] pkt.data;
	}

	void handle_write(int id = 0)
	{
		if(m_session == NULL)
			return;

		auto first = m_packets.begin();
		auto last = m_packets.end();
		while(first != last)
		{
			packet& pkt = *first;
			if(pkt.buff(false).capacity() > 0)
			{
				if(id > 0 && pkt.id == id)
					m_packets.erase(first++);
				else
					++first;

				continue;
			}

			m_session->send(pkt.buff());
			return;
		}

		if(m_send_files.empty())
			return;

		boost::shared_ptr<file> fl = m_send_files.front();
		cmd_e cmd = fl->cmd;
		int size = (int)fl->size();
		int offset = (int)fl->tell();

		char data[4096] = {0};
		size_t len = fl->read(data, sizeof(data) / sizeof(data[0]));
		if(fl->eof())
			m_send_files.pop_front();

		m_packets.push_back(packet(cmd, size, offset));
		m_packets.back().id = get_id();
		m_packets.back().set_data(data, len);
		m_packets.back().set_hash(fl->hash, sizeof(fl->hash) / sizeof(fl->hash[0]));
		m_session->send(m_packets.front().buff());
	}

	void handle_login(const gtl::tstr& name)
	{
		if(m_svr == NULL)
			return;

		auto first = m_packets.begin();
		auto last = m_packets.end();
		for(; first != last; ++first)
		{
			first->reset();
		}

		TRACE(_T("login: %s\n"), name.c_str());
		m_svr->handle_login(shared_from_this(), name);
	}

	void handle_close()
	{
		m_session.reset();
	}

protected:
	uint8 get_id()
	{
		if(++m_id == 0)
			m_id = 1;

		return m_id;
	}

protected:
	class file : public gtl::file
	{
	public:
		virtual ~file()
		{
		}

	public:
		cmd_e cmd;
		char hash[40];
	};

protected:
	uint16 m_id;
	gtl::tstr m_ip;
	gtl::tstr m_name;
	boost::shared_ptr<Session> m_session;

	Server* m_svr;
	gtl::circle_queue m_recv_queue;

	std::list<packet> m_packets;
	std::list<boost::shared_ptr<file>> m_send_files;
};

template<class Server>
class session;

} // end of namespace detail

class server;
class session;

typedef detail::client<server, session, gtl::sha> client;

} // end of namespace svr

} // end of namespace gtl