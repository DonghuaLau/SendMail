// UserInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SendMail.h"
#include "UserInfoDlg.h"
#include "afxdialogex.h"
#include "sendmaillib.h"

// CUserInfoDlg �Ի���

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


// CUserInfoDlg ��Ϣ�������


BOOL CUserInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	SetWindowText("�˺Ź���");

	SetAllText();

	return TRUE;
}


void CUserInfoDlg::OnTestServer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		MessageBox("�����ʼ���������SMTP�����Գɹ���\n��������ʹ��.", "������ʾ", MB_OK);
	}else if(ret == UNDETAILED_INFO){
		MessageBox("�����ʼ���������SMTP������ʧ�ܣ�\n���ṩ��������Ϣ.", "������ʾ", MB_OK | MB_ICONWARNING);
	}else if(ret == CONNECT_ERR){
		MessageBox("�����ʼ���������SMTP������ʧ�ܣ�\n��ȷ����������Ϣ.", "������ʾ", MB_OK | MB_ICONWARNING);
	}else if(ret == LOGON_ERR){
		MessageBox("�����ʼ���������SMTP������ʧ�ܣ�\n��ȷ���û���Ϣ.", "������ʾ", MB_OK | MB_ICONWARNING);
	}else{
		MessageBox("�����ʼ���������SMTP������ʧ�ܣ�\nδ֪������.", "������ʾ", MB_OK | MB_ICONWARNING);
	}

}


void CUserInfoDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
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