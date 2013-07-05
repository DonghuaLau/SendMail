/************************************************************************
* 发送邮件模块
*可以发送文本和附件（支持多个附件一起发送）
*Date:2011-12-01
************************************************************************/
#include "StdAfx.h"
#include "sendmaillib.h"
#include "base64.h"

#pragma comment(lib,"WSOCK32")


CSendMail::CSendMail(void)
{
	m_pcFileBuff=NULL;
	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	memset(m_cReceiveBuff,0,sizeof(m_cReceiveBuff));
}


CSendMail::~CSendMail(void)
{
	DeleteAllPath();
}


void CSendMail::Char2Base64(char* pBuff64,char* pSrcBuff,int iLen)
{
	//1   1   1   1   1   1   1   1
	// 分配给pBuff64  ↑ 分配给pBuff64+1
	//		   point所在的位置
	static char Base64Encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";//base64所映射的字符表
	int point;//每一个源字符拆分的位置，可取2,4,6；初始为2
	point=2;
	//int i;
	int iIndex;//base64字符的索引
	wchar_t n=0;//上一个源字符的残留值
	for(int i=0;i<iLen;i++)
	{
		if(point==2)
		{
			iIndex=((*pSrcBuff)>>point)&0x3f;//取得pSrcBuff的高point位
		}
		else if (point==4)
		{
			iIndex=((*pSrcBuff)>>point)&0xf;//取得pSrcBuff的高point位
		}
		else if(point==6)
		{
			iIndex=((*pSrcBuff)>>point)&0x3;//取得pSrcBuff的高point位
		}
		iIndex+=n;//与pSrcBuff-1的低point结合组成Base64的索引
		*pBuff64++=Base64Encode[iIndex];//由索引表得到pBuff64
		n=((*pSrcBuff)<<(6-point));//计算源字符中的残留值
		n=n&0x3f;//确保n的最高两位为0
		point+=2;//源字符的拆分位置上升2
		if(point==8)//如果拆分位置为8说明pSrcBuff有6位残留，可以组成一个完整的Base64字符，所以直接再组合一次
		{
			iIndex=(*pSrcBuff)&0x3f;//提取低6位，这就是索引了
			*pBuff64++=Base64Encode[iIndex];//
			n=0;//残留值为0
			point=2;//拆分位置设为2
		}
		pSrcBuff++;

	}
	if(n!=0)
	{
		*pBuff64++=Base64Encode[n];
	}
	if(iLen%3==2)//如果源字符串长度不是3的倍数要用'='补全
	{
		*pBuff64='=';
	}
	else if(iLen%3==1)
	{
		*pBuff64++='=';
		*pBuff64='=';
	}
}

void CSendMail::AddFilePath(char * pcFilePath)//添加附件路径
{
	if(pcFilePath==NULL)
	{
		return;
	}
	unsigned int i;
	char* temp;
	for(i=0;i<m_pcFilePathList.size();i++)
	{
		temp=m_pcFilePathList.at(i);
		if(strcmp(pcFilePath,temp)==0)//如果已经存在就不用再添加了
		{
			return;
		}
	}
	char *sPath = new char[MAX_PATH];//注意，删除附件路径时，该内存必须释放掉
	memcpy(sPath, pcFilePath, strlen(pcFilePath)+1);
	m_pcFilePathList.push_back(sPath);
}

void CSendMail::DeleteFilePath(char* pcFilePath)//删除附件路径
{
	unsigned int i;
	char* temp;
	for(i=0;i<m_pcFilePathList.size();i++)
	{
		temp=m_pcFilePathList.at(i);
		if(strcmp(temp,pcFilePath)==0)//找到并删除它，如果没找到就算了
		{
			//处理因更改，未做测试
			//m_pcFilePathList.RemoveAt(m_pcFilePathList.FindIndex(i));
			delete[] temp;//注意，该内存必须释放掉
			return;
		}
	}
}


void CSendMail::DeleteAllPath(void)
{
	unsigned int i;
	char* temp;
	for(i=0;i<m_pcFilePathList.size();i++)
	{
		temp=m_pcFilePathList.at(i);
		delete[] temp;//注意，该内存必须释放掉
	}
	m_pcFilePathList.clear();
}

int CSendMail::GetFileData(char* FilePath)
{
	m_pcFileBuff=NULL;
	if(FilePath==NULL)
	{
		return 0;
	}
	
	CFile f;
	//FILE *f;
	int len;
	
	
	USES_CONVERSION;
	if(!f.Open(FilePath,CFile::modeRead|CFile::modeNoTruncate|CFile::typeBinary))
	{
		return 0;
	}
	len=(int)f.GetLength();
	m_pcFileBuff=new char[len+1];
	memset(m_pcFileBuff,0,len+1);
	f.Read(m_pcFileBuff,len);

	f.Close();
	return len;
}

/*修改了相对路径不能提取文件名的bug*/
void CSendMail::GetFileName(char* fileName,char* filePath)
{
	if(filePath==NULL || fileName==NULL)
	{
		return;
	}
	int len = strlen(filePath);
	for(int i=len-1; i>0; i--)
	{
		if(filePath[i]=='\\')
		{
			memcpy(fileName,filePath+i+1, len-i-1);
			fileName[len-i] = 0;
			return;
		}
	}
	memcpy(fileName,filePath, len+1);
}

bool CSendMail::CReateSocket(SOCKET &sock)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		return false;
	}
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
		HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup( );
		return false; 
	}
	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
	if (sock == INVALID_SOCKET)
	{
		return false;
	}

	sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(m_sMailInfo.m_port);//发邮件一般都是25端口
	if(m_sMailInfo.m_pcIPName=="")
	{
		servaddr.sin_addr.s_addr = inet_addr(m_sMailInfo.m_pcIPAddr);//直接使用IP地址
	}
	else
	{
		struct hostent *hp=gethostbyname(m_sMailInfo.m_pcIPName);//使用名称
		servaddr.sin_addr.s_addr=*(int*)(*hp->h_addr_list);
	}


	int ret = connect(sock,(sockaddr*)&servaddr,sizeof(servaddr));//建立连接
	if (ret == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}


bool CSendMail::Logon(SOCKET &sock)
{
	recv(sock,m_cReceiveBuff,1024,0);

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	//sprintf_s(m_cSendBuff,"HELO []\r\n");
	sprintf_s(m_cSendBuff,"EHLO Liudonghua\r\n");
	send(sock,m_cSendBuff,strlen(m_cSendBuff),0);//开始会话

	int n = 0, flag = 0;;
	while(n < 1){
		recv(sock,m_cReceiveBuff,1024,0);
		if(m_cReceiveBuff[0]!='2' || m_cReceiveBuff[1]!='5' || m_cReceiveBuff[2]!='0')
		{
			flag = 0;
		}else{
			flag = 1;
			break;
		}
		n ++;
	}
	if(flag == 0){
		return false;
	}

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff,"AUTH LOGIN\r\n");
	send(sock,m_cSendBuff,strlen(m_cSendBuff),0);//请求登录
	recv(sock,m_cReceiveBuff,1024,0);
	if(m_cReceiveBuff[0]!='3' || m_cReceiveBuff[1]!='3' || m_cReceiveBuff[2]!='4')
	{
		return false;
	}

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	Char2Base64(m_cSendBuff,m_sMailInfo.m_pcUserName,strlen(m_sMailInfo.m_pcUserName));
	m_cSendBuff[strlen(m_cSendBuff)]='\r';
	m_cSendBuff[strlen(m_cSendBuff)]='\n';
	send(sock,m_cSendBuff,strlen(m_cSendBuff),0);//发送用户名
	recv(sock,m_cReceiveBuff,1024,0);
	if(m_cReceiveBuff[0]!='3' || m_cReceiveBuff[1]!='3' || m_cReceiveBuff[2]!='4')
	{
		return false;
	}

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	Char2Base64(m_cSendBuff,m_sMailInfo.m_pcUserPassWord,strlen(m_sMailInfo.m_pcUserPassWord));
	m_cSendBuff[strlen(m_cSendBuff)]='\r';
	m_cSendBuff[strlen(m_cSendBuff)]='\n';
	send(sock,m_cSendBuff,strlen(m_cSendBuff),0);//发送用户密码
	recv(sock,m_cReceiveBuff,1024,0);
	if(m_cReceiveBuff[0]!='2' || m_cReceiveBuff[1]!='3' || m_cReceiveBuff[2]!='5')
	{
		return false;
	}
	return true;//登录成功
}


bool CSendMail::SendHead(SOCKET &sock)
{
	int rt;
	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff,"MAIL FROM:<%s>\r\n",m_sMailInfo.m_pcSender);
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	
	if(rt!=strlen(m_cSendBuff))
	{
		return false;
	}
	recv(sock,m_cReceiveBuff,1024,0);

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff,"RCPT TO:<%s>\r\n",m_sMailInfo.m_pcReceiver);
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	if(rt!=strlen(m_cSendBuff))
	{
		return false;
	}
	recv(sock,m_cReceiveBuff,1024,0);

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	memcpy(m_cSendBuff,"DATA\r\n",strlen("DATA\r\n"));
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	if(rt!=strlen(m_cSendBuff))
	{
		return false;
	}
	recv(sock,m_cReceiveBuff,1024,0);

	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff,"From: %s <%s>\r\n",m_sMailInfo.m_pcSenderName,m_sMailInfo.m_pcSender);
	//sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)],150,"To:\"INVT.COM.CN\"<%s>\r\n",m_sMailInfo.m_pcReceiver);
	sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)],150,"To: %s <%s>\r\n",m_sMailInfo.m_pcReceiver, m_sMailInfo.m_pcReceiver);
	sprintf_s(&m_cSendBuff[strlen(m_cSendBuff)],150,"Subject:%s\r\nMime-Version: 1.0\r\nContent-Type: multipart/mixed;   boundary=\"INVT\"\r\n\r\n",m_sMailInfo.m_pcTitle);
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	if(rt!=strlen(m_cSendBuff))
	{
		return false;
	}

	return true;
}

bool CSendMail::SendTextBody(SOCKET &sock)
{
	int rt;
	memset(m_cSendBuff,0,sizeof(m_cSendBuff));
	sprintf_s(m_cSendBuff,"--INVT\r\nContent-Type: text/plain;\r\n  charset=\"gb2312\"\r\n\r\n%s\r\n\r\n",m_sMailInfo.m_pcBody);
	rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	if(rt!=strlen(m_cSendBuff))
	{
		return false;
	}
	else
	{
		return true;
	}
}

int CSendMail::SendBuf(SOCKET so, char *buf, int buf_len, int flags)
{
	const int BSIZE = 1460;
	int pos = 0, ret = 0;
	while(pos < buf_len){
		ret = Sendn(so, buf + pos, min(buf_len - pos, BSIZE), flags);
		if(ret == -1){
			return -1;
		}else pos += ret;
	}
	return pos; 
}

int CSendMail::Sendn(SOCKET so, char *buf, int buf_len, int flags)
{
	int ret = 0;
	int pos = 0;
	int slen = buf_len;
	int trytimes = 5;
	while((ret = send(so, buf + pos, slen, flags)) < slen){
		Sleep(500);
		if(ret != -1){
			pos = ret;
			slen -= ret;
		}else{
			trytimes --;
			if(trytimes == 0){
				return -1;
			}
		}		
	}
	slen -= ret;
	return buf_len - slen;
}

int CSendMail::SendFileBody(SOCKET &sock)
{
	CBase64 b64;
	unsigned long boutlen;
	unsigned int i;
	char* filePath;
	int rt, ret = 0;
	int len = 0;
	const int BSIZE = 3000;
	int pt=0;
	char fileName[128];
	for(i=0;i<m_pcFilePathList.size();i++)
	{
		pt=0;
		memset(fileName,0,128);
		filePath=m_pcFilePathList.at(i);
		len=GetFileData(filePath);
		GetFileName(fileName,filePath);

		if(fileName[0] == 0){
			ret = INVALID_FILE_NAME;
			delete [] m_pcFileBuff;
			break;
		}

		sprintf_s(m_cSendBuff,"--INVT\r\nContent-Type: application/octet-stream;\r\n  name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment;\r\n  filename=\"%s\"\r\n\r\n",fileName,fileName);
	
		send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
		while (pt<len)
		{
			memset(m_cSendBuff,0,sizeof(m_cSendBuff));
#if 0
			Char2Base64(m_cSendBuff,&m_pcFileBuff[pt],min(len-pt,BSIZE));
			m_cSendBuff[strlen(m_cSendBuff)]='\r';
			m_cSendBuff[strlen(m_cSendBuff)]='\n';
			rt=send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
			pt+=min(len-pt,BSIZE);
			if(rt!=strlen(m_cSendBuff))
			{
				return SEND_SOCKET_ERR;
			}
#else
			boutlen = 4096;
			b64.Encode((unsigned char *)m_pcFileBuff + pt, (unsigned long)min(len-pt,BSIZE), (unsigned char *)m_cSendBuff, &boutlen);
			m_cSendBuff[strlen(m_cSendBuff)]='\r';
			m_cSendBuff[strlen(m_cSendBuff)]='\n';
			rt=Sendn(sock,m_cSendBuff,strlen(m_cSendBuff),0);
			pt+=min(len-pt,BSIZE);
			if(rt == -1)
			{
				delete [] m_pcFileBuff;
				return SEND_SOCKET_ERR;
			}
#endif
		}	
		if(len!=0)
		{
			delete [] m_pcFileBuff;
		}	
	}
	

	return ret;
}

bool CSendMail::SendEnd(SOCKET &sock)
{
	sprintf_s(m_cSendBuff,"--INVT--\r\n.\r\n");
	send(sock,m_cSendBuff,strlen(m_cSendBuff),0);

	//sprintf_s(m_cSendBuff,"QUIT\r\n");
	//send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	//closesocket(sock);
	//WSACleanup();
	return true;
}

bool CSendMail::CloseSocket(SOCKET &sock)
{
	//sprintf_s(m_cSendBuff,"--INVT--\r\n.\r\n");
	//send(sock,m_cSendBuff,strlen(m_cSendBuff),0);

	sprintf_s(m_cSendBuff,"QUIT\r\n");
	send(sock,m_cSendBuff,strlen(m_cSendBuff),0);
	closesocket(sock);
	WSACleanup();
	return true;
}


int CSendMail::SendMail(sMailInfo &smailInfo, vector<struct failedInfo> &failedList)
{
	memcpy(&m_sMailInfo,&smailInfo,sizeof(smailInfo));
	if(m_sMailInfo.m_pcBody==NULL
		/*|| m_sMailInfo.m_pcIPAddr==NULL*/
		|| m_sMailInfo.m_pcIPName==NULL
		|| m_sMailInfo.m_pcReceiver==NULL
		|| m_sMailInfo.m_pcSender==NULL
		|| m_sMailInfo.m_pcSenderName==NULL
		|| m_sMailInfo.m_pcTitle==NULL
		|| m_sMailInfo.m_pcUserName==NULL
		|| m_sMailInfo.m_pcUserPassWord==NULL)
	{
		return UNDETAILED_INFO;
	}
	SOCKET sock;
	if(!CReateSocket(sock))//建立连接
	{
		return CONNECT_ERR;
	}

	if(!Logon(sock))//登录邮箱
	{
		return LOGON_ERR;
	}

	//给多少发邮件，m_sMailInfo.m_pcSender中以分号隔开收件人
	char *receivers = m_sMailInfo.m_pcReceiver;
	char *ptr = receivers;
	char recer[64];
	int pos = 0;
	int ret = 0;
	struct failedInfo failedItem;
	while(true){

		if(ret != 0){
			failedItem.failedemail = recer;
			failedItem.failedno = ret;
			failedList.push_back(failedItem);
			ret = 0;
		}

		pos = 0;		
		if(ptr[pos] == '\0') break;
		while(ptr[pos] != ';' && ptr[pos] != '\0'){
			pos++;
		}

		//简单判断邮件地址是否有效
		if(pos > 2){ 
			memcpy(recer, ptr, pos);
			recer[pos] = '\0';
			m_sMailInfo.m_pcReceiver = recer;			

			if(!SendHead(sock))//发送邮件头
			{
				ret = SEND_HEAD_ERR;
				continue;
			}

			if(!SendTextBody(sock))//发送邮件文本部分
			{
				ret = SEND_TEXT_ERR;
				continue;
			}
			ret = SendFileBody(sock);
			if(ret != NO_ERR)//发送附件
			{
				continue;
			}			
		
			if(!SendEnd(sock))//结束邮件，并关闭sock
			{
				ret = SEND_END_ERR;
				continue;
			}
		}
		if(ptr[pos] != '\0') ptr += pos + 1;
		else break;
		Sleep(500);
	}
	
	if(!CloseSocket(sock))//结束邮件，并关闭sock
	{
		return CLOSE_ERR;
	}
	
	return ret;
}

int CSendMail::TestServer(sMailInfo &smailInfo)
{
	memcpy(&m_sMailInfo,&smailInfo,sizeof(smailInfo));
	if(/*m_sMailInfo.m_pcBody==NULL*/
		/*|| m_sMailInfo.m_pcIPAddr==NULL
		|| */m_sMailInfo.m_pcIPName==NULL
		/*|| m_sMailInfo.m_pcReceiver==NULL*/
		|| m_sMailInfo.m_pcSender==NULL
		/*|| m_sMailInfo.m_pcSenderName==NULL*/
		/*|| m_sMailInfo.m_pcTitle==NULL*/
		/*|| m_sMailInfo.m_pcUserName==NULL*/
		|| m_sMailInfo.m_pcUserPassWord==NULL)
	{
		return UNDETAILED_INFO;
	}
	SOCKET sock;
	if(!CReateSocket(sock))//建立连接
	{
		return CONNECT_ERR;
	}

	if(!Logon(sock))//登录邮箱
	{
		return LOGON_ERR;
	}	
	
	if(!CloseSocket(sock))//结束邮件，并关闭sock
	{
		;//return CLOSE_ERR;
	}
	
	return NO_ERR;
}