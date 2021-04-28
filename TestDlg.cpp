/* 
 *  Filename:    TestDlg.cpp
 *  Description: Group Combo Box demo dialog
 *  Copyright:   Julijan Sribar, 2012-2013
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author(s) be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "stdafx.h"
#include "TestApplication.h"
#include "TestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg dialog


CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ORDINARY, m_comboBox);
	DDX_Control(pDX, IDC_COMBO_OD_FIXED, m_comboBoxODFixed);
	DDX_Control(pDX, IDC_COMBO_OD_VARIABLE, m_comboBoxODVariable);
	DDX_Control(pDX, IDC_AUTOWIDTH_COMBO_OD_FIXED, m_autowidthComboODFixed);
	DDX_Control(pDX, IDC_AUTOWIDTH_COMBO_OD_VARIABLE, m_autowidthComboODVariable);
	DDX_Control(pDX, IDC_AUTOCOMPLETE_COMBO_OD_FIXED, m_autocompleteComboODFixed);
	DDX_Control(pDX, IDC_AUTOCOMPLETE_COMBO_OD_VARIABLE, m_autocompleteComboODVariable);
	DDX_Control(pDX, IDC_CANCELWESCAPE_COMBO_OD_FIXED, m_undoByEscComboODFixed);
	DDX_Control(pDX, IDC_CANCELWESCAPE_COMBO_OD_VARIABLE, m_undoByEscComboODVariable);
	DDX_Control(pDX, IDC_COMBO_OD_FIXED2, m_comboBoxODFixed2);
	DDX_Control(pDX, IDC_COMBO_OD_COUNTRIES, m_comboCountries);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ADD_RECENT_OD_FIXED, &CTestDlg::OnBnClickedButtonAddRecentOdFixed)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_RECENT_OD_FIXED, &CTestDlg::OnBnClickedButtonRemoveRecentOdFixed)
	ON_BN_CLICKED(IDC_AUTOWIDTH_COMBO_OD_FIXED, &CTestDlg::OnBnClickedAutowidthComboOdFixed)
	ON_BN_CLICKED(IDC_AUTOWIDTH_COMBO_OD_VARIABLE, &CTestDlg::OnBnClickedAutowidthComboOdVariable)
	ON_BN_CLICKED(IDC_AUTOCOMPLETE_COMBO_OD_FIXED, &CTestDlg::OnBnClickedAutocompleteComboOdFixed)
	ON_BN_CLICKED(IDC_AUTOCOMPLETE_COMBO_OD_VARIABLE, &CTestDlg::OnBnClickedAutocompleteComboOdVariable)
	ON_BN_CLICKED(IDC_CANCELWESCAPE_COMBO_OD_FIXED, &CTestDlg::OnBnClickedCancelwescapeComboOdFixed)
	ON_BN_CLICKED(IDC_CANCELWESCAPE_COMBO_OD_VARIABLE, &CTestDlg::OnBnClickedCancelwescComboOdVariable)
	ON_BN_CLICKED(IDC_BUTTON_ADD_RECENT_OD_VARIABLE, &CTestDlg::OnBnClickedButtonAddRecentOdVariable)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_RECENT_OD_VARIABLE, &CTestDlg::OnBnClickedButtonRemoveRecentOdVariable)
END_MESSAGE_MAP()


// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_autowidthComboODFixed.SetCheck(BST_CHECKED);
	m_autocompleteComboODFixed.SetCheck(BST_CHECKED);
	m_undoByEscComboODFixed.SetCheck(BST_CHECKED);
	m_autowidthComboODVariable.SetCheck(BST_CHECKED);
	m_autocompleteComboODVariable.SetCheck(BST_CHECKED);
	m_undoByEscComboODVariable.SetCheck(BST_CHECKED);

	//m_comboBoxODFixed2.SetItemHeight(-1, 15);
	m_comboBoxODFixed2.SetItemHeight(0, 20);

	FillOrdinaryComboBox();
	FillODFixedComboBox();

	m_fontTypes.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);
	m_fontTypes.Add(AfxGetApp()->LoadIcon(IDI_TRUETYPE));
	m_fontTypes.Add(AfxGetApp()->LoadIcon(IDI_OPENTYPE));
	m_fontTypes.Add(AfxGetApp()->LoadIcon(IDI_PRINTER));

	FillODVariableComboBox();

	FillComboBoxCountries();
	m_comboCountries.SetItemHeight(0, 18);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint()
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
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestDlg::FillOrdinaryComboBox()
{
	m_comboBox.AddString(_T("John Lennon"));
	m_comboBox.AddString(_T("Paul McCartney"));
	m_comboBox.AddString(_T("George Harrison"));
	m_comboBox.AddString(_T("Ringo Star"));

	m_comboBox.AddString(_T("Eric Burdon"));
	m_comboBox.AddString(_T("Hilton Valentine"));
	m_comboBox.AddString(_T("Alan Price"));
	m_comboBox.AddString(_T("Chas Chandler"));
	m_comboBox.AddString(_T("John Steel"));

	m_comboBox.AddString(_T("Ray Davies"));
	m_comboBox.AddString(_T("Dave Davies"));
	m_comboBox.AddString(_T("Mick Avory"));
	m_comboBox.AddString(_T("John Dalton"));

	m_comboBox.AddString(_T("Jimmy Page"));
	m_comboBox.AddString(_T("John Bonham"));
	m_comboBox.AddString(_T("John Paul Jones"));
	m_comboBox.AddString(_T("Robert Plant"));

	m_comboBox.AddString(_T("Pete Townshend"));
	m_comboBox.AddString(_T("Roger Daltrey"));
	m_comboBox.AddString(_T("John Entwistle"));
	m_comboBox.AddString(_T("Keith Moon"));

	m_comboBox.AddString(_T("Mick Jagger"));
	m_comboBox.AddString(_T("Keith Richards"));
	m_comboBox.AddString(_T("Bill Wyman"));
	m_comboBox.AddString(_T("Ronnie Wood"));
	m_comboBox.AddString(_T("Brain Jones"));
	m_comboBox.AddString(_T("Charlie Watts"));

	m_comboBox.AddString(_T("Bob Dylan"));
	m_comboBox.AddString(_T("John Lee Hooker"));
	m_comboBox.AddString(_T("Muddy Waters"));
	m_comboBox.AddString(_T("Chuck Berry"));
	m_comboBox.AddString(_T("Jimmy Hendrix"));
	m_comboBox.AddString(_T("Donovan"));
}

void CTestDlg::FillODFixedComboBox()
{
	int group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("Recent items")));

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("The Animals"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Eric Burdon")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Hilton Valentine")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Alan Price")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Chas Chandler")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("John Steel")), group);

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("The Beatles"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("John Lennon")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Paul McCartney")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("George Harrison")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Ringo Star")), group);

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("The Kinks"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Ray Davies")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Dave Davies")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Mick Avory")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("John Dalton")), group);

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("Led Zeppelin"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Jimmy Page")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("John Bonham")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("John Paul Jones")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Robert Plant")), group);

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("The Who"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Pete Townshend")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Roger Daltrey")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("John Entwistle")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Keith Moon")), group);

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("The Rolling Stones"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Mick Jagger")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Keith Richards")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Bill Wyman")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Ronnie Wood")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Brian Jones")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Charlie Watts")), group);

	group = m_comboBoxODFixed.AddGroup(new CGroupComboBoxHeader(_T("Others"), true));
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Bob Dylan")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Chuch Berry")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Donovan")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Muddy Waters")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Jimmy Hendrix")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Van Morrison")), group);
	m_comboBoxODFixed.AddItem(new CGroupComboBoxSimpleItem(_T("Country Joe and the Fish")), group);
}

#include <map>
#include <set>
#include <algorithm>

#define FONTS_GROUP_RASTER _T("Raster Fonts")
#define FONTS_GROUP_DEVICE _T("Device Fonts")
#define FONTS_GROUP_TRUETYPE _T("True Type Fonts")
#define FONTS_GROUP_OPENTYPE _T("Open Type Fonts")

int CALLBACK EnumFontFamiliesExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	std::map<CString, std::set<CString>>* fonts = reinterpret_cast<std::map<CString, std::set<CString>>*>(lParam);
	CString fontGroup;
	switch (FontType)
	{
	case RASTER_FONTTYPE:
		fontGroup = FONTS_GROUP_RASTER;
		break;
	case DEVICE_FONTTYPE:
		fontGroup = FONTS_GROUP_DEVICE;
		break;
	case TRUETYPE_FONTTYPE:
		if ((lpntme->ntmTm.ntmFlags & NTM_TT_OPENTYPE) == NTM_TT_OPENTYPE)
			fontGroup = FONTS_GROUP_OPENTYPE;
		else
			fontGroup = FONTS_GROUP_TRUETYPE;
		break;
	default:
		TRACE(_T("Unknown font type: %s\n"), lpelfe->elfFullName);
		return 1;
	}
	int nameOffset = (lpelfe->elfFullName[0] == _T('@')) ? 1 : 0;
	CString fontname(lpelfe->elfFullName + nameOffset);
	(*fonts)[fontGroup].insert(fontname);
	return 1;
}

void CTestDlg::FillODVariableComboBox()
{
	std::map<CString, std::set<CString>> fonts;

	LOGFONT lf = { 0 };
	lf.lfCharSet = DEFAULT_CHARSET;
	::EnumFontFamiliesEx(GetDC()->m_hDC, &lf, (FONTENUMPROC)EnumFontFamiliesExProc, (LPARAM)(&fonts), 0);

	int groupIndex = m_comboBoxODVariable.AddGroup(_T("Recently used fonts"));
	m_comboBoxODFixed2.AddGroup(_T("Dummy"));

	groupIndex = m_comboBoxODVariable.AddGroup(FONTS_GROUP_RASTER, true);
	m_comboBoxODFixed2.AddGroup(FONTS_GROUP_RASTER, true);
	std::for_each(fonts[FONTS_GROUP_RASTER].begin(), fonts[FONTS_GROUP_RASTER].end(), [this, &groupIndex](const CString& fontName) { 
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = -14;
		lf.lfWeight = FW_NORMAL;
		_tcscpy_s(lf.lfFaceName, fontName);
		m_comboBoxODVariable.AddItem(&lf, groupIndex);
		m_comboBoxODFixed2.AddItem(&lf, groupIndex);
	} );

	groupIndex = m_comboBoxODVariable.AddGroup(FONTS_GROUP_DEVICE, true);
	m_comboBoxODFixed2.AddGroup(FONTS_GROUP_DEVICE, true);
	std::for_each(fonts[FONTS_GROUP_DEVICE].begin(), fonts[FONTS_GROUP_DEVICE].end(), [this, &groupIndex](const CString& fontName) { 
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = -14;
		lf.lfWeight = FW_NORMAL;
		_tcscpy_s(lf.lfFaceName, fontName);
		m_comboBoxODVariable.AddItem(&lf, &m_fontTypes, 2, groupIndex);
		m_comboBoxODFixed2.AddItem(&lf, &m_fontTypes, 2, groupIndex);
	} );

	groupIndex = m_comboBoxODVariable.AddGroup(FONTS_GROUP_TRUETYPE, true);
	m_comboBoxODFixed2.AddGroup(FONTS_GROUP_TRUETYPE, true);
	std::for_each(fonts[FONTS_GROUP_TRUETYPE].begin(), fonts[FONTS_GROUP_TRUETYPE].end(), [this, &groupIndex](const CString& fontName) { 
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = -14;
		lf.lfWeight = FW_NORMAL;
		_tcscpy_s(lf.lfFaceName, fontName);
		m_comboBoxODVariable.AddItem(&lf, &m_fontTypes, 0, groupIndex);
		m_comboBoxODFixed2.AddItem(&lf, &m_fontTypes, 0, groupIndex);
	} );

	std::for_each(fonts[FONTS_GROUP_OPENTYPE].begin(), fonts[FONTS_GROUP_OPENTYPE].end(), [this, &groupIndex](const CString& fontName) { 
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = -14;
		lf.lfWeight = FW_NORMAL;
		_tcscpy_s(lf.lfFaceName, fontName);
		m_comboBoxODVariable.AddItem(&lf, &m_fontTypes, 1, groupIndex);
		m_comboBoxODFixed2.AddItem(&lf, &m_fontTypes, 1, groupIndex);
	} );
}

void CTestDlg::FillComboBoxCountries()
{
	m_comboCountries.AddGroup(_T("Africa"), true);
	m_comboCountries.AddItem(_T("Algeria"), IDI_ALGERIA, 0);
	m_comboCountries.AddItem(_T("Morocco"), IDI_MOROCCO, 0);
	m_comboCountries.AddItem(_T("Egypt"), IDI_EGYPT, 0);
	m_comboCountries.AddItem(_T("Mali"), IDI_MALI, 0);
	m_comboCountries.AddItem(_T("Nigeria"), IDI_NIGERIA, 0);
	m_comboCountries.AddItem(_T("Angola"), IDI_ANGOLA, 0);

	m_comboCountries.AddGroup(_T("America"), true);
	m_comboCountries.AddItem(_T("Canada"), IDI_CANADA, 1);
	m_comboCountries.AddItem(_T("United States"), IDI_UNITED_STATES, 1);
	m_comboCountries.AddItem(_T("Mexico"), IDI_MEXICO, 1);
	m_comboCountries.AddItem(_T("Argentina"), IDI_ARGENTINA, 1);
	m_comboCountries.AddItem(_T("Brazil"), IDI_BRAZIL, 1);

	m_comboCountries.AddGroup(_T("Asia"), true);
	m_comboCountries.AddItem(_T("Vietnam"), IDI_VIETNAM, 2);
	m_comboCountries.AddItem(_T("India"), IDI_INDIA, 2);
	m_comboCountries.AddItem(_T("Japan"), IDI_JAPAN, 2);
	m_comboCountries.AddItem(_T("China"), IDI_CHINA, 2);

	m_comboCountries.AddGroup(_T("Europe"), true);
	m_comboCountries.AddItem(_T("Austria"), IDI_AUSTRIA, 3);
	m_comboCountries.AddItem(_T("Czech Republic"), IDI_CZECH, 3);
	m_comboCountries.AddItem(_T("France"), IDI_FRANCE, 3);
	m_comboCountries.AddItem(_T("Germany"), IDI_GERMANY, 3);
	m_comboCountries.AddItem(_T("Switzerland"), IDI_SWITZERLAND, 3);
	m_comboCountries.AddItem(_T("United Kingdom"), IDI_UNITED_KINGDOM, 3);

}

void CTestDlg::OnBnClickedButtonAddRecentOdFixed()
{
	CString text;
	m_comboBoxODFixed.GetWindowText(text);
	if (text.Trim().IsEmpty())
		return;
	// first check if item exists in the recent list
	int index = m_comboBoxODFixed.FindStringExactInGroup(text, 0);
	if (index != CB_ERR)
		m_comboBoxODFixed.DeleteItem(index);
	m_comboBoxODFixed.InsertItem(new CGroupComboBoxSimpleItem(text), 0);
}

void CTestDlg::OnBnClickedButtonRemoveRecentOdFixed()
{
	CString text;
	m_comboBoxODFixed.GetWindowText(text);
	int index = m_comboBoxODFixed.FindStringExact(0, text);
	if (index != CB_ERR)
		m_comboBoxODFixed.DeleteItem(index);
}

void CTestDlg::OnBnClickedButtonAddRecentOdVariable()
{
	CString text;
	m_comboBoxODVariable.GetWindowText(text);
	if (text.Trim().IsEmpty())
		return;
	// first check if item exists in the recent list
	int index = m_comboBoxODVariable.FindStringExactInGroup(text, 0);
	if (index != CB_ERR)
		m_comboBoxODVariable.DeleteItem(index);
	m_comboBoxODVariable.InsertRecentItem(text);
}


void CTestDlg::OnBnClickedButtonRemoveRecentOdVariable()
{
	CString text;
	m_comboBoxODVariable.GetWindowText(text);
	int index = m_comboBoxODVariable.FindStringExact(0, text);
	if (index != CB_ERR)
		m_comboBoxODVariable.DeleteItem(index);
}

void CTestDlg::OnBnClickedAutowidthComboOdFixed()
{
	m_comboBoxODFixed.EnableDropdownListAutoWidth(m_autowidthComboODFixed.GetCheck() == BST_CHECKED);
}

void CTestDlg::OnBnClickedAutowidthComboOdVariable()
{
	m_comboBoxODVariable.EnableDropdownListAutoWidth(m_autowidthComboODVariable.GetCheck() == BST_CHECKED);
}


void CTestDlg::OnBnClickedAutocompleteComboOdFixed()
{
	m_comboBoxODFixed.EnableAutocomplete(m_autocompleteComboODFixed.GetCheck() == BST_CHECKED);
}


void CTestDlg::OnBnClickedAutocompleteComboOdVariable()
{
	m_comboBoxODVariable.EnableAutocomplete(m_autocompleteComboODVariable.GetCheck() == BST_CHECKED);
}

void CTestDlg::OnBnClickedCancelwescapeComboOdFixed()
{
	m_comboBoxODFixed.EnableSelectionUndoByEscKey(m_undoByEscComboODFixed.GetCheck() == BST_CHECKED);
}

void CTestDlg::OnBnClickedCancelwescComboOdVariable()
{
	m_comboBoxODVariable.EnableSelectionUndoByEscKey(m_undoByEscComboODVariable.GetCheck() == BST_CHECKED);
}
