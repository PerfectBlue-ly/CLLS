#pragma once


// ResultShow 对话框

class ResultShow : public CDialogEx
{
	DECLARE_DYNAMIC(ResultShow)

public:
	ResultShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ResultShow();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMenuButton1();
	afx_msg void OnBnClickedExitButton2();
};
