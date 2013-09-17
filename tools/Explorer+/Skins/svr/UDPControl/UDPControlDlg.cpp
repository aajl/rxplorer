
// UDPControlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "UDPControlDlg.h"
#include "afxdialogex.h"
#include "DialogStartupSetting.h"
#include "DialogRemoteIPSetting.h"
#include "DialogRegister.h"
#include "DialogAbout.h"

#include <gtl/io/path.h>
#include <gtl/string/str.h>
#include <gtl/io/app_path.h>
#include <gtl/io/ostrm.h>
#include <gtl/io/keyboard.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CUDPControlDlg dialog
CUDPControlDlg::CUDPControlDlg(CWnd* pParent /*=NULL*/)
	: CTrayIconDialog(CUDPControlDlg::IDD, pParent)
	, m_nLocalPort(0)
	, m_bSaveLog(FALSE)
	, m_strLog(_T(""))
	, m_strLocalIP(_T(""))
{
	m_start = false;
	m_nTrialTimes = 0;
	m_bAutoStart = false;
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
	DDX_Text(pDX, IDC_TxtLocalPort, m_nLocalPort);
	DDX_Check(pDX, IDC_ChkSaveLog, m_bSaveLog);
	DDX_Text(pDX, IDC_TxtLog, m_strLog);
	DDX_Text(pDX, IDC_LabLocalIP, m_strLocalIP);
}

BEGIN_MESSAGE_MAP(CUDPControlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ChkSaveLog, &CUDPControlDlg::OnBnClickedChksavelog)
	ON_BN_CLICKED(IDC_BtnStart, &CUDPControlDlg::OnBnClickedBtnstart)
	ON_WM_TIMER()
	ON_COMMAND(ID_Setting, &CUDPControlDlg::OnSetting)
	ON_COMMAND(ID_RemoteSetting, &CUDPControlDlg::OnRemotesetting)
	ON_COMMAND(ID_Register, &CUDPControlDlg::OnRegister)
	ON_COMMAND(ID_About, &CUDPControlDlg::OnAbout)
	ON_WM_SYSCOMMAND()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()


// CUDPControlDlg message handlers

BOOL CUDPControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strTitle;
	GetWindowText(strTitle);
	m_strTitle = strTitle;

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CTrayIconDialog::Create(m_hIcon, IDR_Tray, _T("UDP-SPC"));

	m_strConfigFilePath = gtl::path::all_users_app_data() + _T("/uspc");	
	CreateDirectory(m_strConfigFilePath, NULL);
	m_strConfigFilePath += _T("/config.xml");
	m_xml.load(m_strConfigFilePath);

	if(!gtl::net::socket::get_local_ip(m_strLocalIP))
	{
		GetDlgItem(IDC_BtnStart)->EnableWindow(FALSE);
		MessageBox(_T("获取本机IP错误"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return TRUE;
	}

	//m_strLocalIP = gtl::tstr(ip);

	GetRemoteIPs();
	m_nLocalPort = m_xml[_T("config")][_T("local")](_T("port")).cast<int>();
	m_bSaveLog = m_xml[_T("config")](_T("save")).cast<bool>();
	m_bAutoStart = true;
	if(m_nLocalPort <= 0)
	{
		m_bAutoStart = false;
		m_nLocalPort = 11257;
	}

	UpdateData(FALSE);

	gtl::tea tea;
	gtl::tstr license = m_xml[_T("config")](_T("license"));
	license = tea.decrypt(license, _T("udp-spc-key"));

	uint16 flags = 0;
	uint16 month = 0;
	if(!m_license.verify(license, 1, &flags, &month) || month != 12)
	{
		m_nTrialTimes = 10 * 60;
		OnTimer(Timer_Trial);
		SetTimer(Timer_Trial, 1000, NULL);
	}

	if(m_bAutoStart)
		OnBnClickedBtnstart();

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

void CUDPControlDlg::OnNcPaint()
{
	static int i = 2;
	if(m_bAutoStart && i > 0)
	{
		--i;
		ShowWindow(SW_HIDE);
		return;
	}

	CDialogEx::OnNcPaint();
}

void CUDPControlDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	static int i = 0;
	if(m_bAutoStart && i == 0)
	{
		++i;
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}

	return CDialogEx::OnWindowPosChanged(lpwndpos);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CUDPControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUDPControlDlg::OnTrayMsg(UINT nCmdID, UINT nMessage)
{
	if(nCmdID == ID_Exit)
		PostQuitMessage(0);
	else if(nCmdID == ID_ShowMainDialog)
		ShowWindow(SW_SHOW);

	return CTrayIconDialog::OnTrayMsg(nCmdID, nMessage);
}

void CUDPControlDlg::OnBnClickedChksavelog()
{
	// TODO: Add your control notification handler code here
}

static struct CmdInfo
{
	const char* cmd;
	bool (CUDPControlDlg::*fn)(const gtl::str&, gtl::tstr*);
}cmds[] = 
{
	"poweroff:",			&CUDPControlDlg::Poweroff,
	"reboot:",				&CUDPControlDlg::Reboot,
	"cmd:",					&CUDPControlDlg::Cmd,
	"cmdname:",				&CUDPControlDlg::CmdName,
	"ppt:",					&CUDPControlDlg::JumpPPT,
	"src",					&CUDPControlDlg::ScreenSaver,
	"volume+",				&CUDPControlDlg::TurnTheVolumeUp,
	"volume-",				&CUDPControlDlg::TurnTheVolumeDown,
	"ctrl+",				&CUDPControlDlg::Shortcut,
	"shift+",				&CUDPControlDlg::Shortcut,
	"alt+",					&CUDPControlDlg::Shortcut,
	"win+",					&CUDPControlDlg::Shortcut,

	"test",					&CUDPControlDlg::Test,
	"cancel poweroff",		&CUDPControlDlg::CancelPoweroff,
	"cancel reboot",		&CUDPControlDlg::CancelReboot,
	"enter",				&CUDPControlDlg::Shortcut,
	"Backspace",			&CUDPControlDlg::Shortcut,
	"Tab",					&CUDPControlDlg::Shortcut,
	"Pause",				&CUDPControlDlg::Shortcut,
	"CapsLock",				&CUDPControlDlg::Shortcut,
	"Esc",					&CUDPControlDlg::Shortcut,
	"Space",				&CUDPControlDlg::Shortcut,
	"Page Up",				&CUDPControlDlg::Shortcut, 
	"Page Down",			&CUDPControlDlg::Shortcut,
	"End",					&CUDPControlDlg::Shortcut,
	"Home",					&CUDPControlDlg::Shortcut,
	"Left",					&CUDPControlDlg::Shortcut,
	"Up",					&CUDPControlDlg::Shortcut,
	"Right",				&CUDPControlDlg::Shortcut,
	"Down",					&CUDPControlDlg::Shortcut,
	"Insert",				&CUDPControlDlg::Shortcut,
	"Delete",				&CUDPControlDlg::Shortcut,
	"Print Screen",			&CUDPControlDlg::Shortcut,
	"0",					&CUDPControlDlg::Shortcut,
	"1",					&CUDPControlDlg::Shortcut,
	"2",					&CUDPControlDlg::Shortcut,
	"3",					&CUDPControlDlg::Shortcut,
	"4",					&CUDPControlDlg::Shortcut,
	"5",					&CUDPControlDlg::Shortcut,
	"6",					&CUDPControlDlg::Shortcut,
	"7",					&CUDPControlDlg::Shortcut,
	"8",					&CUDPControlDlg::Shortcut,
	"9",					&CUDPControlDlg::Shortcut,
	"A",					&CUDPControlDlg::Shortcut,
	"B",					&CUDPControlDlg::Shortcut,
	"C",					&CUDPControlDlg::Shortcut,
	"D",					&CUDPControlDlg::Shortcut,
	"E",					&CUDPControlDlg::Shortcut,
	"F",					&CUDPControlDlg::Shortcut,
	"G",					&CUDPControlDlg::Shortcut,
	"H",					&CUDPControlDlg::Shortcut,
	"I",					&CUDPControlDlg::Shortcut,
	"J",					&CUDPControlDlg::Shortcut,
	"K",					&CUDPControlDlg::Shortcut,
	"L",					&CUDPControlDlg::Shortcut,
	"M",					&CUDPControlDlg::Shortcut,
	"N",					&CUDPControlDlg::Shortcut,
	"O",					&CUDPControlDlg::Shortcut,
	"P",					&CUDPControlDlg::Shortcut,
	"Q",					&CUDPControlDlg::Shortcut,
	"R",					&CUDPControlDlg::Shortcut,
	"S",					&CUDPControlDlg::Shortcut,
	"T",					&CUDPControlDlg::Shortcut,
	"U",					&CUDPControlDlg::Shortcut,
	"V",					&CUDPControlDlg::Shortcut,
	"W",					&CUDPControlDlg::Shortcut,
	"X",					&CUDPControlDlg::Shortcut,
	"Y",					&CUDPControlDlg::Shortcut,
	"Z",					&CUDPControlDlg::Shortcut,
	"NumLock",				&CUDPControlDlg::Shortcut,
	"Num pad 0",			&CUDPControlDlg::Shortcut,
	"Num pad 1", 			&CUDPControlDlg::Shortcut,
	"Num pad 2", 			&CUDPControlDlg::Shortcut,
	"Num pad 3", 			&CUDPControlDlg::Shortcut,
	"Num pad 4", 			&CUDPControlDlg::Shortcut,
	"Num pad 5", 			&CUDPControlDlg::Shortcut,
	"Num pad 6", 			&CUDPControlDlg::Shortcut,
	"Num pad 7", 			&CUDPControlDlg::Shortcut,
	"Num pad 8", 			&CUDPControlDlg::Shortcut,
	"Num pad 9",			&CUDPControlDlg::Shortcut,
	"F1",					&CUDPControlDlg::Shortcut,
	"F2",					&CUDPControlDlg::Shortcut,
	"F3",					&CUDPControlDlg::Shortcut,
	"F4",					&CUDPControlDlg::Shortcut,
	"F5",					&CUDPControlDlg::Shortcut,
	"F6",					&CUDPControlDlg::Shortcut,
	"F7",					&CUDPControlDlg::Shortcut,
	"F8",					&CUDPControlDlg::Shortcut,
	"F9",					&CUDPControlDlg::Shortcut,
	"F10",					&CUDPControlDlg::Shortcut,
	"F11",					&CUDPControlDlg::Shortcut,
	"F12",					&CUDPControlDlg::Shortcut,
	"\\",					&CUDPControlDlg::Shortcut,
	"-",					&CUDPControlDlg::Shortcut,
	"=",					&CUDPControlDlg::Shortcut,
	"[",					&CUDPControlDlg::Shortcut,
	"]",					&CUDPControlDlg::Shortcut,
	";",					&CUDPControlDlg::Shortcut,
	"\'",					&CUDPControlDlg::Shortcut,
	",",					&CUDPControlDlg::Shortcut,
	".",					&CUDPControlDlg::Shortcut,
	"/",					&CUDPControlDlg::Shortcut,
	"`",					&CUDPControlDlg::Shortcut,
};

void CUDPControlDlg::Start()
{
	m_start = true;
	m_udp.closesocket();
	m_udp.create(m_nLocalPort, SOCK_DGRAM);

	bool bOpt = true;  
	int retsult = setsockopt(m_udp, SOL_SOCKET, SO_REUSEADDR, (char*)&bOpt, sizeof(bOpt));
	if(retsult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		MessageBox(_T("设置重用地址失败"), _T("错误"), MB_OK | MB_ICONINFORMATION);
	}

	retsult = setsockopt(m_udp, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt)); 
	if(retsult == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
		MessageBox(_T("设置广播地址失败,此次启动将不具备广播功能."), _T("错误"), MB_OK | MB_ICONINFORMATION);
	}

	//ip_mreq mreq;
	//memset(&mreq, 0, sizeof(mreq));
	//mreq.imr_interface.S_un.S_addr = INADDR_ANY;
	//mreq.imr_multiaddr.S_un.S_addr = inet_addr("234.234.234.234");
	//retsult = setsockopt(m_udp, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
	//if(retsult == SOCKET_ERROR)
	//{
	//	int err = WSAGetLastError();
	//	//MessageBox(_T("创建多播地址失败,此次启动将不支持局域网广播."), _T("错误"), MB_OK | MB_ICONINFORMATION);
	//}

	typedef bool (CUDPControlDlg::*mem_fn_type)(const gtl::str&, gtl::tstr*);
	auto find_cmd = [](const char* cmd) -> mem_fn_type {

		mem_fn_type fn = NULL;
		for(int i = 0; i < sizeof(cmds) / sizeof(cmds[0]); ++i)
		{
			if(i <= 10 && gtl::str_warp(cmd).icmp(cmds[i].cmd, -1))
			{
				fn = cmds[i].fn;
				break;
			}
			else if(gtl::tstr(cmds[i].cmd).icmp(gtl::tstr(cmd)))
			{
				fn = cmds[i].fn;
				break;
			}
		}

		return fn;
	};

	auto is_valid_termina = [=](const char* ip, ushort port) -> bool {

		for(size_t i = 0; i < this->m_termina.size(); ++i)
		{
			termina& tmn = m_termina[i];
			if(gtl::str_warp(ip) == tmn.ip)
			{
				return true;
			}
		}

		return false;
	};

	m_thread.start([=]() -> unsigned long {

		for(size_t i = 0; i < this->m_termina.size(); ++i)
		{
			termina& tmn = m_termina[i];
			this->m_udp.sendto("online", 6, tmn.ip, tmn.port);
		}

		while(this->m_start)
		{
			ushort port = 0;
			char ip[16] = {0};
			char buf[1024] = {0};
			int ret = this->m_udp.recvfrom(buf, sizeof(buf) / sizeof(buf[0]), ip, port);
			if(ret <= 0)
				continue;

			CUDPControlDlg* pThis = this;
			CTime time = CTime::GetCurrentTime();
			bool valid_termina = is_valid_termina(ip, port);
			
			gtl::tstr log;
			log << (LPCTSTR)time.Format(_T("%Y-%m-%d %H:%M:%S\r\n"));
			if(!valid_termina)
				log << "[错误: 未知终端] ";
			log << (const char*)ip << ":" << port;
			log << "    [" << (const char*)buf << "]" << "\r\n";
			m_strLog += log;

			if(!valid_termina)
			{
				gtl::str cmd;
				this->m_callback.call(cmd, (mem_fn_type)NULL, log, [=](const gtl::str& cmd, bool (CUDPControlDlg::*fn)(const gtl::str&), const gtl::tstr& log) {
					pThis->UpdateData(FALSE);
				});

				continue;
			}

			mem_fn_type fn = find_cmd(buf);
			if(fn == NULL)
			{
				this->m_udp.sendto("error", 3, ip, port);
				continue;
			}

			gtl::str cmd = buf;
			gtl::str ipaddr = ip;
			this->m_callback.call(cmd, fn, log, [=](const gtl::str& cmd, bool (CUDPControlDlg::*fn)(const gtl::str&), const gtl::tstr& log) {

				if(fn != NULL && (pThis->*fn)(cmd))
					pThis->m_udp.sendto("ack", 3, ipaddr, port);
				else
					pThis->m_udp.sendto("error", 3, ipaddr, port);

				pThis->UpdateData(FALSE);
			});
		}

		gtl::dout << "停止\n";

		return 0;
	});
}

void CUDPControlDlg::Stop()
{
	for(size_t i = 0; i < this->m_termina.size(); ++i)
	{
		termina& tmn = m_termina[i];
		this->m_udp.sendto("offline", 7, tmn.ip, tmn.port);
	}

	m_start = false;
	m_udp.closesocket();
}

void CUDPControlDlg::OnBnClickedBtnstart()
{
	if(m_start)
	{
		Stop();
		UpdateData();

		CTime time = CTime::GetCurrentTime();

		gtl::tstr path = gtl::io::get_app_path<gtl::tstr>() + _T("Log/");
		CreateDirectory(path, NULL);
		path += (LPCTSTR)time.Format(_T("%Y-%m-%d %H.%M.log"));
		gtl::file file;
		if(m_bSaveLog && !m_strLog.IsEmpty() && file.open(path, _T("w")))
		{
			file.write_str((LPTSTR)(LPCTSTR)m_strLog);
			m_strLog.Empty();
			UpdateData(FALSE);
		}	

		GetDlgItem(IDC_TxtLocalPort)->EnableWindow(TRUE);
		GetDlgItem(IDC_BtnStart)->SetWindowText(_T("启动"));
		return;
	}

	UpdateData();
	if(m_nLocalPort <= 1024 || m_nLocalPort > 60000)
	{
		MessageBox(_T("本机接收端口的范围为: 1025-60000"), _T("错误"), MB_OK | MB_ICONINFORMATION);
		return;
	}

	SaveSetting();
	GetDlgItem(IDC_TxtLocalPort)->EnableWindow(FALSE);
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
	m_xml.insert_if_not(true, true);
	m_xml[_T("config")][_T("local")](_T("port")) = gtl::tstr(m_nLocalPort);
	m_xml[_T("config")](_T("save")) = gtl::tstr(m_bSaveLog);
	m_xml.insert_if_not(false, true);

	GetRemoteIPs();

	m_xml.save(m_strConfigFilePath, _T("utf-8"));
}

void CUDPControlDlg::SetRegistered()
{
	KillTimer(Timer_Trial);
	SetWindowText(m_strTitle);
}

void CUDPControlDlg::GetRemoteIPs()
{
	m_termina.clear();

	const gtl::xml::nodes_type& nodes = m_xml[_T("config")][_T("termina")];
	auto first = nodes.begin();
	auto last = nodes.end();
	for(; first != last; ++first)
	{
		gtl::xml& nd = *(*first);

		std::vector<gtl::str> ips;
		gtl::str ip(nd(_T("ip")));
		ushort port = nd(_T("port")).cast<ushort>();
		ip.split(ips, ".");
		if(ips.size() != 4 || port < 1025 || port > 60000)
			continue;

		m_termina.push_back(termina());
		m_termina.back().ip = ip;
		m_termina.back().port = port;
	}
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

bool CUDPControlDlg::Test(const gtl::str& cmd, gtl::tstr* result)
{
	return true;
}

bool CUDPControlDlg::Poweroff(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	SetTimer(Timer_Poweroff, vecCmd[1].cast<int>() * 1000, NULL);
	return true;
}

bool CUDPControlDlg::Reboot(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	SetTimer(Timer_Reboot, vecCmd[1].cast<int>() * 1000, NULL);
	return true;
}

bool CUDPControlDlg::CancelPoweroff(const gtl::str& cmd, gtl::tstr* result)
{
	KillTimer(Timer_Poweroff);
	return true;
}

bool CUDPControlDlg::CancelReboot(const gtl::str& cmd, gtl::tstr* result)
{
	KillTimer(Timer_Reboot);
	return true;
}

bool CUDPControlDlg::Cmd(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	int index = vecCmd[1].cast<int>();
	if(index > 8)
		return false;

	const gtl::tstr& cmdline = m_xml[_T("config")][_T("cmd")][gtl::tstr(_T("cmd")) << index](_T("cmd"));
	if(cmdline.empty())
		return false;

	ShellExecute(NULL, _T("open"), cmdline, NULL, NULL, SW_SHOWMAXIMIZED);
	return true;
}

bool CUDPControlDlg::CmdName(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	int index = vecCmd[1].cast<int>();
	if(index > 8)
		return false;

	const gtl::tstr& cmdline = m_xml[_T("config")][_T("cmd")][gtl::tstr(_T("cmd")) << index](_T("cmd"));
	if(cmdline.empty())
		return false;

	gtl::path path;
	path = cmdline;
	if(result != NULL)
		*result = path.filename();

	return true;
}

bool CUDPControlDlg::Shortcut(const gtl::str& cmd, gtl::tstr* result)
{
	return gtl::keyboard::press(gtl::tstr(cmd));
}

bool CUDPControlDlg::JumpPPT(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, ":");
	if(vecCmd.size() != 2 || vecCmd.size() < 2 || vecCmd[1].cast<int>() <= 0)
		return false;

	const gtl::str& index = vecCmd[1];
	for(int i = 0; i < index.size(); ++i)
	{
		gtl::keyboard::press(gtl::tstr((tchar)index[i]));
		Sleep(100);
	}

	gtl::keyboard::press(_T("enter"));

	return true;
}

bool CUDPControlDlg::TurnTheVolumeUp(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, "+");
	if(vecCmd.empty())
		return false;

	int delta = 1;
	if(vecCmd.size() >= 2)
	{
		delta = vecCmd[1].cast<int>();
		if(delta < 1)
			delta = 1;
	}

	int volume = m_volume.GetVolume();
	if(volume >= 100)
		return true;

	m_volume.SetVolume(volume + delta);
	return true;
}

bool CUDPControlDlg::TurnTheVolumeDown(const gtl::str& cmd, gtl::tstr* result)
{
	std::vector<gtl::str> vecCmd;
	cmd.split(vecCmd, "-");
	if(vecCmd.empty())
		return false;

	int delta = 1;
	if(vecCmd.size() >= 2)
	{
		delta = vecCmd[1].cast<int>();
		if(delta < 1)
			delta = 1;
	}

	int volume = m_volume.GetVolume();
	if(volume <= 0)
		return true;

	m_volume.SetVolume(volume - delta);
	return true;
}

bool CUDPControlDlg::ScreenSaver(const gtl::str& cmd, gtl::tstr* result)
{
	int active = 0;
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, &active, 0);
	if(!active)
		return false;

	int time = 0;
	SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &time, 0);
	SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 1, NULL, 0);
	Sleep(2000);
	SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, time, NULL, 0);
	return true;
}

void CUDPControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == Timer_Poweroff)
	{
		KillTimer(nIDEvent);
		SysPoweroff();
	}
	else if(nIDEvent == Timer_Reboot)
	{
		KillTimer(nIDEvent);
		SysReboot();
	}
	else if(nIDEvent == Timer_Trial)
	{
		SetWindowText(gtl::tstr(m_strTitle) << _T("  试用倒计时: ") << m_nTrialTimes--);
		if(m_nTrialTimes <= 0)
		{
			KillTimer(nIDEvent);
			PostQuitMessage(0);
		}

		return;
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CUDPControlDlg::OnSetting()
{
	CDialogStartupSetting dlg;
	dlg.set_xml(&m_xml);
	dlg.set_dlg(this);
	dlg.set_icon(m_hIcon);
	dlg.DoModal();
}

void CUDPControlDlg::OnRemotesetting()
{
	CDialogRemoteIPSetting dlg;
	dlg.set_xml(&m_xml);
	dlg.set_dlg(this);
	dlg.set_icon(m_hIcon);
	dlg.DoModal();
}

void CUDPControlDlg::OnRegister()
{
	CDialogRegister dlg;
	dlg.set_xml(&m_xml);
	dlg.set_dlg(this);
	dlg.set_icon(m_hIcon);
	dlg.DoModal();
}

void CUDPControlDlg::OnAbout()
{
	CDialogAbout dlg;
	dlg.set_icon(m_hIcon);
	dlg.DoModal();
}

void CUDPControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
	}

	CTrayIconDialogT::OnSysCommand(nID, lParam);
}
