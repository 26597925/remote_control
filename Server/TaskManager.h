#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include <tlhelp32.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

//���������(���̺ͷ����ʺ��������������)

class CTaskManager
{
public:
	CTaskManager(void);
	~CTaskManager(void);
	void SendProgressList(IN SOCKET sock, IN int nTag);  //���͵�ǰ�����б���Ϣ
	UINT32 GetSysProgressInfo(IN SOCKET sock, IN int nTag);
	BOOL GetProcessFilePathByPId(IN const DWORD dwProcessId, OUT CString & cstrPath );  //���ݽ��̵�ID����ȡ�������ڵ�·��
	PROGRESS_LEVEL GetProcessLevel(IN const DWORD dwProcessId);
	bool EnableDebugPrivilege();  //��ö�ٵĽ�������SE_DEBUG_NAMEȨ�� , ����ö�ٳ��������·��
	void TaskkillProc(IN PROGRESS_S tagProInfo, IN SOCKET sock);  ////���ݽ��̵�PID������ָ������
	UINT32 KillPorcessByPID(IN const DWORD dwProcessPid, OUT bool* pbSuccess );
private:
	CModuleSocket m_moduleSocket;
};
