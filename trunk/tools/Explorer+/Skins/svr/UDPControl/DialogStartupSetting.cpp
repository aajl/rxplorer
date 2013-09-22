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
	, m_strCmd9(_T(""))
	, m_strCmd10(_T(""))
	, m_strCmd11(_T(""))
	, m_strCmd12(_T(""))
	, m_strCmd13(_T(""))
	, m_strCmd14(_T(""))
	, m_strCmd15(_T(""))
	, m_strCmd16(_T(""))
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
	DDX_Text(pDX, IDC_TxtCmd2, m_strCmd2);
	DDX_Text(pDX, IDC_TxtCmd3, m_strCmd3);
	DDX_Text(pDX, IDC_TxtCmd4, m_strCmd4);
	DDX_Text(pDX, IDC_TxtCmd5, m_strCmd5);
	DDX_Text(pDX, IDC_TxtCmd6, m_strCmd6);
	DDX_Text(pDX, IDC_TxtCmd7, m_strCmd7);
	DDX_Text(pDX, IDC_TxtCmd8, m_strCmd8);
	DDX_Text(pDX, IDC_TxtCmd9, m_strCmd9);
	DDX_Text(pDX, IDC_TxtCmd10, m_strCmd10);
	DDX_Text(pDX, IDC_TxtCmd11, m_strCmd11);
	DDX_Text(pDX, IDC_TxtCmd12, m_strCmd12);
	DDX_Text(pDX, IDC_TxtCmd13, m_strCmd13);
	DDX_Text(pDX, IDC_TxtCmd14, m_strCmd14);
	DDX_Text(pDX, IDC_TxtCmd15, m_strCmd15);
	DDX_Text(pDX, IDC_TxtCmd16, m_strCmd16);
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
	ON_BN_CLICKED(IDC_BtnBrowse9, &CDialogStartupSetting::OnBnClickedBtnbrowse9)
	ON_BN_CLICKED(IDC_BtnBrowse10, &CDialogStartupSetting::OnBnClickedBtnbrowse10)
	ON_BN_CLICKED(IDC_BtnBrowse11, &CDialogStartupSetting::OnBnClickedBtnbrowse11)
	ON_BN_CLICKED(IDC_BtnBrowse12, &CDialogStartupSetting::OnBnClickedBtnbrowse12)
	ON_BN_CLICKED(IDC_BtnBrowse13, &CDialogStartupSetting::OnBnClickedBtnbrowse13)
	ON_BN_CLICKED(IDC_BtnBrowse14, &CDialogStartupSetting::OnBnClickedBtnbrowse14)
	ON_BN_CLICKED(IDC_BtnBrowse15, &CDialogStartupSetting::OnBnClickedBtnbrowse15)
	ON_BN_CLICKED(IDC_BtnBrowse16, &CDialogStartupSetting::OnBnClickedBtnbrowse16)
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
		m_strCmd2 = xml[_T("config")][_T("cmd")][_T("cmd2")](_T("cmd"));
		m_strCmd3 = xml[_T("config")][_T("cmd")][_T("cmd3")](_T("cmd"));
		m_strCmd4 = xml[_T("config")][_T("cmd")][_T("cmd4")](_T("cmd"));
		m_strCmd5 = xml[_T("config")][_T("cmd")][_T("cmd5")](_T("cmd"));
		m_strCmd6 = xml[_T("config")][_T("cmd")][_T("cmd6")](_T("cmd"));
		m_strCmd7 = xml[_T("config")][_T("cmd")][_T("cmd7")](_T("cmd"));
		m_strCmd8 = xml[_T("config")][_T("cmd")][_T("cmd8")](_T("cmd"));
		m_strCmd9 = xml[_T("config")][_T("cmd")][_T("cmd9")](_T("cmd"));
		m_strCmd10 = xml[_T("config")][_T("cmd")][_T("cmd10")](_T("cmd"));
		m_strCmd11 = xml[_T("config")][_T("cmd")][_T("cmd11")](_T("cmd"));
		m_strCmd12 = xml[_T("config")][_T("cmd")][_T("cmd12")](_T("cmd"));
		m_strCmd13 = xml[_T("config")][_T("cmd")][_T("cmd13")](_T("cmd"));
		m_strCmd14 = xml[_T("config")][_T("cmd")][_T("cmd14")](_T("cmd"));
		m_strCmd15 = xml[_T("config")][_T("cmd")][_T("cmd15")](_T("cmd"));
		m_strCmd16 = xml[_T("config")][_T("cmd")][_T("cmd16")](_T("cmd"));

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
	xml[_T("config")][_T("cmd")][_T("cmd9")](_T("cmd")) = m_strCmd9;
	xml[_T("config")][_T("cmd")][_T("cmd10")](_T("cmd")) = m_strCmd10;
	xml[_T("config")][_T("cmd")][_T("cmd11")](_T("cmd")) = m_strCmd11;
	xml[_T("config")][_T("cmd")][_T("cmd12")](_T("cmd")) = m_strCmd12;
	xml[_T("config")][_T("cmd")][_T("cmd13")](_T("cmd")) = m_strCmd13;
	xml[_T("config")][_T("cmd")][_T("cmd14")](_T("cmd")) = m_strCmd14;
	xml[_T("config")][_T("cmd")][_T("cmd15")](_T("cmd")) = m_strCmd15;
	xml[_T("config")][_T("cmd")][_T("cmd16")](_T("cmd")) = m_strCmd16;
	xml.insert_if_not(false, true);

	if(m_dlg != NULL)
		m_dlg->SaveSetting();

	CDialogEx::OnOK();
}


void CDialogStartupSetting::OnBnClickedBtnbrowse9()
{
	SelectFile(m_strCmd9);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse10()
{
	SelectFile(m_strCmd10);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse11()
{
	SelectFile(m_strCmd11);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse12()
{
	SelectFile(m_strCmd12);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse13()
{
	SelectFile(m_strCmd13);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse14()
{
	SelectFile(m_strCmd14);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse15()
{
	SelectFile(m_strCmd15);
}


void CDialogStartupSetting::OnBnClickedBtnbrowse16()
{
	SelectFile(m_strCmd16);
}
