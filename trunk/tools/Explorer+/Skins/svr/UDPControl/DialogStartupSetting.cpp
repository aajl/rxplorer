// DialogStartupSetting.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "DialogStartupSetting.h"
#include "afxdialogex.h"
#include "UDPControlDlg.h"

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
	m_dlg = NULL;
	m_xml = NULL;
	m_hIcon = NULL;
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
BOOL CDialogStartupSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	if(m_xml != NULL)
	{
		gtl::xml& xml = *m_xml;

		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd1")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd2")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd3")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd4")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd5")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd6")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd7")](_T("cmd"));
		m_strCmd1 = xml[_T("config")][_T("cmd")][_T("cmd8")](_T("cmd"));

		UpdateData(FALSE);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDialogStartupSetting::SelectFile(CString& strFilepath)
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("所有文件(*.*)|*.*||"));
	if(dlg.DoModal() != IDOK)
		return;

	strFilepath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse1()
{
	SelectFile(m_strCmd1);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse2()
{
	SelectFile(m_strCmd2);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse3()
{
	SelectFile(m_strCmd3);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse4()
{
	SelectFile(m_strCmd4);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse5()
{
	SelectFile(m_strCmd5);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse6()
{
	SelectFile(m_strCmd6);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse7()
{
	SelectFile(m_strCmd7);
}

void CDialogStartupSetting::OnBnClickedBtnbrowse8()
{
	SelectFile(m_strCmd8);
}

void CDialogStartupSetting::OnBnClickedOk()
{
	UpdateData();

	if(m_xml == NULL)
		return;

	gtl::xml& xml = *m_xml;
	xml.insert_if_not(true, true);
	xml[_T("config")][_T("cmd")][_T("cmd1")](_T("cmd")) = m_strCmd1;
	xml[_T("config")][_T("cmd")][_T("cmd2")](_T("cmd")) = m_strCmd2;
	xml[_T("config")][_T("cmd")][_T("cmd3")](_T("cmd")) = m_strCmd3;
	xml[_T("config")][_T("cmd")][_T("cmd4")](_T("cmd")) = m_strCmd4;
	xml[_T("config")][_T("cmd")][_T("cmd5")](_T("cmd")) = m_strCmd5;
	xml[_T("config")][_T("cmd")][_T("cmd6")](_T("cmd")) = m_strCmd6;
	xml[_T("config")][_T("cmd")][_T("cmd7")](_T("cmd")) = m_strCmd7;
	xml[_T("config")][_T("cmd")][_T("cmd8")](_T("cmd")) = m_strCmd8;
	xml.insert_if_not(false, true);

	if(m_dlg != NULL)
		m_dlg->SaveSetting();

	CDialogEx::OnOK();
}
