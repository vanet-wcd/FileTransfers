// FileTransfers.h : main header file for the FILETRANSFERS application
//

#if !defined(AFX_FILETRANSFERS_H__A2898495_8C52_40AD_890E_6EBFD89A08EE__INCLUDED_)
#define AFX_FILETRANSFERS_H__A2898495_8C52_40AD_890E_6EBFD89A08EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFileTransfersApp:
// See FileTransfers.cpp for the implementation of this class
//

class CFileTransfersApp : public CWinApp
{
public:
	CFileTransfersApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileTransfersApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFileTransfersApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILETRANSFERS_H__A2898495_8C52_40AD_890E_6EBFD89A08EE__INCLUDED_)
