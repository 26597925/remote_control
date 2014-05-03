#pragma once

//Զ������㲥
// CRemoteCmd �Ի���
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"
#include "afxwin.h"

class CRemoteCmdDlg : public CDialog
{
	DECLARE_DYNAMIC(CRemoteCmdDlg)

public:
	CRemoteCmdDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CRemoteCmdDlg();

// �Ի�������
	enum { IDD = IDD_DLG_BROADCAST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	UINT GetIconType();  //��ȡ������ͼ��
	UINT GetBtnType();  //��ȡ��ť����
	HICON m_hIcon;
public:
	CStatusBar m_StatusBar;  //״̬��
	CButton m_checkRun;
	virtual BOOL OnInitDialog();
	void DlgInit();
	afx_msg void OnBnClickedBtnOpenWebsite();
	afx_msg void OnBnClickedBtnDownWebsite();
	afx_msg void OnBnClickedBtnSendWnd();
	afx_msg void OnBnClickedBtnPreviewWnd();
	void SendCmdMsg(IN BROADCAST_C tagBroadCast);  //������Ϣ����
	void UpDateStatusBar(IN const wchar_t* pRetContent);  //����״̬��

	CComboBox m_ComboxIcon;
	CComboBox m_ComboxButton;
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
