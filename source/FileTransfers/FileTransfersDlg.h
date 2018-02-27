// FileTransfersDlg.h : header file
//
#include "ListenSocket.h"
#include "ClientSocket.h"

#if !defined(AFX_FILETRANSFERSDLG_H__5153CD35_4A44_4D66_8040_19063C3E46B2__INCLUDED_)
#define AFX_FILETRANSFERSDLG_H__5153CD35_4A44_4D66_8040_19063C3E46B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFileTransfersDlg dialog

#define PORT 1024
#define BLOCKSIZE 1024         //»º³åÇø´óÐ¡

#define SERVER 0
#define CLIENT 1

#define CONNECT_BE_ACCEPT    0x00
#define CONNECT_BE_REFUSE    0x01
#define DISCONNECT           0x02
#define REQUEST              0x03
#define ACCEPT               0x04
#define REFUSE               0x05
#define CANCEL               0x06

class CListenSocket;
class CClientSocket;

class CFileTransfersDlg : public CDialog
{
// Construction
public:
	CListenSocket* m_psockServer;
	CClientSocket* m_psockClient;

	CString m_strPath;
	DWORD m_dwFileSize;

	int m_nTimer;
	BOOL m_bIsClient;
	BOOL m_bIsStop, m_bIsWait, m_bIsTransmitting;

	void ProcessAccept();
	void ProcessReceive(CClientSocket* pSocket);

	void SendFile(CSocket &senSo);
	void ReceiveFile(CSocket &recSo);
	
	void TransfersFailed();
	CString GetError(DWORD error);
	int GetLocalHostInfo(CString &strHostName, CString &strIPAddress);
	
	CFileTransfersDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFileTransfersDlg)
	enum { IDD = IDD_FILETRANSFERS_DIALOG };
	CProgressCtrl	m_ctrlProgress;
	CString	m_strFileName;
	CString	m_strFileSize;
	UINT	m_wPort;
	int		m_nServerType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileTransfersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFileTransfersDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	afx_msg void OnRadioServer();
	afx_msg void OnRadioClient();
	afx_msg void OnBegin();
	afx_msg void OnDisconnect();
	afx_msg void OnSelectFile();
	afx_msg void OnStopTransfers();
	virtual void OnOK();
	afx_msg LRESULT OnAcceptTransfers(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETRANSFERSDLG_H__5153CD35_4A44_4D66_8040_19063C3E46B2__INCLUDED_)
