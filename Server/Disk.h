#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include <list>

class CDisk
{
public:
	CDisk(void);
	~CDisk(void);

     void SendDiskInfo(IN SOCKET sock); //���ʹ�����Ϣ
	 void SendFileList(IN wchar_t* pPath, IN SOCKET sock);  //���ʹ����б���Ϣ
	 void CreateFolder(IN const CString strPath, OUT bool* pbSuccess);  //����Ŀ¼
	 void FolderExist(IN const CString strPath, OUT bool* pbExist);  //���Ŀ¼�Ƿ����
	 void ExecCreateFolder(IN wchar_t* pFilePath, IN SOCKET sock);  //ִ�д����ļ�����
	 void DeleteTargetFile(IN FILEINFO_S tagFileInfo, IN SOCKET sock);  //ɾ���ļ����ļ���
	 void AddCopyFileIntoList(IN FILECOPY_S tagFileInfo);  //�������Ƶ��ļ�·�����뵽list������
	 void ExecuteCopyFile(IN wchar_t* pPath, IN SOCKET sock);   //ִ���ļ���������
	 void FileReName(IN FILECOPY_S tagFileInfo, IN SOCKET sock);  //������
	 void RefreshList(IN wchar_t* pPath, IN SOCKET sock); //����ˢ�¿ͻ����б�
	 void ReNameFile(IN FILECOPY_S tagFileInfo);   //�������ļ���
	 void ReNameFolder(IN FILECOPY_S tagFileInfo);  //�������ļ�����
	
	 UINT32 DeleteDirectory( IN const CString strInstallPath, OUT bool* pbSuccess ); //ɾ��Ŀ¼�ļ���
	 UINT32 DeleteSpecifiedFile( IN const CString strFileFullPath);  //ɾ��ָ���ļ�
	 void DeleteEmptyFolder( IN const CString strFolderPath);  //ɾ�����ļ���
private:
	CModuleSocket m_moduleSocket;
	std::list<FILECOPY_S> m_listFileInfo;   //�����Ƶ��ļ�·��
};
