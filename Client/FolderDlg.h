#pragma once

//����Ŀ¼
// CFolderDlg �Ի���

class CFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderDlg)

public:
	CFolderDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFolderDlg();

// �Ի�������
	enum { IDD = IDD_DLG_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString m_strFolderName;  //�ļ�Ŀ¼��
public:
	bool m_bOk;
	CString GetFolderName() {return m_strFolderName;}
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
