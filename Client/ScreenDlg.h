#pragma once

#include "Common.h"
#include "ModuleSocket.h"
// CScreenDlg �Ի���

class CScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
	CScreenDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CScreenDlg();

// �Ի�������
	enum { IDD = IDD_DLG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	void GetScreenDataInfo(BMPDATA_C tagBmpData) /*��ʾͼ�� */;
private:
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	BITMAPINFO *m_pBMPINFO;  //���λͼͷ��Ϣ��ָ��
	BYTE* m_pData;  //�����Ļ���ݵ�ָ��
	BMPDATA_C m_TagBmpData;

protected:
	virtual void PostNcDestroy();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
