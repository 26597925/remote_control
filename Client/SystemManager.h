#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"
#include "ProgressDlg.h"
#include "SystemServerDlg.h"
#include "ReadPswDlg.h"

//ϵͳ����Ի���(�������̡�����)
// CSystemManager �Ի���

class CSystemManager : public CDialog
{
	DECLARE_DYNAMIC(CSystemManager)

public:
	CSystemManager(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CSystemManager();

// �Ի�������
	enum { IDD = IDD_DLG_SYSTEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CImageList m_ImageTab;
	CTabCtrl m_sysTab;
	virtual BOOL OnInitDialog();
	void DlgInit();
	CProgressDlg* GeProgressPoint() { return m_progressDlg; }  //��ȡ������Ϣָ��
	CSystemServerDlg* GetSysServerPoint() { return m_pSysServerDlg; }  //��ȡϵͳ����ָ��
	CReadPswDlg* GetReadPswPoint()  {return m_pReadPswSlg;}
	CStatusBar m_sysStatusBar;   //״̬��
	CProgressDlg* m_progressDlg;  //���̹�����
	CSystemServerDlg* m_pSysServerDlg;  //ϵͳ������
	CReadPswDlg*  m_pReadPswSlg;   //�ʺ����������
private:
	HICON m_hIcon;
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;  
public:
	afx_msg void OnTcnSelchangeTabSystem(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
