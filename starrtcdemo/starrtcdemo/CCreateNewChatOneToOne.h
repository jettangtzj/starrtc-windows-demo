#pragma once


// CCreateNewChatOneToOne 对话框

class CCreateNewChatOneToOne : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateNewChatOneToOne)

public:
	CCreateNewChatOneToOne(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateNewChatOneToOne();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CREATE_NEW_CHAT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_userNameEdit;
	afx_msg void OnBnClickedButtonStartNewChat();
	CString m_strUserName;
};
