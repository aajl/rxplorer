
// UDPControlDlg.h : header file
//

#pragma once

#include <gtl/net/socket.h>
#include <gtl/xml/xml.h>
#include <gtl/thread/thread.h>
#include <gtl/thread/callback.h>
#include <gtl/mpl/fun.h>
#include <gtl/crypto/tea.h>
#include <gtl/modules/license.h>
#include <gtl/ui/TrayIcon.h>
#include "Volume.h"

// CUDPControlDlg dialog
class CUDPControlDlg
	: public CTrayIconDialogT<CDialogEx>
{
// Construction
public:
	CUDPControlDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CUDPControlDlg();

// Dialog Data
	enum { IDD = IDD_UDPCONTROL_DIALOG };
	enum { Timer_Poweroff, Timer_Reboot, Timer_Trial };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnTrayMsg(UINT nCmdID, UINT nMessage);

public:
	bool Test(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool Poweroff(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool Reboot(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool CancelPoweroff(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool CancelReboot(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool Cmd(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool CmdName(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool Shortcut(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool JumpPPT(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool TurnTheVolumeUp(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool TurnTheVolumeDown(const gtl::str& cmd, gtl::tstr* result = NULL);
	bool ScreenSaver(const gtl::str& cmd, gtl::tstr* result = NULL);

	void SaveSetting();
	void SetRegistered();

// Implementation
protected:
	HICON m_hIcon;
	void Start();
	void Stop();

	BOOL SysPoweroff();
	BOOL SysReboot();
	void GetRemoteIPs();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	struct termina
	{
		gtl::str ip;
		ushort port;
	};

	afx_msg void OnBnClickedChksavelog();
	afx_msg void OnBnClickedBtnstart();
	gtl::tstr m_strTitle;
	int m_nLocalPort;
	BOOL m_bSaveLog;
	CString m_strLog;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_strLocalIP;
	gtl::xml m_xml;
	gtl::net::socket m_udp;
	gtl::thread<> m_thread;
	gtl::callback<gtl::mpl::fun<void (const gtl::str& /*cmd*/, 	bool (CUDPControlDlg::*)(const gtl::str&), const gtl::tstr& /*log*/)>> m_callback;
	bool m_start;
	std::vector<termina> m_termina;
	CLicense m_license;
	int m_nTrialTimes;
	gtl::tstr m_strConfigFilePath;
	bool m_bAutoStart;
	CVolume m_volume;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetting();
	afx_msg void OnRemotesetting();
	afx_msg void OnRegister();
	afx_msg void OnAbout();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
