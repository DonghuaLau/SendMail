
// SendMailDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SendMail.h"
#include "SendMailDlg.h"
#include "afxdialogex.h"
#include "UserInfoDlg.h"
#include "common.h"
#include "Base64.h"
#include "NewCellTypes\GridCellCheck.h"

//#include <fstream>
#include <string>
#include <io.h>
#include <sys/stat.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnUserMng();
//	afx_msg void OnMenuAboutbox();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



CString CSendInfo::GetRecvAddr(CString &addr, int index/* = 1*/){//��ȡ��index���ռ��˵�ַ
	int nth = 0;
	int startpos = 0, lastpos = 0, findpos = 0;
	int len = m_RecvAddr.GetLength();
	addr = "";
	while(startpos < len){
		findpos = m_RecvAddr.Find(';', startpos);
		if(findpos == -1) break;
		nth ++;
		if(nth == index){
			if(findpos > lastpos + 1)
				addr = m_RecvAddr.Mid(lastpos+1, findpos - lastpos - 1);
			break;
		}		
		lastpos = findpos;
	}
	return addr;
}

CString CSendInfo::GetAppendixPath(CString &path, int index/* = 1*/){//��ȡ��index������·��
	int nth = 0;
	int lastpos = 0, findpos = 0;
	int len = AppendixPaths.GetLength();
	path = "";
	while(lastpos < len){
		findpos = AppendixPaths.Find(';', lastpos);
		if(findpos == -1) break;
		nth ++;
		if(nth == index){
			if(findpos > lastpos)				
				path = AppendixPaths.Mid(lastpos, findpos - lastpos);
			break;
		}
		//startpos += findpos;
		lastpos = findpos + 1;
	}	
	return path;
}

bool CSendInfo::IsAllEmpty()
{
	int size = m_RecvAddr.IsEmpty() + m_Name.IsEmpty() + m_Title.IsEmpty() + m_Content.IsEmpty() + AppendixPaths.IsEmpty();
	if(size == 5) return true;
	else return false;
}

// CSendMailDlg �Ի���

CSendMailDlg::CSendMailDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSendMailDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAIN_ICON);//IDR_MAINFRAME);	
}

CSendMailDlg::~CSendMailDlg()
{
	CloseLog();
}

void CSendMailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM1, m_EmailGrid);
	DDX_Control(pDX, IDC_USER_STATUS, m_UserStatus);
	DDX_Control(pDX, IDC_EDIT1, m_AppendixDir);
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Control(pDX, IDC_PROGRESS1, m_ShowProcess);
	DDX_Control(pDX, IDC_PROCESS_TEXT, m_ShowProcessText);
}

BEGIN_MESSAGE_MAP(CSendMailDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_NOTIFY(NM_RCLICK, IDC_CUSTOM1, OnEmailGridRClick)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON1, &CSendMailDlg::OnBrowseAppendixDir)
	ON_BN_CLICKED(IDC_BUTTON3, &CSendMailDlg::OnAddRow)
	ON_BN_CLICKED(IDC_BUTTON4, &CSendMailDlg::OnDelRow)
	ON_BN_CLICKED(IDC_BUTTON2, &CSendMailDlg::OnSendMail)
	ON_COMMAND(ID_USER_MNG, &CSendMailDlg::OnUserMng)
	ON_COMMAND(IDS_MENU_ABOUTBOX, &CSendMailDlg::OnMenuAboutbox)
	ON_COMMAND(ID_SAVE_CONFIG, &CSendMailDlg::OnSaveConfig)
END_MESSAGE_MAP()


// CSendMailDlg ��Ϣ�������

BOOL CSendMailDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	CString msg;
	ReadConfig();
	OpenLog();
	InitGridCtrl();
	m_SendingFlag = 0;
	m_AppendixDir.SetWindowTextA(m_WorkDir);
	if(m_EmailUserInfo.sendname.IsEmpty()){
		m_UserStatus.SetWindowTextA("��ǰ�û���δ����");
	}else{
		
		msg.Format("��ǰ�û���%s", m_EmailUserInfo.sendname);
		m_UserStatus.SetWindowTextA(msg);
	}
	m_ShowProcessText.SetWindowTextA("");
	m_ShowProcessText.ShowWindow(SW_HIDE);
	m_ShowProcess.ShowWindow(SW_HIDE);
	msg = "";
	msg.Format("[%s] ��ӭʹ��...", current_time());
	m_Log.SetWindowTextA(msg);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSendMailDlg::InitGridCtrl()
{
	CRect rect;//m_EmailGrid
	
    GetDlgItem(IDC_CUSTOM1)->GetWindowRect(rect); //�õ���ʾ����
    ScreenToClient(&rect);
    //m_EmailGrid.Create(rect,this,100);
    m_EmailGrid.SetEditable(TRUE);
    m_EmailGrid.SetTextBkColor(RGB(0xFF, 0xFF, 0xE8)); //��ɫ����

	int MailItemSize = m_MailItem.size();
	int rows = MailItemSize>8?MailItemSize:8;
	int cols = 6;
    m_EmailGrid.SetRowCount(rows); //��ʼΪ10��
    m_EmailGrid.SetColumnCount(cols); //��ʼ��Ϊ11��
    m_EmailGrid.SetFixedRowCount(1); //��ͷΪһ��
    m_EmailGrid.SetFixedColumnCount(1); //��ͷΪһ�� 

	m_EmailGrid.EnableDragAndDrop(TRUE);
	m_EmailGrid.EnableDragRowMode(FALSE);
	m_EmailGrid.EnableScrollBar(SB_VERT, TRUE);
	m_EmailGrid.AutoSizeRows();

	GV_ITEM Item; 
    Item.mask = GVIF_TEXT;//|GVIS_MODIFIED;//|GVIF_FORMAT;
    Item.row = 0;
    Item.col = 0;

	for (int row = 1; row < rows; row++){
        for (int col = 0; col < cols; col++)
        { 
			if (col < 1) //����0�б�ͷ��ʾ
            {
                if(row< m_EmailGrid.GetRowCount())
                {
					Item.row = row;
					Item.col = col;
                    Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
                    Item.strText.Format("%d",row);
					m_EmailGrid.SetItem(&Item); 
					m_EmailGrid.SetCellType(row, col+1, RUNTIME_CLASS(CGridCellCheck));
                }
				break;
            }   
        }   
		m_EmailGrid.SetItemMultiRow(row, 5);
	}
		
    //--------------�������о�------------------
	m_EmailGrid.SetRowHeight(0,25);
	for(int a = 1; a < m_EmailGrid.GetRowCount();a++)
		m_EmailGrid.SetRowHeight(a,40); //���ø��и� 
	
	int col = 0;
	m_EmailGrid.SetColumnWidth(col++,30); //����0�п�
	m_EmailGrid.SetColumnWidth(col++,150);
	m_EmailGrid.SetColumnWidth(col++,80);
	m_EmailGrid.SetColumnWidth(col++,100);
	m_EmailGrid.SetColumnWidth(col++,150);
	//m_EmailGrid.SetColumnWidth(col++,80);
	m_EmailGrid.SetColumnWidth(col++,300);

	Item.mask = GVIF_TEXT;
	Item.nFormat = DT_CENTER |DT_VCENTER;//|DT_WORDBREAK
    Item.row = 0;
	
	Item.col = 0;
	Item.strText = "No";
	m_EmailGrid.SetItem(&Item);

    Item.col++;
	Item.strText = "�ռ��˵�ַ";
	m_EmailGrid.SetItem(&Item);

	Item.col++;
	Item.strText = "�ռ���";
	m_EmailGrid.SetItem(&Item);

	Item.col++;
	Item.strText = "����";
	m_EmailGrid.SetItem(&Item);

	Item.col++;
	Item.strText = "����";
	m_EmailGrid.SetItem(&Item);

	//Item.col++;
	//Item.strText = "������С";
	//m_EmailGrid.SetItem(&Item);

	Item.col++;
	Item.strText = "����";
	m_EmailGrid.SetItem(&Item);

	m_EmailGrid.Invalidate();

	CGridCellCheck* cell;
	int textcol = 1;
	for (int row = 1; row <= MailItemSize; row++){
		textcol = 1;
		if(m_MailItem[row-1].m_Check == 1){
			cell = (CGridCellCheck*)m_EmailGrid.GetCell(row,1);
			cell->SetCheck(TRUE);
		}
		m_EmailGrid.SetItemText(row, textcol++, m_MailItem[row-1].m_RecvAddr);
		m_EmailGrid.SetItemText(row, textcol++, m_MailItem[row-1].m_Name);
		m_EmailGrid.SetItemText(row, textcol++, m_MailItem[row-1].m_Title);
		m_EmailGrid.SetItemText(row, textcol++, m_MailItem[row-1].m_Content);
		//m_EmailGrid.SetItemText(row, textcol++, m_MailItem[row-1].Appdsize);
		m_EmailGrid.SetItemText(row, textcol++, m_MailItem[row-1].AppendixPaths);

		//m_EmailGrid.SetItemMultiRow(row, 6);
	}
	m_MailItem.clear();//�����������������

	UpdateData(FALSE);

	
}

void CSendMailDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSendMailDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSendMailDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSendMailDlg::OnEmailGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	/*static int flag = 0;
	static CMenu menu, *pPopup; 
	static CPoint point;
	GetCursorPos(&point);
	if(flag == 0){
		menu.LoadMenu(IDR_RD_MENU); 
		pPopup = menu.GetSubMenu(0); 
		flag = 1;
	}	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,this);
	*/
}

int CSendMailDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	this->DragAcceptFiles(TRUE); 

	return 0;
}


void CSendMailDlg::OnDropFiles(HDROP hDropInfo)
{
	static CPoint point;
	POINT dragpoint;
	GetCursorPos(&point);
	m_EmailGrid.GetWindowRect(m_GridRect);
	ScreenToClient(m_GridRect);
	::DragQueryPoint(hDropInfo, &dragpoint);
	point.x = dragpoint.x - m_GridRect.left;
	point.y = dragpoint.y - m_GridRect.top;
	GV_ITEM Item, Item1; 
    Item.mask = GVIF_TEXT;
    Item.row = -1;
    Item.col = -1;
	GetRowColByPos(point, Item.row, Item.col);
	//if(Item.col != 6) return ;
	Item.col = 5;
	m_EmailGrid.GetItem(&Item);

	//������ƺܷ������������ͬһ��GV_ITEM�����������strTextʱ���ܳ���"Buffer too small"�Ĵ�������CSting�й�
	Item1.mask = Item.mask;
	Item1.row = Item.row;
	Item1.col = Item.col;
	Item1.crBkClr = Item.crBkClr;
	Item1.crFgClr = Item.crFgClr;
	Item1.iImage = Item.iImage;
	Item1.lfFont = Item.lfFont;
	Item1.lParam = Item.lParam;
	Item1.nFormat = Item.nFormat;
	Item1.nMargin = Item.nMargin;
	Item1.nState = Item.nState;
	Item1.strText = Item.strText;

	HDROP m_hDropInfo = hDropInfo; 
	CStringA Filename; 
	CStringA AppendixDir;
	CString sizeStr;
	int pos = 0;
	m_AppendixDir.GetWindowTextA(AppendixDir);

	struct stat sta;

	if (m_hDropInfo) { 
		int iFiles = DragQueryFile(m_hDropInfo, (UINT)-1, NULL, 0); 
		char* pFilename;
		for (int i=0; i<iFiles; i++) { 
			pFilename = Filename.GetBuffer(_MAX_PATH); 
			DragQueryFile(m_hDropInfo, i, pFilename, _MAX_PATH); 
			// do whatever...
			stat(pFilename, &sta);
			size2str(sizeStr, sta.st_size);
			pos = strncmp(pFilename, AppendixDir.GetBuffer(), AppendixDir.GetLength());
			if(pos == 0){ 
				pos = AppendixDir.GetLength();
				if(pFilename[pos] == '\\') pos++;
			}else pos = 0;
			if(!Item1.strText.IsEmpty()) Item1.strText += '\n';
			Item1.strText += (pFilename + pos);
			Item1.strText += '(' + sizeStr + ')';
			Item1.strText += ';';
		} // for each files... 
	} // if DropInfo 

	//if(Item.strText.IsEmpty()) Item1.strText.Format("%s;", pFilename + pos);
	//else Item1.strText.Format("%s\n%s;", Item.strText.GetBuffer(), pFilename + pos);
	m_EmailGrid.SetItem(&Item1);

	DragFinish(m_hDropInfo); 

	m_EmailGrid.Invalidate();
	m_hDropInfo = 0; 

}

int CSendMailDlg::GetScrollRowAndCol(int &row, int &col)
{
	/*
	SCROLLINFO v_scrollinfo, h_scrollinfo;
	m_EmailGrid.GetScrollInfo(SB_VERT, &v_scrollinfo);
	m_EmailGrid.GetScrollInfo(SB_HORZ, &h_scrollinfo);

	int vh, vw;
	vh = m_EmailGrid.GetVirtualHeight();
	vw = m_EmailGrid.GetVirtualWidth();


	double vRate, hRate;
	//vRate = (double)v_scrollinfo.nPos/

	//int vBarLen = v_scrollinfo.nMax
	CCellRange cr = m_EmailGrid.GetVisibleNonFixedCellRange();
	//int colspan = cr.GetColSpan();
	//int rowspan = cr.GetRowSpan();
	//int maxcol = cr.GetMaxCol();
	//int maxrow = cr.GetMaxRow();
	int mincol = cr.GetMinCol();
	int minrow = cr.GetMinRow();	
	//int topleft = cr.GetTopLeft();
	*/
	return 0;
}

int CSendMailDlg::GetRowColByPos(const CPoint &point, int &row, int &col)
{
	int nRow = m_EmailGrid.GetRowCount();
	int nCol = m_EmailGrid.GetColumnCount();
	int xpos = 0, ypos = 0;
	int rowh = 0, colw = 0;

	CCellRange cr = m_EmailGrid.GetVisibleNonFixedCellRange();
	int srlCol = cr.GetMinCol();
	int srlRow = cr.GetMinRow();

	for(int i = 0; i < nRow; i++){
		rowh = m_EmailGrid.GetRowHeight(i);

		if(i < srlRow && i != 0) continue;
		
		ypos += rowh;		
		if(ypos < point.y || ypos - rowh > point.y || i == 0) continue;
		for(int j = 0; j < nCol; j++){
			colw = m_EmailGrid.GetColumnWidth(j);
			xpos += colw;
			if(j == 0) continue;
			if(xpos >= point.x && xpos - colw <= point.x){
				row = i;
				col = j;
				return 0;
			}
		}
	}
	return 0;	
}

void CSendMailDlg::OnBrowseAppendixDir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CString m_FileDir;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH*2];
	szFolder[0] = _T('\0');
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
		bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
		m_AppendixDir.SetWindowTextA(szFolder);
		m_WorkDir = szFolder;
	}
}


void CSendMailDlg::OnAddRow()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static int rows = 0;
	static GV_ITEM Item;
	CString str;

	rows = m_EmailGrid.GetRowCount();
	rows++;
	m_EmailGrid.SetRowCount(rows);
	m_EmailGrid.SetItemMultiRow(rows-1, 5);
	str.Format("%d", rows-1);
	m_EmailGrid.SetItemText(rows-1, 0, str);
	m_EmailGrid.SetCellType(rows-1, 1, RUNTIME_CLASS(CGridCellCheck));
	
}


void CSendMailDlg::OnDelRow()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static int rows = 0, cols = 0;
	rows = m_EmailGrid.GetRowCount();
	cols = m_EmailGrid.GetColumnCount();
	if(rows == 1) return;
	rows--;
	
	int flag = 0;
	CString str;
	for(int i = 1; i < cols; i++){
		str = m_EmailGrid.GetItemText(rows, i);
		if(!str.IsEmpty()){
			flag = 1;
			break;
		}
	}

	if(flag){
		flag = MessageBox("ɾ���к����ݽ��ᶪʧ��ȷ����", "��ʾ", MB_YESNO|MB_ICONQUESTION|MB_SYSTEMMODAL);
	}else{
		flag = IDYES;
	}

	if(flag == IDYES) m_EmailGrid.SetRowCount(rows);
}


void CSendMailDlg::OnSendMail()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if(m_SendingFlag == 0){
		m_SendingFlag = 1;
		GetDlgItem(IDC_BUTTON2)->SetWindowTextA("ȡ������");
	}else if(m_SendingFlag == 1){
		m_SendingFlag = 2;
		//GetDlgItem(IDC_BUTTON2)->SetWindowTextA("����");
		GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
		return ;
	}else if(m_SendingFlag == 2){
		m_SendingFlag = 0;
		GetDlgItem(IDC_BUTTON2)->SetWindowTextA("����");
		return ;
	}

	int ret = MessageBox("ȷ�Ϸ��������ʼ���", "��ʾ", MB_YESNO);
	if(ret != IDYES){
		m_SendingFlag = 0;
		GetDlgItem(IDC_BUTTON2)->SetWindowTextA("����");
		return;		
	}

	SendingProcess();	
	
}

int CSendMailDlg::DoSendMail(CSendInfo &pSendInfo)
{
	CString path, sendpaths;
	CString msg, msg1, msgfailed, strtmp;

	m_Mail.DeleteAllPath();	
	int index = 1;
	int ret = 0;
	do{
		pSendInfo.GetAppendixPath(path, index++);
		del_char(path, '\n');
		del_chars(path, '(', ')');
		if(!path.IsEmpty() && _access(path, 0)){
			msg.Format("%s�����ڣ�\n�Ƿ������%s�����ʼ���", path, pSendInfo.m_RecvAddr.GetBuffer());
			ret = MessageBox(msg, "", MB_YESNO|MB_ICONWARNING);
			if(ret != IDYES) return -1;
		}else{
			if(!path.IsEmpty()){
				m_Mail.AddFilePath(path.GetBuffer());
				if(!sendpaths.IsEmpty()) sendpaths += ";";
				sendpaths += path;
			}
		}		
	}while(!path.IsEmpty());

	sMailInfo smailInfo;
	memset(&smailInfo, 0, sizeof(sMailInfo));

	smailInfo.m_pcUserName = m_EmailUserInfo.emailaddr.GetBuffer();
	smailInfo.m_pcUserPassWord = m_EmailUserInfo.passwd.GetBuffer();
	smailInfo.m_pcSenderName = m_EmailUserInfo.emailaddr.GetBuffer();
	smailInfo.m_pcSender = m_EmailUserInfo.emailaddr.GetBuffer();
	smailInfo.m_pcReceiver = pSendInfo.m_RecvAddr.GetBuffer();
	smailInfo.m_pcTitle = pSendInfo.m_Title.GetBuffer();
	smailInfo.m_pcBody = pSendInfo.m_Content.GetBuffer();
	smailInfo.m_pcIPAddr = NULL;
	smailInfo.m_pcIPName = m_EmailUserInfo.pop3server.GetBuffer();
	if(m_EmailUserInfo.pop3port.IsEmpty()){
		m_EmailUserInfo.pop3port = "25";
		smailInfo.m_port = 25;
	}else{
		smailInfo.m_port = (unsigned short)StrToIntA(m_EmailUserInfo.pop3port);
	}

	//��־���������ļ���־�ʹ�����ʾ��־	
	static char logbuf[2048];
	vector<struct failedInfo> failedInfoList;
	
	int listSize;
	ret = 0;
	m_Log.GetWindowTextA(msg);
	sprintf_s(logbuf, 2048, "[%s] ����:%s; ����:%s...", current_time(), smailInfo.m_pcTitle, sendpaths);
	m_Flog << logbuf;

	ret = m_Mail.SendMail(smailInfo, failedInfoList);
	/*if(ret == 0){*/
		
		listSize = failedInfoList.size();
		for(int i = 0; i < listSize; i++){
			strtmp.Format("%s����ʧ��[0x%x].", failedInfoList[i].failedemail.c_str(), failedInfoList[i].failedno);
			if(i == 0){
				msgfailed += "\r\n";
			}
			msgfailed += strtmp.GetBuffer();
			if(listSize != i + 1) msgfailed += "\r\n";
		}

		m_Log.GetWindowTextA(msg);
		sprintf_s(logbuf, 2048, "%s �������. From %s to %s.%s",logbuf, 
			m_EmailUserInfo.emailaddr.GetBuffer(),pSendInfo.m_RecvAddr.GetBuffer(),msgfailed.GetBuffer());
		
		
		
		msg1.Format("%s\r\n%s", msg.GetBuffer(), logbuf);

	/*}else{
		m_Log.GetWindowTextA(msg);
		sprintf_s(logbuf, 2048, "%s ����ʧ��[0x%x]. From %s to %s.",logbuf, ret,
			m_EmailUserInfo.emailaddr.GetBuffer(),pSendInfo.m_RecvAddr.GetBuffer());
		msg1.Format("%s\r\n%s", msg.GetBuffer(), logbuf);
		msg = "";
		msg.Format("\"%s\"����ʧ��[0x%x].",smailInfo.m_pcTitle, ret);
	}*/
	m_Flog << logbuf << endl;
	m_Flog.flush();

	
	m_Log.SetWindowTextA(msg1);
	int lines = m_Log.GetLineCount();
	m_Log.LineScroll(lines);
	
	if(ret != 0){
		MessageBox(msg, "������ʾ", MB_OK|MB_ICONERROR);
	}
	
	return ret;
}

void CSendMailDlg::OnUserMng()
{
	// TODO: �ڴ���������������
	static CUserInfoDlg dlg;
	
	dlg.SetEmailUserInfo(m_EmailUserInfo);
	if(dlg.DoModal() == IDOK){
		CString str;
		dlg.GetEmailUserInfo(m_EmailUserInfo);	
		str.Format("��ǰ�û���%s", m_EmailUserInfo.sendname);
		//if(m_EmailUserInfo.serverstatus == 1 || m_EmailUserInfo.serverstatus == 0) 
			m_UserStatus.SetWindowTextA(str);
	}	
}


void CSendMailDlg::OnMenuAboutbox()
{
	// TODO: �ڴ���������������
	CAboutDlg dlg;
	dlg.DoModal();
}

int CSendMailDlg::ReadEmailConfig(string line, CSendInfo &item)
{
	int pos = line.find('=');
	if(pos == -1) return -1;
	int size = line.size();
	string head, tail;
	head = line.substr(0, pos);
	tail = line.substr(pos+1, size - pos - 1);

	add_char(item.AppendixPaths, ';', '\n');

	if(head == "receiver" && size) item.m_RecvAddr = tail.c_str();
	else if(head == "name" && size) item.m_Name = tail.c_str();
	else if(head == "title" && size) item.m_Title = tail.c_str();
	else if(head == "text" && size) item.m_Content = tail.c_str();
	/*else if(head == "appdsize" && size) item.Appdsize = tail.c_str();*/
	else if(head == "appdenix" && size) item.AppendixPaths = tail.c_str();
	else if(head == "check" && size){
		if(tail == "1") item.m_Check = 1;
		else item.m_Check = 0;
	}

	return 0;
}

int CSendMailDlg::ReadAccountConfig(string line)
{
	int pos = line.find('=');
	if(pos == -1) return -1;
	int size = line.size();
	string head, tail;
	head = line.substr(0, pos);
	tail = line.substr(pos+1, size - pos - 1);
	if(head == "sendername" && size) m_EmailUserInfo.sendname = tail.c_str();
	else if(head == "sender" && size) m_EmailUserInfo.emailaddr = tail.c_str();
	else if(head == "pop3server" && size) m_EmailUserInfo.pop3server = tail.c_str();
	else if(head == "pop3port" && size) m_EmailUserInfo.pop3port = tail.c_str();
	else if(head == "passwd" && size){
		//���뿼�Ǽ���
		CBase64 base;
		unsigned long basesize;
		unsigned char basebuf[256];
		if(base.Decode(tail,basebuf, &basesize)){
			if(basesize < 256){
				basebuf[basesize] = 0;
				m_EmailUserInfo.passwd = basebuf;
			}
		}
	}
	return 0;
}

int CSendMailDlg::ReadConfig()
{
	char file[] = "config.ini";

	int nSize = 0;
	string line;	
	ifstream fin(file);
	if(!fin.is_open()){
		MessageBox("�����ļ���ʧ��", "����", MB_OK | MB_ICONWARNING);
		return -1;
	}

	char type[64];//, name[64];
	int nCount = 0, ret = 0;
	int ItemFlag = 0, lastFlag = 0; //0�� �ޣ� 1�� setting�� 2��email�� 3��account
	int pos;
	CSendInfo item;
	item.m_Check = 0;

	while(getline(fin, line)){
		nSize = line.size();
		if(nSize < 1) continue;
		if(line[0] == '#'){
			continue;
		}else if(line[0] == '['){			
			pos = line.find(']');
			ret = sscanf(line.c_str(), "[%[^]]]", type);

			if(ItemFlag == 2){
				if(!item.IsAllEmpty())
					m_MailItem.push_back(item);
			}

			//lastFlag = ItemFlag;
			if(ret == 0) continue;
			if(strncmp(type, "setting", strlen(type)) == 0){				
				ItemFlag = 1;
			}else if(strncmp(type, "account", strlen(type)) == 0){
				ItemFlag = 3;
			}else if(strncmp(type, "email", strlen(type)) == 0){
				ItemFlag = 2;
			}else {
				ItemFlag = 0;
			}
		}else{
			if(ItemFlag == 0){
				//���账��
			}else if(ItemFlag == 1){
				pos = line.find('=');
				if(pos == -1) continue;
				ret = strncmp(line.c_str(), "workdir", 7);
				if(ret == 0){
					string sub;
					sub = line.substr(pos+1, line.size()-pos-1);
					m_WorkDir.Format("%s", sub.c_str());
				}
			}else if(ItemFlag == 2){
				ret = ReadEmailConfig(line, item);				
			}else if(ItemFlag == 3){
				ReadAccountConfig(line);
			}else{
				//���账��
			}			
		}
	}

	if(ItemFlag == 2){
		if(!item.IsAllEmpty())
			m_MailItem.push_back(item);
	}

	fin.close();
	return 0;
}

int CSendMailDlg::SaveConfig()
{
	char file[] = "config.ini";

	int nSize = 0;
	string line;	
	ofstream fout(file);
	if(!fout.is_open()){
		MessageBox("�����ļ���ʧ��", "����", MB_OK | MB_ICONWARNING);
		return -1;
	}

	//setting
	m_AppendixDir.GetWindowTextA(m_WorkDir);
	fout << "[setting]" << endl;
	fout << "workdir=" << m_WorkDir.GetBuffer() << endl;
	fout << endl;

	//account
	CBase64 base;
	string passwd;
	base.Encode((unsigned char *)m_EmailUserInfo.passwd.GetBuffer(), (unsigned long)m_EmailUserInfo.passwd.GetLength(), passwd);

	fout << "[account]" << endl;
	fout << "sendername=" << m_EmailUserInfo.sendname.GetBuffer() << endl;
	fout << "sender=" << m_EmailUserInfo.emailaddr.GetBuffer() << endl;
	fout << "passwd=" << passwd << endl;
	fout << "pop3server=" << m_EmailUserInfo.pop3server.GetBuffer() << endl;
	fout << "pop3port=" << m_EmailUserInfo.pop3port.GetBuffer() << endl;
	fout << endl;

	//email
	int rows = m_EmailGrid.GetRowCount();
	int col = 1;
	CSendInfo item;
	CGridCellCheck *cell;
	for(int i = 1; i < rows; i++){
		col = 1;
		item.m_RecvAddr = m_EmailGrid.GetItemText(i, col++);
		item.m_Name = m_EmailGrid.GetItemText(i, col++);
		item.m_Title = m_EmailGrid.GetItemText(i, col++);
		item.m_Content = m_EmailGrid.GetItemText(i, col++);
		item.AppendixPaths = m_EmailGrid.GetItemText(i, col++);

		if(!item.IsAllEmpty()){
			fout << "[email]" << endl;
			fout << "receiver=" << item.m_RecvAddr.GetBuffer() << endl;
			fout << "name=" << item.m_Name.GetBuffer() << endl;
			fout << "title=" << item.m_Title.GetBuffer() << endl;
			fout << "text=" << item.m_Content.GetBuffer() << endl;
			del_char(item.AppendixPaths, '\n');
			fout << "appdenix=" << item.AppendixPaths.GetBuffer() << endl;
			cell = (CGridCellCheck*)m_EmailGrid.GetCell(i,1);
			if(cell->GetCheck() == FALSE){
				fout << "check=" << "0" << endl;
			}else{
				fout << "check=" << "1" << endl;
			}
			fout << endl;
		}

	}

	fout.close();
	return 0;
}

void CSendMailDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	int ret = MessageBox("�Ƿ񱣴浱ǰ���ã�", "��ʾ", MB_YESNO|MB_ICONQUESTION);
	if(ret == IDYES)
		SaveConfig();
	CDialogEx::OnOK();
}


void CSendMailDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	int ret = MessageBox("�Ƿ񱣴浱ǰ���ã�", "��ʾ", MB_YESNO|MB_ICONQUESTION);
	if(ret == IDYES)
		SaveConfig();
	CDialogEx::OnCancel();
}


bool CSendMailDlg::OpenLog()
{
	m_Flog.open("sendlog.txt", ios::app);
	if(!m_Flog.is_open()){
		CString msg;
		msg.Format("��־��ʧ��[0x%x]", GetLastError());
		MessageBox(msg, "��ʾ", MB_OK | MB_ICONWARNING);
		return false;
	}
	return true;
}

bool CSendMailDlg::CloseLog()
{
	if(m_Flog.is_open()) m_Flog.close();
	return true;
}

inline
int 
CSendMailDlg::SendingProcess()
{
	m_ShowThread = _beginthreadex(NULL, 0, SendingThreadProc, this, 0, NULL);
	if(m_ShowThread == -1){
		MessageBox("������ʾ�̴߳���ʧ��", "����", MB_OK | MB_ICONWARNING);
	}
	return m_ShowThread;
}

unsigned int 
WINAPI 
CSendMailDlg::SendingThreadProc(void *para)
{
	CSendMailDlg *dlg = (CSendMailDlg*)para;
	dlg->Sending();
	return 0;
}

int CSendMailDlg::Sending(){
	m_ShowProcessText.ShowWindow(SW_SHOW);
	m_ShowProcess.ShowWindow(SW_SHOW);
	m_ShowProcessText.SetWindowTextA("");

	m_AppendixDir.EnableWindow(FALSE);
	m_EmailGrid.EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);

	static int rows = 0, cols = 0;
	rows = m_EmailGrid.GetRowCount();
	m_ShowProcess.SetRange(1, rows);
	m_ShowProcess.SetPos(0);

	CString workdir, msg;
	CHAR curdir[MAX_PATH];
	m_AppendixDir.GetWindowTextA(workdir);
	::GetCurrentDirectoryA(MAX_PATH, curdir);
	if(!workdir.IsEmpty()) ::SetCurrentDirectoryA(workdir);

	CSendInfo tSendInfo;
	CGridCellCheck *cell;
	for(int i = 1; i < rows; i++){
		if(m_SendingFlag == 2){//ȡ������
			break;
		}
		cell = (CGridCellCheck*)m_EmailGrid.GetCell(i,1);
		if(cell->GetCheck() == FALSE) continue;
		tSendInfo.m_RecvAddr = m_EmailGrid.GetItemText(i,1);
		tSendInfo.m_Name = m_EmailGrid.GetItemText(i,2);
		tSendInfo.m_Title = m_EmailGrid.GetItemText(i,3);
		tSendInfo.m_Content = m_EmailGrid.GetItemText(i,4);
		tSendInfo.AppendixPaths = m_EmailGrid.GetItemText(i,5);		

		if(!tSendInfo.m_RecvAddr.IsEmpty()){ 
			msg.Format("���ڸ�%s�����ʼ�...", tSendInfo.m_RecvAddr.GetBuffer());
			m_ShowProcessText.SetWindowTextA(msg);
			m_ShowProcess.SetPos(i);	

			DoSendMail(tSendInfo);		
		}
	}//end for

	if(m_SendingFlag == 2){
		m_ShowProcessText.SetWindowTextA("��ȡ��.");
		m_ShowProcess.SetPos(rows);
	}else{
		m_ShowProcessText.SetWindowTextA("���.");
		m_ShowProcess.SetPos(rows);
	}

	m_SendingFlag = 0;
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->SetWindowTextA("����");

	if(!workdir.IsEmpty()) ::SetCurrentDirectoryA(curdir);

	m_AppendixDir.EnableWindow(TRUE);
	m_EmailGrid.EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	//GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);

	Sleep(1000);
	m_ShowProcessText.ShowWindow(SW_HIDE);
	m_ShowProcess.ShowWindow(SW_HIDE);
	return 0;
}

void CSendMailDlg::OnSaveConfig()
{
	// TODO: �ڴ���������������
	if(SaveConfig() == 0){
		MessageBox("�û����ñ������", "��ʾ", MB_OK);
	}
}
