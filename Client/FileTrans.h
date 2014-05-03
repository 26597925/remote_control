#pragma once
#include "afxcmn.h"
#include "Common.h"
#include "ModuleSocket.h"

// CFileTrans �Ի���

class CFileTrans : public CDialog
{
	DECLARE_DYNAMIC(CFileTrans)

public:
	CFileTrans(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CFileTrans();

// �Ի�������
	enum { IDD = IDD_DLG_FILETRANS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
	bool m_bBeginWork;   //�Ƿ�ʼ����(����)
	CFile* m_pFile;     //�ļ�����
	CModuleSocket m_moduleSocket;
	CWnd *m_pWndMsg;   //������ָ
    DWORD m_dwStartTime;
public:
	CListCtrl m_listCtrl;
	CImageList m_ImageList;
	CProgressCtrl m_filePro;  //������
	void DlgInit();   //�Ի���ĳ�ʼ��
	void OnAddFileList(IN int nType, IN const CString strSourcePath, IN const CString strDesPath, IN const CString strSize, IN const __int64* puSize);
	virtual BOOL OnInitDialog();
	static DWORD WINAPI BeginToTransFileProc(LPVOID lpParameter);   //��ʼ�����ļ�
	void TransFile();  //�����ļ�
	void SetFileData(IN DOWNFILEDATA_C tagDownFileData);
	CString GetFileTranSpeed(DWORD size,DWORD time);
	void SendUploadFileInfo(IN SOCKET sock);  //�ϴ��ļ�ʱ�������ļ����ݿ�
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
