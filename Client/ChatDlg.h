#pragma once
#include "afxwin.h"
#include "Common.h"
#include "ModuleSocket.h"
#include "Define_Inc.h"

//������ʹ����
// CChatDlg �Ի���

class CChatDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CWnd* pParent = NULL, SOCKET sock = NULL,  CString strIP = NULL);   // ��׼���캯��
	virtual ~CChatDlg();

// �Ի�������
	enum { IDD = IDD_DLG_CHAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	CEdit m_chatRecv;
	CEdit m_chatSend;
	SOCKET m_socket;
    CModuleSocket m_moduleSocket;
	bool m_bfirstSend;  //�Ƿ��һ�η���

public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	void SendChatMsg();   //������Ϣ
	void RecvChatMsg(IN CHATMSG_C tagChatMsg);  //������Ϣ

protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
public:
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CString m_strChatMsg;  //�����¼
	CString m_strIP;   //�Է���IP
	bool m_bQuit;     //������Ƿ��˳���
	afx_msg void OnBnClickedBtnSendMsg();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
