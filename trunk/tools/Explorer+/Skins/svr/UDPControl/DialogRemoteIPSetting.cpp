// DialogRemoteIPSetting.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "DialogRemoteIPSetting.h"
#include "afxdialogex.h"


// CDialogRemoteIPSetting dialog

IMPLEMENT_DYNAMIC(CDialogRemoteIPSetting, CDialogEx)

CDialogRemoteIPSetting::CDialogRemoteIPSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogRemoteIPSetting::IDD, pParent)
	, m_strRemoteIP1(_T(""))
	, m_strRemoteIP2(_T(""))
	, m_strRemoteIP3(_T(""))
	, m_strRemoteIP4(_T(""))
	, m_strRemoteIP5(_T(""))
	, m_nRemotePort1(0)
	, m_nRemotePort2(0)
	, m_nRemotePort3(0)
	, m_nRemotePort4(0)
	, m_nRemotePort5(0)
{

}

CDialogRemoteIPSetting::~CDialogRemoteIPSetting()
{
}

void CDialogRemoteIPSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TxtRemotIP1, m_strRemoteIP1);
	DDX_Text(pDX, IDC_TxtRemotIP2, m_strRemoteIP2);
	DDX_Text(pDX, IDC_TxtRemotIP3, m_strRemoteIP3);
	DDX_Text(pDX, IDC_TxtRemotIP4, m_strRemoteIP4);
	DDX_Text(pDX, IDC_TxtRemotIP5, m_strRemoteIP5);
	DDX_Text(pDX, IDC_TxtRemotePort1, m_nRemotePort1);
	DDX_Text(pDX, IDC_TxtRemotePort2, m_nRemotePort2);
	DDX_Text(pDX, IDC_TxtRemotePort3, m_nRemotePort3);
	DDX_Text(pDX, IDC_TxtRemotePort4, m_nRemotePort4);
	DDX_Text(pDX, IDC_TxtRemotePort5, m_nRemotePort5);
}


BEGIN_MESSAGE_MAP(CDialogRemoteIPSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogRemoteIPSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogRemoteIPSetting message handlers


void CDialogRemoteIPSetting::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
