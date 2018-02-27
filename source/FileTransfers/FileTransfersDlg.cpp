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

CWinThread* pThreadListen;		//�����߳� --> _ListenThread
CWinThread* pThreadSend;        //�����߳� --> _ConnectThread

CFileTransfersDlg::CFileTransfersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileTransfersDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileTransfersDlg)
	m_strFileName = _T("");
	m_strFileSize = _T("0 �ֽ�");
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

/* ---------------------------------- < �߳� > ---------------------------------- */

//�����ļ��߳�
UINT _ListenThread(LPVOID lparam)
{
	CFileTransfersDlg* pDlg = (CFileTransfersDlg*)lparam;
	
	//�����׽���
	CSocket sockSrvr;
	if(!sockSrvr.Create(pDlg->m_wPort + PORT))
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("����"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	//��ʼ����
	if(!sockSrvr.Listen())
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("����"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	pDlg->SendMessage(WM_ACCEPT_TRANSFERS);
	
	//��������
	CSocket recSo;
	if(!sockSrvr.Accept(recSo))
	{
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("����"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	sockSrvr.Close();

	pDlg->ReceiveFile(recSo);

	return 0;
}

//�����ļ��߳�
UINT _SendThread(LPVOID lparam)
{
	CFileTransfersDlg* pDlg = (CFileTransfersDlg*) lparam;
	
	CSocket sockClient;
	if(!sockClient.Create())
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("����"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	CString strIPAddress;
	UINT nPort;
	pDlg->m_psockClient->GetPeerName(strIPAddress, nPort);
	if(!sockClient.Connect(strIPAddress, pDlg->m_wPort + PORT))
	{
		pDlg->TransfersFailed();
		::MessageBox((HWND)lparam, pDlg->GetError(GetLastError()), _T("����"), MB_ICONHAND|MB_OK);
		return -1;
	}
	
	pDlg->SendFile(sockClient);
	
	return 0;
}

/* ----------------------------------< �߳� >---------------------------------- */

void CFileTransfersDlg::OnOK() 
{	
	//	CDialog::OnOK();
}

void CFileTransfersDlg::OnCancel() 
{
	if(m_bIsWait)
	{
		MessageBox(_T("�ȴ��У�����ֹͣ���ͺ����˳�"), _T("����"), MB_ICONEXCLAMATION);
		return ;
	}
	
	if(m_bIsTransmitting)
	{
		MessageBox(_T("�ļ������У�����ֹͣ���ͺ����˳�"), _T("����"), MB_ICONEXCLAMATION);
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
	GetDlgItem(IDC_BEGIN)->SetWindowText(_T("��  ��(&B)"));
	GetDlgItem(IDC_DISCONNECT)->SetWindowText(_T("��  ��(&D)"));
	GetDlgItem(IDC_TRANSFERS_TIP)->SetWindowText(_T("�ѷ���:"));
}

void CFileTransfersDlg::OnRadioClient() 
{
	GetDlgItem(IDC_IPADDRESS)->SetWindowText(_T(""));
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEGIN)->SetWindowText(_T("��  ��(&B)"));
	GetDlgItem(IDC_DISCONNECT)->SetWindowText(_T("��  ��(&D)"));
}

void CFileTransfersDlg::OnBegin() 
{	
	UpdateData(TRUE);
	
	//��������Ϊ������
	if(m_nServerType == SERVER)
	{
		//�����������׽���
		m_psockServer = new CListenSocket(this);
		if(!m_psockServer->Create(m_wPort))
		{
			delete m_psockServer;
			m_psockServer = NULL;
			
			MessageBox(GetError(GetLastError()), _T("����"), MB_ICONHAND);
			
			return ;
		}
		
		//�����ͻ�������
		if(!m_psockServer->Listen())
		{
			delete m_psockServer;
			m_psockServer = NULL;
			
			MessageBox(GetError(GetLastError()), _T("����"), MB_ICONHAND);
			
			return ;
		}
	}
	else
	{
		//��������Ϊ�ͻ���
		if(((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS))->IsBlank())
		{
			MessageBox("IP ��ַ����Ϊ��", "����", MB_ICONHAND);
			return ;
		}
		
		//�����ͻ����׽���
		m_psockClient = new CClientSocket(this);
		if(!m_psockClient->Create())
		{
			delete m_psockClient;
			m_psockClient = NULL;
			
			MessageBox(GetError(GetLastError()), _T("����"), MB_ICONHAND);
			
			return ;
		}
		
		//���������������
		CString strIPAddress;
		GetDlgItem(IDC_IPADDRESS)->GetWindowText(strIPAddress);
		if(!m_psockClient->Connect(strIPAddress, m_wPort))
		{
			delete m_psockClient;
			m_psockClient = NULL;
			
			MessageBox(GetError(GetLastError()), _T("����"), MB_ICONHAND);
			
			return ;
		}
		
		//��ʼ���׽���
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
	
	//�رտͻ����׽���m_psockClient
	if(m_psockClient != NULL)
	{
		//���߶Է����͵ȴ���ȡ��
		CMessage* pMsg = new CMessage(DISCONNECT);
		m_psockClient->SendMsg(pMsg);
		
		delete m_psockClient;
		m_psockClient = NULL;
	}
	
	//�رշ��������׽���m_psockServer
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
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "�����ļ� (*.*)|*.*||", this);
	dlg.m_ofn.lpstrTitle = _T("��");
	if(dlg.DoModal() == IDOK)
	{
		m_bIsWait = TRUE;
		m_bIsClient = TRUE;
		m_strPath = dlg.GetPathName();
		m_strFileName = dlg.GetFileName();
		
		//���ļ�
		CFile file(m_strPath, CFile::modeRead);
		//��ȡ�ļ���С
		m_dwFileSize = file.GetLength();
		m_strFileSize.Format("%ld �ֽ�", m_dwFileSize);
		//�ر��ļ�
		file.Close();
		UpdateData(FALSE);
		
		//�����ļ���������
		CMessage* pMsg = new CMessage(REQUEST, m_strFileName, m_dwFileSize);
		m_psockClient->SendMsg(pMsg);
		
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(TRUE);
		GetDlgItem(IDC_TRANSFERS_TIP)->SetWindowText(_T("�ѷ���:"));
		
		//���õȴ���ʱ��ʱ��
		m_nTimer = SetTimer(1, 50000, NULL);
	}
}

void CFileTransfersDlg::OnStopTransfers() 
{	
	if(m_bIsWait)
	{
		if(MessageBox(_T("���Ҫֹͣ�ȴ���"), _T("����"), MB_ICONEXCLAMATION|MB_YESNO) == IDYES)
		{
			m_bIsWait = FALSE;
			if(!m_bIsClient)
			{
				//ֹͣIDΪ2�ļ�ʱ��
				if(KillTimer(2))
				{
					//��������
					CSocket sockClient;
					sockClient.Create();
					sockClient.Connect(_T("127.0.0.1"), m_wPort + PORT);
					sockClient.Close();
				}
			}
			else
			{
				//ֹͣIDΪ1�ļ�ʱ��
				if(KillTimer(1))
				{					
					//���߶Է����͵ȴ���ȡ��
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
	
	if(MessageBox(_T("���Ҫֹͣ�ļ�������"), _T("����"), MB_ICONEXCLAMATION|MB_YESNO) == IDYES)
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
		MessageBox(GetError(GetLastError()), _T("����"), MB_ICONHAND|MB_OK);
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
		strError="��ʼ������";
		break;
	case WSAENOTCONN:
		strError="�Է�û������";
		break;
	case WSAEWOULDBLOCK :
		strError="�Է��Ѿ��ر�";
		break;
	case WSAECONNREFUSED:
		strError="���ӵĳ��Ա��ܾ�";
		break;
	case WSAENOTSOCK:
		strError="��һ�����׽����ϳ�����һ������";
		break;
	case WSAEADDRINUSE:
		strError="�ض��ĵ�ַ����ʹ����";
		break;
	case WSAECONNRESET:
		strError="�����������ӱ��ر�";
		break;
	default:
		strError="һ���Դ���";	
	}
	return strError;
}

void CFileTransfersDlg::ProcessAccept()
{
	CClientSocket* pSocket = new CClientSocket(this);
	//����������������õ�һ���µ��׽���pSocket
	if(m_psockServer->Accept(*pSocket))
	{
		//��ʼ���׽���pSocket
		pSocket->Init();
		
		CMessage* pMsg;
		//���m_psockClient�׽���Ϊ�գ����ʾ��û�к��κοͻ��˽�������
		if(m_psockClient == NULL)
		{
			//��ͻ��˷���һ����Ϣ����ʾ���ӱ�����
			pMsg = new CMessage(CONNECT_BE_ACCEPT);
			pSocket->SendMsg(pMsg);
			m_psockClient = pSocket;
			
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		}
		else
		{
			//������ͻ��˷�һ����Ϣ���������Ѿ���������
			pMsg = new CMessage(CONNECT_BE_REFUSE);
			pSocket->SendMsg(pMsg);
		}
	}
}

void CFileTransfersDlg::ProcessReceive(CClientSocket* pSocket)
{
	//��ȡ��Ϣ
	CMessage* pMsg = new CMessage();
	pSocket->ReceiveMsg(pMsg);
	
	//����Ϣ����Ϊ���ӱ�����ʱִ�и�if������������
	if(pMsg->m_nType == CONNECT_BE_ACCEPT)
	{
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		return;
	}

	//����Ϣ����Ϊ���ӱ��ܾ�ʱִ�и�if������������
	if(pMsg->m_nType == CONNECT_BE_REFUSE)
	{
		MessageBox(_T("�������Ѿ�������Ŀͻ��˽������ӣ����һ�������ӡ�"), _T("����"), MB_ICONHAND);
		
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
	
	//����Ϣ����Ϊ���ӱ��Ͽ�ʱִ�и�if������������
	if(pMsg->m_nType == DISCONNECT)
	{
		MessageBox(_T("�Է��Ѿ��ر�"), _T("����"), MB_ICONHAND);
		
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

	//���յ������ļ�����ʱִ�и�if������������
	if(pMsg->m_nType == REQUEST)
	{
		m_bIsWait = TRUE;
		
		m_strFileName = pMsg->m_strFileName;
		m_dwFileSize = pMsg->m_dwFileSize;
		
		CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "�����ļ� (*.*)|*.*||", this);
		dlg.m_ofn.lpstrTitle = _T("���Ϊ");
		strcpy(dlg.m_ofn.lpstrFile, m_strFileName.GetBuffer(m_strFileName.GetLength()));
		if(dlg.DoModal() == IDOK)
		{
			if(m_bIsWait == FALSE)
			{
				MessageBox(_T("�Է��Ѿ�ȡ���ļ�����"), _T("����"), MB_ICONEXCLAMATION);
				return ;
			}

			m_bIsClient = FALSE;
			m_strPath = dlg.GetPathName();
			
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(TRUE);
			
			m_strFileSize.Format("%ld �ֽ�", m_dwFileSize);
			GetDlgItem(IDC_FILE_NAME)->SetWindowText(dlg.GetFileName());
			GetDlgItem(IDC_FILE_SIZE)->SetWindowText(m_strFileSize);
			GetDlgItem(IDC_TRANSFERS_TIP)->SetWindowText(_T("���յ�:"));
			
			//���������ļ����߳�
			pThreadListen = ::AfxBeginThread(_ListenThread, this);
			return ;
		}
		
		if(m_bIsWait == TRUE)
		{		
			//���߶Է��ļ��������󱻾ܾ�
			CMessage* pMsg = new CMessage(REFUSE);
			m_psockClient->SendMsg(pMsg);
		}
		
		m_bIsWait = FALSE;
		return ;
	}

	//���Է�ͬ����׼���ý����ļ�ʱִ�и�if������������
	if(pMsg->m_nType == ACCEPT)
	{	
		KillTimer(1);
		m_bIsWait = FALSE;
		//�����ļ������߳�
		pThreadSend = ::AfxBeginThread(_SendThread, this);
		return ;
	}
	
	//�������ļ����󱻾ܾ�ʱִ�и�if������������
	if(pMsg->m_nType == REFUSE)
	{	
		m_bIsWait = FALSE;
		
		MessageBox(_T("���󱻾ܾ�"), _T("����"), MB_ICONEXCLAMATION);
		
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
		return ;
	}

	//���Է�ȡ���ļ�����ʱִ�и�if������������
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

	//��Ҫ���͵��ļ�
	CFile file;
	if(!file.Open(m_strPath, CFile::modeRead | CFile::typeBinary))
	{
		AfxMessageBox(_T("�ļ���ʧ��"));
		
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
	//��ʼ�����ļ�
	for(;;)
	{
		//һ�ζ�ȡBLOCKSIZE��С���ļ�����
		nLen = file.Read(buf, BLOCKSIZE);
		if(nLen == 0)
			break;
		
		//�����ļ�����
		nSize = senSo.Send(buf, nLen);
		
		dwCount += nSize;
		m_ctrlProgress.SetPos(dwCount);
		CString strTransfersSize;
		strTransfersSize.Format("%ld �ֽ�", dwCount);
		GetDlgItem(IDC_RECEIVE_SIZE)->SetWindowText(strTransfersSize);
		
		if(m_bIsStop)
		{
			m_bIsStop = FALSE;
			break;
		}
		
		if(nSize == SOCKET_ERROR)
			break;
	}
	//�ر��ļ�
	file.Close();
	//�ر��׽���
	senSo.Close();
	
	if(m_dwFileSize == dwCount)
		AfxMessageBox(_T("�ļ����ͳɹ�"));
	else
		AfxMessageBox(_T("�ļ�����ʧ��"));
	
	m_ctrlProgress.SetPos(0);
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);

	m_bIsTransmitting = FALSE;	
}

void CFileTransfersDlg::ReceiveFile(CSocket &recSo)
{
	//ֹͣ�ȴ���ʱ��ʱ��
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
	//����һ���ļ�
	CFile file(m_strPath, CFile::modeCreate|CFile::modeWrite);
	//��ʼ�����ļ�
	for(;;)
	{
		//ÿ�ν���BLOCKSIZE��С���ļ�����
		nSize = recSo.Receive(buf, BLOCKSIZE);
		if(nSize == 0)
			break;
		
		//�����յ����ļ�д���½����ļ���ȥ
		file.Write(buf, nSize);
		dwCount += nSize;
		m_ctrlProgress.SetPos(dwCount);
		CString strTransfersSize;
		strTransfersSize.Format("%ld �ֽ�", dwCount);
		GetDlgItem(IDC_RECEIVE_SIZE)->SetWindowText(strTransfersSize);
		
		//�û��Ƿ�Ҫֹͣ����
		if(m_bIsStop)
		{
			m_bIsStop = FALSE;
			break;
		}
	}
	//�ر��ļ�
	file.Close();
	//�ر��׽���
	recSo.Close();
	
	if(m_dwFileSize == dwCount)
		AfxMessageBox(_T("�ļ����ճɹ�"));
	else
		AfxMessageBox(_T("�ļ�����ʧ��"));
	
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
	//���߶Է��ļ����󱻽�����׼���ý���	
	CMessage* pMsg = new CMessage(ACCEPT);
	m_psockClient->SendMsg(pMsg);
	//����һ��IDΪ2�ĳ�ʱ��ʱ��
	m_nTimer = SetTimer(2, 5000, NULL);
	return 0;
}

void CFileTransfersDlg::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	//IDΪ1�ļ�ʱ��
	case 1:
		{
			//����IDΪ1�ļ�ʱ��
			KillTimer(1);
			m_bIsWait = FALSE;
			
			//���߶Է����͵ȴ���ȡ��
			CMessage* pMsg = new CMessage(CANCEL);
			m_psockClient->SendMsg(pMsg);
			
			MessageBox(_T("�ȴ���ʱ"), _T(" ����"), MB_ICONEXCLAMATION);
			
			GetDlgItem(IDC_DISCONNECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_SELECT_FILE)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP_TRANSFERS)->EnableWindow(FALSE);
			break;
		}
	//IDΪ2�ļ�ʱ��
	case 2:
		{
			//����IDΪ2�ļ�ʱ��
			KillTimer(2);
			//��������
			CSocket sockClient;
			sockClient.Create();
			sockClient.Connect(_T("127.0.0.1"), m_wPort + PORT);
			sockClient.Close();
			break;
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}
