
// LicenseDlg.h : header file
//

#pragma once

#include <map>
#include <gtl/modules/license.h>
#include <gtl/xml/xml.h>
#include "afxwin.h"

// CLicenseDlg dialog
class CLicenseDlg : public CDialogEx
{
public:
	struct license
	{
		uint8 key;
		uint16 flags;
		uint16 month;
		uint16 index;
		gtl::tstr desc;
		gtl::tstr include_date;
	};

// Construction
public:
	CLicenseDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CLicenseDlg();

// Dialog Data
	enum { IDD = IDD_LICENSE_DIALOG };

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
	afx_msg void OnBnClickedBtndetect();
	afx_msg void OnBnClickedBtngenerate();
	afx_msg void OnCbnSelchangeCmbproducts();
	afx_msg void OnCbnSelchangeCmbflags();

protected:
	const license* GetLicenseInfo(const CString& flags);

protected:
	struct product 
	{
		uint8 key;
		uint16 index;
		std::vector<license> licns;
	};

	gtl::xml m_xml;
	CLicense m_license;
	std::map<gtl::tstr, product> m_products;

public:
	CComboBox m_cmbProducts;
	short m_index;
	short m_count;
	short m_month;
	BOOL m_date;
	CComboBox m_cmbFlags;
	CString m_strFlags;
	CString m_prodcut;
	CString m_strLicenses;
	gtl::tstr m_title;
};
