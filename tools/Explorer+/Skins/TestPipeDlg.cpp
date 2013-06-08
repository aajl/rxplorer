// TestPipeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestPipe.h"
#include "TestPipeDlg.h"
#include <math.h>
#include <list>

#include <gtl/io/file.h>
#include <gtl/type/inner.h>
#include <gtl/string/str.h>
#include <gtl/container/circle_queue.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
		cmd = 0;
		flag = 0;
		size = 0;
		offset = 0;
		len = 0;
		data = 0;
	}

	uint8 identify; // 0xFF
	uint8 cmd;
	uint8 flag;
	uint32 size;
	uint32 offset;
	uint16 len;
	char* data;
};
#pragma pack(pop)

template<class Server, class Session>
class client_
{
public:
	client_(Server* svr, const gtl::tstr& ip, const gtl::tstr& name, boost::shared_ptr<Session> session)
	{
		m_ip = ip;
		m_name = name;
		m_session = session;
		m_svr = svr;
		m_type = 0;
		m_curr_file_pos = 0;

		send("aaaaaaaaaa", type_image);
	}

public:
	const gtl::tstr& get_ip()
	{
		return m_ip;
	}

	const gtl::tstr& get_name()
	{
		return m_name;
	}

public:
	void send(const gtl::tstr& data, type_e type)
	{
		if(m_session == NULL)
			return;

		m_type = type;
		m_data = data;
		if(type == type_text)
		{
			gtl::str pkt;
			gtl::str txt = data.utf8();
			m_send_data.push_back(pack(pkt, cmd_text, txt.length(), txt.data()));

			if(m_send_data.size() == 1)
				m_session->send(m_send_data.back());
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

			if(m_send_files.size() == 1 && m_send_data.empty())
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

				gtl::str pkt;
				m_send_data.push_back(pack(pkt, type, len, dt, size, offset));
				m_session->send(m_send_data.back());
			}
		}
	}

public:
	void handle_recv(const char* data, size_t len)
	{
		packet pkt;
		bool alloc_data = false;
		const int header_len = sizeof(packet) - sizeof(char*);
		if(!m_queue.empty())
		{
			m_queue.push(data, len);
			if(m_queue[m_queue.front()] != 0xFF)
			{
				handle_close();
				return;
			}

			if(m_queue.length() < header_len)
				return;

			m_queue.get((char*)&pkt, header_len);
			if(m_queue.length() < header_len + pkt.len)
				return;

			alloc_data = true;
			m_queue.skip(header_len);
			pkt.data = m_queue.pop(pkt.len);
		}
		else if(len < header_len)
		{
			m_queue.push(data, len);
			return;
		}
		else
		{
			packet* pckt = (packet*)data;
			if(len < header_len + pckt->len)
			{
				m_queue.push(data, len);
				return;
			}
			else if(len > header_len + pckt->len)
			{
				m_queue.push(data + header_len + pckt->len, len - header_len + pckt->len);
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
				m_svr->handle_login(m_session, gtl::tstr(name));
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
				else
					m_curr_file_pos = pkt.offset + pkt.len;
			}

			break;
		}

		if(alloc_data)
			delete[] pkt.data;
	}

	void handle_write()
	{
		if(!m_send_data.empty())
			m_send_data.pop_front();

		if(!m_send_data.empty() && m_session != NULL)
			m_session->send(m_send_data.front());
	}

	void handle_close()
	{
		m_session.reset();
	}

protected:
	const gtl::str& pack(gtl::str& pkt, uint8 cmd, uint8 flag)
	{
		return pack(pkt, cmd, 0, NULL, 0, 0, flag);
	}

	const gtl::str& pack(gtl::str& pkt, uint8 cmd, uint16 len, char* data, uint32 size = 0, uint32 offset = 0, uint8 flag = 0)
	{
		const int header_len = sizeof(packet) - sizeof(char*);
		pkt.resize(header_len + len);
		pkt << (uint8)0xFF << cmd << flag << size << offset << len;
		if(data != NULL)
			memcpy(pkt.data() + header_len, data, len);

		return pkt;
	}

protected:
	gtl::tstr m_ip;
	gtl::tstr m_name;
	uint8 m_type;
	gtl::tstr m_data;
	uint32 m_curr_file_pos;
	boost::shared_ptr<Session> m_session;

	Server* m_svr;
	gtl::circle_queue m_queue;

	std::list<gtl::str> m_send_data;
	std::list<boost::shared_ptr<gtl::file>> m_send_files;
};

class server;
class session;

typedef client_<server, session> client;

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

			if(strcmp(m_recv_buff, "exit") == 0)
				boost::asio::async_write(m_socket, boost::asio::buffer(m_recv_buff,
				bytes_transferred), boost::bind(&session::handle_write,
				this, boost::asio::placeholders::error));
		}
		else
		{
			if(m_client != NULL)
				m_client->handle_close();

			TRACE("read closed\n");
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			TRACE("send completed\n");
			if(m_client != NULL)
				m_client->handle_write();
		}
		else
		{
			TRACE("write closed\n");
			if(m_client != NULL)
				m_client->handle_close();
		}
	}

public:
	void send(const gtl::str& data)
	{
		boost::asio::async_write(m_socket, boost::asio::buffer(data.c_str(), data.capacity()),
									boost::bind(&session::handle_write, this, boost::asio::placeholders::error));
	}

private:
	enum
	{
		max_length = 1024
	};

	client* m_client;
	char m_recv_buff[max_length];
	boost::asio::ip::tcp::socket m_socket;
};

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
			new_session->start();
			handle_login(new_session, "aaa");

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
	void handle_login(boost::shared_ptr<session> sess, const gtl::tstr& name)
	{
		gtl::tstr ip(sess->socket().remote_endpoint().address().to_string());
		boost::shared_ptr<client> clnt(new client(this, name, ip, sess));
		sess->set_client(clnt.get());

		m_clients.push_back(clnt);
	}

private:
	boost::shared_ptr<boost::thread> io_thread_;
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;

	std::list<boost::shared_ptr<client>> m_clients;
};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestPipeDlg dialog

const gtl::str& pack(gtl::str& pkt, uint8 cmd, uint16 len, char* data, uint32 size = 0, uint32 offset = 0, uint8 flag = 0)
{
	const int header_len = sizeof(packet) - sizeof(char*);
	pkt.resize(header_len + len);
	pkt << (uint8)0xFF << cmd << flag << size << offset << len;
	if(data != NULL)
		memcpy(pkt.data() + header_len, data, len);

	return pkt;
}

const gtl::str& pack(gtl::str& pkt, uint8 cmd, uint8 flag)
{
	return pack(pkt, cmd, 0, NULL, 0, 0, flag);
}

CTestPipeDlg::CTestPipeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestPipeDlg::IDD, pParent)
	, m_nTestValue(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	gtl::str ss;
	pack(ss, cmd_text, flag_ack);

	m_svr = new server(m_ios, 8845);
	m_svr->run();
}

CTestPipeDlg::~CTestPipeDlg()
{
	m_svr->stop();
	delete m_svr;
}

void CTestPipeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_nTestValue);
}

BEGIN_MESSAGE_MAP(CTestPipeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CTestPipeDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestPipeDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CTestPipeDlg message handlers

BOOL CTestPipeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//m_converter.Convert(NULL, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestPipeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestPipeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestPipeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestPipeDlg::OnBnClickedButton1()
{
	UpdateData();

	TRACE("%d\n", m_nTestValue);
}

void CTestPipeDlg::OnBnClickedButton2()
{
	UpdateData(FALSE);

	TRACE("%d\n", m_nTestValue);
}
