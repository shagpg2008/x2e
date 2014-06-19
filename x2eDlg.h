// x2eDlg.h : header file
//

#if !defined(AFX_X2EDLG_H__C23D6959_4E0D_450E_9684_F44A30B8AB7A__INCLUDED_)
#define AFX_X2EDLG_H__C23D6959_4E0D_450E_9684_F44A30B8AB7A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CX2eDlg dialog

class CX2eDlg : public CDialog
{
// Construction
public:
	CX2eDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CX2eDlg)
	enum { IDD = IDD_X2E_DIALOG };
	CEdit	m_ctrlInfo;
	CString	m_szFilePath;
	CString	m_strTargetFilePath;
	CString	m_strInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CX2eDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CX2eDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonBrowse();
	afx_msg void OnDropFiles( HDROP hDropInfo );
	virtual void OnOK();
	afx_msg void OnButtonTarget();
	afx_msg void OnChangeEditTarget();
	afx_msg void OnButtonOpenTarget();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_X2EDLG_H__C23D6959_4E0D_450E_9684_F44A30B8AB7A__INCLUDED_)
