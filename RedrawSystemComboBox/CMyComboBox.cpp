#include "stdafx.h"
#include "CMyComboBox.h"
#include <vector>

IMPLEMENT_DYNAMIC(CMyComboBox, CComboBox)

BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
	//消息反射声明
	ON_CONTROL_REFLECT(CBN_DROPDOWN, &CMyComboBox::OnCbnDropdown)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CMyComboBox::OnCbnSelchange)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CMyComboBox::OnCbnKillfocus)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(UMSG_LISTBOX_MOUSE_POSITION, &CMyComboBox::OnUmsgListboxMousePosition)
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_MESSAGE(UMSG_EDITBOX_MOUSE_ACTION, &CMyComboBox::OnUmsgEditboxMouseAction)
END_MESSAGE_MAP()

WNDPROC CMyComboBox::s_oldListWndProc = NULL;
WNDPROC CMyComboBox::s_oldEditWndProc = NULL;
HWND CMyComboBox::s_hwndComboBox;
CMap<HWND, HWND, HWND, HWND> CMyComboBox::s_mapComboWnd;
// CMyComboBox 消息处理程序
void CMyComboBox::OnCbnDropdown()
{
	CClientDC dc(this);
	int nWitdh = 10;
	int nSaveDC = dc.SaveDC();

	//获取字体信息，
	dc.SelectObject(GetFont());

	//计算最大的显示长度
	for (int i = 0; i < GetCount(); i++)
	{
		CString strLable = _T("");
		GetLBText(i, strLable);

		nWitdh = max(nWitdh, dc.GetTextExtent(strLable).cx);
	}

	//多增加的冗余宽度
	nWitdh += DEL_RECT_WIDTH + 5;

	//设置下拉列表宽度
	SetDroppedWidth(nWitdh);
	//恢复实际dc
	dc.RestoreDC(nSaveDC);
}
int CMyComboBox::SetCurSel(int nSelect)
{
	int nRet = CComboBox::SetCurSel(nSelect);
	::PostMessage(GetParent()->GetSafeHwnd(), UMSG_COMBO_SEL_CHANGE, (WPARAM)GetDlgCtrlID(), 0);
	return nRet;
}

void CMyComboBox::OnCbnSelchange()
{
	// TODO: Add your control notification handler code here
	int nSel = GetCurSel();
	if (nSel == -1) {
		m_strLastSelected = TEXT("");
	}
	else {
		GetLBText(nSel, m_strLastSelected);
	}
	::PostMessage(GetParent()->GetSafeHwnd(), UMSG_COMBO_SEL_CHANGE, (WPARAM)GetDlgCtrlID(), 0);
}


void CMyComboBox::OnCbnKillfocus()
{
	// TODO: Add your control notification handler code here
	GetWindowText(m_strLastSelected);

	if (!m_strLastSelected.IsEmpty())
	{
		if (FindString(-1, m_strLastSelected) == CB_ERR)
		{
			AddString(m_strLastSelected);
		}
	}
}
LRESULT CALLBACK HookListboxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_MOUSEMOVE)//鼠标移动
    {
        WORD xPos, yPos;
        xPos = LOWORD(lParam);
        yPos = HIWORD(lParam);
        HWND relWnd;
        CMyComboBox::s_mapComboWnd.Lookup(hWnd, relWnd);
        ::SendMessage(relWnd, UMSG_LISTBOX_MOUSE_POSITION, xPos, yPos);//发送鼠标位置给列表窗口
    }
    else if (message == WM_LBUTTONDOWN)
    {
        HWND relWnd;
        CMyComboBox::s_mapComboWnd.Lookup(hWnd, relWnd);
        //消息类型我是直接以数字发送，觉得看着不舒服的，可自行定义
        //如#define WM_MOUSEPOSITION WM_USER+100
        LRESULT result = ::SendMessage(relWnd, UMSG_LISTBOX_MOUSE_POSITION, (WPARAM)-1, 0);//第三参数-1表明鼠标左键按下
        if (result == 1) 
            return 0;//返回0,不让默认消息处理函数处理WM_LBUTTONDOWN
    }
    //调用列表框之前的默认消息处理函数
    return ::CallWindowProc(CMyComboBox::s_oldListWndProc, hWnd, message, wParam, lParam);
}
LRESULT CALLBACK HookEditBoxWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //TRACE("HookEditBoxWndProc, Msg: %d\n", message);
    switch (message)
    {
    case WM_SETFOCUS:
    case WM_KILLFOCUS:
    case WM_MOUSEMOVE:
        HWND relWnd;
        CMyComboBox::s_mapComboWnd.Lookup(hWnd, relWnd);
        ::PostMessage(relWnd, UMSG_EDITBOX_MOUSE_ACTION, (WPARAM)message, lParam);
        break;
    default:
        break;

    }

    return ::CallWindowProc(CMyComboBox::s_oldEditWndProc, hWnd, message, wParam, lParam);
}

void CMyComboBox::PreSubclassWindow()
{
    // TODO: Add your specialized code here and/or call the base class

    CComboBox::PreSubclassWindow();

    COMBOBOXINFO cbi;
    cbi.cbSize = sizeof(COMBOBOXINFO);
    ::GetComboBoxInfo(this->m_hWnd, &cbi);  //获取组合框信息

    //替换列表框消息处理函数

    LONG_PTR style = GetWindowLongPtr(cbi.hwndCombo, GWL_STYLE);
    int nDropType = (style & CBS_DROPDOWNLIST);

    m_bIsDropList = FALSE;
    if (nDropType == CBS_SIMPLE)
    {
        //
    }
    else
    {
        s_oldListWndProc = (WNDPROC)::SetWindowLongPtr(cbi.hwndList, GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (HookListboxWndProc));
        if (nDropType == CBS_DROPDOWNLIST)
        {
            m_bIsDropList = TRUE;
        }
        else // CBS_DROPDOWN
        {
            CEdit* pEdit = (CEdit*)GetWindow(GW_CHILD);
            if (pEdit)
            {
                s_oldEditWndProc = (WNDPROC)::SetWindowLongPtr(pEdit->GetSafeHwnd(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR> (HookEditBoxWndProc));

                s_mapComboWnd.SetAt(pEdit->GetSafeHwnd(), this->m_hWnd);
                s_mapComboWnd.SetAt(this->m_hWnd, pEdit->GetSafeHwnd());
            }
        }
    }

    //相互关联，到时可以依据两个中的任一窗口句柄，查询到另一个
    //我想，对于CMap类一定会有解决此问题的方法，但我现在实在不想深入了解 
    //只能给它互相关联，不知道是不是“主流”方法。
    s_mapComboWnd.SetAt(cbi.hwndList, this->m_hWnd);
    s_mapComboWnd.SetAt(this->m_hWnd, cbi.hwndList);


    SendMessage(CB_SETITEMHEIGHT, (WPARAM)-1, (LPARAM)BOX_HEIGHT);   //改变控件本身的高度

    m_fontText.CreateFont(
        16,                 //字体高度(旋转后的字体宽度)=56   
        0,                 //字体宽度(旋转后的字体高度)=20 
        0,             //字体显示角度=270° 
        0,                                     //nOrientation=0 
        FW_NORMAL,                                   //字体磅数=10 
        FALSE,                             //非斜体 
        FALSE,                             //无下划线 
        FALSE,             //无删除线 
        DEFAULT_CHARSET,         //使用缺省字符集 
        OUT_DEFAULT_PRECIS,   //缺省输出精度 
        CLIP_DEFAULT_PRECIS,//缺省裁减精度   
        DEFAULT_QUALITY,         //nQuality=缺省值 
        DEFAULT_PITCH,             //nPitchAndFamily=缺省值 
        L"Tahoma");                   //字体名=@system 

    //SetFont(&m_fontText);
    m_rcArrow = cbi.rcButton;
    m_rcItem = cbi.rcItem;
}



void CMyComboBox::SetMouseOver(BOOL bOn)
{
    static ULONGLONG timeOn = GetTickCount64();
    static ULONGLONG timLeave = GetTickCount64();
    static const UINT TIME_FILTER_MS = 300;

    if (m_bMouseOverBox == bOn)
    {
        return;
    }

    if (bOn)
    {
        
        ULONGLONG span;
        timeOn = GetTickCount64();
        span = timeOn - timLeave;
        TRACE("ON: TimeSpan(%d)\n", span);
        if (span > TIME_FILTER_MS)
        {
            m_bMouseOverBox = TRUE;
        }
    }
    else
    {
        timLeave = GetTickCount64();
        m_bMouseOverBox = FALSE;
    }
}

void CMyComboBox::OnMouseMove(UINT nFlags, CPoint point)
{

    if (!m_bMouseOverBox)
    {
        SetTimer(TIMER_REFRESH_BOX, 50, 0);
    }

    CComboBox::OnMouseMove(nFlags, point);
}

void CMyComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);//组合框DC
    CRect itemRect(lpDrawItemStruct->rcItem);//项区域
    int nState = lpDrawItemStruct->itemState;//项状态
    int nIndex = lpDrawItemStruct->itemID;//项索引
    int nAction = lpDrawItemStruct->itemAction;

    //TRACE("Index: %d(DelBtn:%d), state: %04X, Action: %04X\n", nIndex, m_bMouseHoverOnDelBtn, nState, nAction);
  
    if (nIndex != -1)
    {
      
        if (nState & ODS_SELECTED)//如果选中该项
        {
            CPen pen;

            //计算位置
            m_rcDelButton.left = itemRect.right - DEL_RECT_WIDTH;
            m_rcDelButton.top = itemRect.top;
            m_rcDelButton.right = itemRect.right;
            m_rcDelButton.bottom = itemRect.bottom;

            dc.FillSolidRect(&itemRect, ITEM_SELECT_BK_COLOR);
            if (m_bMouseHoverOnDelBtn) //鼠标停留 
            {
                // 填充红色背影
                dc.FillSolidRect(&m_rcDelButton, DEL_BUTTON_MOUSE_ON_COLOR);
                pen.CreatePen(1, 2, ITEM_SELECT_TEXT_COLOR);
                itemRect.right -= DEL_RECT_WIDTH;
                // 画白色X按钮
                CRect temDelRect = m_rcDelButton;
                temDelRect.top += 5;
                temDelRect.right -= 5;
                temDelRect.bottom -= 5;
                temDelRect.left += 5;
                //temDelRect.DeflateRect(2, 2, 2, 2);
                dc.SelectObject(&pen);

                //TRACE("temDelRect:(%d,%d,%d,%d)\n", RECT_PARAM(temDelRect));
                dc.MoveTo(temDelRect.left, temDelRect.top);
                dc.LineTo(temDelRect.right, temDelRect.bottom);
                dc.MoveTo(temDelRect.right, temDelRect.top);
                dc.LineTo(temDelRect.left, temDelRect.bottom);
            }
               
            dc.SetTextColor(ITEM_SELECT_TEXT_COLOR);
        }
        else
        {
            dc.SetTextColor(ITEM_UNSELECT_TEXT_COLOR);
            dc.FillSolidRect(itemRect, ITEM_UNSELECT_BK_COLOR);
        }
        dc.SetBkMode(TRANSPARENT);
        if (nState == (ES_WANTRETURN | EN_CHANGE)) // 0x1300
        {
        }
        CString str;
        GetLBText(nIndex, str);
        itemRect.DeflateRect(2, 0, 0, 0);

        dc.DrawText(str, itemRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//显示文本
    }
    
    dc.Detach();
}


void  CMyComboBox::RectResetCoord(RECT & rect, int offsetX, int offsetY)
{
    rect.right = RECT_WIDTH(rect) + offsetX;
    rect.bottom = RECT_HEIGHT(rect) + offsetY;
    rect.top = offsetY;
    rect.left = offsetX;
}

#define USE_COMPATIBLE_DC   0
void CMyComboBox::OnPaint()
{
#if 0
    CComboBox::OnPaint();
#else

#if USE_COMPATIBLE_DC
    CPaintDC parentDc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CComboBox::OnPaint() for painting messages

    CDC dc;
    CBitmap bmp;
    dc.CreateCompatibleDC(&parentDc);//创建与目标DC相兼容的内存DC，
    bmp.CreateCompatibleBitmap(&parentDc, rect.Width(), rect.Height());//根据目标DC创建位图，为什么？看后面
    dc.SelectObject(&bmp);//把位图选入内存DC
#else
    CPaintDC dc(this);
#endif

    static BOOL bFirstTime = TRUE;
    COLORREF clrBkground(COMBO_NORMAL_BK_COLOR);
    COLORREF clrText(ITEM_UNSELECT_TEXT_COLOR);
    COLORREF clrFrame(COMBO_NORMAL_FRAME_COLOR);
    POINT ptArrowBeg, ptArrowMid, ptArrowEnd;
    CRect rect, comboRect;
    COMBOBOXINFO cbi;
    cbi.cbSize = sizeof(COMBOBOXINFO);

    GetClientRect(&comboRect);
    GetComboBoxInfo(&cbi);  //获取组合框信息

    
    rect = comboRect;
    RectResetCoord(rect);

    m_rcArrow = cbi.rcButton;


    int nArrowPaddingX = (m_rcArrow.Width() - ARROW_TRIANGLE_WIDTH) / 2 - 1;
    int nArrowPaddingY = (m_rcArrow.Height() - ARROW_TRIANGLE_HEIGHT) / 2 - 1;

    // arrow的坐标 6*4 倒三角形， 底部上下加距相等
    ptArrowBeg.x = m_rcArrow.left + nArrowPaddingX;
    ptArrowBeg.y = m_rcArrow.top + nArrowPaddingY;
    ptArrowMid.x = m_rcArrow.left + m_rcArrow.Width() / 2;
    ptArrowMid.y = ptArrowBeg.y + ARROW_TRIANGLE_HEIGHT;
    ptArrowEnd.x = m_rcArrow.right - nArrowPaddingX;
    ptArrowEnd.y = ptArrowBeg.y;
   
    BOOL bDrawArrowRect = FALSE;
    BOOL bListDropped = GetDroppedState();
    BOOL bCurseInCtrl = FALSE;

    TRACE("OnPaint: Cursor: %d, ListDropped: %d, BtnState: %d, Enable: %d, Over: %d, ArrowOver: %d, DropList: %d\n", IsCaretInWindow(), bListDropped, cbi.stateButton, IsWindowEnabled(), m_bMouseOverBox, m_bMouseOverArrow, m_bIsDropList);

    if (!IsWindowEnabled())
    {
        clrBkground = COMBO_DISABLED_BK_COLOR;
        clrText = COMBO_DISABLED_TEXT_COLOR;
        clrFrame = COMBO_DISABLED_FRAME_COLOR;
    }
    else 
    {

        if (m_bMouseOverBox || bListDropped)
        {
            clrFrame = (m_bIsDropList || m_bIsEditFocused || bListDropped) ? COMBO_MOUSE_ON_FRAME_COLOR : COMBO_NORMAL_MOUSE_ON_EDIT_COLOR;
            clrText = COMBO_MOUSE_ON_TEXT_COLOR;
            clrBkground = m_bIsDropList ? COMBO_MOUSE_ON_BK_COLOR : COMBO_NORMAL_BK_COLOR;

            if (m_bMouseOverArrow && !m_bIsDropList)
            {
                bDrawArrowRect = TRUE;
            }
        }
        else
        {
            clrText = COMBO_NORMAL_TEXT_COLOR;
            clrFrame = COMBO_NORMAL_FRAME_COLOR;
            clrBkground = m_bIsDropList ? COMBO_READONLY_BK_COLOR : COMBO_NORMAL_BK_COLOR;
        }
    }


    CFont *pOldFont;
    CPen *pOldPen;
    CPen penArrow;
    CBrush brushBkground;
    CBrush brushArrow;
    CString str;

    dc.SetBkMode(TRANSPARENT);
    dc.Draw3dRect(rect, clrFrame, clrFrame);//画边框
    rect.DeflateRect(1, 1, 1, 1);

   
    if (m_bIsDropList)
    {
        dc.FillSolidRect(rect, clrBkground);
    }
    else
    {
        CRgn rgnBox, rgnEdit;
        rgnBox.CreateRectRgnIndirect(rect);
        rgnEdit.CreateRectRgnIndirect(&cbi.rcItem);
        // 处理后的rgnBox只包括了Edit框之外的客户区域，这样，Edit将不会被我的背景覆盖而导致重画。 
        if (rgnBox.CombineRgn(&rgnBox, &rgnEdit, RGN_XOR) == ERROR)
        {
            ASSERT(FALSE);
        }
        if (IsWindowEnabled())
        {
            brushBkground.CreateSolidBrush(clrBkground);
        }
        else {
            brushBkground.CreateSolidBrush(COMBO_DISABLED_EDIT_OUTTER_COLOR);
        }
        dc.FillRgn(&rgnBox, &brushBkground);
    }

    if (bDrawArrowRect && IsWindowEnabled())
    {
        cbi.rcButton.right = rect.right + 1;
        cbi.rcButton.top = 0;
        cbi.rcButton.bottom = RECT_HEIGHT(rect) + 2;
        dc.Draw3dRect(&cbi.rcButton, COMBO_MOUSE_ON_FRAME_COLOR, COMBO_MOUSE_ON_FRAME_COLOR);
    }

    // Set Window Text
    {
        GetWindowText(str);
        pOldFont = dc.SelectObject(&m_fontText);
        dc.SetTextColor(clrText);

        if (!m_bIsDropList && IsWindowEnabled())
        {
            //
        }
        else
        {
            cbi.rcItem.left += 1;
            dc.DrawText(str, &cbi.rcItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE);//显示文本
        }
        
    }

    // Draw Button
    {
        penArrow.CreatePen(PS_DASH, 1, COMBO_ARROW_COLOR);
        pOldPen = dc.SelectObject(&penArrow);

        int x = ptArrowBeg.x+1;
        int y = ptArrowBeg.y+1;
        for (; x <= (ptArrowMid.x-1); x++, y++)
        {
            dc.SetPixel(x, y, COMBO_ARROW_COLOR);
        }

        for (; x <= (ptArrowEnd.x-1); x++, y--)
        {
            dc.SetPixel(x, y, COMBO_ARROW_COLOR);
        }

        
    }
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldFont);

#if USE_COMPATIBLE_DC
    parentDc.BitBlt(comboRect.left, comboRect.top, comboRect.Width(), comboRect.Height(), &dc, 0, 0, SRCCOPY);

    bmp.DeleteObject();
    dc.DeleteDC();
#endif

#endif
}



int CMyComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{

    // TODO:  Add your code to determine the sorting order of the specified items
    // return -1 = item 1 sorts before item 2
    // return 0 = item 1 and item 2 sort the same
    // return 1 = item 1 sorts after item 2

    LPCWSTR pStr1 = (LPCWSTR)lpCompareItemStruct->itemData1;
    LPCWSTR pStr2 = (LPCWSTR)lpCompareItemStruct->itemData2;

    int nResult = StrCmpW(pStr1, pStr2);

    if (nResult < 0) {
        nResult = -1;
    }
    else if (nResult > 0) {
        nResult = 1;
    }

    return nResult;
}


BOOL CMyComboBox::IsCaretInWindow() const
{
    CRect rect;
    GetWindowRect(&rect);

    CPoint  pt;
    GetCursorPos(&pt);

    TRACE("Window: (%d,%d,%d,%d)\n", RECT_PARAM(rect));
    TRACE("Cursor: (%d,%d)\n", POINT_PARAM(pt));

    return (rect.PtInRect(pt));
}

afx_msg LRESULT CMyComboBox::OnUmsgListboxMousePosition(WPARAM wParam, LPARAM lParam)
{
    if (m_bMouseHoverOnDelBtn && (wParam == -1))//如果是单击消息，并且鼠标当前停留在下拉按钮里
    {
        this->DeleteString(GetCurSel());
        this->SelectString(-1, m_strLastSelected);
        return 1;
    }
    CPoint point((int)wParam, (int)lParam);
    int result = m_rcDelButton.PtInRect(point);
    if (result && !m_bMouseHoverOnDelBtn)//如果鼠标在组合框内，并且是初次鼠标进入
    {
        m_bMouseHoverOnDelBtn = TRUE;
        HWND listWnd;
        s_mapComboWnd.Lookup(this->m_hWnd, listWnd);
        ::InvalidateRect(listWnd, m_rcDelButton, true);
    }
    else if ((result == 0) && m_bMouseHoverOnDelBtn)
    {
        m_bMouseHoverOnDelBtn = FALSE;
        HWND listWnd;
        s_mapComboWnd.Lookup(this->m_hWnd, listWnd);
        ::InvalidateRect(listWnd, m_rcDelButton, true);
    }
    //TRACE("DelButtonState: %d\n", m_bMouseHoverOnDelBtn);
    //TRACE("DelRect: %d, %d, Point: %d\n", m_rcDelButton.left, m_rcDelButton.right, point.x);
    return 0;
}


void CMyComboBox::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/)
{

    // TODO:  Add your code to determine the size of specified item
}



void CMyComboBox::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default

    CRect rect;
    GetWindowRect(&rect);

    CPoint  pt;
    GetCursorPos(&pt);

    if (rect.PtInRect(pt))
    {
        ScreenToClient(&pt);
        if (!m_bMouseOverArrow && m_rcArrow.PtInRect(pt))
        {
            m_bMouseOverArrow = TRUE;
            InvalidateRect(&m_rcArrow);
        }

        if (!m_bMouseOverBox)
        {
            m_bMouseOverBox = TRUE;
            //InvalidateRect(&rect);
            Invalidate();
        }
    }
    else
    {

        if (!GetDroppedState())
        {
            m_bMouseOverArrow = FALSE;
            m_bMouseOverBox = FALSE;
            Invalidate();
        }
        if (!m_bMouseOverBox)
        {
            KillTimer(TIMER_REFRESH_BOX);
        }
    }
    TRACE("Timer: OverBox: %d, OverArrow: %d\n", m_bMouseOverBox, m_bMouseOverArrow);
   
    CComboBox::OnTimer(nIDEvent);
}


afx_msg LRESULT CMyComboBox::OnUmsgEditboxMouseAction(WPARAM wParam, LPARAM lParam)
{
    UINT msg = (UINT)wParam;

    if (WM_SETFOCUS == msg)
    {
        m_bIsEditFocused = TRUE;
    }
    else if (WM_KILLFOCUS == msg)
    {
        m_bIsEditFocused = FALSE;
    }
    else if (WM_MOUSEMOVE == msg)
    {
        if (!m_bMouseOverBox)
        {
            SetTimer(TIMER_REFRESH_BOX, 50, 0);
        }
    }
    //TRACE("EditBox Focus: %d\n", m_bIsEditFocused);
    return 0;
}
