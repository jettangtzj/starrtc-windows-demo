// CGroupDialog.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "CGroupDialog.h"
#include "afxdialogex.h"
#include "CCreateNewGroupDlg.h"
#include "CNewDlg.h"
#include "CUtil.h"
// CGroupDialog 对话框

IMPLEMENT_DYNAMIC(CGroupDialog, CDialogEx)

CGroupDialog::CGroupDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_IM_GROUP, pParent)
{
	m_pCurrentGroup = NULL;
	m_pGroupManager = new XHGroupManager(this);
	XHGroupManager::addGroupGetListListener(this);
	m_pSqliteDB = new CSqliteDB();
	m_pSqliteDB->openDB("chatDB.db");
	
}

CGroupDialog::~CGroupDialog()
{
	XHGroupManager::addGroupGetListListener(NULL);
	if (m_pGroupManager != NULL)
	{
		delete m_pGroupManager;
		m_pGroupManager = NULL;
	}
	if (m_pCurrentGroup != NULL)
	{
		delete m_pCurrentGroup;
		m_pCurrentGroup = NULL;
	}
	clearMsgList();
}

void CGroupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GROUP_LIST, m_groupList);
	DDX_Control(pDX, IDC_LIST_GROUP_HISTORY_MSG, m_recvDataListBox);
	DDX_Control(pDX, IDC_EDIT_GROUP_SEND_MSG, m_sendMsgEdit);
	DDX_Control(pDX, IDC_STATIC_GROUP_NAME, m_GroupName);
	DDX_Control(pDX, IDC_LIST_GROUP_MEMBER_LIST, m_GroupMemberList);
}


BEGIN_MESSAGE_MAP(CGroupDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_GROUP, &CGroupDialog::OnBnClickedButtonCreateGroup)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_SEND_MSG, &CGroupDialog::OnBnClickedButtonGroupSendMsg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_GROUP_LIST, &CGroupDialog::OnNMClickListGroupList)
	ON_COMMAND(ID_MENU_GROUP_ADD_MEM, &CGroupDialog::OnMenuGroupAddMem)
	ON_COMMAND(ID_MENU_GROUP_SET_RECVMSG_STATUS, &CGroupDialog::OnMenuGroupSetRecvmsgStatus)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_GROUP_LIST, &CGroupDialog::OnNMRClickListGroupList)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_ADD_MEMBER, &CGroupDialog::OnBnClickedButtonGroupAddMember)
	ON_BN_CLICKED(IDC_BUTTON__GROUP_LIST_BRUSH, &CGroupDialog::OnBnClickedButton)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_GROUP, &CGroupDialog::OnBnClickedButtonDeleteGroup)
END_MESSAGE_MAP()


// CGroupDialog 消息处理程序


BOOL CGroupDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LONG lStyle;
	lStyle = GetWindowLong(m_groupList.m_hWnd, GWL_STYLE);// 获取当前窗口style 
	lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
	lStyle |= LVS_REPORT; // 设置style 
	SetWindowLong(m_groupList.m_hWnd, GWL_STYLE, lStyle);// 设置style 
	DWORD dwStyle = m_groupList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_groupList.SetExtendedStyle(dwStyle); // 设置扩展风格 
	// 载入64*64像素 24位真彩(ILC_COLOR24)图片

	CImageList m_imList;
	m_imList.Create(64, 64, ILC_COLOR24, 10, 20);    // 创建图像序列CImageList对象
	CBitmap * pBmp = NULL;
	pBmp = new CBitmap();
	pBmp->LoadBitmap("1.jpg");              // 载入位图IDB_BITMAP1
	//pBmp->LoadBitmap(IDB_BITMAP1);
	m_imList.Add(pBmp, RGB(0, 0, 0));
	delete pBmp;


	// 设置CImageList图像列表与CListCtrl控件关联 LVSIL_SMALL小图标列表
	m_groupList.SetImageList(&m_imList, LVSIL_SMALL);
	CRect mRect;
	m_groupList.GetWindowRect(&mRect);                     // 获取控件矩形区域
	int length = mRect.Width();
	m_groupList.InsertColumn(0, _T("图像"), LVCFMT_LEFT, length / 10, -1);
	m_groupList.InsertColumn(1, _T("名称"), LVCFMT_LEFT, length * 3 / 7, -1);
	m_groupList.InsertColumn(2, _T("创建者"), LVCFMT_LEFT, length / 4, -1);
	m_groupList.InsertColumn(3, _T("id"), LVCFMT_LEFT, length / 4, -1);


	lStyle = GetWindowLong(m_GroupMemberList.m_hWnd, GWL_STYLE);// 获取当前窗口style 
	lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
	lStyle |= LVS_REPORT; // 设置style 
	SetWindowLong(m_GroupMemberList.m_hWnd, GWL_STYLE, lStyle);// 设置style 
	dwStyle = m_GroupMemberList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_GroupMemberList.SetExtendedStyle(dwStyle); // 设置扩展风格 
	// 载入64*64像素 24位真彩(ILC_COLOR24)图片

	CImageList m_imList1;
	m_imList1.Create(64, 64, ILC_COLOR24, 10, 20);    // 创建图像序列CImageList对象
	CBitmap * pBmp1 = NULL;
	pBmp1 = new CBitmap();
	pBmp1->LoadBitmap("1.jpg");              // 载入位图IDB_BITMAP1
	//pBmp->LoadBitmap(IDB_BITMAP1);
	m_imList1.Add(pBmp1, RGB(0, 0, 0));
	delete pBmp1;


	// 设置CImageList图像列表与CListCtrl控件关联 LVSIL_SMALL小图标列表
	m_GroupMemberList.SetImageList(&m_imList, LVSIL_SMALL);
	m_GroupMemberList.GetWindowRect(&mRect);                     // 获取控件矩形区域
	length = mRect.Width();
	m_GroupMemberList.InsertColumn(0, _T("群成员"), LVCFMT_LEFT, length / 5, -1);
	m_GroupMemberList.InsertColumn(1, _T(""), LVCFMT_LEFT, length * 4 / 5, -1);

	getGroupList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CGroupDialog::getGroupList()
{
	XHGroupManager::getGroupList();
}

void CGroupDialog::resetGroupList()
{
	m_groupList.DeleteAllItems();
	int nRow = 0;
	for (list<CGroupInfo>::iterator it = mGroupList.begin(); it != mGroupList.end(); ++it)
	{
		LVITEM lvItem = { 0 };                               // 列表视图控 LVITEM用于定义"项"的结构
		//第一行数据
		lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;   // 文字、图片、状态
		lvItem.iItem = nRow;                                // 行号(第一行)
		lvItem.iImage = 0;                               // 图片索引号(第一幅图片 IDB_BITMAP1)
		lvItem.iSubItem = 0;                             // 子列号
		nRow = m_groupList.InsertItem(&lvItem);               // 第一列为图片
		m_groupList.SetItemText(nRow, 1, it->m_strName.c_str());            // 第二列为名字
		m_groupList.SetItemText(nRow, 2, it->m_strCreaterId.c_str());     // 第三列为格言
		m_groupList.SetItemText(nRow, 3, it->m_strId.c_str());     // 第三列为格言
		nRow++;
	}
}

void CGroupDialog::OnBnClickedButtonCreateGroup()
{
	CCreateNewGroupDlg dlg;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	if (m_pCurrentGroup != NULL)
	{
		delete m_pCurrentGroup;
		m_pCurrentGroup = NULL;
	}
	
	if (m_pGroupManager != NULL)
	{
		string strId = m_pGroupManager->createGroup(m_pCurrentGroup->m_strName);
		if (strId != "")
		{
			m_pCurrentGroup = new CGroupInfo();
			m_pCurrentGroup->m_strName = dlg.m_strGroupName.GetBuffer(0);
			m_pCurrentGroup->m_strId = strId;
		}
	}
	getGroupList();
}


void CGroupDialog::OnBnClickedButtonGroupSendMsg()
{
	CString strMsg = "";
	m_sendMsgEdit.GetWindowText(strMsg);

	if (strMsg == "")
	{
		return;
	}
	list<string> atUserIDs;
	CIMMessage* pIMMessage = m_pGroupManager->sendMessage((char*)m_pCurrentGroup->m_strId.c_str(), atUserIDs, strMsg.GetBuffer(0));

	CMessageBean* pMessageBean = new CMessageBean();
	pMessageBean->setConversationId(pIMMessage->m_strTargetId);
	pMessageBean->setTime(CUtil::getTime());
	pMessageBean->setMsg(pIMMessage->m_strContentData);
	pMessageBean->setFromId(pIMMessage->m_strFromId);

	if (m_pSqliteDB != NULL)
	{
		m_pSqliteDB->setMessage(pMessageBean);
	}
	mDatas.push_front(pMessageBean);

	resetMsgList();
	m_sendMsgEdit.SetSel(0, -1); // 选中所有字符
	m_sendMsgEdit.ReplaceSel(_T(""));
}

/**
 * 获取group list回调函数
 */
int CGroupDialog::applyGetGroupListFin(list<CGroupInfo>& groupInfoList)
{
	mGroupList.clear();
	list<CGroupInfo>::iterator iter = groupInfoList.begin();
	for (; iter != groupInfoList.end(); iter++)
	{
		mGroupList.push_back(*iter);
	}
	resetGroupList();
	return 0;
}

/**
 * 查询用户列表回调
 */
int CGroupDialog::applyGetUserListFin(list<string>& userList)
{
	mGroupMemberList.clear();
	list<string>::iterator iter = userList.begin();
	for (; iter != userList.end(); iter++)
	{
		mGroupMemberList.push_back(*iter);
	}
	resetGroupMemberList();
	return 0;
}

/**
 * 群成员数发生变化
 * @param groupID
 * @param number
 */
void CGroupDialog::onMembersUpdeted(string groupID, int number)
{
}

/**
 * 自己被移出群组
 * @param groupID
 */
void CGroupDialog::onSelfKicked(string groupID)
{
}

/**
 * 群组已经被删除
 * @param groupID
 */
void CGroupDialog::onGroupDeleted(string groupID)
{
}

/**
 *  收到消息
 * @param message
 */
void CGroupDialog::onReceivedMessage(string groupId, CIMMessage* pMessage)
{
	CMessageBean* pMessageBean = new CMessageBean();
	pMessageBean->setConversationId(pMessage->m_strFromId);
	pMessageBean->setTime(CUtil::getTime());
	pMessageBean->setMsg(pMessage->m_strContentData);
	pMessageBean->setFromId(pMessage->m_strFromId);

	if (m_pSqliteDB != NULL)
	{
		m_pSqliteDB->setMessage(pMessageBean);
	}
	
	if (m_pCurrentGroup->m_strId == groupId)
	{
		mDatas.push_front(pMessageBean);
		resetMsgList();
	}
}

void CGroupDialog::onApplyCreateGroupFin(string groupID)
{
	m_pCurrentGroup->m_strId = groupID;
	m_GroupName.SetWindowText(m_pCurrentGroup->m_strName.c_str());
	getGroupMemberList(m_pCurrentGroup->m_strId);
	getGroupList();
}

void CGroupDialog::onApplyDelGroupFin(string groupID)
{
	if (m_pCurrentGroup->m_strId == groupID)
	{
		delete m_pCurrentGroup;
		m_pCurrentGroup = NULL;
		m_GroupName.SetWindowText("");
		mGroupMemberList.clear();
		resetGroupMemberList();
	}
	getGroupList();
}

void CGroupDialog::onApplyAddUserToGroupFin(string groupID)
{
	if (m_pCurrentGroup->m_strId == groupID)
	{
		getGroupMemberList(groupID);
	}	
}

void CGroupDialog::onApplyRemoveUserFromGroupFin(string groupID)
{
	if (m_pCurrentGroup->m_strId == groupID)
	{
		getGroupMemberList(groupID);
	}	
}

void CGroupDialog::clearMsgList()
{
	for (list<CMessageBean*>::iterator it = mDatas.begin(); it != mDatas.end(); ++it)
	{
		if (*it != NULL)
		{
			delete *it;
			*it = NULL;
		}
	}
	mDatas.clear();
}

void CGroupDialog::getMsgList(string conversationId)
{
	clearMsgList();
	if (m_pSqliteDB != NULL)
	{
		list<CMessageBean*> msgList = m_pSqliteDB->getMessageList(conversationId);
		if (msgList.size() > 0)
		{
			mDatas.splice(mDatas.begin(), msgList);
		}
	}
}

void CGroupDialog::resetMsgList()
{
	m_recvDataListBox.ResetContent();
	for (list<CMessageBean*>::iterator it = mDatas.begin(); it != mDatas.end(); ++it)
	{
		if (*it != NULL)
		{
			CString strMsg = "";
			strMsg.Format("%s:%s", (*it)->getFromId().c_str(), (*it)->getMsg().c_str());
			m_recvDataListBox.InsertString(0, strMsg);
		}
	}

}

void CGroupDialog::getGroupMemberList(string strGroupId)
{
	XHGroupManager::getUserList(strGroupId);
}

void CGroupDialog::resetGroupMemberList()
{
	m_GroupMemberList.DeleteAllItems();
	int nRow = 0;
	for (list<string>::iterator it = mGroupMemberList.begin(); it != mGroupMemberList.end(); ++it)
	{
		LVITEM lvItem = { 0 };                               // 列表视图控 LVITEM用于定义"项"的结构
		//第一行数据
		lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;   // 文字、图片、状态
		lvItem.iItem = nRow;                                // 行号(第一行)
		lvItem.iImage = 0;                               // 图片索引号(第一幅图片 IDB_BITMAP1)
		lvItem.iSubItem = 0;                             // 子列号
		nRow = m_GroupMemberList.InsertItem(&lvItem);               // 第一列为图片
		m_GroupMemberList.SetItemText(nRow, 1, (*it).c_str());            // 第二列为名字
		nRow++;
	}
}

void CGroupDialog::OnNMClickListGroupList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	*pResult = 0;
	int nItem = -1;
	if (pNMItemActivate != NULL)
	{
		nItem = pNMItemActivate->iItem;
		if (nItem >= 0)
		{
			CString str = m_groupList.GetItemText(nItem, 1);
			//int pos = str.Find("\r\n");
			//str = str.Left(pos);

			CString strAimId;
			m_GroupName.GetWindowText(strAimId);
			if (strAimId == str)
			{
				return;
			}

			if (m_pCurrentGroup != NULL)
			{
				delete m_pCurrentGroup;
				m_pCurrentGroup = NULL;
			}
			m_pCurrentGroup = new CGroupInfo();
			m_pCurrentGroup->m_strName = m_groupList.GetItemText(nItem, 1);
			m_pCurrentGroup->m_strCreaterId = m_groupList.GetItemText(nItem, 2);
			m_pCurrentGroup->m_strId = m_groupList.GetItemText(nItem, 3);

			getMsgList(m_pCurrentGroup->m_strId);
			//重绘
			resetMsgList();
			m_GroupName.SetWindowText(str);
			getGroupMemberList(m_pCurrentGroup->m_strId);
		}
	}
}


void CGroupDialog::OnMenuGroupAddMem()
{
	OnBnClickedButtonGroupAddMember();
}


void CGroupDialog::OnMenuGroupSetRecvmsgStatus()
{
	//m_pGroupManager->();
}


void CGroupDialog::OnNMRClickListGroupList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	*pResult = 0;

	if (pNMItemActivate != NULL)
	{
		int nItem = -1;
		nItem = pNMItemActivate->iItem;
		if (nItem >= 0)
		{
			CMenu menu;
			CMenu *pSubMenu;
			menu.LoadMenu(IDR_MENU2);
			pSubMenu = menu.GetSubMenu(0);
			CPoint point1;
			GetCursorPos(&point1);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point1.x, point1.y, this);
			pSubMenu->DestroyMenu();
			menu.DestroyMenu();
		}
	}
}


void CGroupDialog::OnBnClickedButtonGroupAddMember()
{
	CNewDlg dlg;
	dlg.m_strText = "请输入用户Id";
	dlg.m_strSureButton = "加入";
	dlg.m_strTitle = "添加成员";

	if (dlg.DoModal() == IDOK)
	{
		if (m_pGroupManager != NULL && m_pCurrentGroup != NULL)
		{
			list<string> memlist;
			memlist.push_back(dlg.m_strContent.GetBuffer(0));
			m_pGroupManager->addGroupMembers(m_pCurrentGroup->m_strId, memlist);
			getGroupMemberList(m_pCurrentGroup->m_strId);
		}
	}
}


void CGroupDialog::OnBnClickedButton()
{
	getGroupList();
}


void CGroupDialog::OnBnClickedButtonDeleteGroup()
{
	CNewDlg dlg;
	dlg.m_strText = "请输入删除的群组Id";
	dlg.m_strSureButton = "确定";
	dlg.m_strTitle = "删除群组";
	if (dlg.DoModal() == IDOK)
	{
		bool bRet = m_pGroupManager->deleteGroup(dlg.m_strContent.GetBuffer(0));
		if (!bRet)
		{
			CString errInfo = "";
			errInfo.Format("创建 %s 群组失败!", dlg.m_strContent.GetBuffer(0));
			AfxMessageBox(errInfo);
		}
		getGroupList();
	}
}
