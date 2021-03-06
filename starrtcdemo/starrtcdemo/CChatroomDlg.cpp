// CChatroomDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "starrtcdemo.h"
#include "CChatroomDlg.h"
#include "afxdialogex.h"
#include "CCreateNewChatroomDlg.h"
#include "CNewDlg.h"
// CChatroomDlg 对话框

IMPLEMENT_DYNAMIC(CChatroomDlg, CDialogEx)

CChatroomDlg::CChatroomDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CHATROOM, pParent)
{
	XHChatroomManager::addChatroomGetListListener(this);
	m_pChatroomManager = new XHChatroomManager(this);
	m_pCurrentShow = NULL;
}

CChatroomDlg::~CChatroomDlg()
{
	XHChatroomManager::addChatroomGetListListener(NULL);
	if (m_pChatroomManager = NULL)
	{
		delete m_pChatroomManager;
		m_pChatroomManager = NULL;
	}
}

void CChatroomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHATROOM_LIST, m_ChatroomList);
	DDX_Control(pDX, IDC_LIST_CHATROOM_HISTORY_MSG, m_HistoryMsgListBox);
	DDX_Control(pDX, IDC_EDIT_CHATROOM_SEND_MSG, m_SendMsgEdit);
	DDX_Control(pDX, IDC_STATIC_CHATROOM_NAME, m_ChatroomName);
}


BEGIN_MESSAGE_MAP(CChatroomDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CHATROOM_SEND_MSG, &CChatroomDlg::OnBnClickedButtonChatroomSendMsg)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_CHATROOM, &CChatroomDlg::OnBnClickedButtonCreateChatroom)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CHATROOM_LIST, &CChatroomDlg::OnNMClickListChatroomList)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CHATROOM_LIST, &CChatroomDlg::OnNMRClickListChatroomList)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_CHATROOM, &CChatroomDlg::OnBnClickedButtonDeleteChatroom)
END_MESSAGE_MAP()


// CChatroomDlg 消息处理程序

void CChatroomDlg::getChatroomList()
{
	XHChatroomManager::getChatroomList(CHATROOM_LIST_TYPE_CHATROOM);
}

void CChatroomDlg::OnBnClickedButtonChatroomSendMsg()
{
	CString strMsg = "";
	m_SendMsgEdit.GetWindowText(strMsg);

	if (strMsg == "")
	{
		return;
	}
	if (m_pChatroomManager != NULL && m_pCurrentShow != NULL)
	{
		CIMMessage* pIMMessage = m_pChatroomManager->sendMessage(strMsg.GetBuffer(0));
		CString strMsg = "";
		strMsg.Format("%s:%s", pIMMessage->m_strFromId.c_str(), pIMMessage->m_strContentData.c_str());
		m_HistoryMsgListBox.InsertString(m_HistoryMsgListBox.GetCount(), strMsg);
		delete pIMMessage;
		pIMMessage = NULL;
		m_SendMsgEdit.SetSel(0, -1); // 选中所有字符
		m_SendMsgEdit.ReplaceSel(_T(""));
	}
}


void CChatroomDlg::OnBnClickedButtonCreateChatroom()
{
	CCreateNewChatroomDlg dlg;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	m_HistoryMsgListBox.ResetContent();
	m_SendMsgEdit.SetSel(0, -1); // 选中所有字符
	m_SendMsgEdit.ReplaceSel(_T(""));
	if (m_pCurrentShow != NULL)
	{
		delete m_pCurrentShow;
		m_pCurrentShow = NULL;
	}
	
	if (m_pChatroomManager != NULL)
	{
		CString errInfo = "";
		KillTimer(0);
		m_pChatroomManager->exitChatroom(m_pCurrentShow->m_strRoomId);

		CHATROOM_TYPE chatRoomType = CHATROOM_TYPE::CHATROOM_TYPE_PUBLIC;
		bool bRet = m_pChatroomManager->createChatRoom(dlg.m_strRoomName.GetBuffer(0), chatRoomType);
		if (!bRet)
		{
			errInfo.Format("创建 %s 聊天室失败!", dlg.m_strRoomName);
			AfxMessageBox(errInfo);
		}
		else
		{
			bRet = m_pChatroomManager->joinChatRoom(m_pChatroomManager->getChatroomId());
			if (!bRet)
			{
				errInfo.Format("加入 %s 聊天室失败!", m_pCurrentShow->m_strName.c_str());
				AfxMessageBox(errInfo);
			}
			else
			{
				m_pCurrentShow = new ChatroomInfo();
				m_pCurrentShow->m_strName = dlg.m_strRoomName;
				m_pCurrentShow->m_strRoomId = m_pChatroomManager->getChatroomId();
				m_ChatroomName.SetWindowText(m_pCurrentShow->m_strName.c_str());
				SetTimer(0, 5000, 0);
			}
			getChatroomList();
			//resetChatroomList();
		}

	}
}


void CChatroomDlg::OnNMClickListChatroomList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	int nItem = -1;
	if (pNMItemActivate != NULL)
	{
		nItem = pNMItemActivate->iItem;
		if (nItem >= 0)
		{
			if (m_pCurrentShow != NULL)
			{
				delete m_pCurrentShow;
				m_pCurrentShow = NULL;
			}
			m_HistoryMsgListBox.ResetContent();
			m_SendMsgEdit.SetSel(0, -1); // 选中所有字符
			m_SendMsgEdit.ReplaceSel(_T(""));

			m_pCurrentShow = new ChatroomInfo();
			m_pCurrentShow->m_strName = m_ChatroomList.GetItemText(nItem, 1);
			m_pCurrentShow->m_strCreaterId = m_ChatroomList.GetItemText(nItem, 2);
			m_pCurrentShow->m_strRoomId = m_ChatroomList.GetItemText(nItem, 3);
			m_ChatroomName.SetWindowText(m_pCurrentShow->m_strName.c_str());
			if (m_pChatroomManager != NULL)
			{
				CString errInfo = "";
				KillTimer(0);
				m_pChatroomManager->exitChatroom(m_pChatroomManager->getChatroomId());
				bool bRet = m_pChatroomManager->joinChatRoom(m_pCurrentShow->m_strRoomId);
				if (!bRet)
				{	
					errInfo.Format("加入 %s 聊天室失败!", m_pCurrentShow->m_strName.c_str());
					AfxMessageBox(errInfo);
				}
				else
				{
					SetTimer(0, 3000, 0);
				}			
			}
		}
	}
}

void CChatroomDlg::resetChatroomList()
{
	m_ChatroomList.DeleteAllItems();
	int nRow = 0;
	for (list<ChatroomInfo>::iterator it = mDatas.begin(); it != mDatas.end(); ++it)
	{	
		LVITEM lvItem = { 0 };                               // 列表视图控 LVITEM用于定义"项"的结构
		//第一行数据
		lvItem.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_STATE;   // 文字、图片、状态
		lvItem.iItem = nRow;                                // 行号(第一行)
		lvItem.iImage = 0;                               // 图片索引号(第一幅图片 IDB_BITMAP1)
		lvItem.iSubItem = 0;                             // 子列号
		nRow = m_ChatroomList.InsertItem(&lvItem);               // 第一列为图片
		m_ChatroomList.SetItemText(nRow, 1, it->m_strName.c_str());            // 第二列为名字
		m_ChatroomList.SetItemText(nRow, 2, it->m_strCreaterId.c_str());     // 第三列为格言
		m_ChatroomList.SetItemText(nRow, 3, it->m_strRoomId.c_str());     // 第三列为格言
		nRow++;
	}
}

BOOL CChatroomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	LONG lStyle;
	lStyle = GetWindowLong(m_ChatroomList.m_hWnd, GWL_STYLE);// 获取当前窗口style 
	lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
	lStyle |= LVS_REPORT; // 设置style 
	SetWindowLong(m_ChatroomList.m_hWnd, GWL_STYLE, lStyle);// 设置style 
	DWORD dwStyle = m_ChatroomList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;// 选中某行使整行高亮（只适用与report 风格的listctrl ） 
	dwStyle |= LVS_EX_SUBITEMIMAGES;
	m_ChatroomList.SetExtendedStyle(dwStyle); // 设置扩展风格 
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
	m_ChatroomList.SetImageList(&m_imList, LVSIL_SMALL);
	CRect mRect;
	m_ChatroomList.GetWindowRect(&mRect);                     // 获取控件矩形区域
	int length = mRect.Width();
	m_ChatroomList.InsertColumn(0, _T("图像"), LVCFMT_LEFT, length /10, -1);
	m_ChatroomList.InsertColumn(1, _T("名称"), LVCFMT_LEFT, length *2/7, -1);
	m_ChatroomList.InsertColumn(2, _T("创建者"), LVCFMT_LEFT, length /5, -1);
	m_ChatroomList.InsertColumn(3, _T("id"), LVCFMT_LEFT, length / 2, -1);

	getChatroomList();
	//resetChatroomList();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
/**
 * 查询聊天室列表回调
 */
int CChatroomDlg::chatroomQueryAllListOK(list<ChatroomInfo>& listData)
{
	mDatas.clear();
	list<ChatroomInfo>::iterator iter = listData.begin();
	for (; iter != listData.end(); iter++)
	{
		mDatas.push_back(*iter);
	}
	resetChatroomList();
	return 0;
}

/**
 * 聊天室成员数变化
 * @param number
 */
void CChatroomDlg::onMembersUpdated(int number)
{
	if (m_pCurrentShow != NULL)
	{
		CString str = "";
		str.Format("%s(%d)", m_pCurrentShow->m_strName.c_str(), number);
		m_ChatroomName.SetWindowText(str);
	}
}

/**
 * 自己被踢出聊天室
 */
void CChatroomDlg::onSelfKicked()
{
}

/**
 * 自己被踢出聊天室
 */
void CChatroomDlg::onSelfMuted(int seconds)
{
}

/**
 * 聊天室已关闭
 */
void CChatroomDlg::onClosed()
{
}

/**
 * 收到消息
 * @param message
 */
void CChatroomDlg::onReceivedMessage(CIMMessage* pMessage)
{
	CString strMsg = "";
	strMsg.Format("%s:%s", pMessage->m_strFromId.c_str(), pMessage->m_strContentData.c_str());
	
	m_HistoryMsgListBox.InsertString(m_HistoryMsgListBox.GetCount(), strMsg);
}

/**
 * 收到私信消息
 * @param message
 */
void CChatroomDlg::onReceivePrivateMessage(CIMMessage* pMessage)
{
	CString strMsg = "";
	strMsg.Format("%s:%s", pMessage->m_strFromId.c_str(), pMessage->m_strContentData.c_str());
	m_HistoryMsgListBox.AddString(strMsg);
}


void CChatroomDlg::OnTimer(UINT_PTR nIDEvent)
{
	//定时获取在线人数
	if (m_pChatroomManager != NULL && m_pCurrentShow != NULL)
	{
		m_pChatroomManager->getOnlineNumber(m_pCurrentShow->m_strRoomId);
	}
	__super::OnTimer(nIDEvent);
}


void CChatroomDlg::OnDestroy()
{
	if (m_pCurrentShow != NULL)
	{
		delete m_pCurrentShow;
		m_pCurrentShow = NULL;
	}
	if (m_pChatroomManager != NULL)
	{
		KillTimer(0);
		m_pChatroomManager->exitChatroom(m_pChatroomManager->getChatroomId());
		delete m_pChatroomManager;
		m_pChatroomManager = NULL;
	}
	mDatas.clear();
	__super::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


BOOL CChatroomDlg::PreTranslateMessage(MSG* pMsg)
{
	//判断是否为键盘消息
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
	{
		//判断是否按下键盘Enter键
		if (pMsg->wParam == VK_RETURN)
		{

			//Do anything what you want to
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
void CChatroomDlg::OnNMRClickListChatroomList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	int nItem = -1;
	if (pNMItemActivate != NULL)
	{
		nItem = pNMItemActivate->iItem;
		if (nItem >= 0)
		{
			m_pCurrentShow = new ChatroomInfo();
			m_pCurrentShow->m_strName = m_ChatroomList.GetItemText(nItem, 1);
			m_pCurrentShow->m_strCreaterId = m_ChatroomList.GetItemText(nItem, 2);
			m_pCurrentShow->m_strRoomId = m_ChatroomList.GetItemText(nItem, 3);
			m_ChatroomName.SetWindowText(m_pCurrentShow->m_strName.c_str());
			if (m_pChatroomManager != NULL)
			{
				CString errInfo = "";
				KillTimer(0);
				m_pChatroomManager->exitChatroom(m_pChatroomManager->getChatroomId());
				bool bRet = m_pChatroomManager->joinChatRoom(m_pCurrentShow->m_strRoomId);
				if (!bRet)
				{
					errInfo.Format("加入 %s 聊天室失败!", m_pCurrentShow->m_strName.c_str());
					AfxMessageBox(errInfo);
				}
				else
				{
					SetTimer(0, 3000, 0);
				}	
			}
		}
	}
}


void CChatroomDlg::OnBnClickedButtonDeleteChatroom()
{
	CNewDlg dlg;
	dlg.m_strText = "请输入删除的聊天室Id";
	dlg.m_strSureButton = "确定";
	dlg.m_strTitle = "删除聊天室";
	if (dlg.DoModal() == IDOK)
	{
		bool bRet = m_pChatroomManager->deleteChatRoom(dlg.m_strContent.GetBuffer(0));
		if (!bRet)
		{
			CString errInfo = "";
			errInfo.Format("删除 %s 聊天室失败!", dlg.m_strContent.GetBuffer(0));
			AfxMessageBox(errInfo);
		}
		getChatroomList();
	}
}
