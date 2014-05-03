#pragma once

//�������
// CDesktopDlg �Ի���
#include "ModuleSocket.h"
#include "Common.h"
#include "Define_Inc.h"

class CDesktopDlg : public CDialog
{
	DECLARE_DYNAMIC(CDesktopDlg)

public:
	CDesktopDlg(CWnd* pParent = NULL, SOCKET sock = NULL, CString strTitle = NULL);   // ��׼���캯��
	virtual ~CDesktopDlg();

// �Ի�������
	enum { IDD = IDD_DLG_DESKTOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	char* m_pBuffer;
	CString m_strCilpBoard;  //����������
	CString m_strTitle;  //���ڱ���
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnHideMouse();
	afx_msg void OnBnClickedBtnAllowMouse();
	void SendCommandMsg(IN const DESKTOPINFO_C tagDesktopInfo);
	DWORD SetClipBoardContext(IN const CLIPBOARD_C tagClipBoard);    //��ʾԶ�̼����������
	afx_msg void OnBnClickedBtnHideDesktop();
	afx_msg void OnBnClickedBtnShowDesktop();
	afx_msg void OnBnClickedBtnHideTaskbar();
	afx_msg void OnBnClickedBtnShowTaskbar();
	afx_msg void OnBnClickedBtnViewClip();
	CStringW CStrA2CStrW(const CStringA &cstrSrcA);  // CStringAתCStringW  Unicode������
	afx_msg void OnBnClickedBtnSaveClip();
};
