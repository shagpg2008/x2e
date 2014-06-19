// x2eDlg.cpp : implementation file
//

#include "stdafx.h"
#include "x2e.h"
#include "x2eDlg.h"
#include <io.h> 
#include <time.h>
#include <shellapi.h>

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
// CX2eDlg dialog

CX2eDlg::CX2eDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CX2eDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CX2eDlg)
	m_szFilePath = _T("");
	m_strTargetFilePath = _T("");
	m_strInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CX2eDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CX2eDlg)
	DDX_Control(pDX, IDC_EDIT_INFO, m_ctrlInfo);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_szFilePath);
	DDX_Text(pDX, IDC_EDIT_TARGET, m_strTargetFilePath);
	//DDX_Text(pDX, IDC_EDIT_INFO, m_strInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CX2eDlg, CDialog)
	//{{AFX_MSG_MAP(CX2eDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_TARGET, OnButtonTarget)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_TARGET, OnButtonOpenTarget)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CX2eDlg message handlers

BOOL CX2eDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CX2eDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CX2eDlg::OnPaint() 
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
HCURSOR CX2eDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CX2eDlg::OnButtonBrowse()
{
	CFileDialog dlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY, 
		_T("IMAGE Files (*.log;*.txt)|*.log;*.txt||"));
	if(dlg.DoModal() != IDOK) {
		return ;
	}

	m_szFilePath = dlg.GetPathName();
	UpdateData(FALSE);
}

void CX2eDlg::OnDropFiles( HDROP hDropInfo )
{
	char szFile[MAX_PATH] = {0};
	DragQueryFile(hDropInfo, 0, szFile, sizeof(szFile));
	DragFinish(hDropInfo);

	m_szFilePath = szFile;
	UpdateData(FALSE);
}

void GetBakeFileName(const char *filename, char *backup, int len)
{
	FILE *fp = NULL;
	char file[260] = {0};
	char *tmp = NULL;
	const char *ext = NULL;
	int index = 0;

	if((fp=fopen(filename, "rb")) == NULL) {
		strncpy(backup, filename, len);
		return ;
	}

	strncpy(file, filename, sizeof(file)-1);
	if((tmp = strrchr(file, '.')) == NULL) {
		ext = "";
		tmp = file + strlen(file);
	} else {
		ext = filename+(int)(tmp-file);
	}

	do {
		sprintf(tmp, "_%d%s", index++, ext);
		if(fp != NULL) {
			fclose(fp);
		}
	}while((fp=fopen(file, "rb")) != NULL);
	strncpy(backup, file, len);
}

extern "C" int convert_pcap_file(const char *from, const char *target);
void CX2eDlg::OnOK() 
{
	CString target;
	int num = 0;

	UpdateData();
	target = m_strTargetFilePath;
	if(m_szFilePath == _T("")) {
		AfxMessageBox(_T("Please select the tcpdump log file."));
		return ;
	}

	if(_access(m_szFilePath, 0) == -1) {
		AfxMessageBox(_T("File doesn't exists."));
		return ;
	}

	if(m_strTargetFilePath == _T("")) {
		char buff[MAX_PATH]  = {0};
		target = m_szFilePath.Left(m_szFilePath.ReverseFind('.'))+_T(".pcap");
		GetBakeFileName(target, buff, MAX_PATH);
		target = buff;
	} 

	DWORD nStart = GetTickCount();
	num = convert_pcap_file(m_szFilePath, target);
	CString info;
	time_t now = time(NULL);
	struct tm now_tm = *localtime(&now);
	info.Format(_T("%d:%d:%d Process: %d items, time cost:%dms, target:%s\r\n\r\n"), 
		now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec,
		num, GetTickCount() - nStart, target );

	int len = m_ctrlInfo.GetWindowTextLength();
	m_ctrlInfo.SetSel(len,len);             //将插入光标放在最后
	m_ctrlInfo.ReplaceSel(info);
	m_ctrlInfo.ScrollWindow(0,0);            //滚动到插入点
	m_strTargetFilePath = target;
}

void CX2eDlg::OnButtonOpenTarget()
{
	if(m_strTargetFilePath == _T("")
		|| _access(m_strTargetFilePath, 0) == -1) {
		AfxMessageBox(_T("Please transfer the pcap file first."));
		return ;
	}

	ShellExecute(NULL,"open",m_strTargetFilePath,NULL,NULL,SW_SHOW);
}

void CX2eDlg::OnButtonTarget() 
{
	CFileDialog dlg(FALSE, NULL, NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("IMAGE Files (*.pcap)|*.pcap||"));

	UpdateData();
	if(dlg.DoModal() != IDOK) {
		return ;
	}

	m_strTargetFilePath = dlg.GetPathName();
	if(m_strTargetFilePath.Find(_T(".pcap"), 0) < 0
		&& m_strTargetFilePath.Find(_T(".PCAP"), 0) < 0) {
		m_strTargetFilePath += _T(".pcap");
	}
	UpdateData(FALSE);
}
