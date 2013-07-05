// UserInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SendMail.h"
#include "UserInfoDlg.h"
#include "afxdialogex.h"
#include "sendmaillib.h"

// CUserInfoDlg 对话框

IMPLEMENT_DYNAMIC(CUserInfoDlg, CDialogEx)

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUserInfoDlg::IDD, pParent)
{

}

CUserInfoDlg::~CUserInfoDlg()
{
}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EmailAddr);
	DDX_Control(pDX, IDC_EDIT2, m_Passwd);
	DDX_Control(pDX, IDC_EDIT3, m_POP3Server);
	DDX_Control(pDX, IDC_EDIT4, m_POP3Port);
	DDX_Control(pDX, IDC_EDIT5, m_SendName);
}


BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CUserInfoDlg::OnTestServer)
END_MESSAGE_MAP()


// CUserInfoDlg 消息处理程序


BOOL CUserInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetWindowText("账号管理");

	SetAllText();

	return TRUE;
}


void CUserInfoDlg::OnTestServer()
{
	// TODO: 在此添加控件通知处理程序代码
	GetAllText();

	CSendMail sendmail;
	sMailInfo smailInfo;
	memset(&smailInfo, 0, sizeof(sMailInfo));

	smailInfo.m_pcUserName = m_EmailUserInfo.emailaddr.GetBuffer();
	smailInfo.m_pcUserPassWord = m_EmailUserInfo.passwd.GetBuffer();
	smailInfo.m_pcSenderName = m_EmailUserInfo.emailaddr.GetBuffer();
	smailInfo.m_pcSender = m_EmailUserInfo.emailaddr.GetBuffer();
	smailInfo.m_pcReceiver = NULL;
	smailInfo.m_pcTitle = NULL;
	smailInfo.m_pcBody = NULL;
	smailInfo.m_pcIPAddr = NULL;
	smailInfo.m_pcIPName = m_EmailUserInfo.pop3server.GetBuffer();
	if(m_EmailUserInfo.pop3port.IsEmpty()){
		m_EmailUserInfo.pop3port = "25";
		smailInfo.m_port = 25;
	}else{
		smailInfo.m_port = (unsigned short)StrToIntA(m_EmailUserInfo.pop3port);
	}

	int ret = sendmail.TestServer(smailInfo);
	if(ret == 0){
		MessageBox("发送邮件服务器（SMTP）测试成功！\n可以正常使用.", "测试提示", MB_OK);
	}else if(ret == UNDETAILED_INFO){
		MessageBox("发送邮件服务器（SMTP）测试失败！\n请提供完整的信息.", "测试提示", MB_OK | MB_ICONWARNING);
	}else if(ret == CONNECT_ERR){
		MessageBox("发送邮件服务器（SMTP）测试失败！\n请确定服务器信息.", "测试提示", MB_OK | MB_ICONWARNING);
	}else if(ret == LOGON_ERR){
		MessageBox("发送邮件服务器（SMTP）测试失败！\n请确定用户信息.", "测试提示", MB_OK | MB_ICONWARNING);
	}else{
		MessageBox("发送邮件服务器（SMTP）测试失败！\n未知道错误.", "测试提示", MB_OK | MB_ICONWARNING);
	}

}


void CUserInfoDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	GetAllText();
	CDialogEx::OnOK();
}

void CUserInfoDlg::GetAllText()
{
	m_SendName.GetWindowTextA(m_EmailUserInfo.sendname);
	m_EmailAddr.GetWindowTextA(m_EmailUserInfo.emailaddr);
	m_Passwd.GetWindowTextA(m_EmailUserInfo.passwd);
	m_POP3Server.GetWindowTextA(m_EmailUserInfo.pop3server);
	m_POP3Port.GetWindowTextA(m_EmailUserInfo.pop3port);
}

void CUserInfoDlg::SetAllText()
{
	m_SendName.SetWindowTextA(m_EmailUserInfo.sendname);
	m_EmailAddr.SetWindowTextA(m_EmailUserInfo.emailaddr);
	m_Passwd.SetWindowTextA(m_EmailUserInfo.passwd);
	m_POP3Server.SetWindowTextA(m_EmailUserInfo.pop3server);
	m_POP3Port.SetWindowTextA(m_EmailUserInfo.pop3port);
}