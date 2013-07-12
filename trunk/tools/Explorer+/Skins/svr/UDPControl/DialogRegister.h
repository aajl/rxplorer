#pragma once

#include <gtl/string/str.h>
#include <gtl/string/tchar.h>
#include <gtl/mpl/fun.h>
#include <gtl/thread/callback.h>
#include <gtl/xml/xml.h>

class CUDPControlDlg;

// CDialogRegister dialog
class CDialogRegister : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogRegister)

public:
	CDialogRegister(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogRegister();

// Dialog Data
	enum { IDD = IDD_Register };

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

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CString m_strLicenseCode;
	CString m_strMachineCode;
	gtl::xml* m_xml;
	HICON m_hIcon;
	CUDPControlDlg* m_dlg;
	gtl::callback<gtl::mpl::fun<void ()>> m_callback;
};
