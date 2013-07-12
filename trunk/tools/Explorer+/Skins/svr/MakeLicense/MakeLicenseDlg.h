
// MakeLicenseDlg.h : header file
//

#pragma once


// CMakeLicenseDlg dialog
class CMakeLicenseDlg : public CDialogEx
{
// Construction
public:
	CMakeLicenseDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MAKELICENSE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	int m_nID;
	int m_nMachineCode;
	CString m_strLicenseCode;
};
