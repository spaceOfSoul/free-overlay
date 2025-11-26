
// OverlayDlg.h: 헤더 파일
//

#pragma once
#include <shellapi.h>

#define WM_TRAYICON (WM_APP + 1)
#define TRAY_ICON_ID 1

// COverlayDlg 대화 상자
class COverlayDlg : public CDialogEx
{
// 생성입니다.
public:
	COverlayDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OVERLAY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	NOTIFYICONDATA m_nid;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnTrayOverlaySetting();
	afx_msg void OnTrayExit();

	DECLARE_MESSAGE_MAP()
};
