#if !defined(AFX_LISTENSOCKET_H__9020F185_706C_4AAC_8CDE_DFC8F9DAD490__INCLUDED_)
#define AFX_LISTENSOCKET_H__9020F185_706C_4AAC_8CDE_DFC8F9DAD490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListenSocket.h : header file
//

#include "FileTransfersDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CListenSocket command target

class CListenSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CListenSocket();
	virtual ~CListenSocket();

// Overrides
public:
	CListenSocket(CFileTransfersDlg* pdlgMain);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListenSocket)
	public:
	virtual void OnAccept(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CListenSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	CFileTransfersDlg* m_pdlgMain;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTENSOCKET_H__9020F185_706C_4AAC_8CDE_DFC8F9DAD490__INCLUDED_)
