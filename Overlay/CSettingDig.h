#pragma once
#include "afxdialogex.h"

// CSettingDig 대화 상자

class CSettingDig : public CDialogEx
{
	DECLARE_DYNAMIC(CSettingDig)

public:
	CSettingDig(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSettingDig();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnNewRect();
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnSaveAndClose();
};
