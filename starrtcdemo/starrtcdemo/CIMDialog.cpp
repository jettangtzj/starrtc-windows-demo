// CIMDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "CIMDialog.h"
#include "afxdialogex.h"
#include "CIMOneToOneDialog.h"
#include "CChatroomDlg.h"
#include "CGroupDialog.h"
// CIMDialog 对话框

IMPLEMENT_DYNAMIC(CIMDialog, CDialogEx)

CIMDialog::CIMDialog(CUserManager* pUserManager, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_IM, pParent)
{
	m_pUserManager = pUserManager;
}

CIMDialog::~CIMDialog()
{
}

void CIMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIMDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_IM_ONE_TO_ONE, &CIMDialog::OnBnClickedButtonImOneToOne)
	ON_BN_CLICKED(IDC_BUTTON_CHATROOM, &CIMDialog::OnBnClickedButtonChatroom)
	ON_BN_CLICKED(IDC_BUTTON_IM_GROUP, &CIMDialog::OnBnClickedButtonImGroup)
END_MESSAGE_MAP()


// CIMDialog 消息处理程序


void CIMDialog::OnBnClickedButtonImOneToOne()
{
	CIMOneToOneDialog dlg;
	dlg.DoModal();
}


void CIMDialog::OnBnClickedButtonChatroom()
{
	CChatroomDlg dlg;
	dlg.DoModal();
}


void CIMDialog::OnBnClickedButtonImGroup()
{
	CGroupDialog dlg;
	dlg.DoModal();
}
