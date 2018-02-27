// FileTransfersDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileTransfers.h"
#include "FileTransfersDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTransfersDlg dialog
#define WM_ACCEPT_TRANSFERS WM_USER + 100

CWinThread* pThreadListen;		//监听线程 --> _ListenThread
CWinThread* pThreadSend;        //连接线程 --> _ConnectThread

CFileTransfersDlg::CFileTransfersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileTransfersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileTransfersDlg)
	m_strFileName = _T("");
	m_strFileSize = _T("0 字节");
	m_wPort = 9600;
	m_nServerType = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_psockServer = NULL;
	m_psockClient = NULL;

	m_strPath = _T("");
	m_dwFileSize = 0;

	m_bIsStop = FALSE;
	m_bIsWait = FALSE;
	m_bIsTransmitting = FALSE;
}

void CFileTransfersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileTransfersDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Text(pDX, IDC_FILE_NAME, m_strFileName);
	DDX_Text(pDX, IDC_FILE_SIZE, m_strFileSize);
	DDX_Text(pDX, IDC_PORT, m_wPort);
	DDX_Radio(pDX, IDC_RADIO_SERVER, m_nServerType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileTransfersDlg, CDialog)
	//{{AFX_MSG_MAP(CFileTransfersDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_SERVER, OnRadioServer)
	ON_BN_CLICKED(IDC_RADIO_CLIENT, OnRadioClient)
	ON_BN_CLICKED(IDC_BEGIN, OnBegin)
	ON_BN_CLICKED(IDC_DISCONNECT, OnDisconnect)
	ON_BN_CLICKED(IDC_SELECT_FILE, OnSelectFile)
	ON_BN_CLICKED(IDC_STOP_TRANSFERS, OnStopTransfers)
	ON_MESSAGE(WM_ACCEPT_TRANSFERS, OnAcceptTransfers)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTransfersDlg message handlers

BOOL CFileTransfersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CString strHostName,strIPAddress;
	if(GetLocalHostInfo(strHostName, strIPAddress))
		return FALSE;
	
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(strIPAddress);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileTransfersDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFileTransfersDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFileTransfersDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/* ---------------------------------- < 线程 > ---------------------------------- */

//接收文件线程
UINT _ListenThread(LPVOID lparam)
{
	CFileTransfersDlg* pDlg = (CFileTransfersDlg*)lparam;
	
	//创建套接字
	CSocket sockSrvr;
	if(!sockSrvr.Create(pDlg->m_wPort + PORT))
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("错误"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	//开始监听
	if(!sockSrvr.Listen())
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("错误"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	pDlg->SendMessage(WM_ACCEPT_TRANSFERS);
	
	//接受连接
	CSocket recSo;
	if(!sockSrvr.Accept(recSo))
	{
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("错误"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	sockSrvr.Close();

	pDlg->ReceiveFile(recSo);

	return 0;
}

//发送文件线程
UINT _SendThread(LPVOID lparam)
{
	CFileTransfersDlg* pDlg = (CFileTransfersDlg*) lparam;
	
	CSocket sockClient;
	if(!sockClient.Create())
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("错误"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	CString strIPAddress;
	UINT nPort;
	pDlg->m_psockClient->GetPeerName(strIPAddress, nPort);
	if(!sockClient.Connect(strIPAddress, pDlg->m_wPort + PORT))
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("错误"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	pDlg->SendFile(sockClient);
	
	return 0;
}

/* ----------------------------------< 线程 >---------------------------------- */

void CFileTransfersDlg::OnOK() 
{	
	//	CDialog::OnOK();
}

void CFileTransfersDlg::OnCancel() 
{
	if(m_bIsWait)
	{
		MessageBox(_T("等待中，请先停止传送后再退出"), _T("警告"), MB_ICONEXCLAMATION);
		return ;
	}
	
	if(m_bIsTransmitting)
	{
		MessageBox(_T("文件传输中，请先停止传送后再退出"), _T("警告"), MB_ICONEXCLAMATION);
		return ;
	}
	
	OnDisconnect();
	CDialog::OnCancel();
}

void CFileTransfersDlg::OnRadioServer() 
{
	CString strHostName,strIPAddress;
	if(GetLocalHostInfo(strHostName, strIPAddress))
		return ;
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(strIPAddress);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BEGIN)->SetWindowText(_T("启  动(&B)"));
	GetDlgItem(IDC_DISCONNECT)->SetWindowText(_T("关  闭(&D)"));
	GetDlgItem(IDC_TRANSFERS_TIP)->SetWindowText(_T("已发送:"));
}

void CFileTransfersDlg::OnRadioClient() 
{
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(_T(""));
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEGIN)->SetWindowText(_T("连  接(&B)"));
	GetDlgItem(IDC_DISCONNECT)->SetWindowText(_T("断  开(&D)"));
}

void CFileTransfersDlg::OnBegin() 
{	
	UpdateData(TRUE);
	
	//当程序作为服务器
	if(m_nServerType == SERVER)
	{
		//创建服务器套接字
		m_psockServer = new CListenSocket(this);
		if(!m_psockServer->Create(m_wPort))
		{
			delete m_psockServer;
			m_psockServer = NULL;
			
			MessageBox(GetError(GetLastError()), _T("错误"), MB_ICONHAND);
			
			return ;
		}
		
		//监听客户端连接
		if(!m_psockServer->Listen())
		{
			delete m_psockServer;
			m_psockServer = NULL;
			
			MessageBox(GetError(GetLastError()), _T("错误"), MB_ICONHAND);
			
			return ;
		}
	}
	else
	{
		//当程序作为客户端
		if(((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS))->IsBlank())
		{
			MessageBox("IP 地址不能为空", "错误", MB_ICONHAND);
			return ;
		}
		
		//创建客户端套接字
		m_psockClient = new CClientSocket(this);
		if(!m_psockClient->Create())
		{
			delete m_psockClient;
			m_psockClient = NULL;
			
			MessageBox(GetError(GetLastError()), _T("错误"), MB_ICONHAND);
			
			return ;
		}
		
		//与服务器建立连接
		CString strIPAddress;
		GetDlgItem(IDC_IPADDRESS)->GetWindowText(strIPAddress);
		if(!m_psockClient->Connect(strIPAddress, m_wPort))
		{
			delete m_psockClient;
			m_psockClient = NULL;
			
			MessageBox(GetError(GetLastError()), _T("错误"), MB_ICONHAND);
			
			return ;
		}
		
		//初始化套接字
		m_psockClient->Init();
		
		GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_RADIO_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_CLIENT)->EnableWindow(FALSE);
	GetDlgItem(IDC_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BEGIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
}

void CFileTransfersDlg::OnDisconnect() 
{	
	UpdateData(TRUE);
	
	//关闭客户端套接字m_psockClient
	if(m_psockClient != NULL)
	{
		//告诉对方发送等待被取消
		CMessage* pMsg = new CMessage(DISCONNECT);
		m_psockClient->SendMsg(pMsg);
		
		delete m_psockClient;
		m_psockClient = NULL;
	}
	
	//关闭服务器端套接字m_psockServer
	if(m_psockServer != NULL)
	{
		delete m_psockServer;
		m_psockServer = NULL;
	}
	
	GetDlgItem(IDC_RADIO_SERVER)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_CLIENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
	if(m_nServerType == CLIENT)
		GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
}

void CFileTransfersDlg::OnSelectFile() 
{	
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "所有文件 (*.*)|*.*||", this);
	dlg.m_ofn.lpstrTitle = _T("打开");
	if(dlg.DoModal() == IDOK)
	{
		m_bIsWait = TRUE;
		m_bIsClient = TRUE;
		m_strPath = dlg.GetPathName();
		m_strFileName = dlg.GetFileName();
		
		//打开文件
		CFile file(m_strPath, CFile::modeRead);
		//获取文件大小
		m_dwFileSize = file.GetLength();
		m_strFileSize.Format("%ld 字节", m_dwFileSize);
		//关闭文件
		file.Close();
		UpdateData(FALSE);
		
		//发出文件发送请求
		CMessage* pMsg = new CMessage(REQUEST, m_strFileName, m_dwFileSize);
		m_psockClient->SendMsg(pMsg);
		
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRANSFERS_TIP)->SetWindowText(_T("已发送:"));
		
		//设置等待超时定时器
		m_nTimer = SetTimer(1, 50000, NULL);
	}
}

void CFileTransfersDlg::OnStopTransfers() 
{	
	if(m_bIsWait)
	{
		if(MessageBox(_T("真的要停止等待吗？"), _T("警告"), MB_ICONEXCLAMATION|MB_YESNO) == IDYES)
		{
			m_bIsWait = FALSE;
			if(!m_bIsClient)
			{
				//停止ID为2的计时器
				if(KillTimer(2))
				{
					//结束监听
					CSocket sockClient;
					sockClient.Create();
					sockClient.Connect(_T("127.0.0.1"), m_wPort + PORT);
					sockClient.Close();
				}
			}
			else
			{
				//停止ID为1的计时器
				if(KillTimer(1))
				{					
					//告诉对方发送等待被取消
					CMessage* pMsg = new CMessage(CANCEL);
					m_psockClient->SendMsg(pMsg);
					
					GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
					GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
					GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
				}
			}
		}
		return ;
	}
	
	if(MessageBox(_T("真的要停止文件传输吗？"), _T("警告"), MB_ICONEXCLAMATION|MB_YESNO) == IDYES)
	{
		m_bIsStop = TRUE;
		return ;
	}
}

int CFileTransfersDlg::GetLocalHostInfo(CString &strHostName, CString &strIPAddress)
{
	char szHostName[256];
	
	if(gethostname(szHostName, sizeof(szHostName)))
	{
		strHostName = _T("");
		MessageBox(GetError(GetLastError()), _T("错误"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	PHOSTENT hostinfo;
	if((hostinfo = gethostbyname(szHostName)) == NULL)
		return GetLastError();
	
	LPCSTR ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
	
	strIPAddress = ip;
	strHostName = szHostName;
	
	return 0;
}

CString CFileTransfersDlg::GetError(DWORD error)
{
	CString strError;
	switch(error)
	{
	case WSANOTINITIALISED:
		strError="初始化错误";
		break;
	case WSAENOTCONN:
		strError="对方没有启动";
		break;
	case WSAEWOULDBLOCK :
		strError="对方已经关闭";
		break;
	case WSAECONNREFUSED:
		strError="连接的尝试被拒绝";
		break;
	case WSAENOTSOCK:
		strError="在一个非套接字上尝试了一个操作";
		break;
	case WSAEADDRINUSE:
		strError="特定的地址已在使用中";
		break;
	case WSAECONNRESET:
		strError="与主机的连接被关闭";
		break;
	default:
		strError="一般性错误";	
	}
	return strError;
}

void CFileTransfersDlg::ProcessAccept()
{
	CClientSocket* pSocket = new CClientSocket(this);
	//将请求接收下来，得到一个新的套接字pSocket
	if(m_psockServer->Accept(*pSocket))
	{
		//初始化套接字pSocket
		pSocket->Init();
		
		CMessage* pMsg;
		//如果m_psockClient套接字为空，则表示还没有和任何客户端建立连接
		if(m_psockClient == NULL)
		{
			//向客户端发送一个消息，表示连接被接受
			pMsg = new CMessage(CONNECT_BE_ACCEPT);
			pSocket->SendMsg(pMsg);
			m_psockClient = pSocket;
			
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		}
		else
		{
			//否则向客户端发一个信息，服务器已经存在连接
			pMsg = new CMessage(CONNECT_BE_REFUSE);
			pSocket->SendMsg(pMsg);
		}
	}
}

void CFileTransfersDlg::ProcessReceive(CClientSocket* pSocket)
{
	//获取信息
	CMessage* pMsg = new CMessage();
	pSocket->ReceiveMsg(pMsg);
	
	//当消息类型为连接被接受时执行该if语句里面的内容
	if(pMsg->m_nType == CONNECT_BE_ACCEPT)
	{
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		return;
	}

	//当消息类型为连接被拒绝时执行该if语句里面的内容
	if(pMsg->m_nType == CONNECT_BE_REFUSE)
	{
		MessageBox(_T("服务器已经和另外的客户端建立连接，请等一下再连接。"), _T("错误"), MB_ICONHAND);
		
		delete m_psockClient;
		m_psockClient = NULL;
		
		GetDlgItem(IDC_RADIO_SERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_CLIENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
		GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
		return ;
	}
	
	//当消息类型为连接被断开时执行该if语句里面的内容
	if(pMsg->m_nType == DISCONNECT)
	{
		MessageBox(_T("对方已经关闭"), _T("警告"), MB_ICONHAND);
		
		if(m_psockClient != NULL)
		{
			delete m_psockClient;
			m_psockClient = NULL;
		}
		
		if(m_nServerType == CLIENT)
		{	
			GetDlgItem(IDC_RADIO_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_CLIENT)->EnableWindow(TRUE);
			GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
		}
		return ;
	}

	//当收到传输文件请求时执行该if语句里面的内容
	if(pMsg->m_nType == REQUEST)
	{
		m_bIsWait = TRUE;
		
		m_strFileName = pMsg->m_strFileName;
		m_dwFileSize = pMsg->m_dwFileSize;
		
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "所有文件 (*.*)|*.*||", this);
		dlg.m_ofn.lpstrTitle = _T("另存为");
		strcpy(dlg.m_ofn.lpstrFile, m_strFileName.GetBuffer(m_strFileName.GetLength()));
		if(dlg.DoModal() == IDOK)
		{
			if(m_bIsWait == FALSE)
			{
				MessageBox(_T("对方已经取消文件发送"), _T("警告"), MB_ICONEXCLAMATION);
				return ;
			}

			m_bIsClient = FALSE;
			m_strPath = dlg.GetPathName();
			
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(TRUE);
			
			m_strFileSize.Format("%ld 字节", m_dwFileSize);
			GetDlgItem(IDC_FILE_NAME)->SetWindowText(dlg.GetFileName());
			GetDlgItem(IDC_FILE_SIZE)->SetWindowText(m_strFileSize);
			GetDlgItem(IDC_TRANSFERS_TIP)->SetWindowText(_T("已收到:"));
			
			//启动接收文件的线程
			pThreadListen = ::AfxBeginThread(_ListenThread, this);
			return ;
		}
		
		if(m_bIsWait == TRUE)
		{		
			//告诉对方文件发送请求被拒绝
			CMessage* pMsg = new CMessage(REFUSE);
			m_psockClient->SendMsg(pMsg);
		}
		
		m_bIsWait = FALSE;
		return ;
	}

	//当对方同意且准备好接收文件时执行该if语句里面的内容
	if(pMsg->m_nType == ACCEPT)
	{	
		KillTimer(1);
		m_bIsWait = FALSE;
		//启动文件发送线程
		pThreadSend = ::AfxBeginThread(_SendThread, this);
		return ;
	}
	
	//当发送文件请求被拒绝时执行该if语句里面的内容
	if(pMsg->m_nType == REFUSE)
	{	
		m_bIsWait = FALSE;
		
		MessageBox(_T("请求被拒绝"), _T("警告"), MB_ICONEXCLAMATION);
		
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
		return ;
	}

	//当对方取消文件传输时执行该if语句里面的内容
	if(pMsg->m_nType == CANCEL)
	{
		m_bIsWait = FALSE;
		return ;
	}
	
	return ;
}

void CFileTransfersDlg::SendFile(CSocket &senSo)
{
	m_bIsTransmitting = TRUE;

	//打开要发送的文件
	CFile file;
	if(!file.Open(m_strPath, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("文件打开失败"));
		
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
		
		senSo.Close();
		
		return ;
	}
	
	m_ctrlProgress.SetRange32(0, m_dwFileSize);
	
	int nSize = 0, nLen = 0;
	DWORD dwCount = 0;
	char buf[BLOCKSIZE] = {0};
	file.Seek(0, CFile::begin);
	//开始传送文件
	for(;;)
	{
		//一次读取BLOCKSIZE大小的文件内容
		nLen = file.Read(buf, BLOCKSIZE);
		if(nLen == 0)
			break;
		
		//发送文件内容
		nSize = senSo.Send(buf, nLen);
		
		dwCount += nSize;
		m_ctrlProgress.SetPos(dwCount);
		CString strTransfersSize;
		strTransfersSize.Format("%ld 字节", dwCount);
		GetDlgItem(IDC_RECEIVE_SIZE)->SetWindowText(strTransfersSize);
		
		if(m_bIsStop)
		{
			m_bIsStop = FALSE;
			break;
		}
		
		if(nSize == SOCKET_ERROR)
			break;
	}
	//关闭文件
	file.Close();
	//关闭套接字
	senSo.Close();
	
	if(m_dwFileSize == dwCount)
		AfxMessageBox(_T("文件发送成功"));
	else
		AfxMessageBox(_T("文件发送失败"));
	
	m_ctrlProgress.SetPos(0);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);

	m_bIsTransmitting = FALSE;	
}

void CFileTransfersDlg::ReceiveFile(CSocket &recSo)
{
	//停止等待超时计时器
	KillTimer(2);
	m_bIsWait = FALSE;
	m_bIsTransmitting = TRUE;
	
	m_ctrlProgress.SetRange32(0, m_dwFileSize);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(TRUE);
	
	int nSize = 0;
	DWORD dwCount = 0;
	char buf[BLOCKSIZE] = {0};	
	//创建一个文件
	CFile file(m_strPath, CFile::modeCreate|CFile::modeWrite);
	//开始接收文件
	for(;;)
	{
		//每次接收BLOCKSIZE大小的文件内容
		nSize = recSo.Receive(buf, BLOCKSIZE);
		if(nSize == 0)
			break;
		
		//将接收到的文件写到新建的文件中去
		file.Write(buf, nSize);
		dwCount += nSize;
		m_ctrlProgress.SetPos(dwCount);
		CString strTransfersSize;
		strTransfersSize.Format("%ld 字节", dwCount);
		GetDlgItem(IDC_RECEIVE_SIZE)->SetWindowText(strTransfersSize);
		
		//用户是否要停止接收
		if(m_bIsStop)
		{
			m_bIsStop = FALSE;
			break;
		}
	}
	//关闭文件
	file.Close();
	//关闭套接字
	recSo.Close();
	
	if(m_dwFileSize == dwCount)
		AfxMessageBox(_T("文件接收成功"));
	else
		AfxMessageBox(_T("文件接收失败"));
	
	m_ctrlProgress.SetPos(0);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
	
	m_bIsTransmitting = FALSE;
}

void CFileTransfersDlg::TransfersFailed()
{
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
}

LRESULT CFileTransfersDlg::OnAcceptTransfers(WPARAM wParam, LPARAM lParam)
{	
	//告诉对方文件请求被接受且准备好接收	
	CMessage* pMsg = new CMessage(ACCEPT);
	m_psockClient->SendMsg(pMsg);
	//设置一个ID为2的超时几时器
	m_nTimer = SetTimer(2, 5000, NULL);
	return 0;
}

void CFileTransfersDlg::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	//ID为1的计时器
	case 1:
		{
			//结束ID为1的计时器
			KillTimer(1);
			m_bIsWait = FALSE;
			
			//告诉对方发送等待被取消
			CMessage* pMsg = new CMessage(CANCEL);
			m_psockClient->SendMsg(pMsg);
			
			MessageBox(_T("等待超时"), _T(" 警告"), MB_ICONEXCLAMATION);
			
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
			break;
		}
	//ID为2的计时器
	case 2:
		{
			//结束ID为2的计时器
			KillTimer(2);
			//结束监听
			CSocket sockClient;
			sockClient.Create();
			sockClient.Connect(_T("127.0.0.1"), m_wPort + PORT);
			sockClient.Close();
			break;
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}
