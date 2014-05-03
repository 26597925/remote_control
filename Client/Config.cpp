#include "StdAfx.h"
#include "Config.h"

CConfig::CConfig(void)
{
}

CConfig::~CConfig(void)
{
}

void CConfig::CheckConfigFile()
{
	CFileFind ff;
	if (FALSE == ff.FindFile(_T(".\\Config.ini")))
	{
		WriteDefaultInfo();
	}
}

void CConfig::WriteDefaultInfo()
{
	WritePrivateProfileStringW(_T("Config"), _T("Port"), _T("9527"), _T(".\\Config.ini"));       //�˿�
	WritePrivateProfileStringW(_T("Config"), _T("MaxConn"), _T("1000") , _T(".\\Config.ini"));  //���������
	WritePrivateProfileStringW(_T("Config"), _T("DefaultPath"), _T(""), _T(".\\Config.ini"));  //Ĭ������·��
	WritePrivateProfileStringW(_T("Config"), _T("Sound"), _T("1"), _T(".\\Config.ini"));   //��������״̬
}

UINT CConfig::GetPort()
{
	return GetPrivateProfileInt(_T("Config"), _T("Port"), 9527 , _T(".\\Config.ini"));
}

UINT CConfig::GetMaxConn()
{
	return GetPrivateProfileInt(_T("Config"), _T("MaxConn"), 1000 , _T(".\\Config.ini"));
}

UINT CConfig::GetSound()
{
	return GetPrivateProfileInt(_T("Config"), _T("Sound"), 1 , _T(".\\Config.ini"));
}