#pragma once
#include "afxcmn.h"
#include "ModuleSocket.h"
#include "Common.h"
#include "FileDir.h"
//�ļ�������ͼ�Ի���
// CFileView �Ի���

static CString GetRealSize(__int64 nSize);   //�ļ���С��ʽ��

class CFileView : public CDialog
{
	DECLARE_DYNAMIC(CFileView)

public:
	CFileView(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CFileView();

// �Ի�������
	enum { IDD = IDD_DLG_FILEVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CRect m_rect;
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	int m_nDirCount;  //�ļ�������
	CWnd *m_pWndMsg;   //������ָ��
    CBitmap m_MenuBmp[8];   //�˵���ͼ��
	bool m_bCopy;    //�Ƿ���ָ���
public:
	CTreeCtrl m_treeFile;  //���ؼ�
	CListCtrl m_listCtrl;  //�б�ؼ�
	CImageList m_ImageTree;   //���ؼ�ͼ���б�
	CImageList m_ImageList;   //list�ؼ�ͼ���б�
	HTREEITEM m_hItem;   //���ؼ���ǰѡ����ľ��
	HTREEITEM m_Root_tree;  //���ؼ����ڵ�
	virtual BOOL OnInitDialog();
	void InitDlg();  //����ĳ�ʼ��
	void SetDiskInfo(DRIVER_C tagDisk);  //���ô�����Ϣ
	void SetFileListInfo( IN FILEINFO_C tagFileInfo );  //�����ļ��б���Ϣ
	void DelChildItem(IN HTREEITEM hItem);  //ɾ���ļ����µ�����
	CString GetCurFilePath(IN HTREEITEM hCurrentItem);  //��ȡ��ǰѡ�����·��
	HTREEITEM GetCurhItemByPath(IN CString strFolderName);
    void UpDataStatusBar(HTREEITEM hItem);  //����״̬������Ϣ 
	void SetFileExecInfo(IN FILEEXECINFO_C tagFileInfo);  //�����ļ�ִ�нṹ��Ϣ
	void SetArrtibuteInfo(IN ATTRIBUTE_C tagAttribute);  //��ʾ���ԶԻ���
public:
	afx_msg void OnNMClickTreeFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileDownload();   //�ļ�����
	afx_msg void OnFileUpload();   //�ļ��ϴ�
	afx_msg void OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileRun();
	afx_msg void OnFileRefresh();
	afx_msg void OnFolderCreate();
	afx_msg void OnFileDel();
	afx_msg void OnFileCopy();
	afx_msg void OnFilePaste();
	afx_msg void OnFileRename();
	afx_msg void OnFileAttribute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
