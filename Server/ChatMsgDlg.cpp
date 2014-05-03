// ChatMsgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "ChatMsgDlg.h"
#include "Common.h"

static const int nHotkey  = 2000;
// CChatMsgDlg �Ի���

IMPLEMENT_DYNAMIC(CChatMsgDlg, CDialog)

CChatMsgDlg::CChatMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatMsgDlg::IDD, pParent)
{
	m_strChatMsg = _T("");
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CHATMSG);
	m_bForceQuit = false;
}

CChatMsgDlg::~CChatMsgDlg()
{
}

void CChatMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHAT_RECV, m_chatRecv);
	DDX_Control(pDX, IDC_EDIT_CHAT_SEND, m_chatSend);
}


BEGIN_MESSAGE_MAP(CChatMsgDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SEND, &CChatMsgDlg::OnBnClickedBtnSend)
	ON_MESSAGE(WM_HOTKEY,&CChatMsgDlg::OnHotKey)
	ON_BN_CLICKED(IDCANCEL, &CChatMsgDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CChatMsgDlg ��Ϣ�������

BOOL CChatMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, FALSE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	BOOL bRet = RegisterHotKey(this->GetSafeHwnd(), nHotkey, MOD_CONTROL, VK_RETURN);  //����ע���ݼ�ʧ�ܣ�ԭ���Ǵ��ھ��
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CChatMsgDlg::OnBnClickedBtnSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendChatMsg();
}

void CChatMsgDlg::SetSendSocket(IN SOCKET l_socket)
{
	m_socket = l_socket;
}

LRESULT CChatMsgDlg::OnHotKey( WPARAM wParam,LPARAM lParam )
{
	if (nHotkey == wParam)
	{
		SendChatMsg();   //��������� Ctrl + Enter , ���͵�ǰ����Ϣ
		m_chatSend.SetWindowText(_T(""));  //���
	}
	return 0;
}

void CChatMsgDlg::SendChatMsg()
{
	CString strSendMsg = _T("");  //���͵���Ϣ
	GetDlgItemText(IDC_EDIT_CHAT_SEND, strSendMsg);
	if (strSendMsg.IsEmpty())  //���͵��ǿ�
	{
		return ;
	}
	CHATMSG_S tagChatMsg;
	memset(&tagChatMsg, 0, sizeof(CHATMSG_S));
	tagChatMsg.bfirstSend = false;
	tagChatMsg.bClose = false;   //�û��Ƿ�ѡ��رնԻ�����
	wsprintfW(tagChatMsg.szChatMsg, strSendMsg);

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_CHATMSG;        //����������Ϣ
	memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	if (bSuccess)
	{
		m_strChatMsg += _T("YourSelf: ");
		m_strChatMsg += strSendMsg;
		m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
	}
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatSend.SetWindowText(_T(""));  //���
	m_chatRecv.SetSel(-1);
}

void CChatMsgDlg::RecvChatMsg(IN CHATMSG_S tagChatMsg)
{
	CString strRecvMsg = _T("");
	if (false == tagChatMsg.bClose)
	{
		strRecvMsg.Format(_T("Hacker: %s"), tagChatMsg.szChatMsg);
		m_strChatMsg += strRecvMsg;
		m_strChatMsg += _T("\r\n\r\n--------------------END---------------------\r\n\r\n");
		m_bForceQuit = false;
	}
	else   //���͵��ر��������Ϣ
	{
		m_bForceQuit = true;   //���ͻ���ǿ���˳�
		OnCancel();  //�ر�
		return;
	}
	m_chatRecv.SetWindowText(m_strChatMsg);
	m_chatRecv.SetSel(-1);
}

void CChatMsgDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UnregisterHotKey(GetSafeHwnd(), nHotkey);//ע��Alt+1��
	OnCancel();
}

void CChatMsgDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	//���͹ر���Ϣ
	if (!m_bForceQuit)
	{
		CHATMSG_S tagChatMsg;
		memset(&tagChatMsg, 0, sizeof(CHATMSG_S));
		tagChatMsg.bfirstSend = false;
		tagChatMsg.bClose = true;   //�ر��˶Ի���

		MSGINFO_S tagMsgInfo;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_CHATMSG;        //����������Ϣ
		memcpy((char*)tagMsgInfo.context, (char*)&tagChatMsg, sizeof(CHATMSG_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
    Sleep(100);
	CDialog::OnCancel();
}
