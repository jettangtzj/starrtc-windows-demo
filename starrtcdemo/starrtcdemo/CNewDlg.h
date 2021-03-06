#pragma once


// CNewDlg 对话框

class CNewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewDlg)

public:
	CNewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_NEW_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTitle;
	CString m_strText;
	CString m_strSureButton;
	CButton m_Text;
	CEdit m_edit;
	CButton m_SureButton;
	CString m_strContent;
	afx_msg void OnBnClickedButtonStartNew();
	virtual BOOL OnInitDialog();
};
