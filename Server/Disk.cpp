#include "StdAfx.h"
#include "Disk.h"

CDisk::CDisk(void)
{
}

CDisk::~CDisk(void)
{
}

void CDisk::SendDiskInfo(IN SOCKET sock)
{
	if (INVALID_SOCKET == sock)
	{
		return;
	}

	MSGINFO_S tagMsg;
	DRIVER_S tagDir;  //���̽ṹ��
	for (wchar_t i = 'B'; i <= 'Z'; i++)  //Ϊʲô����A��ʼ��Ӧ��A�п��������̣����ⲻ�����������ĵĶ���
	{
		memset(&tagDir, 0, sizeof(DRIVER_S));  //�����
		memset(&tagMsg, 0, sizeof(MSGINFO_S));
		unsigned __int64 i32FreeBytesToCaller = 0;
		unsigned __int64 i32TotalBytes = 0;
		unsigned __int64 i32FreeBytes = 0;
		TCHAR szTemp[] = {i, ':', '\0'};
		UINT uType = GetDriveType(szTemp);  //��ȡ���̵�����
		switch (uType)
		{
		case DRIVE_FIXED:  //�̶�Ӳ��
			{
				tagDir.nType = DISK_FIXED;
			}
			break;
		case DRIVE_REMOVABLE:  //�ƶ�Ӳ��
			{
				tagDir.nType = DISK_REMOVABLE;
			}
			break;
		case DRIVE_CDROM:  //CD-ROM
			{
				tagDir.nType = DISK_CDROM;
			}
			break;
		default:          //����
			continue;
		}
        tagDir.disk = i;   //������
		BOOL bSuccess = GetDiskFreeSpaceEx(szTemp,
			                               (PULARGE_INTEGER)&i32FreeBytesToCaller,
										   (PULARGE_INTEGER)&i32TotalBytes,
										   (PULARGE_INTEGER)&i32FreeBytes);
		if (bSuccess)  //��ȡ�����̿ռ���Ϣ
		{
			tagDir.dwTotal = (double)(i32TotalBytes/1024.0/1024/1024);
			tagDir.dwFree  = (double)(i32FreeBytesToCaller/1024.0/1024/1024);
		}
		else
		{
			tagDir.dwTotal = 0.0;
			tagDir.dwFree = 0.0;
		}

		tagMsg.Msg_id = CMD_DISKINFO;
		memcpy((char*)tagMsg.context, (char*)&tagDir, sizeof(DRIVER_S));
		bool bSend = true;
		UINT32 result = m_moduleSocket.SendCommand(sock, (char*)&tagMsg, sizeof(MSGINFO_S), &bSend);
		if (!bSend)
		{
			printf("������Ϣ����ʧ��! Error Code: %d\n", result);
		}
		else
		{
			printf("�ɹ�����%c������Ϣ\n", i);
		}
	}
}

void CDisk::SendFileList(IN wchar_t* pPath, IN SOCKET sock)
{
	if (INVALID_SOCKET == sock)
	{
		return;
	}

	WIN32_FIND_DATAW FindData;
	memset(&FindData, 0, sizeof(WIN32_FIND_DATAW));
	HANDLE hRet = INVALID_HANDLE_VALUE;
	int nFileCount = 0;   //�ļ�����
	TCHAR szFilePathName[MAX_PATH] = {0};
	wchar_t filePath[MAX_PATH] = {0};
	wcscpy_s(filePath, pPath);
	wsprintfW(szFilePathName, _T("%s\\*.*"), filePath);
	hRet = FindFirstFileW(szFilePathName, &FindData);
	if (INVALID_HANDLE_VALUE == hRet)
	{
		printf("�ļ�·��������!\n");
		return ;
	}
	FILEINFO_S tagFileInfo;  //�ļ���Ϣ�ṹ��
	MSGINFO_S tagMsg;
	DWORDLONG dl = 0;
	bool bSuccess = true;
	memset(&tagFileInfo, 0, sizeof(FILEINFO_S));
	memset(&tagMsg, 0, sizeof(MSGINFO_S));

	while (::FindNextFile(hRet, &FindData))  //����
	{
		if (!wcscmp(_T("."), FindData.cFileName) || !wcscmp(_T(".."), FindData.cFileName))
		{
			continue;
		}
		//����������·��, �ļ���Ŀ+1
		tagMsg.Msg_id = CMD_FILELIST;

		wsprintfW(tagFileInfo.szFileName, _T("%s"), FindData.cFileName);  //�ļ�·��
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  //�ļ���
		{
			tagFileInfo.nType = FOLDER_ATTR;
		}
		else
		{
			tagFileInfo.nType = FILE_ATTR;
			tagFileInfo.size = ((__int64)FindData.nFileSizeHigh * (dl + 1)) + (__int64)FindData.nFileSizeLow;
		}
		nFileCount++;  //�ļ���������
		memcpy((char*)tagMsg.context, (char*)&tagFileInfo, sizeof(FILEINFO_S));
		m_moduleSocket.SendCommand(sock, (char*)&tagMsg, sizeof(MSGINFO_S), &bSuccess);
		memset(&FindData, 0, sizeof(WIN32_FIND_DATAW)); //���
		memset(&tagFileInfo, 0, sizeof(FILEINFO_S));
		memset(&tagMsg, 0, sizeof(MSGINFO_S));
	}
	tagMsg.Msg_id = CMD_FILELIST;
	tagFileInfo.nType = FILE_TAG;  //�ļ���Ϣ�������
	memcpy((char*)tagMsg.context, (char*)&tagFileInfo, sizeof(FILEINFO_S));
	m_moduleSocket.SendCommand(sock, (char*)&tagMsg, sizeof(MSGINFO_S), &bSuccess);
	if (bSuccess)
	{
		printf("�ļ��б���Ϣ�ɹ��������!\n");
	}
	else
	{
		printf("�ļ��б���Ϣ����ʧ��!\n");
	}
}

void CDisk::ExecCreateFolder(IN wchar_t* pFilePath, IN SOCKET sock)
{
	bool bExist = false;
	FolderExist(pFilePath, &bExist);
	if (!bExist)   //������
	{
		CreateFolder(pFilePath, &bExist);
		if (bExist)
		{
			printf("�����ļ�Ŀ¼�ɹ�!\n");
		}
		else
		{
			printf("�����ļ�Ŀ¼ʧ��\n");
		}
	}
	else
	{
		printf("Ŀ¼�Ѿ�����");
		return ;
	}
	RefreshList(pFilePath, sock);  //����ˢ���ļ��б�
}

void CDisk::FolderExist(IN const CString strPath, OUT bool* pbExist)
{
	WIN32_FIND_DATA  wfd;
	bool rValue = false;
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = true;   
	}
	FindClose(hFind);
    *pbExist = rValue;
}

void CDisk::CreateFolder(IN const CString strPath, OUT bool* pbSuccess)
{
	bool bSuccess = true;
	SECURITY_ATTRIBUTES attrib;
	attrib.bInheritHandle = FALSE;
	attrib.lpSecurityDescriptor = NULL;
	attrib.nLength =sizeof(SECURITY_ATTRIBUTES);
	bSuccess = (TRUE == CreateDirectory(strPath, &attrib) ? true: false);
	*pbSuccess = bSuccess;
}

void CDisk::DeleteTargetFile(IN FILEINFO_S tagFileInfo, IN SOCKET sock)
{
	CString strPath = _T("");
	strPath.Format(_T("%s"), tagFileInfo.szFileName);

	if (FILE_ATTR == tagFileInfo.nType)  //ɾ�������ļ�
	{
		DeleteSpecifiedFile(strPath);
	}
	else if (FOLDER_ATTR == tagFileInfo.nType)  //ɾ�������ļ���
	{
		bool bSuccess = true;
		DeleteDirectory(strPath, &bSuccess);
	}

	RefreshList(tagFileInfo.szFileName, sock);  //����ˢ���ļ��б�
}

//ɾ���ļ���
UINT32 CDisk::DeleteDirectory( IN const CString strInstallPath, 
								   OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (strInstallPath.IsEmpty() || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		*pbSuccess = false;
		return result;
	}
	CFileFind findFile;
	bool bSuccess = true;
	BOOL bFound = TRUE;
	CString strAllFilePath = _T("");
	CString strFileName = _T(""); //�ļ���
	if (_T("\\") != strInstallPath.Right(1))
	{
		strAllFilePath.Format(_T("%s\\*.*"), strInstallPath);
	}
	else
	{
		strAllFilePath.Format(_T("%s"), strInstallPath);
	}
	try
	{
		bFound = findFile.FindFile(strAllFilePath);
		if (0 == bFound)//û���ҵ�
		{
			result = GetLastError();
			bSuccess = false;
			throw result;
		}
		while (bFound)
		{
			bFound = findFile.FindNextFile();
			if (findFile.IsDirectory())//������ļ���
			{
				CString strTempDir = _T("");
				if (findFile.IsDots())
				{
					continue;
				}
				strFileName.Format(_T("%s"), findFile.GetFileName());
				strTempDir = strInstallPath + _T("\\") + strFileName;
				if (findFile.IsReadOnly()) //����ļ�����ֻ�����ԵĻ���ȥ��ֻ������
				{
					DWORD dwFileAttr = GetFileAttributes(strTempDir);
					if (INVALID_FILE_ATTRIBUTES != dwFileAttr)
					{
						dwFileAttr &= ~FILE_ATTRIBUTE_READONLY;  //ȥ��ֻ������
						SetFileAttributes(strTempDir, dwFileAttr);
					}
				}
				result = DeleteDirectory(strTempDir, &bSuccess); //�ݹ�ɾ��
			}
			else //������ļ�
			{
				strFileName.Format(_T("%s"), findFile.GetFilePath());
				if (findFile.IsReadOnly())
				{
					SetFileAttributes(strFileName, GetFileAttributes(strFileName) &(~FILE_ATTRIBUTE_READONLY) );
				}
				result = DeleteSpecifiedFile(strFileName);   //ɾ���ļ�
			}
		}
		findFile.Close();
	}
	catch (UINT32)
	{
	}
	DeleteEmptyFolder(strInstallPath);  //ɾ�����ļ���
	*pbSuccess = bSuccess;
	return result;
}

//ɾ��ָ���ļ�
UINT32 CDisk::DeleteSpecifiedFile( IN const CString strFileFullPath)
{
	UINT32 result = ERROR_SUCCESS;
	if (strFileFullPath.IsEmpty())
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}

	try
	{
		if (0 == DeleteFile(strFileFullPath))
		{
			printf("�ļ�ɾ��ʧ�� !\n");
			result = GetLastError();
			throw result;
		}
		else
		{
			printf("�ļ�ɾ���ɹ� !\n");
		}
	}
	catch (UINT32)
	{
	}
	return result;
}

//ɾ�����ļ���
void CDisk::DeleteEmptyFolder( IN const CString strFolderPath)
{
	BOOL bDelete = RemoveDirectory(strFolderPath);
	if (0 == bDelete)
	{
		printf("�ļ���ɾ��ʧ�� !\n");
	}
	else
	{
		printf("�ļ���ɾ���ɹ� !\n");
	}
}

void CDisk::AddCopyFileIntoList(IN FILECOPY_S tagFileInfo)
{
	if (tagFileInfo.bTag)  //������ܵ���ʼ������Ϣ�����LIST����
	{
		m_listFileInfo.clear();
		return;
	}
	m_listFileInfo.push_back(tagFileInfo);  //���ļ���Ϣ�Ľṹ����뵽LIST������
}

void CDisk::ExecuteCopyFile(IN wchar_t* pPath, IN SOCKET sock)
{
	std::list<FILECOPY_S>::const_iterator iter;
	BOOL bRet = true;
	for (iter = m_listFileInfo.begin(); iter != m_listFileInfo.end(); iter++)
	{
		FILECOPY_S tagFileInfo = *iter;
		CString strExistFileName = tagFileInfo.szFilePath;  //�Ѵ����ļ���·��
		CString strNewFileName = _T("");

		CString strPath = _T("");
		strPath.Format(_T("%s"), pPath);
		CString strName = _T("");
		strName.Format(_T("%s"), tagFileInfo.szFileName);
		strNewFileName = strPath + strName;

		bRet = ::CopyFile(strExistFileName, strNewFileName, FALSE);
		if (bRet)
		{
			printf("%s�ļ������ɹ�! \n", strNewFileName);
		}
		else
		{
			printf("%s�ļ�����ʧ��! \n", strNewFileName);
		}
	}
	RefreshList(pPath, sock);
}

void CDisk::FileReName(IN FILECOPY_S tagFileInfo, IN SOCKET sock)
{
	if (tagFileInfo.bTag)  //�������ļ�
	{
		ReNameFile(tagFileInfo);
	}
	else  //�������ļ���
	{
		ReNameFolder(tagFileInfo);
	}
	RefreshList(tagFileInfo.szFilePath, sock);
}

//�������ļ�
void CDisk::ReNameFile(IN FILECOPY_S tagFileInfo)
{
	CString strFolderPath = _T("");
	strFolderPath.Format(_T("%s"), tagFileInfo.szFilePath);
	int nIndex = strFolderPath.ReverseFind(_T('\\'));
	strFolderPath = strFolderPath.Left(nIndex + 1);  //tagFileInfo.szFilePath == c:\windows\1.txt ---> c:\windows\

	CString strFileName = _T("");
	strFileName.Format(_T("%s"), tagFileInfo.szFileName);
    CString strNewFile = strFolderPath + strFileName;  // c:\windows\ ---> c:\windows\2.bat

	try
	{
		CFile::Rename(tagFileInfo.szFilePath, strNewFile);
		printf("�ļ��������ɹ�!\n");
	}
	catch(CFileException* pEx )
	{
		TRACE(_T("File %20s not found, cause = %d\n"), tagFileInfo.szFilePath, pEx->m_cause);
		pEx->Delete();
	}
}

//�������ļ���
void CDisk::ReNameFolder(IN FILECOPY_S tagFileInfo)
{
	CString strFolderPath = _T("");
	strFolderPath.Format(_T("%s"), tagFileInfo.szFilePath);
	int nIndex = strFolderPath.ReverseFind(_T('\\'));
	strFolderPath = strFolderPath.Left(nIndex + 1);  //tagFileInfo.szFilePath == c:\windows\1 ---> c:\windows\
    
	CString strFolderName = _T("");
	strFolderName.Format(_T("%s"), tagFileInfo.szFileName);
	CString strNewFolder = strFolderPath + strFolderName;  // c:\windows\ ---> c:\windows\2
	if (::MoveFile(tagFileInfo.szFilePath, strNewFolder))
	{
		printf("�ļ����������ɹ�!\n");
	}
	else
	{
		printf("�ļ���������ʧ��!\n");
	}
}

//����ˢ�¿ͻ����б�
void CDisk::RefreshList(IN wchar_t* pPath, IN SOCKET sock)
{
	CString strFolderPath = _T("");
	strFolderPath.Format(_T("%s"), pPath);
	int nIndex = strFolderPath.ReverseFind(_T('\\'));
	strFolderPath = strFolderPath.Left(nIndex + 1);
	SendFileList(strFolderPath.GetBuffer(strFolderPath.GetLength()), sock);
}