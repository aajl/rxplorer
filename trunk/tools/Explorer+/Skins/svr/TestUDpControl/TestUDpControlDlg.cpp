
// TestUDpControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestUDpControl.h"
#include "TestUDpControlDlg.h"
#include "afxdialogex.h"

#include <gtl/string/str.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestUDpControlDlg dialog




CTestUDpControlDlg::CTestUDpControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestUDpControlDlg::IDD, pParent)
	, m_strSendText(_T(""))
	, m_nLocalPort(2345)
	//, m_strRemoveIP(_T("192.168.1.157"))
	, m_strRemoveIP(_T("224.0.0.127"))
	, m_nRemovePort(2346)
	, m_strRecvText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestUDpControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TxtSndWnd, m_strSendText);
	DDX_Text(pDX, IDC_TxtLocalPort, m_nLocalPort);
	DDX_Text(pDX, IDC_TxtRemoveIP, m_strRemoveIP);
	DDX_Text(pDX, IDC_TxtRemovePort, m_nRemovePort);
	DDX_Text(pDX, IDC_TxtRecvWnd, m_strRecvText);
}

BEGIN_MESSAGE_MAP(CTestUDpControlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestUDpControlDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTestUDpControlDlg message handlers

BOOL CTestUDpControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	gtl::net::socket::init();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_udp.create(m_nLocalPort, SOCK_DGRAM);

	bool bOpt = true;
	int retsult = setsockopt(m_udp, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)); 
	if(retsult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		MessageBox(_T("ÉèÖÃ¶à²¥µØÖ·Ê§°Ü"), _T("´íÎó"), MB_OK | MB_ICONINFORMATION);
	}

	m_thread.start([=]() -> unsigned long {
		
		while(true)
		{
			ushort port = 0;
			char ip[16] = {0};
			char buf[1024] = {0};
			int ret = this->m_udp.recvfrom(buf, sizeof(buf) / sizeof(buf[0]), ip, port);
			if(ret <= 0)
				continue;

			gtl::tstr log;
			log << (const char*)ip << ":" << port;
			log << "    [" << (const char*)buf << "]";
			
			CTestUDpControlDlg* pThis = this;
			this->m_callback.call(log, [=](const gtl::tstr& log) {
				pThis->m_strRecvText = log;
				pThis->UpdateData(FALSE);
			});
		}
	});

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestUDpControlDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestUDpControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestUDpControlDlg::OnBnClickedButton1()
{
	UpdateData();
	if(m_strSendText.IsEmpty())
		return;

	gtl::str data((LPCTSTR)m_strSendText);
	int ret = m_udp.sendto(data.data(), data.size(), gtl::str((LPCTSTR)m_strRemoveIP), m_nRemovePort);
}
