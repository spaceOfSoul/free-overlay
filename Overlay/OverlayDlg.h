
// OverlayDlg.h: 헤더 파일
//

#pragma once
#include <shellapi.h>
#include <vector>
#include "CSettingDig.h"

#define WM_TRAYICON (WM_APP + 1)
#define TRAY_ICON_ID 1

enum HitType {
	HT_NONE = 0,
	HT_MOVE,
	HT_LEFT,
	HT_RIGHT,
	HT_TOP,
	HT_BOTTOM,
	HT_TOPLEFT,
	HT_TOPRIGHT,
	HT_BOTTOMLEFT,
	HT_BOTTOMRIGHT
};

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

	// setting dig
	CSettingDig m_settingDig;

	// drag implement
	bool m_editModeFlag = false;

	struct OverlayRect {
		CRect rc;
		bool  selected;
	};
	std::vector<OverlayRect> m_rects;

	bool    m_bDragging = false;
	HitType m_hitType = HT_NONE;
	int     m_hitIndex = -1;
	CPoint  m_lastPt;
	CRect   m_rcOrig;

	HitType COverlayDlg::HitTestRect(const CRect& rc, CPoint pt);
	void COverlayDlg::UpdateCursorByHitType(HitType ht);
	void COverlayDlg::OnLButtonDown(UINT nFlags, CPoint point);
	void COverlayDlg::OnLButtonUp(UINT nFlags, CPoint point);

	LRESULT COverlayDlg::OnAddRect(WPARAM wParam, LPARAM lParam);
	LRESULT COverlayDlg::OnClearRect(WPARAM wParam, LPARAM lParam);
	LRESULT COverlayDlg::OnCloseSetting(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
