
// UDPControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "UDPControlDlg.h"
#include "afxdialogex.h"

#include <gtl/string/str.h>
#include <gtl/io/app_path.h>
#include <gtl/io/ostrm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUDPControlDlg dialog
CUDPControlDlg::CUDPControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUDPControlDlg::IDD, pParent)
	, m_strRemoteIP(_T(""))
	, m_nRemotePort(0)
	, m_nLocalPort(0)
	, m_bSaveLog(FALSE)
	, m_strLog(_T(""))
	, m_strLocalIP(_T(""))
{
	m_start = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CUDPControlDlg::~CUDPControlDlg()
{
	Stop();
	Sleep(200);
	SaveSetting();
}

void CUDPControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TxtRemoteIP, m_strRemoteIP);
	DDX_Text(pDX, IDC_TxtRemotePort, m_nRemotePort);
	DDX_Text(pDX, IDC_TxtLocalPort, m_nLocalPort);
	DDX_Check(pDX, IDC_ChkSaveLog, m_bSaveLog);
	DDX_Text(pDX, IDC_TxtLog, m_strLog);
	DDX_Text(pDX, IDC_LabLocalIP, m_strLocalIP);
}

BEGIN_MESSAGE_MAP(CUDPControlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ChkSaveLog, &CUDPControlDlg::OnBnClickedChksavelog)
	ON_BN_CLICKED(IDC_BtnStart, &CUDPControlDlg::OnBnClickedBtnstart)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUDPControlDlg message handlers

BOOL CUDPControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_xml.load(gtl::io::get_app_path<gtl::tstr>() + _T("config.xml"));

	// TODO: Add extra initialization here
	hostent* pHostent = gethostbyname(NULL);
	if(pHostent == NULL)
	{
		MessageBox(_T("获取本机IP错误"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}

	in_addr addr = {0};
	addr.s_addr = *((unsigned long*)pHostent->h_addr_list[0]);
	gtl::str ip = inet_ntoa(addr);
	m_strLocalIP = gtl::tstr(ip);
	
	m_strRemoteIP = m_xml[_T("config")][_T("remote")](_T("ip"));
	m_nRemotePort = m_xml[_T("config")][_T("remote")](_T("port")).cast<int>();
	m_nLocalPort = m_xml[_T("config")][_T("local")](_T("port")).cast<int>();
	m_bSaveLog = m_xml[_T("config")](_T("save")).cast<bool>();

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CUDPControlDlg::OnPaint()
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
HCURSOR CUDPControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUDPControlDlg::OnBnClickedChksavelog()
{
	// TODO: Add your control notification handler code here
}

static struct CmdInfo
{
	const char* cmd;
	bool (CUDPControlDlg::*fn)(const gtl::str&);
}cmds[] = 
{
	"poweroff:",			&CUDPControlDlg::Poweroff,
	"reboot:",				&CUDPControlDlg::Reboot,
	"cmd:",					&CUDPControlDlg::Cmd,
	"ctrl+",				&CUDPControlDlg::Shortcut,
	"shift+",				&CUDPControlDlg::Shortcut,
	"alt+",					&CUDPControlDlg::Shortcut,
	"ctrl+shift+",			&CUDPControlDlg::Shortcut,
	"ctrl+alt+",			&CUDPControlDlg::Shortcut,
	"ctrl+shift+alt+",		&CUDPControlDlg::Shortcut,
	"enter",				&CUDPControlDlg::Shortcut,
};

void CUDPControlDlg::Start()
{
	m_start = true;
	m_udp.closesocket();
	m_udp.create(m_nLocalPort, SOCK_DGRAM);
	m_udp.makedestsockaddr(gtl::str((LPCTSTR)m_strRemoteIP), m_nRemotePort);

	m_thread.start([this]() -> unsigned long {

		this->m_udp.sendto("online", 6);

		while(this->m_start)
		{
			char buf[1024] = {0};
			int ret = this->m_udp.recvfrom(buf, sizeof(buf) / sizeof(buf[0]));
			if(ret <= 0)
				continue;

			bool found = false;
			bool (CUDPControlDlg::*fn)(const gtl::str&) = NULL;
			for(int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); ++i)
			{
				if(gtl::str_warp(buf).icmp(cmds[i].cmd, -1))
				{
					found = true;
					fn = cmds[i].fn;
					break;
				}
			}

			if(!found)
			{
				this->m_udp.sendto("error", 3);
				continue;
			}

			gtl::tstr ip(this->m_udp.getsockaddr());
			int port = this->m_udp.getsockport();

			CTime time = CTime::GetCurrentTime();

			gtl::tstr log;
			log << (LPCTSTR)time.Format(_T("%Y-%m-%d %H:%M:%S\r\n"));
			log << ip << ":" << port;
			log << "    [" << ret << "字节数据]" << "\r\n";

			m_strLog += log;

			gtl::str cmd = buf;
			CUDPControlDlg* pThis = this;
			this->m_callback.call(cmd, fn, log, [pThis](const gtl::str& cmd, bool (CUDPControlDlg::*fn)(const gtl::str&), const gtl::tstr& log) {

				if(fn != NULL && (pThis->*fn)(cmd))
					pThis->m_udp.sendto("ack", 3);
				else
					pThis->m_udp.sendto("error", 3);

				pThis->UpdateData(FALSE);
			});
		}

		this->m_udp.sendto("offline", 6);

		gtl::dout << "停止\n";

		return 0;
	});
}

void CUDPControlDlg::Stop()
{
	m_start = false;
	m_udp.closesocket();
}

void CUDPControlDlg::OnBnClickedBtnstart()
{
	if(m_start)
	{
		Stop();

		GetDlgItem(IDC_TxtLocalPort)->EnableWindow(TRUE);
		GetDlgItem(IDC_TxtRemoteIP)->EnableWindow(TRUE);
		GetDlgItem(IDC_TxtRemotePort)->EnableWindow(TRUE);

		GetDlgItem(IDC_BtnStart)->SetWindowText(_T("启动"));
		return;
	}

	UpdateData();
	if(m_nLocalPort <= 1024 || m_nLocalPort > 60000)
	{
		MessageBox(_T("本机接收端口的范围为: 1025-60000"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	if(m_nRemotePort <= 1024 || m_nRemotePort > 60000)
	{
		MessageBox(_T("对端接收端口的范围为: 1025-60000"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	if(m_strRemoteIP.IsEmpty())
	{
		MessageBox(_T("对端IP不能为空"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	std::vector<gtl::tstr> vecIP;
	gtl::tstr((LPCTSTR)m_strRemoteIP).split(vecIP, _T("."));
	if(vecIP.size() != 4)
	{
		MessageBox(_T("对端IP格式错误"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	SaveSetting();
	GetDlgItem(IDC_TxtLocalPort)->EnableWindow(FALSE);
	GetDlgItem(IDC_TxtRemoteIP)->EnableWindow(FALSE);
	GetDlgItem(IDC_TxtRemotePort)->EnableWindow(FALSE);
	GetDlgItem(IDC_BtnStart)->SetWindowText(_T("停止"));

	Start();
}

BOOL CUDPControlDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CUDPControlDlg::SaveSetting()
{
	m_xml.insert_if_not(true);
	m_xml[_T("config")][_T("remote")](_T("ip")) = m_strRemoteIP;
	m_xml[_T("config")][_T("remote")](_T("port")) = gtl::tstr(m_nRemotePort);
	m_xml[_T("config")][_T("local")](_T("port")) = gtl::tstr(m_nLocalPort);
	m_xml[_T("config")](_T("save")) = gtl::tstr(m_bSaveLog);
	m_xml.insert_if_not(false);

	m_xml.save(gtl::io::get_app_path<gtl::tstr>() + _T("config.xml"), _T("utf-8"));
}

BOOL CUDPControlDlg::SysPoweroff()
{
	HANDLE hToken = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	TOKEN_PRIVILEGES tkp = {0};
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if(GetLastError() != ERROR_SUCCESS)
		return FALSE;

	if(!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
		return FALSE;

	return TRUE;
}

BOOL CUDPControlDlg::SysReboot()
{
	HANDLE hToken = NULL;
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

	TOKEN_PRIVILEGES tkp = {0};
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if(GetLastError() != ERROR_SUCCESS)
		return FALSE;

	if(!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
		return FALSE;

	return TRUE;
}

bool CUDPControlDlg::Poweroff(const gtl::str& cmd)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	SetTimer(Timer_Poweroff, vecCmd[1].cast<int>() * 1000, NULL);
	return true;
}

bool CUDPControlDlg::Reboot(const gtl::str& cmd)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	SetTimer(Timer_Reboot, vecCmd[1].cast<int>() * 1000, NULL);
	return true;
}

bool CUDPControlDlg::CancelPoweroff(const gtl::str& cmd)
{
	KillTimer(Timer_Poweroff);
	return true;
}

bool CUDPControlDlg::CancelReboot(const gtl::str& cmd)
{
	KillTimer(Timer_Reboot);
	return true;
}

bool CUDPControlDlg::Cmd(const gtl::str& cmd)
{
	return true;
}

bool CUDPControlDlg::Shortcut(const gtl::str& cmd)
{
	return true;
}

void CUDPControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);

	if(nIDEvent == Timer_Poweroff)
		SysPoweroff();
	else if(nIDEvent == Timer_Reboot)
		SysReboot();

	CDialogEx::OnTimer(nIDEvent);
}
