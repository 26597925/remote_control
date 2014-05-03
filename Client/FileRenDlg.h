#pragma once

//�ļ����У�������
// CFileRenDlg �Ի���

class CFileRenDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileRenDlg)

public:
	CFileRenDlg(CWnd* pParent = NULL, CString strFileName = _T(""));   // ��׼���캯��
	virtual ~CFileRenDlg();

// �Ի�������
	enum { IDD = IDD_DLG_RENAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	bool m_bOk;
	CString GetFileName() {return m_strFileName;}
private:
	CString m_strFileName;  //���ļ�(��)����
	CString m_strDefault;   //���ļ�(��)����
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
