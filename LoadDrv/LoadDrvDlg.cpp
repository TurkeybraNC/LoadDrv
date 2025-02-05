
// LoadDrvDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "LoadDrv.h"
#include "LoadDrvDlg.h"
#include "afxdialogex.h"
#include"fn.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLoadDrvDlg 对话框

CString szSvcPath;
CString szSvcName{};
CString szFileFullName;//Note: Gobal 

CLoadDrvDlg::CLoadDrvDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOADDRV_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoadDrvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, szPreOutput);
}

BEGIN_MESSAGE_MAP(CLoadDrvDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT1, &CLoadDrvDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &CLoadDrvDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CLoadDrvDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLoadDrvDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CLoadDrvDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CLoadDrvDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// CLoadDrvDlg 消息处理程序

BOOL CLoadDrvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLoadDrvDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLoadDrvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLoadDrvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLoadDrvDlg::PrintMessage(CString szOutput)
{
	CString strOutput;
	szPreOutput.GetWindowTextW(strOutput);
	strOutput = strOutput + szOutput + L"\r\n";

	szPreOutput.SetRedraw(FALSE);
	szPreOutput.SetWindowTextW(strOutput);

	int lines = szPreOutput.GetLineCount();
	int counts = strOutput.GetLength();
	szPreOutput.LineScroll(lines, 0);
	szPreOutput.SetSel(counts, counts);

	szPreOutput.SetRedraw(TRUE);
}
void CLoadDrvDlg::OnEnChangeEdit1()
{
	
}


void CLoadDrvDlg::OnBnClickedButton1()
{
	TCHAR szFilter[] = _T("驱动文件(*.sys)|*.sys|所有文件(*.*)|*.*||");
	CFileDialog fileDlg(TRUE, _T("sys"), NULL, 0, szFilter, this);
	CString strFilePath;

	if (IDOK == fileDlg.DoModal()) {
		strFilePath = fileDlg.GetPathName();
		SetDlgItemText(IDC_EDIT1, strFilePath);
		GetDlgItem(IDC_EDIT1)->GetWindowText(szSvcPath);

		int nPos = szSvcPath.ReverseFind('\\'); // 文件路径，以'\'斜杠分隔的路径
		szFileFullName = szSvcPath.Right(szSvcPath.GetLength() - nPos - 1); // 获取文件全名，包括文件名和扩展名
		nPos = szFileFullName.ReverseFind('.');
		szSvcName = szFileFullName.Left(nPos); // 获取文件名

	}

}
void CLoadDrvDlg::OnBnClickedButton2()
{
	RetInfo stInfo;
    stInfo = SvcInstall((LPTSTR)(LPCTSTR)szSvcPath, (LPTSTR)(LPCTSTR)szSvcName);

    if (stInfo.status == STATUS_SUCCESS) {
		PrintMessage(stInfo.retinfo + szFileFullName);
		PrintMessage((CString)"驱动服务名："+ szSvcName);
		MessageBoxA(NULL,(LPCSTR)"成功安装",(LPCSTR)"信息", MB_ICONINFORMATION);
	}
	else {
		PrintMessage((CString)"加载错误驱动：" + szFileFullName);
		PrintMessage(stInfo.retinfo);
		MessageBoxA(NULL, (LPCSTR)"安装错误\n请见详细信息",(LPCSTR)"错误", MB_ICONERROR);
	}
}


void CLoadDrvDlg::OnBnClickedButton3()
{
	RetInfo stInfo;
	stInfo = DoStartSvc((LPTSTR)(LPCTSTR)szSvcName);

	if (stInfo.status == STATUS_SUCCESS) {
		PrintMessage(stInfo.retinfo + szFileFullName);
		MessageBoxA(NULL, (LPCSTR)"成功启动", (LPCSTR)"信息", MB_ICONINFORMATION);
	}
	else {
		PrintMessage((CString)"启动错误驱动：" + szFileFullName);
		PrintMessage(stInfo.retinfo);
		MessageBoxA(NULL, (LPCSTR)"启动错误\n请见详细信息", (LPCSTR)"错误", MB_ICONERROR);
	}
}


void CLoadDrvDlg::OnBnClickedButton4()
{
	RetInfo stInfo;
	stInfo = DoStopSvc((LPTSTR)(LPCTSTR)szSvcName);

	if (stInfo.status == STATUS_SUCCESS) {
		PrintMessage(stInfo.retinfo + szFileFullName);
		MessageBoxA(NULL, (LPCSTR)"成功停止", (LPCSTR)"信息", MB_ICONINFORMATION);
	}
	else {
		PrintMessage((CString)"停止错误驱动：" + szFileFullName);
		PrintMessage(stInfo.retinfo);
		MessageBoxA(NULL, (LPCSTR)"停止错误\n请见详细信息", (LPCSTR)"错误", MB_ICONERROR);
	}
}



void CLoadDrvDlg::OnBnClickedButton5()
{
	RetInfo stInfo;
	stInfo = DoDeleteSvc((LPTSTR)(LPCTSTR)szSvcName);

	if (stInfo.status == STATUS_SUCCESS) {
		PrintMessage(stInfo.retinfo + szFileFullName);
		MessageBoxA(NULL, (LPCSTR)"成功删除驱动服务", (LPCSTR)"信息", MB_ICONINFORMATION);
	}
	else {
		PrintMessage((CString)"删除错误驱动：" + szFileFullName);
		PrintMessage(stInfo.retinfo);
		MessageBoxA(NULL, (LPCSTR)"删除错误\n请见详细信息", (LPCSTR)"错误", MB_ICONERROR);
	}
}
