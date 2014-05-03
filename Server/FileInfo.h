#pragma once

#include "Common.h"
#include "ModuleSocket.h"

//�ļ�������
class CFileInfo
{
public:
	CFileInfo(void);
	~CFileInfo(void);
	void SendDownFileInfo(IN wchar_t* pPath, IN SOCKET sock);  //����ָ�������ļ���Ϣ
	void CreateUpLoadFile( IN wchar_t* pPath);  //�����ļ�
	void WriteDataInFile(IN DOWNFILEDATA_S tagDownFileDate);  //�����ݿ�д���ļ���
	void ExecuteFile(IN wchar_t* pFilePath, IN SOCKET sock);   //ִ���ļ�
	void SendAttriInfo(IN ATTRIBUTE_S tagAttribute, IN SOCKET sock);   //�����ļ�(��)����
private:
	CModuleSocket m_moduleSocket;
	HANDLE m_hFile;
};
