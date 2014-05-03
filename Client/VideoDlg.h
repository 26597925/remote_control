#pragma once

#include "../Common/XPictureBox.h"
#include "Common.h"
#include "Define_Inc.h"
#include "ModuleSocket.h"
#include "../Common/AviFile.h"
#include "../Common/zlib.h"
#include "../Common/zconf.h"
#pragma comment(lib, "../Common/zlib1.lib")
// CVideoDlg �Ի���

class CVideoDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CVideoDlg();

// �Ի�������
	enum { IDD = IDD_DLG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CStatusBarCtrl    m_wndStatusBar;
	BOOL VideoStart();
	void VideoStop();
	XPictureBox m_PicBox;

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
	HANDLE m_hRecvVideoThread;
	CModuleSocket m_moduleSocket;
    LPBITMAPINFO m_pBmpheader;
	BYTE* m_pCompress;  //������ݵ�ָ��(ѹ�����)
	BYTE* m_pUnCompress;  //������ݵ�ָ��(δѹ����)
	CAviFile* m_pAviFile;
public:
	virtual BOOL OnInitDialog();
	void DlgInit();
	DWORD RecvVideo(IN VIDEODATA_C tagVideoData);
	void SendVideoStartMsg();
	HBITMAP GetBitmapFromData(LPBITMAPINFO lpBmpInfo, BYTE* pDibData);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedBtnRecords();
	afx_msg void OnBnClickedBtnRecorde();
	void StatusTextOut(int iPane,LPCTSTR ptzFormat, ...);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
