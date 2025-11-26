
// OverlayDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Overlay.h"
#include "OverlayDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// COverlayDlg 대화 상자

COverlayDlg::COverlayDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_OVERLAY_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    ZeroMemory(&m_nid, sizeof(m_nid));
}

void COverlayDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(COverlayDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(WM_TRAYICON, &COverlayDlg::OnTrayIcon)
    ON_WM_NCHITTEST()
    ON_WM_WINDOWPOSCHANGING()
    ON_COMMAND(ID_TRAY_OVERLAY_SETTING, &COverlayDlg::OnTrayOverlaySetting)
    ON_COMMAND(ID_TRAY_EXIT, &COverlayDlg::OnTrayExit)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_ADD_RECT, OnAddRect)
    ON_MESSAGE(WM_CLEAR_RECT, OnClearRect)
    ON_MESSAGE(WM_CLOSE_SETTING, OnCloseSetting)
END_MESSAGE_MAP()

BOOL COverlayDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 일반 스타일: 타이틀바/테두리 제거, 팝업
    LONG style = GetWindowLong(m_hWnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
    style |= WS_POPUP;
    SetWindowLong(m_hWnd, GWL_STYLE, style);

    // 확장 스타일: TopMost + Layered + ToolWindow + Transparent + NoActivate
    LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW
        | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE;
    SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle);

    // 전체 화면 크기
    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);
    MoveWindow(0, 0, cx, cy, FALSE);

    // 색상키 투명 처리
    COLORREF transparentColor = RGB(255, 0, 255);
    SetLayeredWindowAttributes(transparentColor, 255, LWA_COLORKEY);

    m_nid.cbSize = sizeof(NOTIFYICONDATA);
    m_nid.hWnd = m_hWnd;
    m_nid.uID = TRAY_ICON_ID;
    m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_nid.uCallbackMessage = WM_TRAYICON;
    m_nid.hIcon = m_hIcon;
    _tcscpy_s(m_nid.szTip, _T("Overlay App"));
    Shell_NotifyIcon(NIM_ADD, &m_nid);

    OverlayRect r;

    const int w = 100;
    const int h = 100;

    CRect rcScreen;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
    int rect_cx = rcScreen.left + (rcScreen.Width() - w) / 2;
    int rect_cy = rcScreen.top + (rcScreen.Height() - h) / 2;

    r.rc = CRect(rect_cx, rect_cy, rect_cx + w, rect_cy + h);
    r.selected = false;

    m_rects.push_back(r);

    // 한 번 TopMost로 올림 (초기 정렬)
    SetWindowPos(&wndTopMost, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    ShowWindow(SW_SHOW);

    return TRUE;
}



BOOL COverlayDlg::OnEraseBkgnd(CDC* pDC)
{
    // 배경을 투명 컬러키로 칠함 (마젠타)
    CRect rc;
    GetClientRect(&rc);

    CBrush brush(RGB(255, 0, 255)); // 컬러키와 동일하게
    pDC->FillRect(&rc, &brush);

    return TRUE;
}

void COverlayDlg::OnPaint()
{
    CPaintDC dc(this);

    CBrush blackBrush(RGB(0, 0, 0));
    CBrush* pOldBrush = dc.SelectObject(&blackBrush);

    for (const auto& r : m_rects)
    {
        dc.Rectangle(r.rc);
    }

    dc.SelectObject(pOldBrush);
}

LRESULT COverlayDlg::OnTrayIcon(WPARAM wParam, LPARAM lParam)
{
    if (wParam != TRAY_ICON_ID)
        return 0;

    switch (lParam)
    {
    case WM_RBUTTONUP:
    {
        CMenu menu;
        if (!menu.LoadMenu(IDR_TRAY_MENU))
            return 0;

        CMenu* pSub = menu.GetSubMenu(0);
        if (!pSub)
            return 0;

        POINT pt;
        GetCursorPos(&pt);

        // TrackPopupMenu 전에 포그라운드로 올려야 메뉴가 정상적으로 닫힘
        SetForegroundWindow();

        UINT cmd = pSub->TrackPopupMenu(
            TPM_RETURNCMD | TPM_RIGHTBUTTON,
            pt.x, pt.y,
            this
        );

        if (cmd != 0)
        {
            PostMessage(WM_COMMAND, cmd, 0);
        }
    }
    break;

    case WM_LBUTTONDBLCLK:
    {
        // 간단히 토글
        if (IsWindowVisible())
            ShowWindow(SW_HIDE);
        else
            ShowWindow(SW_SHOW);
    }
    break;

    default:
        break;
    }

    return 0;
}

void COverlayDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    CDialogEx::OnWindowPosChanging(lpwndpos);

    // 항상 TopMost 유지
    lpwndpos->flags &= ~SWP_NOZORDER;
    lpwndpos->hwndInsertAfter = HWND_TOPMOST;
}

LRESULT COverlayDlg::OnNcHitTest(CPoint point)
{
    return HTCLIENT;
}

void COverlayDlg::OnTrayOverlaySetting()  
{  
   LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);  

   exStyle &= ~(WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);

   SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle); 
   SetWindowPos(nullptr, 0, 0, 0, 0,  
       SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);  

   if (!::IsWindow(m_settingDig.GetSafeHwnd()))
   {
       m_settingDig.Create(IDD_PROPPAGE, this);
   }

   m_settingDig.ShowWindow(SW_SHOW);
}

void COverlayDlg::OnTrayExit()
{
    Shell_NotifyIcon(NIM_DELETE, &m_nid);

    PostQuitMessage(0);
}

HitType COverlayDlg::HitTestRect(const CRect& rc, CPoint pt)
{
    const int EDGE = 5; // 가장자리 판정 여유

    if (!rc.PtInRect(pt))
        return HT_NONE;

    bool left = abs(pt.x - rc.left) <= EDGE;
    bool right = abs(pt.x - rc.right) <= EDGE;
    bool top = abs(pt.y - rc.top) <= EDGE;
    bool bottom = abs(pt.y - rc.bottom) <= EDGE;

    if (left && top)         return HT_TOPLEFT;
    if (right && top)        return HT_TOPRIGHT;
    if (left && bottom)      return HT_BOTTOMLEFT;
    if (right && bottom)     return HT_BOTTOMRIGHT;
    if (left)                return HT_LEFT;
    if (right)               return HT_RIGHT;
    if (top)                 return HT_TOP;
    if (bottom)              return HT_BOTTOM;

    return HT_MOVE; // 내부
}
void COverlayDlg::UpdateCursorByHitType(HitType ht)
{
    HCURSOR hCur = ::LoadCursor(NULL, IDC_ARROW);

    switch (ht)
    {
    case HT_MOVE:
        hCur = ::LoadCursor(NULL, IDC_SIZEALL);
        break;

    case HT_LEFT:
    case HT_RIGHT:
        hCur = ::LoadCursor(NULL, IDC_SIZEWE);
        break;

    case HT_TOP:
    case HT_BOTTOM:
        hCur = ::LoadCursor(NULL, IDC_SIZENS);
        break;

    case HT_TOPLEFT:
    case HT_BOTTOMRIGHT:
        hCur = ::LoadCursor(NULL, IDC_SIZENWSE);
        break;

    case HT_TOPRIGHT:
    case HT_BOTTOMLEFT:
        hCur = ::LoadCursor(NULL, IDC_SIZENESW);
        break;

    case HT_NONE:
    default:
        hCur = ::LoadCursor(NULL, IDC_ARROW);
        break;
    }

    ::SetCursor(hCur);
}

afx_msg void COverlayDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    TRACE(_T("Hit\n"));
    if (m_bDragging && (nFlags & MK_LBUTTON) && m_hitIndex >= 0)
    {
        CPoint delta = point - m_lastPt;
        CRect rc = m_rcOrig;

        const int MIN_W = 10;
        const int MIN_H = 10;

        switch (m_hitType)
        {
        case HT_MOVE:
            rc.OffsetRect(delta);
            break;

        case HT_LEFT:
            rc.left += delta.x;
            if (rc.Width() < MIN_W) rc.left = rc.right - MIN_W;
            break;

        case HT_RIGHT:
            rc.right += delta.x;
            if (rc.Width() < MIN_W) rc.right = rc.left + MIN_W;
            break;

        case HT_TOP:
            rc.top += delta.y;
            if (rc.Height() < MIN_H) rc.top = rc.bottom - MIN_H;
            break;

        case HT_BOTTOM:
            rc.bottom += delta.y;
            if (rc.Height() < MIN_H) rc.bottom = rc.top + MIN_H;
            break;

        case HT_TOPLEFT:
            rc.left += delta.x;
            rc.top += delta.y;
            if (rc.Width() < MIN_W)  rc.left = rc.right - MIN_W;
            if (rc.Height() < MIN_H) rc.top = rc.bottom - MIN_H;
            break;

        case HT_TOPRIGHT:
            rc.right += delta.x;
            rc.top += delta.y;
            if (rc.Width() < MIN_W)  rc.right = rc.left + MIN_W;
            if (rc.Height() < MIN_H) rc.top = rc.bottom - MIN_H;
            break;

        case HT_BOTTOMLEFT:
            rc.left += delta.x;
            rc.bottom += delta.y;
            if (rc.Width() < MIN_W)  rc.left = rc.right - MIN_W;
            if (rc.Height() < MIN_H) rc.bottom = rc.top + MIN_H;
            break;

        case HT_BOTTOMRIGHT:
            rc.right += delta.x;
            rc.bottom += delta.y;
            if (rc.Width() < MIN_W)  rc.right = rc.left + MIN_W;
            if (rc.Height() < MIN_H) rc.bottom = rc.top + MIN_H;
            break;

        default:
            break;
        }

        m_rects[m_hitIndex].rc = rc;
        Invalidate(TRUE);
        UpdateWindow();
        return;
    }

    // 드래그 중이 아니면 커서만 변경
    HitType ht = HT_NONE;
    for (int i = (int)m_rects.size() - 1; i >= 0; --i)
    {
        ht = HitTestRect(m_rects[i].rc, point);
        if (ht != HT_NONE)
            break;
    }

    UpdateCursorByHitType(ht);

    TRACE(_T("HitType = %d\n"), (int)ht);

    CDialogEx::OnMouseMove(nFlags, point);
}


void COverlayDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    CDialogEx::OnLButtonDown(nFlags, point);

    m_bDragging = false;
    m_hitIndex = -1;
    m_hitType = HT_NONE;

    for (int i = (int)m_rects.size() - 1; i >= 0; --i)
    {
        const CRect& rc = m_rects[i].rc;
        HitType ht = HitTestRect(rc, point);
        if (ht != HT_NONE)
        {
            m_bDragging = true;
            m_hitIndex = i;
            m_hitType = ht;
            m_lastPt = point;
            m_rcOrig = rc;
            SetCapture();
            break;
        }
    }

    Invalidate(TRUE);
    UpdateWindow();
}

void COverlayDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bDragging)
    {
        ReleaseCapture();
        m_bDragging = false;
        m_hitIndex = -1;
        m_hitType = HT_NONE;
    }

    Invalidate(TRUE);
    UpdateWindow();
    CDialogEx::OnLButtonUp(nFlags, point);
}

LRESULT COverlayDlg::OnAddRect(WPARAM wParam, LPARAM lParam) {
    OverlayRect r;

    CRect rcScreen;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);

    const int w = 100;
    const int h = 100;

    int cx = rcScreen.left + (rcScreen.Width() - w) / 2;
    int cy = rcScreen.top + (rcScreen.Height() - h) / 2;

    r.rc = CRect(cx, cy, cx + w, cy + h);
    r.selected = false;

    m_rects.push_back(r);

    Invalidate(TRUE);
    UpdateWindow();

    return 0;
}

LRESULT COverlayDlg::OnClearRect(WPARAM wParam, LPARAM lParam) {
    m_rects.clear();

    Invalidate(TRUE);
    UpdateWindow();
    return 0;
}

LRESULT COverlayDlg::OnCloseSetting(WPARAM wParam, LPARAM lParam) {
    LONG exStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);

    exStyle |= (WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);

    SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle);
    SetWindowPos(nullptr, 0, 0, 0, 0,
        SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
    return 0;
}
