// XDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "XFFNews.h"
#include "XDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum HasAdmin{
	XP, YES, NO
};

HasAdmin hasAdmin;
BOOL is64;

BOOL IsWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)
		GetProcAddress(GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
	if (NULL != fnIsWow64Process)
	{
		fnIsWow64Process(GetCurrentProcess(),&bIsWow64) ;
	}
	return bIsWow64;
}


HasAdmin isAdmin(){
	BOOL isele=FALSE;
	HANDLE hToken=NULL;
	UINT16 winVer=LOWORD(GetVersion());
	winVer=MAKEWORD(HIBYTE(winVer),LOBYTE(winVer));
	if(winVer < 0x0600){
		return XP;
	}
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_QUERY,&hToken)){
		struct{
			DWORD TokenIsElevated;
		}te;
		DWORD returnLength=0;
		if(GetTokenInformation(hToken,(_TOKEN_INFORMATION_CLASS)20,&te,sizeof(te),&returnLength)){
			if(returnLength==sizeof(te))
				isele=te.TokenIsElevated;
		}
	}
	CloseHandle(hToken);
	return isele?YES:NO;
}




CXDlg::CXDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ADMIN, NoAdmin);
	DDX_Control(pDX, IDC_WOW, Wowr);
	DDX_Control(pDX, IDC_LIST, Dolist);
	DDX_Control(pDX, IDC_CLEAN, StartB);
}

BEGIN_MESSAGE_MAP(CXDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CLEAN, &CXDlg::OnBnClickedClean)
END_MESSAGE_MAP()

BOOL CXDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	hasAdmin=isAdmin();
	CString str;
	switch(hasAdmin){
		case YES:
			NoAdmin.SetWindowTextW(_T("管理员权限正常"));
			break;
		case NO:
		{
			int f=MessageBox(_T("程序没有管理员权限，继续使用会限制部分功能，继续吗？"),_T("啊这"),MB_YESNO);
			if(f==IDNO){
				OnOK();
			}
			NoAdmin.SetWindowTextW(_T("程序没有管理员权限，建议以管理员身份启动"));
			break;
		}
		case XP:
			NoAdmin.SetWindowTextW(_T("XP"));
	}
	is64=IsWow64();

	Wowr.SetWindowTextW(is64?_T("64位系统"):_T("32位系统"));

	return TRUE; 
}

void CXDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//

void executeAndWait(CString path,CString para) {
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = path;
	ShExecInfo.lpParameters = para;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
}

HCURSOR CXDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CXDlg::OnBnClickedClean()
{
	StartB.EnableWindow(FALSE);
	Dolist.ResetContent();
	if(hasAdmin==NO){
		Dolist.AddString(_T("W: 没有管理员权限，"));
	}
	Dolist.AddString(_T("I: 开始"));
	Dolist.AddString(_T("I: 正在杀死 FlashHelperService"));
	executeAndWait(_T("taskkill.exe"),_T("/im FlashHelperService.exe /f"));
	Dolist.AddString(_T("I: Taskkill 已执行"));
	if(is64){
		Dolist.AddString(_T("I: 正在执行64位注册表修改"));
		CRegKey key;
		key.Create(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run\\"));
		long ret=key.DeleteValue(_T("McumRepairer"));
		key.Close();
		switch(ret){
			case ERROR_FILE_NOT_FOUND:
				Dolist.AddString(_T("I: 找不到键值"));
				break;
			case ERROR_SUCCESS:
				Dolist.AddString(_T("I: 注册表更改成功"));	
				break;
			default:
				Dolist.AddString(_T("E: 更改失败"));
		}
	}
	if(is64){
		Dolist.AddString(_T("I: 正在执行64位清除工作"));
		DeleteFile(_T("C:\\Windows\\SysWOW64\\Macromed\\Flash\\FlashHelperService.exe"));
		Dolist.AddString(_T("I: 执行成功"));
	}else{
		Dolist.AddString(_T("I: 正在执行32位清除工作"));
		DeleteFile(_T("C:\\Windows\\System32\\Macromed\\Flash\\FlashHelperService.exe"));
		Dolist.AddString(_T("I: 执行成功"));
	}
	StartB.EnableWindow(TRUE);
}
