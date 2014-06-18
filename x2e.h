// x2e.h : main header file for the X2E application
//

#if !defined(AFX_X2E_H__646244D3_DB6D_4AAF_8A56_8E683B6E1752__INCLUDED_)
#define AFX_X2E_H__646244D3_DB6D_4AAF_8A56_8E683B6E1752__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CX2eApp:
// See x2e.cpp for the implementation of this class
//

class CX2eApp : public CWinApp
{
public:
	CX2eApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CX2eApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CX2eApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_X2E_H__646244D3_DB6D_4AAF_8A56_8E683B6E1752__INCLUDED_)
