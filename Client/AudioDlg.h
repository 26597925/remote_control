#pragma once

//���������Ի�����
// CAudioDlg �Ի���
#include "ModuleSocket.h"
#include "Define_Inc.h"
#include "../Common/Audio.h"
#include "Common.h"

class CAudioDlg : public CDialog
{
	DECLARE_DYNAMIC(CAudioDlg)

public:
	CAudioDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CAudioDlg();

// �Ի�������
	enum { IDD = IDD_DLG_AUDIO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CAudio m_Audio;
	CModuleSocket m_moduleSocket;
	SOCKET m_socket;
	CString m_strIP;
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	DWORD RecvAudioData(IN AUDIODATA_C tagAudioData);
    BYTE* m_pData;
	HANDLE m_hRecvThread;  //�����̺߳������
	HANDLE m_hWorkThread; //���ͱ����������̺߳������
	static DWORD WINAPI WorkThreadProc(LPVOID lpParameter);  //���ͱ��ص�������Զ��
	static DWORD WINAPI RecvAudioProc(LPVOID lpParameter);  //����Զ������
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
