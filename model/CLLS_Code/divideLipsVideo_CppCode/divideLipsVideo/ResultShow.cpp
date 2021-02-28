// ResultShow.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCtest1.h"
#include "ResultShow.h"
#include "afxdialogex.h"
#include "MFCtest1Dlg.h"

// ResultShow 对话框

IMPLEMENT_DYNAMIC(ResultShow, CDialogEx)

ResultShow::ResultShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(ResultShow::IDD, pParent)
{

}

ResultShow::~ResultShow()
{
}

void ResultShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ResultShow, CDialogEx)
	ON_BN_CLICKED(IDC_MENU_BUTTON1, &ResultShow::OnBnClickedMenuButton1)
	ON_BN_CLICKED(IDC_EXIT_BUTTON2, &ResultShow::OnBnClickedExitButton2)
END_MESSAGE_MAP()


// ResultShow 消息处理程序


void ResultShow::OnBnClickedMenuButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	ResultShow::OnCancel();
	CMFCtest1Dlg Menu;
	Menu.DoModal();
}


void ResultShow::OnBnClickedExitButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	ResultShow::OnCancel();
}
