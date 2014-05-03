#include "StdAfx.h"
#include "TaskManager.h"

CTaskManager::CTaskManager(void)
{
}

CTaskManager::~CTaskManager(void)
{
}

void CTaskManager::SendProgressList( IN SOCKET sock, IN int nTag)
{
	GetSysProgressInfo(sock, nTag);   //bTag����ؼ���������������������Ƿ�ɹ���0 -- δ֪(����)�� 1--�����ɹ� , 2--����ʧ��
}

void CTaskManager::TaskkillProc(IN PROGRESS_S tagProInfo, IN SOCKET sock)
{
	UINT32 result = ERROR_SUCCESS;
	CString strExecInfo = _T("ERROR");  //ִ�н��
    bool bSuccess = true;
	int nTag = 2;  //Ĭ���ǽ���ʧ��
	result = KillPorcessByPID(tagProInfo.nPid, &bSuccess);
	if (ERROR_SUCCESS == result)
	{
		if (bSuccess)  //�رս��̳ɹ�
		{
			strExecInfo = _T("OK");
			nTag = 1;   //�����ɹ�
		}
	}
	Sleep(50);
	SendProgressList(sock, nTag);  //���������̺�����ˢ�б�
}

//���ݽ��̵�PID������ָ������
UINT32 CTaskManager::KillPorcessByPID(IN const DWORD dwProcessPid, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	bool bSuccess = true;
	try
	{
		if (-1 != dwProcessPid || 0 != dwProcessPid)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessPid);
			if (NULL == hProcess)
			{
				result = GetLastError();
				bSuccess = false;
				throw result;
			} 
			else  //�򿪽��̳ɹ�
			{
				if (!TerminateProcess(hProcess,0))
				{
					result = GetLastError();
					throw result;
				}
				CloseHandle(hProcess);
			}
		}
		else
		{
			bSuccess = false;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

UINT32 CTaskManager::GetSysProgressInfo(IN SOCKET sock, IN int nTag)
{
	UINT32 result = ERROR_SUCCESS;
	PROGRESS_S tagProInfo;
	memset(&tagProInfo, 0, sizeof(PROGRESS_S));

	PROCESSENTRY32 pe32;
	//��ʹ�ýṹ֮ǰ�������ô�С
	pe32.dwSize = sizeof(PROCESSENTRY32);
	//��ϵͳ�ڵ����н�����һ������
	try
	{
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (INVALID_HANDLE_VALUE == hProcessSnap)
		{
			result = GetLastError();
			throw result;
		}
		//�������ս��̣�������ʾÿ��������Ϣ
		BOOL bRet = ::Process32First(hProcessSnap, &pe32);
		if (!bRet)
		{
			result = GetLastError();
			throw result;
		}
		else
		{
			while (bRet)
			{
				memcpy(tagProInfo.szProName, pe32.szExeFile, sizeof(tagProInfo.szProName));  //������
				tagProInfo.nPid = pe32.th32ProcessID;  //���̵�PID
				CString strProPath = _T(""); //����·��
				BOOL bSuccess = GetProcessFilePathByPId(tagProInfo.nPid, strProPath);
				wsprintfW(tagProInfo.szProPath, strProPath);  //��ȡ����·��
				PROGRESS_LEVEL nProLevel = GetProcessLevel(tagProInfo.nPid);
				tagProInfo.nLevel = nProLevel;   //���̵����ȼ�
				tagProInfo.nThreadCount = pe32.cntThreads;  //�߳���
                if (1 == nTag || 2 == nTag)  //���������ʾ�ǽ������̲���
				{
					tagProInfo.nTag = nTag; 
				}
				else
				{
					tagProInfo.nTag = 0;
				}
				bRet = ::Process32Next(hProcessSnap,&pe32);  

				MSGINFO_S tagMsgInfo;
				memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
				tagMsgInfo.Msg_id = CMD_PROGRESS;
				memcpy((char*)tagMsgInfo.context, (char*)&tagProInfo, sizeof(PROGRESS_S));
				bool bSend = true;
				m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSend);
				if (!bSend)
				{
					printf("����˻�ȡ������Ϣʧ��! ������: %d\n", result);
				}

			}
			::CloseHandle(hProcessSnap);  //���SNAPSHOT����
		}
	}
	catch (UINT32)
	{
	}
	return result;
}

//���ݽ��̵�ID����ȡ�������ڵ�·��
BOOL CTaskManager::GetProcessFilePathByPId(IN const DWORD dwProcessId, OUT CString & cstrPath )
{
	HANDLE hProcess = NULL;
	BOOL bSuccess = FALSE;

	// ���ڽ���Ȩ�����⣬��Щ�������޷���OpenProcess�ģ���������ý��̵�Ȩ��
	// �ᵽ�����ԡ�Ȩ�ޣ�����ܿ��Դ򿪸���Ľ���
	hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , 
		FALSE, dwProcessId );
	if (EnableDebugPrivilege())  //��ʾȨ��
	{
		do 
		{
			if ( NULL == hProcess )
			{
				// �򿪾��ʧ�ܣ��������Ϊ0�Ľ���
				break;
			}
			// ���ڱ����ļ�·��������һλ����Ϊ�˱�֤���������
			TCHAR szPath[MAX_PATH + 1] = {0};

			// ģ����
			HMODULE hMod = NULL;
			// ������������������û�ô�������Ϊ�˵���EnumProcessModules
			DWORD cbNeeded = 0;
			// ��ȡ·��
			// ��Ϊ�������ֻ��Ҫ��ý��̵�Exe·������ΪExe·�������ڷ��ص����ݵ�
			// ��һλ������ȥ����cbNeeded��hMod�Ｔ��Exe�ļ��ľ��.
			// If this function is called from a 32-bit application running on WOW64, 
			// it can only enumerate the modules of a 32-bit process. 
			// If the process is a 64-bit process, 
			// this function fails and the last error code is ERROR_PARTIAL_COPY (299).
			if( FALSE == EnumProcessModules( hProcess, &hMod, 
				sizeof( hMod ), &cbNeeded ) )
			{
				break;
			}
			// ͨ��ģ��������ȡģ�����ڵ��ļ�·�����˴���Ϊ����·����
			// ����SizeΪMAX_PATH��������MAX_PATH+1������Ϊ��֤��������������
			if ( 0 == GetModuleFileNameEx( hProcess, hMod, szPath, MAX_PATH ) )
			{
				break;
			}
			// �����ļ�·��
			cstrPath = szPath;
			// ���ҳɹ���
			bSuccess = TRUE;
		} while( 0 );
	}
	// �ͷž��
	if ( NULL != hProcess )
	{
		CloseHandle( hProcess );
		hProcess = NULL;
	}
	return bSuccess;
}

//��ö�ٵĽ�������SE_DEBUG_NAMEȨ�� , ����ö�ٳ��������·��
bool CTaskManager::EnableDebugPrivilege()   
{   
	HANDLE hToken;   
	LUID sedebugnameValue;   
	TOKEN_PRIVILEGES tkp;   
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{   
		return  FALSE;   
	}   
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))  
	{   
		CloseHandle(hToken);   
		return false;   
	}   
	tkp.PrivilegeCount = 1;   
	tkp.Privileges[0].Luid = sedebugnameValue;   
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;   
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) 
	{   
		CloseHandle(hToken);   
		return false;   
	}   
	return true;   
}

//��ȡ���̵����ȼ�
PROGRESS_LEVEL CTaskManager::GetProcessLevel(IN const DWORD dwProcessId)
{
	HANDLE hProcess = NULL;
	hProcess = OpenProcess( 
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , 
		FALSE, dwProcessId );
	PROGRESS_LEVEL proLev = UNKNOWN_LEVEL;

	if (NULL != hProcess)
	{
		DWORD dwLevel = GetPriorityClass(hProcess);
		switch (dwLevel)
		{
		case NORMAL_PRIORITY_CLASS:  //��׼
			{
				proLev = NORMAL;
			}
			break;
		case IDLE_PRIORITY_CLASS:   //��
			{
				proLev = IDLE;
			}
			break;
		case REALTIME_PRIORITY_CLASS:  //ʵʱ
			{
				proLev = REALTIME;
			}
			break;
		case HIGH_PRIORITY_CLASS:  //��
			{
				proLev = HIGH;
			}
			break;
		case ABOVE_NORMAL_PRIORITY_CLASS:  //���ڱ�׼
			{
				proLev = ABOVENORMAL;
			}
			break;
		case BELOW_NORMAL_PRIORITY_CLASS:  //���ڱ�׼
			{
				proLev = NORMAL;
			}
			break;
		default:
			{
				proLev = UNKNOWN_LEVEL; //δ֪
				break;  
			}
		}
	}
	else
	{
		printf("��ȡ���̵����ȼ�ʧ��!\n");
	}
	return proLev;
}