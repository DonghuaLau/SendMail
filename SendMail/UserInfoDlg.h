#pragma once
#include "afxwin.h"

enum POP3_SERVER_STATUS{UNTEST = 0, TESTED, TESTFAILED};

struct EmailUserInfo{
	CString sendname;
	CString emailaddr;
	CString passwd;
	CString pop3server;
	CString pop3port;
	int serverstatus;//0, UNTEST; 1, TESTED; 2, TESTFAILED
};

// CUserInfoDlg 对话框

class CUserInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserInfoDlg)

public:
	CUserInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUserInfoDlg();

// 对话框数据
	enum { IDD = IDD_USER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	struct EmailUserInfo m_EmailUserInfo;
	CEdit m_EmailAddr;
	CEdit m_Passwd;
	CEdit m_POP3Server;
	CEdit m_POP3Port;
	CEdit m_SendName;
public:
	virtual BOOL OnInitDialog();

	void SetEmailUserInfo(struct EmailUserInfo &eui){
		m_EmailUserInfo = eui;
	}
	void GetEmailUserInfo(struct EmailUserInfo &eui)
	{
		eui = m_EmailUserInfo;
	}
	

	afx_msg void OnTestServer();
	virtual void OnOK();

private:
	void GetAllText();
	void SetAllText();
	
};
