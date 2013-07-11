
// UDPControlDlg.h : header file
//

#pragma once

#include <gtl/net/socket.h>
#include <gtl/xml/xml.h>
#include <gtl/thread/thread.h>
#include <gtl/thread/callback.h>
#include <gtl/mpl/fun.h>

// CUDPControlDlg dialog
class CUDPControlDlg : public CDialogEx
{
// Construction
public:
	CUDPControlDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CUDPControlDlg();

// Dialog Data
	enum { IDD = IDD_UDPCONTROL_DIALOG };
	enum { Timer_Poweroff, Timer_Reboot };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
	bool Poweroff(const gtl::str& cmd);
	bool Reboot(const gtl::str& cmd);	
	bool CancelPoweroff(const gtl::str& cmd);
	bool CancelReboot(const gtl::str& cmd);
	bool Cmd(const gtl::str& cmd);
	bool Shortcut(const gtl::str& cmd);

// Implementation
protected:
	HICON m_hIcon;
	void SaveSetting();
	void Start();
	void Stop();

	BOOL SysPoweroff();
	BOOL SysReboot();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChksavelog();
	afx_msg void OnBnClickedBtnstart();
	//CString m_strRemoteIP;
	//int m_nRemotePort;
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetting();
	afx_msg void OnRemotesetting();
	afx_msg void OnRegister();
	afx_msg void OnAbout();
};
