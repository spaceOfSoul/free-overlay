// CSettingDig.cpp: 구현 파일
//

#include "pch.h"
#include "Overlay.h"
#include "afxdialogex.h"
#include "CSettingDig.h"


// CSettingDig 대화 상자

IMPLEMENT_DYNAMIC(CSettingDig, CDialogEx)

CSettingDig::CSettingDig(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROPPAGE, pParent)
{

}

CSettingDig::~CSettingDig()
{
}

void CSettingDig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDig, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_NEW_RECT, &CSettingDig::OnBnClickedBtnNewRect)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CSettingDig::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_SAVE_AND_CLOSE, &CSettingDig::OnBnClickedBtnSaveAndClose)
END_MESSAGE_MAP()


// CSettingDig 메시지 처리기

void CSettingDig::OnBnClickedBtnNewRect()
{
    CWnd* pParent = GetParent();
    if (pParent && ::IsWindow(pParent->GetSafeHwnd()))
    {
        pParent->SendMessage(WM_ADD_RECT, 0, 0);
    }
}

void CSettingDig::OnBnClickedBtnClear()
{
    CWnd* pParent = GetParent();
    if (pParent && ::IsWindow(pParent->GetSafeHwnd()))
    {
        pParent->SendMessage(WM_CLEAR_RECT, 0, 0);
    }
}

void CSettingDig::OnBnClickedBtnSaveAndClose()
{
    CWnd* pParent = GetParent();
    if (pParent && ::IsWindow(pParent->GetSafeHwnd()))
    {
        pParent->SendMessage(WM_CLOSE_SETTING, 0, 0);
    }

    DestroyWindow();
}