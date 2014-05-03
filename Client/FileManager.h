#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "FileView.h"
#include "FileTrans.h"
// CFileManager �Ի���

class CFileManager : public CDialog
{
	DECLARE_DYNAMIC(CFileManager)

public:
	CFileManager(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CFileManager();

// �Ի�������
	enum { IDD = IDD_DLG_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;
	SOCKET m_socket;  //ͨ��socket
	CRect m_rect;
	
public:
	void InitDlg();   //��ʼ��
	CFileView* m_pFileView;
	CFileTrans* m_pFileTrans;
	CTabCtrl m_tabCtrl;  //ѡ�
	CStatusBar m_fileStatusBar;  //�ļ�״̬��
	CImageList m_ImageTab;   //ѡ�ͼ���б�
	virtual BOOL OnInitDialog();
	CFileView* GetFileViewPoint() {return this->m_pFileView;}  //�õ��ļ���ͼ�Ի����ָ��
	CFileTrans* GetFileTransPoint() {return this->m_pFileTrans;}  //�õ��ļ�����Ի����ָ��
	afx_msg void OnTcnSelchangeTabFile(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
