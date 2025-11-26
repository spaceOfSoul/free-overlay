
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

    // TRUE/FALSE 상관없지만, 여기서는 TRUE 반환
    return TRUE;
}

void COverlayDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    // 검은 네모 위치 설정 (화면 중앙에 200x200 예시)
    CRect rcClient;
    GetClientRect(&rcClient);

    int width = 200;
    int height = 200;
    int left = (rcClient.Width() - width) / 2;
    int top = (rcClient.Height() - height) / 2;
    int right = left + width;
    int bottom = top + height;

    CBrush blackBrush(RGB(0, 0, 0));
    CBrush* pOldBrush = dc.SelectObject(&blackBrush);

    dc.Rectangle(left, top, right, bottom);

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
            // 선택된 메뉴 ID를 그대로 WM_COMMAND로 처리
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
    return HTTRANSPARENT;
}

void COverlayDlg::OnTrayOverlaySetting()
{
    // 설정창 구현 예정
}

void COverlayDlg::OnTrayExit()
{
    Shell_NotifyIcon(NIM_DELETE, &m_nid);

    PostQuitMessage(0);
}
