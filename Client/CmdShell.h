#pragma once
#include "afxwin.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"

//Զ��TELNET
// CCmdShell �Ի���

class CCmdShell : public CDialog
{
	DECLARE_DYNAMIC(CCmdShell)

public:
	CCmdShell(CWnd* pParent = NULL, SOCKET l_socket = NULL, CString strTitle = _T(""));   // ��׼���캯��
	virtual ~CCmdShell();

// �Ի�������
	enum { IDD = IDD_DLG_TELNET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strTitle;
	CFont * m_pfont;
	bool m_bReadOnly;
	CBrush m_Brush;
	CEdit m_cmdRecvCtrl;
	CEdit m_cmdSendCtrl;
	CStatusBar m_cmdStatusBar;  //CMDShell״̬��
	afx_msg void OnBnClickedBtnCmdSend();
	virtual BOOL OnInitDialog();
	void DlgInit();
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	CString m_strShowMsg;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void CStringToChar(IN CString str, OUT char* w);  //CStringתchar
	CString CharToCString(char* result);  //charתCString

	void SendCommond();
	void SetReturnInfo(IN COMMOND_C tagCmdInfo);   //��������Ϣ��ʾ��ҳ����
protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
};
