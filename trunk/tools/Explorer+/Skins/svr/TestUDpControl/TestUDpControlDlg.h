
// TestUDpControlDlg.h : header file
//

#pragma once

#include <gtl/net/socket.h>
#include <gtl/thread/thread.h>
#include <gtl/thread/callback.h>
#include <gtl/mpl/fun.h>
#include <gtl/string/str.h>

// CTestUDpControlDlg dialog
class CTestUDpControlDlg : public CDialogEx
{
// Construction
public:
	CTestUDpControlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTUDPCONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strSendText;
	afx_msg void OnBnClickedButton1();
	gtl::net::socket m_udp;
	int m_nLocalPort;
	CString m_strRemoveIP;
	int m_nRemovePort;
	CString m_strRecvText;
	gtl::thread<> m_thread;
	gtl::callback<gtl::mpl::fun<void (const gtl::tstr& /*cmd*/)>> m_callback;
};
