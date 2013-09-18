
// LicenseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "License.h"
#include "LicenseDlg.h"
#include "afxdialogex.h"

#include <gtl/io/app_path.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLicenseDlg dialog
CLicenseDlg::CLicenseDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLicenseDlg::IDD, pParent)
	, m_index(0)
	, m_count(0)
	, m_month(0)
	, m_date(FALSE)
	, m_strFlags(_T(""))
	, m_prodcut(_T(""))
	, m_strLicenses(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLicenseDlg::~CLicenseDlg()
{
	m_xml.insert_if_not(true, true);
	m_xml[_T("license")][_T("config")](_T("product")) = m_prodcut;

	m_xml.save(gtl::io::get_app_path<gtl::tstr>() + _T("config.xml"));
}

void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CmbProducts, m_cmbProducts);
	DDX_Text(pDX, IDC_TxtIndex, m_index);
	DDX_Text(pDX, IDC_TxtCount, m_count);
	DDX_Text(pDX, IDC_TxtMonth, m_month);
	DDX_Check(pDX, IDC_ChkDate, m_date);
	DDX_Control(pDX, IDC_CmbFlags, m_cmbFlags);
	DDX_CBString(pDX, IDC_CmbFlags, m_strFlags);
	DDX_CBString(pDX, IDC_CmbProducts, m_prodcut);
	DDX_Text(pDX, IDC_TxtLicense, m_strLicenses);
}

BEGIN_MESSAGE_MAP(CLicenseDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BtnDetect, &CLicenseDlg::OnBnClickedBtndetect)
	ON_BN_CLICKED(IDC_BtnGenerate, &CLicenseDlg::OnBnClickedBtngenerate)
	ON_CBN_SELCHANGE(IDC_CmbProducts, &CLicenseDlg::OnCbnSelchangeCmbproducts)
	ON_CBN_SELCHANGE(IDC_CmbFlags, &CLicenseDlg::OnCbnSelchangeCmbflags)
END_MESSAGE_MAP()


// CLicenseDlg message handlers

BOOL CLicenseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	GetWindowText(m_title.resize(512), 512);

	// xml配置文件格式:
	//<license>
	//	<config product="Explorer+" />
	//	<products>
	//		<product name="Explorer+" key="156" index="5">
	//			<license desc="零售版" key="157" flag="11" index="10" />
	//			<license desc="试用版" key="158" flag="12" month="3" date="1" />
	//		</product>
	//		<product name="UDP-SPC" />
	//	</products>
	//</license>
	//
	// 说明: 如果license节点的key和index节点为空,则使用product节点的值.
	//		 保存时,如果找到了对应的license节点,则index保存在license节点,否则保存在product节点.

	if(m_xml.load(gtl::io::get_app_path<gtl::tstr>() + _T("config.xml")))
	{
		m_prodcut = m_xml[_T("license")][_T("config")](_T("product"));
		const gtl::xml::nodes_type& products = m_xml[_T("license")][_T("products")];
		for(auto first = products.begin(), last = products.end(); first != last; ++first)
		{
			gtl::xml& prdct = *(*first);
			const gtl::tstr& name = prdct(_T("name"));
			if(name.empty())
				continue;

			// 读取product节点信息
			m_products[name].key = prdct(_T("key"), false);
			m_products[name].index = prdct(_T("index"), false);
			m_cmbProducts.AddString(name);

			const gtl::xml::nodes_type& licenses = prdct.nodes();
			for(auto ite = licenses.begin(); ite != licenses.end(); ++ite)
			{
				gtl::xml& lcns = *(*ite);

				license licns;
				licns.desc = lcns(_T("desc"));
				licns.flags = lcns(_T("flag"), false);
				licns.month = lcns(_T("month"), false);
				licns.include_date = lcns(_T("date"));

				gtl::tstr_warp key = lcns(_T("key"), false);
				licns.key = key.empty() ? m_products[name].key : key.cast<uint8>();

				gtl::tstr_warp index = lcns(_T("index"), false);
				licns.index = index.empty() ? m_products[name].index : index.cast<uint8>();

				m_products[name].licns.push_back(licns);
			}
		}

		if(!m_prodcut.IsEmpty())
		{
			m_cmbProducts.SetCurSel(m_cmbProducts.FindString(0, m_prodcut));
			OnCbnSelchangeCmbproducts();
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLicenseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLicenseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLicenseDlg::OnCbnSelchangeCmbproducts()
{
	UpdateData();
	if(m_prodcut.IsEmpty())
		return;

	m_cmbFlags.ResetContent();
	gtl::tstr product = (LPCTSTR)m_prodcut;
	auto& licns = m_products[product].licns;
	for(auto first = licns.begin(), last = licns.end(); first != last; ++first)
	{
		m_cmbFlags.AddString((*first).desc);
	}

	m_cmbFlags.SetCurSel(0);

	m_strFlags.Empty();
	m_index = m_products[product].index;
	UpdateData(FALSE);

	SetWindowText(gtl::tstr(m_title) << _T("  (key: ") << m_products[product].key << _T(")"));
}

const CLicenseDlg::license* CLicenseDlg::GetLicenseInfo(const CString& flags)
{
	if(flags.IsEmpty())
		return NULL;

	gtl::tstr product = (LPCTSTR)m_prodcut;
	auto& licns = m_products[product].licns;
	for(auto first = licns.begin(), last = licns.end(); first != last; ++first)
	{
		const license& licns = (*first);
		if(licns.desc == (LPCTSTR)flags)
			return &licns;
	}

	return NULL;
}

void CLicenseDlg::OnCbnSelchangeCmbflags()
{
	int index = m_cmbFlags.GetCurSel();
	if(index < 0)
		return;

	m_cmbFlags.GetLBText(index, m_strFlags);
	const license* licns = GetLicenseInfo(m_strFlags);
	if(licns == NULL)
		return;

	m_month = licns->month;
	GetDlgItem(IDC_TxtMonth)->EnableWindow(m_month == 0);

	m_date = licns->include_date.empty() ? 0 : licns->include_date.cast<int>();
	GetDlgItem(IDC_ChkDate)->EnableWindow(licns->include_date.empty() ? TRUE : FALSE);

	m_index = licns->index;
	UpdateData(FALSE);

	SetWindowText(gtl::tstr(m_title) << _T("  (key: ") << licns->key << _T(")"));
}

void CLicenseDlg::OnBnClickedBtndetect()
{
	// TODO: Add your control notification handler code here
}

void CLicenseDlg::OnBnClickedBtngenerate()
{
	if(m_products.empty())
		return;

	UpdateData();

	const license* licns = GetLicenseInfo(m_strFlags);
	uint8 key = m_products[(LPCTSTR)m_prodcut].key;
	if(licns != NULL)
		key = licns->key;

	uint16 flags = gtl::tstr_warp((LPCTSTR)m_strFlags).cast<uint16>();
	if(licns != NULL)
		flags = licns->flags;

	m_strLicenses.Empty();
	UpdateData(FALSE);

	for(short index = m_index; index < m_index + m_count; ++index)
	{
		gtl::tstr lcns = m_license.generate(index, key, flags, m_month, m_date);
		m_strLicenses += lcns + _T("\r\n");
	}

	UpdateData(FALSE);

	const gtl::xml::nodes_type& products = m_xml[_T("license")][_T("products")];
	for(auto first = products.begin(), last = products.end(); first != last; ++first)
	{
		gtl::xml& prdct = *(*first);
		const gtl::tstr& name = prdct(_T("name"));
		if(name == (LPCTSTR)m_prodcut)
		{
			if(licns != NULL)
			{
				const gtl::xml::nodes_type& licenses = prdct.nodes();
				for(auto ite = licenses.begin(); ite != licenses.end(); ++ite)
				{
					gtl::xml& lcns = *(*ite);
					if(lcns(_T("desc")) == licns->desc)
					{
						lcns.insert_if_not(true);
						lcns(_T("index")) = gtl::tstr(m_index + m_count);
						lcns.insert_if_not(false);
						return;
					}
				}
			}

			prdct.insert_if_not(true);
			prdct(_T("index")) = gtl::tstr(m_index + m_count);
			prdct.insert_if_not(false);
			return;
		}
	}
}
