/*********************************************************************
* SEU_QQwry.h
*
*
* ˵��:����IP���ݿ� QQWry.dat�Ĳ�����
* 
* ���ִ�����Դ������,���������ʹ��,�������޸�.���ǲ���������ҵ��;
*********************************************************************/
#include "stdafx.h"
#if !defined(AFX_SEU_QQWRY_H)
#define AFX_SEU_QQWRY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXBUF 50

typedef struct _tagEndInfo
{
	BYTE b0;
	BYTE b1;
	BYTE b2;
	BYTE b3;
	BYTE buf[MAXBUF];
	BYTE bMode;
	int offset1;
	int offset2;
}EndInfo,PEndInfo;

typedef struct _tagIPOFF
{
	BYTE b0;
	BYTE b1;
	BYTE b2;
	BYTE b3;
	BYTE off1;
	BYTE off2;
	BYTE off3;
}IPOFF,*PIPOFF;

typedef struct _tagBE
{
	int uBOff;
	int uEOff;
}BE,*PBE;

class CQQwry  
{
public://��ͨ�û��ӿں���
	void SetPath(CString path);//����QQWry.dat��·��
    CStringA IPtoAdd(CStringA szIP);//������IP,����IP��Ӧ�ĵ�ַ   
	void SaveToFile(CStringA Name);//��QQWry.dat�ļ������ݵ���Ϊָ�����ļ�

public://�߼��û�����	
	DWORD m_dwLastIP;
	CStringA GetCountryLocal(int index);
	DWORD GetSIP(int index);
	DWORD IPtoDWORD(CStringA szIP);
	int GetIndex(CStringA szIP);
	CStringA GetStr(void);
	CStringA GetCountryLocal(BYTE bMode,int ioffset);
	CStringA GetStr(int ioffset);
	int GetRecordCount(void);
	int m_i;
	int GetStartIPInfo(int iIndex);
	CStringA m_buf;
	bool GetBE(void);
	bool OpenQQwry(CString szFileName);//��QQwry���ݿ�
	void CloseQQwry(void);//�ر�QQwry���ݿ�
	BE m_be;
	IPOFF m_ipoff;
	EndInfo m_ei;
	CQQwry();
	virtual ~CQQwry();
private:
	bool m_bOpen;
	CFile m_file;
};

#endif // !defined(AFX_SEU_QQWRY_H)
