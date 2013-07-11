#pragma once

#include <gtl/xml/xml.h>

// CDialogStartupSetting dialog
class CDialogStartupSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogStartupSetting)

public:
	CDialogStartupSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogStartupSetting();

// Dialog Data
	enum { IDD = IDD_StartupSetting };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnbrowse1();
	afx_msg void OnBnClickedBtnbrowse2();
	afx_msg void OnBnClickedBtnbrowse3();
	afx_msg void OnBnClickedBtnbrowse4();
	afx_msg void OnBnClickedBtnbrowse5();
	afx_msg void OnBnClickedBtnbrowse6();
	afx_msg void OnBnClickedBtnbrowse7();
	afx_msg void OnBnClickedBtnbrowse8();
	afx_msg void OnBnClickedOk();
	CString m_strCmd1;
	CString m_strCmd2;
	CString m_strCmd3;
	CString m_strCmd4;
	CString m_strCmd5;
	CString m_strCmd6;
	CString m_strCmd7;
	CString m_strCmd8;
};
