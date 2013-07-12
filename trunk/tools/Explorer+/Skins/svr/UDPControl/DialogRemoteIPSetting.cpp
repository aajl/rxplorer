// DialogRemoteIPSetting.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "DialogRemoteIPSetting.h"
#include "afxdialogex.h"
#include "UDPControlDlg.h"

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
	m_dlg = NULL;
	m_xml = NULL;
	m_hIcon = NULL;
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
BOOL CDialogRemoteIPSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	if(m_xml != NULL)
	{
		gtl::xml& xml = *m_xml;
		m_strRemoteIP1 = xml[_T("config")][_T("termina")][_T("ip1")](_T("ip"));
		m_strRemoteIP2 = xml[_T("config")][_T("termina")][_T("ip2")](_T("ip"));
		m_strRemoteIP3 = xml[_T("config")][_T("termina")][_T("ip3")](_T("ip"));
		m_strRemoteIP4 = xml[_T("config")][_T("termina")][_T("ip4")](_T("ip"));
		m_strRemoteIP5 = xml[_T("config")][_T("termina")][_T("ip5")](_T("ip"));

		m_nRemotePort1 = xml[_T("config")][_T("termina")][_T("ip1")](_T("port")).cast<ushort>();
		m_nRemotePort2 = xml[_T("config")][_T("termina")][_T("ip2")](_T("port")).cast<ushort>();
		m_nRemotePort3 = xml[_T("config")][_T("termina")][_T("ip3")](_T("port")).cast<ushort>();
		m_nRemotePort4 = xml[_T("config")][_T("termina")][_T("ip4")](_T("port")).cast<ushort>();
		m_nRemotePort5 = xml[_T("config")][_T("termina")][_T("ip5")](_T("port")).cast<ushort>();

		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDialogRemoteIPSetting::OnBnClickedOk()
{
	UpdateData();

	if(m_xml == NULL)
		return;

	gtl::xml& xml = *m_xml;
	xml.insert_if_not(true, true);
	xml[_T("config")][_T("termina")][_T("ip1")](_T("ip")) = m_strRemoteIP1;
	xml[_T("config")][_T("termina")][_T("ip2")](_T("ip")) = m_strRemoteIP2;
	xml[_T("config")][_T("termina")][_T("ip3")](_T("ip")) = m_strRemoteIP3;
	xml[_T("config")][_T("termina")][_T("ip4")](_T("ip")) = m_strRemoteIP4;
	xml[_T("config")][_T("termina")][_T("ip5")](_T("ip")) = m_strRemoteIP5;

	xml[_T("config")][_T("termina")][_T("ip1")](_T("port")) = gtl::tstr(m_nRemotePort1);
	xml[_T("config")][_T("termina")][_T("ip2")](_T("port")) = gtl::tstr(m_nRemotePort2);
	xml[_T("config")][_T("termina")][_T("ip3")](_T("port")) = gtl::tstr(m_nRemotePort3);
	xml[_T("config")][_T("termina")][_T("ip4")](_T("port")) = gtl::tstr(m_nRemotePort4);
	xml[_T("config")][_T("termina")][_T("ip5")](_T("port")) = gtl::tstr(m_nRemotePort5);
	xml.insert_if_not(false, true);

	if(m_dlg != NULL)
		m_dlg->SaveSetting();

	CDialogEx::OnOK();
}
