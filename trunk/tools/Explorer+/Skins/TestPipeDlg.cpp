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
