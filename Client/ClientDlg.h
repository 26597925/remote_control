
// ClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "MsgNoticeWnd.h"
#include "BroilerData.h"
#include "Config.h"

// CClientDlg �Ի���
class CClientDlg : public CDialog
{
// ����
public:
	CClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DLG_CLIENT };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void InitTray();  //��ʼ������
	void ShowTrayTask();  //��ʾ����
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CBitmap m_MenuBmp[7];   //�˵���ͼ��
    CImageList m_ImageList;  //ͼ���б�
	CImageList m_ImageHostList;  //����ͷͼ���б�
	CListCtrl m_listCtrl;
	CToolBar m_toolBarCtrl;    //������
	CStatusBar m_statusBarCtrl;  //״̬��
	CMsgNoticeWnd *m_pMsgWindow;  //������ʾ��Ϣ��
	NOTIFYICONDATA m_nid;  //����
	bool m_bNotify;  //�Ƿ��Ѿ���ʾ����
private:
	CRect m_rect;
	SOCKET m_ListenSocket;  //����socket
	bool m_bSound;     //��������ʾ
    CConfig m_cfg;     //������Ϣ
	UINT m_maxConn;   //���������
	UINT m_nPort;     //�˿ں�
public:
	afx_msg LRESULT OnShowTask(WPARAM wParam,LPARAM lParam); //��������
    void UpdateMain(IN const UINT nPort);
	UINT32 InitSocket(OUT bool* pbSuccess);
	static DWORD WINAPI InitSocketProc(LPVOID lpParameter);  //socket��ʼ���̺߳���
	void AddHost(SOCKET socket, sockaddr_in addr);          //�����������
	void AddNewItemData(SOCKET socket, sockaddr_in addr);
    void UpdateStatusBarInfo();   //����״̬����Ϣ
	void OnFileManager();  //�ļ�����
	void OnCmdShellManager();  //CMDShell����
	void OnSysManager();  //ϵͳ����
    void OnScreenManager();  //��Ļ�鿴
	void OnVideoManager();  //����ͷ

	CBroilerData* GetSelfItemInfo();  //�õ�ѡ�����

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnAddHost(WPARAM wparam, LPARAM lparam);  //��������
	LRESULT OnOffLine(WPARAM wparam, LPARAM lparam);  //��������
	void ShowMsgNotice(IN CString strCaption, IN CString strOnlineMsg);  //������ʾ��Ϣ
	afx_msg void OnClose();
	afx_msg void OnNMRClickListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuMsg();
	afx_msg void OnMenuShutdown();
	afx_msg void OnMenuReset();
	afx_msg void OnMenuLogoff();
	afx_msg void OnMenuVoice();
	afx_msg void OnMenuRemoteCmd();
	afx_msg void OnMenuDesktop();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnMenuTrayHide();
	afx_msg void OnMenuTrayShow();
	afx_msg void OnMenuTrayExit();
};

