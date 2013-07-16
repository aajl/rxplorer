#pragma once


// CDialogAbout dialog

class CDialogAbout : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogAbout)

public:
	CDialogAbout(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogAbout();

// Dialog Data
	enum { IDD = IDD_About };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void set_icon(HICON hIcon)
	{
		m_hIcon = hIcon;
	}

protected:
	HICON m_hIcon;
public:
	virtual BOOL OnInitDialog();
};
