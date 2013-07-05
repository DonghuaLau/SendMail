/***********************************************************************
*�����ʼ�ģ��ͷ�ļ�
*���Է����ı��͸�����֧�ֶ������һ���ͣ�
*************************************************************************/
#pragma once
#include "StdAfx.h"
#include <winsock2.h>
//#include <afxtempl.h>
#include <vector>
using namespace std;

//typedef UINT_PTR SOCKET;

struct sMailInfo //�ʼ���Ϣ
{
	char*	m_pcUserName;//�û���¼���������
	char*	m_pcUserPassWord;//�û���¼���������
	char*	m_pcSenderName;//�û�����ʱ��ʾ������
	char*	m_pcSender;//�����ߵ������ַ
	char*	m_pcReceiver;//�����ߵ������ַ
	char*	m_pcTitle;//�������
	char*	m_pcBody;//�ʼ��ı�����
	char*	m_pcIPAddr;//��������IP
	char*	m_pcIPName;//�����������ƣ�IP�����ƶ�ѡһ������ȡ���ƣ�
	unsigned short m_port;//�������Ķ˿ڣ�Ĭ��25
	sMailInfo(){memset(this,0,sizeof(sMailInfo));}
};

struct failedInfo{
	int failedno;
	string failedemail;
};


class CSendMail
{
public:
	CSendMail(void);
	~CSendMail(void);

public:
	int SendMail(sMailInfo &smailInfo, vector<struct failedInfo> &failedList);//�����ʼ�����Ҫ�ڷ��͵�ʱ���ʼ���ʼ���Ϣ
	void AddFilePath(char * pcFilePath);//��Ӹ����ľ���·���������б���
	void DeleteFilePath(char* pcFilePath);//ɾ������·��������еĻ�
	void DeleteAllPath(void);//ɾ��ȫ��������·��
	int TestServer(sMailInfo &smailInfo);

protected:
	void GetFileName(char* fileName,char* filePath);//�Ӹ�����·���л�ȡ�ļ�����
	void Char2Base64(char* pBuff64,char* pSrcBuff,int iLen);//��char����ת����Base64����
	bool  CReateSocket(SOCKET &sock);//����socket����
	bool  CloseSocket(SOCKET &sock);//����socket����
	bool Logon(SOCKET &sock);//��¼���䣬��Ҫ���з��ʼ�ǰ��׼������
	int GetFileData(char* FilePath);//���ļ�·����ȡ��������

	bool SendHead(SOCKET &sock);//�����ʼ�ͷ
	bool SendTextBody(SOCKET &sock);//�����ʼ��ı�����
	int SendFileBody(SOCKET &sock);//�����ʼ�����
	bool SendEnd(SOCKET &sock);//�����ʼ���β

	int SendBuf(SOCKET so, char *buf, int buf_len, int flags);
	int Sendn(SOCKET so, char *buf, int buf_len, int flags);
protected:
	vector<char*> m_pcFilePathList;//��¼����·��

	char  m_cSendBuff[4096];//���ͻ�����
	char  m_cReceiveBuff[1024];
	char* m_pcFileBuff;//ָ�򸽼�����
	sMailInfo m_sMailInfo;


};


enum SENDMAILERROR{
	NO_ERR = 0,
	CONNECT_ERR,
	LOGON_ERR,
	SEND_HEAD_ERR,
	SEND_TEXT_ERR,
	SEND_FILE_ERR,
	INVALID_FILE_NAME,
	SEND_SOCKET_ERR,
	SEND_END_ERR,
	UNDETAILED_INFO,
	CLOSE_ERR,

};