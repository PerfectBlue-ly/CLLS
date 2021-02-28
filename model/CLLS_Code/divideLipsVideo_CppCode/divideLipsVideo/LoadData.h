#pragma once


// LoadData 对话框

class LoadData : public CDialogEx
{
	DECLARE_DYNAMIC(LoadData)

public:
	LoadData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~LoadData();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVideoButton1();
	afx_msg void OnBnClickedJsonButton2();
	afx_msg void OnBnClickedProcessButton4();
	afx_msg void OnBnClickedProcessButton5();
	afx_msg void OnBnClickedMenu1Button1();
	afx_msg void OnBnClickedExit1Button2();
};
