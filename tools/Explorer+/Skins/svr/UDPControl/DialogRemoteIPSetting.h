#pragma once

#include <gtl/xml/xml.h>

class CUDPControlDlg;

// CDialogRemoteIPSetting dialog
class CDialogRemoteIPSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogRemoteIPSetting)

public:
	CDialogRemoteIPSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogRemoteIPSetting();

// Dialog Data
	enum { IDD = IDD_RemoteSetting };

	void set_xml(gtl::xml* xml)
	{
		m_xml = xml;
	}

	void set_icon(HICON hIcon)
	{
		m_hIcon = hIcon;
	}
	
	void set_dlg(CUDPControlDlg* dlg)
	{
		m_dlg = dlg;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
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
	gtl::xml* m_xml;
	HICON m_hIcon;
	CUDPControlDlg* m_dlg;
};
