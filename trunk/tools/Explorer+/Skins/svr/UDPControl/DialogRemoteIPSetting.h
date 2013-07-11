#pragma once

#include <gtl/xml/xml.h>

// CDialogRemoteIPSetting dialog

class CDialogRemoteIPSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogRemoteIPSetting)

public:
	CDialogRemoteIPSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogRemoteIPSetting();

// Dialog Data
	enum { IDD = IDD_RemoteSetting };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	int m_nRemotePort1;
	int m_nRemotePort2;
	int m_nRemotePort3;
	int m_nRemotePort4;
	int m_nRemotePort5;
	CString m_strRemoteIP1;
	CString m_strRemoteIP2;
	CString m_strRemoteIP3;
	CString m_strRemoteIP4;
	CString m_strRemoteIP5;
};
