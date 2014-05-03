#pragma once
#include "afxwin.h"
#include "Resource.h"
#include "ModuleSocket.h"
// CChatMsgDlg �Ի���

class CChatMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatMsgDlg)

public:
	CChatMsgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CChatMsgDlg();

// �Ի�������
	enum { IDD = IDD_DLG_CHATMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	CEdit m_chatRecv;
	CEdit m_chatSend;
	afx_msg void OnBnClickedBtnSend();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
public:	
	void SendChatMsg();  //������Ϣ
	void RecvChatMsg(IN CHATMSG_S tagChatMsg);  //������Ϣ
	void SetSendSocket(IN SOCKET l_socket);  //���÷��͵�Socket
	afx_msg void OnBnClickedCancel();
	CString m_strChatMsg;
	bool m_bForceQuit;  //�Ƿ�ǿ���˳�
protected:
	virtual void OnCancel();
};
