#pragma once

#include <gtl/io/file.h>
#include <gtl/string/str.h>
#include <gtl/type/inner.h>
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
	}

	uint8 identify; // 0xFF
	uint8 id;
	uint8 cmd;
	uint8 flag;
	uint32 size;
	uint32 offset;
	uint16 len;
	char* data;

	void set_data(const char* data, uint16 len)
	{
		if(data == NULL)
			return;


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

	static int header_len()
	{
		return sizeof(packet) - sizeof(char*) - sizeof(gtl::str);
	}

private:
	gtl::str m_buff;
};
#pragma pack(pop)

namespace detail
{

template<class Server, class Session>
class client : public boost::enable_shared_from_this<client<Server, Session>>
{
public:
	client(Server* svr, const gtl::tstr& ip, const gtl::tstr& name, boost::shared_ptr<Session> session)
	{
		m_ip = ip;
		m_name = name;
		m_session = session;
		m_svr = svr;
		m_type = 0;
		m_curr_file_pos = 0;
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

		m_type = type;
		m_data = data;
		if(type == type_text)
		{
			gtl::str txt = data.utf8();
			m_packets.push_back(packet(cmd_text));
			m_packets.back().set_data(txt, txt.length());

			if(m_packets.size() == 1)
				m_session->send(m_packets.front().buff());
		}
		else if(type == type_image || type == type_video)
		{
			boost::shared_ptr<gtl::file> file(new gtl::file());
			if(!file->open(m_data, _T("rb")))
			{
				m_data.clear();
				return;
			}

			m_send_files.push_back(file);

			if(m_send_files.size() == 1 && m_packets.empty())
			{
				int size = file->size();
				int offset = file->tell();
				if(size < 0 || offset < 0)
				{
					m_send_files.pop_back();
					return;
				}

				char dt[4096] = {0};
				size_t len = file->read(dt, sizeof(dt) / sizeof(dt[0]));

				m_packets.push_back(packet(type, len, dt, size, offset));
				m_session->send(m_packets.front().buff());
			}
		}
	}

public:
	void handle_recv(const char* data, size_t len)
	{
		packet pkt;
		bool alloc_data = false;
		const int header_len = sizeof(packet) - sizeof(char*);
		if(!m_recv_queue.empty())
		{
			m_recv_queue.push(data, len);
			if(m_recv_queue[m_recv_queue.front()] != 0xFF)
			{
				handle_close();
				return;
			}

			if(m_recv_queue.length() < header_len)
				return;

			m_recv_queue.get((char*)&pkt, header_len);
			if(m_recv_queue.length() < header_len + pkt.len)
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
				m_recv_queue.push(data + header_len + pckt->len, len - header_len + pckt->len);
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
				m_svr->handle_login(shared_from_this(), gtl::tstr(name));

				TRACE("login: %s\n", name.c_str());
			}
			break;

		case cmd_text:
			if(pkt.flag == flag_ack)
				m_data.clear();

			break;

		case cmd_image:
		case cmd_video:
			if(pkt.flag == flag_ack)
			{
				if(pkt.offset + pkt.len == pkt.size)
					m_data.clear();
			}

			break;
		}

		if(alloc_data)
			delete[] pkt.data;
	}

	void handle_write()
	{
		if(m_session == NULL)
			return;

		auto first = m_packets.begin();
		auto last = m_packets.end();
		for(; first != last; ++first)
		{
			const packet& pkt = *first;
			if(pkt.buff(false).capacity() > 0)
				continue;

			m_session->send(pkt.buff());
			break;
		}
	}

	void handle_close()
	{
		m_session.reset();
	}

protected:
	gtl::tstr m_ip;
	gtl::tstr m_name;
	boost::shared_ptr<Session> m_session;

	Server* m_svr;
	gtl::circle_queue m_recv_queue;

	std::list<packet> m_packets;
	std::list<boost::shared_ptr<gtl::file>> m_send_files;
};

template<class Server>
class session;

} // end of namespace detail

class server;
class session;

typedef detail::client<server, session> client;

} // end of namespace svr

} // end of namespace gtl