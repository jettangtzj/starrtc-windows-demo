#pragma once
#include <string>
using namespace std;
// CVoipP2PDlg 对话框

class CVoipP2PDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVoipP2PDlg)

public:
	CVoipP2PDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CVoipP2PDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VOIP_P2P };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCalling();
	virtual BOOL OnInitDialog();
private:
	string m_strLocalIP;
	string m_strAimIP;
	CIPAddressCtrl m_LocalIP;
	CIPAddressCtrl m_AimIP;	
};
