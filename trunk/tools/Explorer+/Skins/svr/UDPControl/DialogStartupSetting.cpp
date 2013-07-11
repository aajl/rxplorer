// DialogStartupSetting.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "DialogStartupSetting.h"
#include "afxdialogex.h"


// CDialogStartupSetting dialog

IMPLEMENT_DYNAMIC(CDialogStartupSetting, CDialogEx)

CDialogStartupSetting::CDialogStartupSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogStartupSetting::IDD, pParent)
	, m_strCmd1(_T(""))
	, m_strCmd2(_T(""))
	, m_strCmd3(_T(""))
	, m_strCmd4(_T(""))
	, m_strCmd5(_T(""))
	, m_strCmd6(_T(""))
	, m_strCmd7(_T(""))
	, m_strCmd8(_T(""))
{

}

CDialogStartupSetting::~CDialogStartupSetting()
{
}

void CDialogStartupSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd1);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd2);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd3);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd4);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd5);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd6);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd7);
	DDX_Text(pDX, IDC_TxtCmd1, m_strCmd8);
}


BEGIN_MESSAGE_MAP(CDialogStartupSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BtnBrowse1, &CDialogStartupSetting::OnBnClickedBtnbrowse1)
	ON_BN_CLICKED(IDC_BtnBrowse2, &CDialogStartupSetting::OnBnClickedBtnbrowse2)
	ON_BN_CLICKED(IDC_BtnBrowse3, &CDialogStartupSetting::OnBnClickedBtnbrowse3)
	ON_BN_CLICKED(IDC_BtnBrowse4, &CDialogStartupSetting::OnBnClickedBtnbrowse4)
	ON_BN_CLICKED(IDC_BtnBrowse5, &CDialogStartupSetting::OnBnClickedBtnbrowse5)
	ON_BN_CLICKED(IDC_BtnBrowse6, &CDialogStartupSetting::OnBnClickedBtnbrowse6)
	ON_BN_CLICKED(IDC_BtnBrowse7, &CDialogStartupSetting::OnBnClickedBtnbrowse7)
	ON_BN_CLICKED(IDC_BtnBrowse8, &CDialogStartupSetting::OnBnClickedBtnbrowse8)
	ON_BN_CLICKED(IDOK, &CDialogStartupSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogStartupSetting message handlers


void CDialogStartupSetting::OnBnClickedBtnbrowse1()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse2()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse3()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse4()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse5()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse6()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse7()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedBtnbrowse8()
{
	// TODO: Add your control notification handler code here
}


void CDialogStartupSetting::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
