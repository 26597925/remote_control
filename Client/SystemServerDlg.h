#pragma once


// CSystemServerDlg �Ի���

class CSystemServerDlg : public CDialog
{
	DECLARE_DYNAMIC(CSystemServerDlg)

public:
	CSystemServerDlg(CWnd* pParent = NULL, SOCKET sock = NULL);   // ��׼���캯��
	virtual ~CSystemServerDlg();

// �Ի�������
	enum { IDD = IDD_DLG_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	SOCKET m_socket;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnCancel();
	virtual void PostNcDestroy();
};
