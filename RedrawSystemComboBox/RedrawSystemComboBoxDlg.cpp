
// RedrawSystemComboBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RedrawSystemComboBox.h"
#include "RedrawSystemComboBoxDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRedrawSystemComboBoxDlg dialog



CRedrawSystemComboBoxDlg::CRedrawSystemComboBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REDRAWSYSTEMCOMBOBOX_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRedrawSystemComboBoxDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_DROPDOWN, m_cbDropDown);
    DDX_Control(pDX, IDC_COMBO_DROPLIST, m_cbDropList);
}

BEGIN_MESSAGE_MAP(CRedrawSystemComboBoxDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_DISABLE_ENABLE, &CRedrawSystemComboBoxDlg::OnBnClickedButtonDisableEnable)
END_MESSAGE_MAP()


// CRedrawSystemComboBoxDlg message handlers

BOOL CRedrawSystemComboBoxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: Add extra initialization here

    static const CString TEST_STRINGS[] = { L"TestItem1", L"TestItem2", L"TestItem3" };
    UINT size = ARRAYSIZE(TEST_STRINGS);

    for (UINT i = 0; i < size; i++)
    {
        m_cbDropDown.AddString(TEST_STRINGS[i]);
        m_cbDropList.AddString(TEST_STRINGS[i]);
    }

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRedrawSystemComboBoxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRedrawSystemComboBoxDlg::OnPaint()
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
HCURSOR CRedrawSystemComboBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRedrawSystemComboBoxDlg::EnableCtrl(BOOL bEnable)
{
    const static UINT CONTROLS[] = {
        IDC_COMBO_DROPDOWN, IDC_COMBO_DROPLIST,
        IDC_BUTTON_DISABLE_ENABLE,
    };
    const static UINT size = ARRAYSIZE(CONTROLS);

    for (UINT i = 0; i < size; i++)
    {
        CWnd *pWnd = GetDlgItem(CONTROLS[i]);
        if (pWnd)
        {
            pWnd->EnableWindow(bEnable);
        }
    }

}

unsigned WINAPI ThreadDisableEnableTest(LPVOID lP)
{
    CRedrawSystemComboBoxDlg *pDlg = (CRedrawSystemComboBoxDlg*)lP;

    pDlg->EnableCtrl(FALSE);

    Sleep(500);

    pDlg->EnableCtrl(TRUE);

    return 0;
}


void CRedrawSystemComboBoxDlg::OnBnClickedButtonDisableEnable()
{
    _beginthreadex(NULL, 0, ThreadDisableEnableTest, this, 0, NULL);
}
