
// RedrawSystemComboBoxDlg.h : header file
//

#pragma once

#include "CMyComboBox.h"

// CRedrawSystemComboBoxDlg dialog
class CRedrawSystemComboBoxDlg : public CDialogEx
{
// Construction
public:
	CRedrawSystemComboBoxDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REDRAWSYSTEMCOMBOBOX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    void EnableCtrl(BOOL bEnable);
    CMyComboBox m_cbDropDown;
    CMyComboBox m_cbDropList;
    afx_msg void OnBnClickedButtonDisableEnable();
};
