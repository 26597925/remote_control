#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"

//���̶Ի���
// CProgressDlg �Ի���

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CProgressDlg();

// �Ի�������
	enum { IDD = IDD_DLG_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CBitmap m_MenuBmp[2];
	CListCtrl m_proListCtrl;  //List�ؼ�
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
    CWnd *m_pWndMsg;   //������ָ��
	bool m_bRefresh;   //�Ƿ���ˢ���б�
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	void SetProcessInfo(IN PROGRESS_C tagProInfo);  //���ý�����Ϣ
	CString GetProcessLevel(IN int proLevel);  //��ȡ���̵����ȼ�
	void UpDataStatusBar(IN CString strLeft = NULL, IN CString strRight = NULL);  //����״̬��
	void SetRefresh(bool bRefresh)  {m_bRefresh = bRefresh;}  //����ˢ��״̬
	afx_msg void OnNMRClickListProgress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListProgress(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnProcTaskkill();
	afx_msg void OnProcRefresh();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
