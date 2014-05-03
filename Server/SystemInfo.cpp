#include "StdAfx.h"
#include "SystemInfo.h"
#include "Common.h"
#include <Wininet.h>
#include <windows.h>
#include <atlbase.h>
#include <Vfw.h>

#pragma comment(lib,"Wininet.lib")
#pragma comment(lib,"Vfw32.lib")

using namespace std;

CSystemInfo::CSystemInfo(void)
{
}

CSystemInfo::~CSystemInfo(void)
{
}

int CSystemInfo::GetSysVersion()
{
	/*
	��PEB�ṹ�д���Ų���ϵͳ�汾��Ϣ�ĺ�����
	ULONG OSMajorVersion; // A4h ƫ�Ƶ�ַa4h��ʾ����ϵͳ���汾��
	ULONG OSMinorVersion; // A8h ƫ�Ƶ�ַa8h��ʾ����ϵͳ�Ӱ汾��
	ͨ����ȡOSMajorVersion��OSMinorVersion�������ܹ�ȷ������ϵͳ�İ汾��
	FS�μĴ���ָ��ǰ��TEB�ṹ����TEBƫ��0x30����PEBָ�룬ͨ�����ָ�����ȡ�ô����PEB�е���Ϣ��
	��ͬ�Ĳ���ϵͳ���汾��Ϣ�Ĵ�ŵ�λ���ǲ�ͬ�ģ�����
	2000 0X0500   
	XP 0X0501
	2003 0X0502
	VISTA 0X0600
	Win7 0X0601
	����������Щ��Ϣ���ǿ���ͨ��������API��������ȡ����ϵͳ�İ汾��Ϣ��
	// _WIN32_WINNT version constants
	//
	Windows 8                                             _WIN32_WINNT_WIN8  (0x0602)
	Windows 7                                             _WIN32_WINNT_WIN7  (0x0601)
	Windows Server 2008                                   _WIN32_WINNT_WS08  (0x0600)
	Windows Vista                                         _WIN32_WINNT_VISTA (0x0600)
	Windows Server 2003 with SP1, Windows XP with SP2     _WIN32_WINNT_WS03  (0x0502)
	Windows Server 2003, Windows XP                       _WIN32_WINNT_WINXP (0x0501)
	*/

	int a=0 , b=0, i=0, j=0;
	int nVersion = 0;
	_asm
	{
		pushad
			mov ebx,fs:[0x18] ; get self pointer from TEB
			mov eax,fs:[0x30] ; get pointer to PEB / database
			mov ebx,[eax+0A8h] ; get OSMinorVersion
			mov eax,[eax+0A4h] ; get OSMajorVersion
			mov j,ebx
			mov i,eax
			popad
	}
	if((i==5)&&(j==0))
	{
		printf("ϵͳ�汾Ϊ Windows 2000\n");
		nVersion = OS_2000;
	}
	else if((i==5)&&(j==1))
	{ 
		printf("ϵͳ�汾Ϊ Windows XP\n");
		nVersion = OS_XP;
	}
	else if((i==5)&&(j==2))
	{
		printf("ϵͳ�汾Ϊ Windows 2003 sever\n");
		nVersion = OS_2003;
	}
	else if((i==6)&&(j==0))
	{
		printf("ϵͳ�汾Ϊ Windows Vista\n");
		nVersion = OS_Vista;
	}
	else if((i==6)&&(j==1))
	{
		printf("ϵͳ�汾Ϊ Win7\n");
		nVersion = OS_WIN7;
	}
	else if((i==6)&&(j==2))
	{
		printf("ϵͳ�汾Ϊ Win8\n");
		nVersion = OS_WIN8;
	}
	else
	{
		printf("δ֪����ϵͳ�汾\n");
		nVersion = OS_UNKNOWN;
	}
	return nVersion;
}

void CSystemInfo::GetInternetIP(OUT char* pWANIP, OUT bool* pbSuccess )
{
	bool bSuccess = true;
	HINTERNET hInternet=NULL; 
	HINTERNET hConnect=NULL; 
	char szText[512]= {0}; 
    TCHAR szAccept[] = _T("Accept: */*\r\n\r\n"); 
    TCHAR szUrl[] =_T("http://iframe.ip138.com/ic.asp"); 
	DWORD dwReadLen=0;
	hInternet=InternetOpen(_T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)"),0,NULL,NULL,0); 
	if (!hInternet) 
	{
		bSuccess = false;
		goto __Error;
	}
	hConnect=InternetOpenUrl(hInternet,szUrl,szAccept,wcslen(szAccept),INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD,0); 
	if (!hConnect) 
	{
		bSuccess = false;
		goto __Error;
	}
	InternetReadFile(hConnect, szText,512, &dwReadLen);
	if (hInternet) 
	{
		InternetCloseHandle(hInternet);
	}
	if (hConnect) 
	{
		InternetCloseHandle(hConnect);
	}
	char* pStart = strstr(szText,"[");  // [110.123.43.16]
	if (NULL == pStart) 
	{
		bSuccess = false;
		goto __Error;
	}
	pStart++;
	char* pEnd = strstr(szText, "]");
	if (NULL == pEnd)
	{
		bSuccess = false;
		goto __Error;
	}
	strncpy(pWANIP, pStart, pEnd - pStart);

__Error:
	*pbSuccess = bSuccess;
}

void CSystemInfo::GetHostName(OUT char* hostName, OUT char* szLocalIP, OUT bool* pbSuccess)
{
	char host_name[255] = {0};
	bool bSuccess = true;
	int iRet = ::gethostname(host_name, sizeof(host_name));
	if (SOCKET_ERROR == iRet)
	{
		printf("Get chicken hostName failed ! Error Code:%d\n", WSAGetLastError());
		bSuccess = false;
		return ;
	}
	else
	{
		memcpy(hostName, host_name, sizeof(host_name));  //��ȡ������
		struct hostent *phe = gethostbyname(host_name);
		if (!phe)
		{
			printf("Yow! Bad host lookup.");
			bSuccess = false;
		}
		else
		{
			//ѭ���ó����ػ�������IP��ַ
			WSADATA wsaData = {0};
			char name[255] = {0};
			PHOSTENT hostinfo;
			char *pLocalIP = NULL;
			if (0 == WSAStartup(MAKEWORD(2, 2),&wsaData))
			{
				if (0 == gethostname(name,sizeof(name)))
				{
					if(NULL !=(hostinfo=gethostbyname(name)))
					{
						pLocalIP = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
						memcpy(szLocalIP, pLocalIP, IPADDR_SIZE);
					}
				}
				WSACleanup();
			}
		}
	}
	*pbSuccess = bSuccess;
}

//��ȡCPU�ĺ�����
DWORD CSystemInfo::GetProcessorCoreCount()
{
#if (_WIN32_WINNT < 0x0600) // [zyl910] �Ͱ汾��Windows SDKû�ж��� RelationProcessorPackage �ȳ���
#define RelationProcessorPackage 3
#define RelationGroup 4
#endif
	typedef BOOL (WINAPI *LPFN_GLPI)(
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
		PDWORD);

	LPFN_GLPI glpi = (LPFN_GLPI) GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation");

	if (NULL == glpi)
		return 0;

	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	DWORD returnLength = 0;
	DWORD processorCoreCount = 0;
	while (true)
	{
		DWORD rc = glpi(buffer, &returnLength);
		if (FALSE == rc)
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer)
				{
					free(buffer);
				}
				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
				if (NULL == buffer)
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			break;
		}
	}
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
	DWORD byteOffset = 0;
	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
	{
		switch (ptr->Relationship)
		{
		case RelationProcessorCore:
			++processorCoreCount;
			break;

		default:
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		++ptr;
	}
	free(buffer);
	return processorCoreCount;
}

//��ȡCPU����Ƶ��Ϣ
DWORD CSystemInfo::GetCPULevel()
{
	DWORD dwMHz = 0; 
	CString strPath = _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0");//ע����Ӽ�·��
	CRegKey regkey;//����ע��������
	LONG lResult;//LONG�ͱ�������Ӧ���
	lResult=regkey.Open(HKEY_LOCAL_MACHINE, LPCTSTR(strPath), KEY_ALL_ACCESS); //��ע���� 
	if (ERROR_SUCCESS != lResult)
	{
		printf("�޷���ȡ��CPU��Ƶ��Ϣ\n");
		return dwMHz;
	}
	lResult = regkey.QueryDWORDValue(_T("~MHz"), dwMHz);//��ȡVendorIdentifier�ֶ�ֵ
	if (ERROR_SUCCESS != lResult)
	{
		dwMHz = 0;
	}
	return dwMHz;
}

//�Ƿ�������ͷ
BOOL CSystemInfo::IsWebCam()
{
	BOOL  bRet = false;
	TCHAR lpszName[100], lpszVer[50];
	for (int i = 0; i < 10 && !bRet; i++)
	{
		bRet = capGetDriverDescription(i, lpszName, sizeof(lpszName),
			lpszVer, sizeof(lpszVer));
	}
	return bRet;
}

//��ȡ���̵��ܴ�С
DWORD CSystemInfo::GetTotalDiskInfo()
{
	DWORD dwTotal = 0;
	for (wchar_t i = 'B'; i <= 'Z'; i++)  //Ϊʲô����A��ʼ��Ӧ��A�п��������̣����ⲻ�����������ĵĶ���
	{
		unsigned __int64 i32FreeBytesToCaller = 0;
		unsigned __int64 i32TotalBytes = 0;
		unsigned __int64 i32FreeBytes = 0;
		TCHAR szTemp[] = {i, ':', '\0'};
		UINT uType = GetDriveType(szTemp);  //��ȡ���̵�����
		switch (uType)
		{
		case DRIVE_FIXED:  //�̶�Ӳ��
			{
				BOOL bSuccess = GetDiskFreeSpaceEx(szTemp,
					(PULARGE_INTEGER)&i32FreeBytesToCaller,
					(PULARGE_INTEGER)&i32TotalBytes,
					(PULARGE_INTEGER)&i32FreeBytes);
				if (bSuccess)  //��ȡ�����̿ռ���Ϣ
				{
					dwTotal += (double)(i32TotalBytes/1024.0/1024/1024);
				}
				else
				{
					dwTotal += 0.0;
				}		
			}
			break;
		default:          //����
			continue;
		}
	}
	return dwTotal;
}

UINT32 CSystemInfo::SendSysInfo( IN SOCKET sock, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (INVALID_SOCKET == sock || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	SYSTEMINFO_S sysInfo;
	memset(&sysInfo, 0, sizeof(SYSTEMINFO_S));
	sysInfo.OSVer = GetSysVersion();  //��ȡ����ϵͳ�汾
	GetInternetIP(sysInfo.szWANIP, &bSuccess);   //��ȡ����IP
	if (!bSuccess)  //�����ȡ����IPʧ��
	{
		char* szWANIP = "0.0.0.0"; 
		memcpy(sysInfo.szWANIP, szWANIP, sizeof(sysInfo.szWANIP));
	}
	GetHostName(sysInfo.hostName, sysInfo.szLocalIP, &bSuccess);  //��ȡ��������ͱ���IP
	sysInfo.Cam = false;
	DWORD dwCPUCount = GetProcessorCoreCount();   //CPU������
	DWORD dwCPULevel = GetCPULevel();  //CPU��Ƶ
	CString strCPUInfo = _T("");
	strCPUInfo.Format(_T("%d*%dMHz"), dwCPUCount, dwCPULevel);
	wsprintfW(sysInfo.szCPUInfo, strCPUInfo);
	sysInfo.Cam = (IsWebCam() ? true : false);  //�Ƿ�������ͷ
	sysInfo.dwDiskSize = GetTotalDiskInfo();  //���̿ռ��С

	MSGINFO_S msgCMD;
	memset(&msgCMD, 0, sizeof(MSGINFO_S));
	msgCMD.Msg_id = CMD_SYSINFO;
	memcpy((char*)msgCMD.context, (char*)&sysInfo, CONTEXT_BUF_SIZE);
	m_moduleSocket.SendCommand(sock, (char*)&msgCMD, sizeof(MSGINFO_S), &bSuccess);
	return result;
}

