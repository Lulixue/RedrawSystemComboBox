#pragma once
#include <afxwin.h>
#include <set>
using std::set;

#define UMSG_COMBO_SEL_CHANGE		(WM_USER + 0x1234)
#define UMSG_LISTBOX_MOUSE_POSITION (WM_USER + 0x300)
#define UMSG_EDITBOX_MOUSE_ACTION   (WM_USER + 0x301)

#define DEL_BUTTON_MOUSE_ON_COLOR   (RGB(231, 17, 35))      // 红色
#define ITEM_SELECT_TEXT_COLOR      (RGB(255, 255, 255))    // 白色
#define ITEM_UNSELECT_TEXT_COLOR    (RGB(0, 0, 0))          // 黑色
#define ITEM_SELECT_BK_COLOR        (RGB(0, 120, 215))       // 蓝色
#define ITEM_UNSELECT_BK_COLOR      (RGB(255, 255, 255))    // 白色
#define COMBO_ARROW_COLOR           (RGB(86, 86, 86))       // 灰色
#define COMBO_READONLY_BK_COLOR       (RGB(225, 225, 225))    // 灰色
#define COMBO_DISABLED_TEXT_COLOR     (RGB(125, 109, 109))    // 深灰色
#define COMBO_DISABLED_BK_COLOR         (RGB(204, 204, 204))
#define COMBO_DISABLED_FRAME_COLOR      (RGB(191, 191, 191))
#define COMBO_DISABLED_EDIT_OUTTER_COLOR    (RGB(255, 255, 255))    // 白色
#define COMBO_MOUSE_ON_BK_COLOR         (RGB(229, 241, 251))      // 浅灰色
#define COMBO_MOUSE_ON_FRAME_COLOR      ITEM_SELECT_BK_COLOR      // 蓝色
#define COMBO_MOUSE_ON_TEXT_COLOR       ITEM_UNSELECT_TEXT_COLOR
#define COMBO_NORMAL_FRAME_COLOR        (RGB(173, 173, 173))      // 较深的灰色
#define COMBO_NORMAL_BK_COLOR           ITEM_UNSELECT_BK_COLOR      // 
#define COMBO_NORMAL_TEXT_COLOR         ITEM_UNSELECT_TEXT_COLOR
#define COMBO_NORMAL_MOUSE_ON_EDIT_COLOR    (RGB(23, 23, 23))       // 接近黑色


#define RECT_PARAM(r)   r.top,r.left,r.right, r.bottom
#define POINT_PARAM(p)  p.x, p.y
#define RECT_WIDTH(r)   (r.right - r.left)
#define RECT_HEIGHT(r)  (r.bottom - r.top)

class CMyComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CMyComboBox)
public:
	CMyComboBox() {}
    virtual ~CMyComboBox() { m_fontText.DeleteObject(); }


protected:
	DECLARE_MESSAGE_MAP()


private:
    CFont m_fontText;
	CString m_strLastSelected;
    const static UINT DEL_RECT_WIDTH = 20;
    const static UINT BOX_HEIGHT = 18;
    const static UINT ARROW_HEIGHT = BOX_HEIGHT+6;
    const static UINT ARROW_WIDTH = 20;
    const static UINT ARROW_TRIANGLE_HEIGHT = 4;
    const static UINT ARROW_TRIANGLE_WIDTH = 8;
    const static UINT TIMER_REFRESH_BOX = 1;

public:
	int SetCurSel(int nSelect);
	CString GetLastSelected() const { return m_strLastSelected; }
	void SetLastSelected(CString item) { m_strLastSelected = item;  }
	afx_msg void OnCbnDropdown();

	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnKillfocus();
    virtual void PreSubclassWindow();

public:
    void SetMouseOver(BOOL bOn);
    void RectResetCoord(RECT & rect, int offsetX = 0, int offsetY = 0);
    BOOL IsCaretInWindow() const;
    BOOL m_bIsDropList{ FALSE };
    BOOL m_bIsEditFocused{ FALSE };
    BOOL m_bMouseHoverOnDelBtn{ FALSE };//用于判选中项按钮的状态，0正常,1鼠标停留。
    CRect m_rcDelButton;//选中项按钮大小
    CRect m_rcArrow;
    CRect m_rcItem;
    BOOL m_bMouseOverBox{ FALSE };
    BOOL m_bMouseOverArrow{ FALSE };
    static HWND s_hwndComboBox;
    static WNDPROC s_oldEditWndProc;
    static WNDPROC s_oldListWndProc;//用于储存组合框里列表框默认消息处理函数
    static CMap<HWND, HWND, HWND, HWND> s_mapComboWnd;//关联组表框窗口句柄和其下的列表框窗口句柄
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    virtual int CompareItem(LPCOMPAREITEMSTRUCT /*lpCompareItemStruct*/);
protected:
    afx_msg LRESULT OnUmsgListboxMousePosition(WPARAM wParam, LPARAM lParam);
public:
    virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
    afx_msg LRESULT OnUmsgEditboxMouseAction(WPARAM wParam, LPARAM lParam);
};

