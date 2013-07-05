
// SendMailDlg.h : ͷ�ļ�
//

#pragma once
#include "stdafx.h"
#include "GridCtrl_src/GridCtrl.h"
//#include "afxwin.h"
#include "UserInfoDlg.h"
#include "sendmaillib.h"
//#include "sendmail_1.h"
#include <fstream>
#include "afxcmn.h"
#include "afxwin.h"

class CSendInfo{
public:
	CString m_RecvAddr;
	CString m_Name;
	CString m_Title;
	CString m_Content;
	CString Appdsize;
	CString AppendixPaths;
	int m_Check;//0, unchecked; 1, checked;

	CString GetRecvAddr(CString &addr, int index = 1);//��ȡ��index���ռ��˵�ַ
	CString GetAppendixPath(CString &path, int index = 1);//��ȡ��index������·��
	bool IsAllEmpty();
};

// CSendMailDlg �Ի���
class CSendMailDlg : public CDialogEx
{
// ����
public:
	CSendMailDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSendMailDlg();

// �Ի�������
	enum { IDD = IDD_SENDMAIL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void InitGridCtrl();
	int GetRowColByPos(const CPoint &point, int &row, int &col);
	

	afx_msg void OnEmailGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo); 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	CStatic m_UserStatus;
	CEdit m_AppendixDir;
	struct EmailUserInfo m_EmailUserInfo;
	CGridCtrl m_EmailGrid;
	CRect m_GridRect;
	CRect m_DlgRect;
	CEdit m_Log;
	CSendMail m_Mail;
	CString m_WorkDir;

	vector<CSendInfo> m_MailItem;

	//�������ǲ�����
	//vector< vector<CString>> m_Appendixs;//Ϊÿ�б������и�������·�����б���ֻ��ʾ�ļ����ʹ�С��

	ofstream m_Flog;
	bool OpenLog();
	bool CloseLog();

	int ReadConfig();
	int SaveConfig();
	int ReadEmailConfig(string line, CSendInfo &item);
	int ReadAccountConfig(string line);


	int DoSendMail(CSendInfo &pSendInfo);


	unsigned long m_ShowThread;	
	inline int SendingProcess();
	static unsigned int WINAPI SendingThreadProc(void *para);
	int Sending();

	int m_SendingFlag;//0, no operation; 1, sending, 2, cancel sending

	CProgressCtrl m_ShowProcess;
	CStatic m_ShowProcessText;

	int GetScrollRowAndCol(int &row, int &col);

public:
	afx_msg void OnBrowseAppendixDir();	
	afx_msg void OnAddRow();
	afx_msg void OnDelRow();
	afx_msg void OnSendMail();
	afx_msg void OnUserMng();
	afx_msg void OnMenuAboutbox();
	virtual void OnOK();
	virtual void OnCancel();
	
	afx_msg void OnSaveConfig();
};
