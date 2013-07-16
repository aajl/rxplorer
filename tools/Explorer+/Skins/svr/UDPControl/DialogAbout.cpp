// DialogAbout.cpp : implementation file
//

#include "stdafx.h"
#include "UDPControl.h"
#include "DialogAbout.h"
#include "afxdialogex.h"


// CDialogAbout dialog

IMPLEMENT_DYNAMIC(CDialogAbout, CDialogEx)

CDialogAbout::CDialogAbout(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDialogAbout::IDD, pParent)
{
	m_hIcon = NULL;
}

CDialogAbout::~CDialogAbout()
{
}

void CDialogAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogAbout, CDialogEx)
END_MESSAGE_MAP()


// CDialogAbout message handlers


BOOL CDialogAbout::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
