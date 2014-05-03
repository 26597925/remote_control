// Server.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Server.h"
#include "WorkThreadMain.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

//���пͻ��ˣ���ˣ�
void RunClient()
{
	CWorkThreadMain workThread;
	workThread.m_bRunFlag = true;
	workThread.GetSysInfo();   //��ȡ���Ե���Ϣ

	SOCKET socket;
	while (true)
	{
		if (!workThread.m_bRunFlag)
		{
			break;
		}
		socket = workThread.Run();
		workThread.ShakeHandCheck(socket, &workThread.m_bRunFlag);
		if (workThread.m_bRunFlag)  //��֤�ɹ�
		{
			workThread.ExecCommand(socket);  //ִ������
		}
	}
	::closesocket(socket);
	WSACleanup();
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))  //������仰��������ʾ��MFC�ĶԻ�����Դ��
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
	}
	RunClient();
	return 0;
}
