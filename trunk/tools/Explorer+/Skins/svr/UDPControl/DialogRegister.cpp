// DialogRegister.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "DialogRegister.h"
#include "afxdialogex.h"
#include "UDPControlDlg.h"

#include <gtl/crypto/tea.h>
#include <gtl/modules/license.h>
#include <gtl/net/socket.h>

// CDialogRegister dialog

IMPLEMENT_DYNAMIC(CDialogRegister, CDialogEx)

CDialogRegister::CDialogRegister(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogRegister::IDD, pParent)
	, m_strLicenseCode(_T(""))
	, m_strMachineCode(_T(""))
{
	m_dlg = NULL;
	m_xml = NULL;
	m_hIcon = NULL;
}

CDialogRegister::~CDialogRegister()
{
}

void CDialogRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TxtMachineCode, m_strMachineCode);
	DDX_Text(pDX, IDC_TxtLicenseCode, m_strLicenseCode);
}

BEGIN_MESSAGE_MAP(CDialogRegister, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDialogRegister::OnBnClickedOk)
END_MESSAGE_MAP()

// CDialogRegister message handlers
BOOL CDialogRegister::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	gtl::tstr mac;
	gtl::net::socket::get_mac_addr(mac);
	m_strMachineCode = gtl::tstr(mac.hash() % 65535);
	UpdateData(FALSE);

	GetDlgItem(IDC_TxtLicenseCode)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogRegister::OnBnClickedOk()
{
	UpdateData();

	if(m_strLicenseCode.IsEmpty())
	{
		m_callback.call([=] {
			this->MessageBox(_T("×¢²áÂëÎª¿Õ"), _T("´íÎó"), MB_OK | MB_ICONINFORMATION);
		});

		return;
	}

	uint16 flags = 0;
	uint16 month = 0;
	CLicense license;
	uint16 mcode = _ttoi(m_strMachineCode);
	if(!license.verify((LPCTSTR)m_strLicenseCode, 1, &flags, &month) || month != 12 || (flags != 0 && flags != mcode))
	{
		m_callback.call([=] {
			this->MessageBox(_T("·Ç·¨×¢²áÂë"), _T("´íÎó"), MB_OK | MB_ICONINFORMATION);
		});

		return;
	}

	if(m_xml != NULL && m_dlg != NULL)
	{
		gtl::tea tea;
		gtl::xml& xml = *m_xml;
		xml.insert_if_not(true, true);
		xml[_T("config")](_T("license")) = tea.encrypt((LPCTSTR)m_strLicenseCode, _T("udp-spc-key"));
		xml.insert_if_not(false, true);

		m_dlg->SaveSetting();
		m_dlg->SetRegistered();
	}

	CDialogEx::OnOK();
}
