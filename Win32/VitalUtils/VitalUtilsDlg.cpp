#include "stdafx.h"
#include <afxole.h>
#include "VitalUtils.h"
#include "VitalUtilsDlg.h"
#include "afxdialogex.h"
#include <filesystem>

#define LVIS_CHECKED 0x2000 
#define LVIS_UNCHECKED 0x1000

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

size_t fputstr(const string& s, FILE* f) {
	if (s.empty()) return 0;
	return fwrite(&s[0], 1, s.size(), f);
}

CVitalUtilsDlg::CVitalUtilsDlg(CWnd* pParent)
: CDialogEx(IDD_VITALUTILS_DIALOG, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVitalUtilsDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOG, m_ctrLog);
	DDX_Text(pDX, IDC_IDIR, m_strIdir);
	DDX_Text(pDX, IDC_ODIR, m_strOdir);
	DDX_Check(pDX, IDC_SKIP, m_bSkip);
	DDX_Control(pDX, IDC_RUN, m_btnRun);
	DDX_Control(pDX, IDC_STOP, m_btnStop);
	DDX_Text(pDX, IDC_TOOL_DESC6, m_strProgress);
	DDX_Control(pDX, IDC_FILELIST, m_ctrlFileList);
	DDX_Control(pDX, IDC_TRKLIST, m_ctrlTrkList);
	DDX_Check(pDX, IDC_MAKE_SUBDIR, m_bMakeSubDir);

	DDX_Control(pDX, IDC_SEL_RUN, m_ctrlSelRun);
	DDX_Control(pDX, IDC_SEL_COPY, m_ctrlSelCopyFiles);
	DDX_Control(pDX, IDC_SEL_RECS, m_ctrlSelRecs);
	DDX_Control(pDX, IDC_SEL_DEL_TRKS, m_ctrlSelDelTrks);
	DDX_Control(pDX, IDC_SEL_RENAME_TRKS, m_ctrlSelRenameTrks);
	DDX_Control(pDX, IDC_SEL_RENAME_DEV, m_ctrlSelRenameDev);

	DDX_Control(pDX, IDC_BTN_IDIR, m_btnIdir);
	DDX_Control(pDX, IDC_RESCAN, m_btnScan);
	DDX_Control(pDX, IDC_SAVE_LIST, m_btnSaveList);
	DDX_Control(pDX, IDC_TRK_ALL, m_btnTrkAll);
	DDX_Control(pDX, IDC_TRK_NONE, m_btnTrkNone);
	DDX_Control(pDX, IDC_FILE_ALL, m_btnFileAll);
	DDX_Control(pDX, IDC_FILE_NONE, m_btnFileNone);
	DDX_Control(pDX, IDC_BTN_ODIR, m_btnOdir);
	DDX_Control(pDX, IDC_CLEAR, m_btnClear);
	DDX_Control(pDX, IDC_TOOL_DESC4, m_ctrlOdirStatic);
	DDX_Control(pDX, IDC_FOLDER, m_ctrlIdirStatic);
	DDX_Control(pDX, IDC_TOOL_DESC6, m_ctrProgress);
	DDX_Control(pDX, IDC_SELECT, m_btnSelect);
	DDX_Control(pDX, IDC_FILTER, m_ctrlFilter);
	DDX_Control(pDX, IDC_TRK_OPER, m_ctrlSelOper);
	DDX_Control(pDX, IDC_EXACT, m_ctrlExact);
	DDX_Control(pDX, IDC_TRACK_COUNT, m_ctrlTrkCnt);
	DDX_Control(pDX, IDC_FILE_COUNT, m_ctrlFileCnt);
	DDX_Control(pDX, IDC_IDIR, m_ctrIdir);
	DDX_Control(pDX, IDC_ODIR, m_ctrOdir);
	DDX_Control(pDX, IDC_MAKE_SUBDIR, m_ctrlMakeSubDir);
	DDX_Control(pDX, IDC_SKIP, m_ctrlSkip);
	DDX_Control(pDX, IDC_SELECTED_TRKS, m_ctrlSelTrks);
	DDX_Control(pDX, IDC_TRK_FILTER, m_ctrlTrkFilter);
}

CString g_name = _T("VitalUtils");

BEGIN_MESSAGE_MAP(CVitalUtilsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_IDIR, &CVitalUtilsDlg::OnBnClickedBtnIdir)
	ON_BN_CLICKED(IDC_BTN_ODIR, &CVitalUtilsDlg::OnBnClickedBtnOdir)
	ON_BN_CLICKED(IDC_RUN, &CVitalUtilsDlg::OnBnClickedRun)
	ON_BN_CLICKED(IDC_STOP, &CVitalUtilsDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CLEAR, &CVitalUtilsDlg::OnBnClickedClear)
	ON_WM_DESTROY()
	ON_NOTIFY(LVN_GETDISPINFO, IDC_FILELIST, &CVitalUtilsDlg::OnGetdispinfoFilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, &CVitalUtilsDlg::OnNMDblclkFilelist)
	ON_BN_CLICKED(IDC_RESCAN, &CVitalUtilsDlg::OnBnClickedRescan)
	ON_BN_CLICKED(IDC_TRK_ALL, &CVitalUtilsDlg::OnBnClickedTrkAll)
	ON_BN_CLICKED(IDC_TRK_NONE, &CVitalUtilsDlg::OnBnClickedTrkNone)
	ON_BN_CLICKED(IDC_FILE_ALL, &CVitalUtilsDlg::OnBnClickedFileAll)
	ON_BN_CLICKED(IDC_FILE_NONE, &CVitalUtilsDlg::OnBnClickedFileNone)
	ON_BN_CLICKED(IDC_SAVE_LIST, &CVitalUtilsDlg::OnBnClickedSaveList)

	ON_BN_CLICKED(IDC_SEL_RUN, &CVitalUtilsDlg::OnBnClickedSelRunScript)
	ON_BN_CLICKED(IDC_SEL_COPY, &CVitalUtilsDlg::OnBnClickedSelCopyFiles)
	ON_BN_CLICKED(IDC_SEL_RECS, &CVitalUtilsDlg::OnBnClickedSelRecs)

	ON_BN_CLICKED(IDC_SEL_RENAME_DEV, &CVitalUtilsDlg::OnBnClickedSelRenameDev)
	ON_BN_CLICKED(IDC_SEL_DEL_TRKS, &CVitalUtilsDlg::OnBnClickedSelDelTrks)
	ON_BN_CLICKED(IDC_SEL_RENAME_TRKS, &CVitalUtilsDlg::OnBnClickedSelRenameTrks)

	ON_CBN_SELCHANGE(IDC_TRK_OPER, &CVitalUtilsDlg::OnCbnSelchangeTrkOper)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_FILELIST, &CVitalUtilsDlg::OnLvnItemchangedFilelist)
	ON_BN_CLICKED(IDC_SELECT, &CVitalUtilsDlg::OnBnClickedSelect)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CVitalUtilsDlg::OnHdnItemclickFilelist)
	ON_BN_CLICKED(IDC_TRK_SELECT, &CVitalUtilsDlg::OnBnClickedTrkAdd)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_FILELIST, &CVitalUtilsDlg::OnLvnBegindragFilelist)
	ON_EN_KILLFOCUS(IDC_IDIR, &CVitalUtilsDlg::OnEnKillfocusIdir)
	ON_EN_KILLFOCUS(IDC_ODIR, &CVitalUtilsDlg::OnEnKillfocusOdir)
	ON_BN_CLICKED(IDC_IDIR_OPEN, &CVitalUtilsDlg::OnBnClickedIdirOpen)
	ON_LBN_DBLCLK(IDC_TRKLIST, &CVitalUtilsDlg::OnLbnDblclkTrklist)
	ON_EN_CHANGE(IDC_TRK_FILTER, &CVitalUtilsDlg::OnEnChangeTrkFilter)
	ON_EN_CHANGE(IDC_SELECTED_TRKS, &CVitalUtilsDlg::OnEnChangeSelectedTrks)
END_MESSAGE_MAP()

void CVitalUtilsDlg::OnBnClickedRescan() {
	CString str;
	m_btnScan.GetWindowText(str);
	if (str == "Scan") { // rescan
		m_path_trklist.clear();
		m_parses.Clear();

		theApp.log("Scanning folder");

		m_btnScan.SetWindowText("Stop");
		m_ctrIdir.EnableWindow(FALSE);
		m_btnIdir.EnableWindow(FALSE);

		// ���� Ʈ������ ���� ����
		{
			decltype(m_dtnames) blank;
			lock_guard<shared_mutex> lk(m_mutex_dtnames);
			::swap(blank, m_dtnames);
		}
		m_ctrlTrkList.SetCurSel(-1);
		m_ctrlTrkList.ResetContent();
		m_shown.clear();

		// ���� ���� ������ ���� �����
		m_ctrlFileList.SetItemCountEx(0, NULL);
		decltype(m_files) copy;
		{
			unique_lock<shared_mutex> lk(m_mutex_file);
			swap(copy, m_files);
		}
		for (auto& p : copy) delete p;

		// �Է� ���丮 �б� ������ ����
		// workthread ���� �˾Ƽ� scanning�� ������ ���̴�
		auto rootdir = rtrim((LPCTSTR)m_strIdir, "\\");

		m_dtstart = time(nullptr);
		m_scans.Push(rootdir + "\\"); // root directory�� �Ľ� �����忡 �߰�
		m_ntotal = 1;
		m_nJob = JOB_SCANNING;

		m_btnRun.EnableWindow(FALSE);
		m_btnStop.EnableWindow(FALSE);
		m_ctrlSelRun.EnableWindow(FALSE);
		m_ctrlSelCopyFiles.EnableWindow(FALSE);
		m_ctrlSelRecs.EnableWindow(FALSE);
		m_ctrlSelDelTrks.EnableWindow(FALSE);
		m_ctrlSelRenameTrks.EnableWindow(FALSE);
		m_ctrlSelRenameDev.EnableWindow(FALSE);
	} else { // parsing �ߴ�
		m_parses.Clear();
		m_scans.Clear();
		m_dtstart = 0;
		m_ntotal = 0;
		m_bStopping = true;

		m_btnScan.SetWindowText("Scan");
		m_ctrIdir.EnableWindow(TRUE);
		m_btnIdir.EnableWindow(TRUE);
	}
}

// running �ߴ�
void CVitalUtilsDlg::OnBnClickedCancel() {
	m_jobs.Clear();
	m_dtstart = 0;
	m_ntotal = 0;
	m_bStopping = true;
}

BOOL CVitalUtilsDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	m_ctrlSelOper.SelectString(0, "OR");

	m_canFolder.LoadIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_FOLDER), IMAGE_ICON, 16, 16, 0));

	m_ctrlSelRun.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_COPY), IMAGE_ICON, 16, 16, 0));
	m_ctrlSelCopyFiles.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_COPY), IMAGE_ICON, 16, 16, 0));
	m_ctrlSelRecs.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CSV), IMAGE_ICON, 16, 16, 0));
	m_ctrlSelDelTrks.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DEL), IMAGE_ICON, 16, 16, 0));
	m_ctrlSelRenameDev.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_DEV), IMAGE_ICON, 16, 16, 0));
	m_ctrlSelRenameTrks.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_TRK), IMAGE_ICON, 16, 16, 0));

	m_btnSaveList.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_SAVE), IMAGE_ICON, 16, 16, 0));
	m_btnClear.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_CLEAR), IMAGE_ICON, 16, 16, 0));
	m_btnRun.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_RUN), IMAGE_ICON, 16, 16, 0));
	m_btnStop.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_STOP), IMAGE_ICON, 16, 16, 0));
	m_btnSelect.SetIcon((HICON)LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_SELECT), IMAGE_ICON, 16, 16, 0));

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	m_ctrLog.SetOptions(ECOOP_OR, ECO_SAVESEL); // turn on savesel (��Ŀ���� �Ҿ ���ÿ����� ����)
	m_ctrLog.SetOptions(ECOOP_AND, ~(ECO_AUTOVSCROLL | ECO_AUTOHSCROLL)); // turn off autoscroll
	m_ctrLog.HideSelection(FALSE, TRUE);

	m_strIdir = theApp.GetProfileString(g_name, _T("idir"));
	if (!filesystem::is_directory((LPCTSTR)m_strIdir)) {
		char temp[MAX_PATH];
		SHGetSpecialFolderPath(NULL, temp, CSIDL_MYDOCUMENTS, FALSE);
		m_strIdir = temp;
	}

	m_strOdir = theApp.GetProfileString(g_name, _T("odir"));
	if (!filesystem::is_directory((LPCTSTR)m_strOdir)) {
		char temp[MAX_PATH];
		SHGetSpecialFolderPath(NULL, temp, CSIDL_DESKTOPDIRECTORY, FALSE);
		m_strOdir = temp;
	}

	UpdateData(FALSE);

//	m_imglist.Create(IDB_CHECKBOX, 16, 0, RGB(255, 0, 255));
//	m_ctrlFileList.SetImageList(&m_imglist, LVSIL_SMALL);

	m_ctrlFileList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlFileList.InsertColumn(0, "Filename", LVCFMT_LEFT, 160);
	m_ctrlFileList.InsertColumn(1, "Location", LVCFMT_LEFT, 70);
	m_ctrlFileList.InsertColumn(2, "Modification", LVCFMT_LEFT, 115);
	m_ctrlFileList.InsertColumn(3, "Size", LVCFMT_RIGHT, 55);
	m_ctrlFileList.InsertColumn(4, "Start Time", LVCFMT_RIGHT, 115);
	m_ctrlFileList.InsertColumn(5, "End Time", LVCFMT_RIGHT, 115);
	m_ctrlFileList.InsertColumn(6, "Length", LVCFMT_RIGHT, 40);

	// ���� ���� ���̾˷α׵��� ����
	m_dlgRun.Create(IDD_OPT_RUN_SCRIPT, this);
	m_dlgCopy.Create(IDD_OPT_COPY_FILES, this);
	m_dlgRecs.Create(IDD_OPT_RECS, this);
	m_dlgRename.Create(IDD_OPT_RENAME, this);
	m_dlgDelTrks.Create(IDD_OPT_DEL_TRKS, this);

	m_ctrlFilter.SetLimitText(0);

	CRect rwc;
	m_ctrlSelRun.GetWindowRect(rwc);
	ScreenToClient(rwc);
	m_dlgRun.SetWindowPos(NULL, rwc.right + 10, rwc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_dlgCopy.SetWindowPos(NULL, rwc.right + 10, rwc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_dlgRecs.SetWindowPos(NULL, rwc.right + 10, rwc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_dlgRename.SetWindowPos(NULL, rwc.right + 10, rwc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_dlgDelTrks.SetWindowPos(NULL, rwc.right + 10, rwc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	m_ctrlSelRun.SetCheck(TRUE);

	// refresh timer
	SetTimer(0, 1000, nullptr);
	SetTimer(1, 1000, nullptr);

	// begin worker thread pool
	auto ncores = (int)thread::hardware_concurrency();
	if (ncores > 16) ncores = 16;
	for (int i = 0; i < ncores; ++i) {
		m_thread_worker[i] = thread(&CVitalUtilsDlg::worker_thread_func, this);
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.
void CVitalUtilsDlg::OnPaint() {
	CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.
	if (IsIconic()) {
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CRect rw;
		m_ctrlIdirStatic.GetWindowRect(rw); 
		ScreenToClient(rw);
		m_canFolder.BitBltTrans(dc.m_hDC, rw.left - 25, rw.top - 2);

		m_ctrlOdirStatic.GetWindowRect(rw); 
		ScreenToClient(rw);
		m_canFolder.BitBltTrans(dc.m_hDC, rw.left - 25, rw.top - 2);

		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CVitalUtilsDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

int CALLBACK BrowseCallbackIdir(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	if (uMsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

int CALLBACK BrowseCallbackOdir(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
	if (uMsg == BFFM_INITIALIZED) {
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);

		// �ش� Control ID�� shell32.dll version 5.0 �̻󿡼� ���ȴ�.
		// ���� ���������� ���� tree control�� id�� �ٸ���
		// ������ ��ư�� ������ �� ����.
		HWND hShell = GetDlgItem(hwnd, 0);  // 0x00000000(Shell Class)
		HWND hTree = GetDlgItem(hShell, 100); // 0x00000064(Tree Control)
		HWND hNew = GetDlgItem(hwnd, 14150); // 0x00003746(New Folder Button)
		HWND hOK = GetDlgItem(hwnd, 1);  // 0x00000001(OK Button)
		HWND hCancel = GetDlgItem(hwnd, 2);  // 0x00000002(Cancel Button)
		HWND hStatic = GetDlgItem(hwnd, 14146); // 0x00003742(Static Control)
												
		if (!hShell || !hTree || !hNew || !hOK || !hCancel) return 0;  // �ϳ��� ���������� �⺻ �������� ó���Ѵ�.

		CRect rectWnd;
		CRect rectNew;
		CRect rectOK;
		CRect rectCancel;
		GetClientRect(hwnd, &rectWnd);
		GetClientRect(hNew, &rectNew);
		GetClientRect(hOK, &rectOK);
		GetClientRect(hCancel, &rectCancel);

		MoveWindow(hNew, rectWnd.left + 10, rectWnd.bottom - rectNew.Height() - 10, rectNew.Width(), rectNew.Height(), TRUE);  // �� ���� Button ����
		MoveWindow(hOK, rectWnd.right - 10 - rectCancel.Width() - 5 - rectOK.Width(), rectWnd.bottom - rectOK.Height() - 10, rectOK.Width(), rectOK.Height(), TRUE);  // Ȯ�� Button
		MoveWindow(hCancel, rectWnd.right - 10 - rectCancel.Width(), rectWnd.bottom - rectCancel.Height() - 10, rectCancel.Width(), rectCancel.Height(), TRUE);  // ��� Button
	}
	return 0;
}

void CVitalUtilsDlg::OnBnClickedBtnIdir() {
	UpdateData(TRUE);
	auto olddir = m_strIdir;
	while (true) {
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(bi));
		TCHAR szDisplayName[MAX_PATH];
		szDisplayName[0] = ' ';

		bi.hwndOwner = NULL;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDisplayName;
		bi.lpszTitle = _T("Please select a folder:");
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bi.lpfn = BrowseCallbackIdir;
		bi.lParam = (LPARAM)olddir.GetString();
		bi.iImage = 0;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi); // ��θ� �޾ƿ�
		TCHAR szPathName[MAX_PATH];
		_tcsncpy_s(szPathName, (LPCTSTR)m_strIdir, MAX_PATH);
		if (NULL == pidl) return;

		BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
		if (!bRet) return;
		
		CString newdir(szPathName);

		if (newdir.Left(m_strOdir.GetLength()) == m_strOdir ||
			m_strOdir.Left(newdir.GetLength()) == newdir) {
			AfxMessageBox("Output folder must not overlap with input folder\nPlease select another folder");
			continue;
		}

		m_strIdir = newdir;
		break;
	}
	UpdateData(FALSE);

	theApp.WriteProfileString(g_name, _T("idir"), m_strIdir);
	if(m_strIdir != olddir) OnBnClickedRescan();
}

void CVitalUtilsDlg::OnBnClickedBtnOdir() {
	UpdateData(TRUE);

	while (true) {
		auto olddir = m_strOdir;

		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(bi));
		TCHAR szDisplayName[MAX_PATH];
		szDisplayName[0] = ' ';

		bi.hwndOwner = NULL;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDisplayName;
		bi.lpszTitle = _T("Please select a folder:");
		bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_USENEWUI;
		bi.lpfn = BrowseCallbackOdir;
		bi.lParam = (LPARAM)olddir.GetString();
		bi.iImage = 0;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		TCHAR szPathName[MAX_PATH];
		_tcsncpy_s(szPathName, (LPCTSTR)m_strOdir, MAX_PATH);
		if (NULL == pidl) return;

		BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
		if (!bRet) return;

		CString newdir(szPathName);

		if (newdir.Left(m_strIdir.GetLength()) == m_strIdir ||
			m_strIdir.Left(newdir.GetLength()) == newdir) {
			AfxMessageBox("Output folder must not overlap with input folder\nPlease select another folder");
			continue;
		}

		m_strOdir = szPathName;
		break;
	}
	UpdateData(FALSE);

	theApp.WriteProfileString(g_name, _T("odir"), m_strOdir);
}

#include <queue>
#include <vector>
#include <set>
using namespace std;

bool file_exists(string path) {
	CFile fil;
	if (fil.Open(path.c_str(), CFile::modeRead | CFile::shareDenyNone)) { // Open succeeded, file exists 
		fil.Close();
		return true;
	}
	if (ERROR_FILE_NOT_FOUND == ::GetLastError()) return false;
	return false;
}

void CVitalUtilsDlg::OnBnClickedRun() {
	UpdateData(TRUE);

	// �� �������
	string stool, spre, spost;
	if (m_ctrlSelRun.GetCheck()) { // ��ũ��Ʈ ����
		string python_exe = get_module_dir() + "python\\python.exe";
		if (!file_exists(python_exe)) {
			auto str = "Running Script requires python interpreter\nDownload and setup python?";
			if (IDOK != AfxMessageBox(str, MB_OKCANCEL)) return;
			theApp.InstallPython(); // ���ŷ
		}
		m_dlgRun.UpdateData(TRUE);
		stool = "\"" + python_exe + "\" \"" + get_module_dir() + "scripts\\" + string(m_dlgRun.m_strScript) + "\"";
	} else if (m_ctrlSelCopyFiles.GetCheck()) { // export vital
		stool = "\"" + get_module_dir() + "utilities\\vital_copy.exe\"";

		// �ɼ� ������
		m_dlgCopy.UpdateData();
		if (m_dlgCopy.m_bTracks) { // ���õ� Ʈ���� ����
			auto seltrks = get_selected_trks(true);
			string strtrks; // ������ Ʈ��
			for (const auto& seltrk : seltrks) {
				if (!strtrks.empty()) strtrks += ',';
				strtrks += seltrk;
			}
			if (!strtrks.empty()) {
				spost += _T("\"") + strtrks + _T("\"");
			}
		}
	} else if (m_ctrlSelRecs.GetCheck()) { // export csv
		stool = "\"" + get_module_dir() + "utilities\\vital_recs.exe\"";

		// �ɼ� ������
		m_dlgRecs.UpdateData();
		if (m_dlgRecs.m_bUnixTime) spre += _T("u");
		if (m_dlgRecs.m_bAbstime) spre += _T("a");
		if (m_dlgRecs.m_bRestricted) spre += _T("r");
		if (m_dlgRecs.m_bLast) spre += _T("l");
		if (m_dlgRecs.m_bPrintHeader) spre += _T("h");
		if (m_dlgRecs.m_bPrintFilename) spre += _T("c");
		if (m_dlgRecs.m_bPrintMean) spre += _T("m");
		if (m_dlgRecs.m_bPrintClosest) spre += _T("n");
		if (m_dlgRecs.m_bSkipBlank) spre += _T("s");
		if (m_dlgRecs.m_bPrintDname) spre += _T("d");
		if (!spre.empty()) spre = _T("-") + spre;

		spost = str_format("%f ", m_dlgRecs.m_fInterval);

		// ������ Ʈ��
		auto seltrks = get_selected_trks(true);
		string strtrks; // ������ Ʈ��
		for (const auto& seltrk : seltrks) {
			if (!strtrks.empty()) strtrks += ',';
			strtrks += seltrk;
		}
		if (!strtrks.empty()) {
			spost += _T("\"") + strtrks + _T("\"");
		}
	} else if (m_ctrlSelDelTrks.GetCheck()) { // delete tracks
		stool = "\"" + get_module_dir() + "utilities\\vital_edit_trks.exe\"";

		auto seltrks = get_selected_trks(true);
		
		string strtrks; // ������ Ʈ��
		string smsg;
		for (const auto& seltrk : seltrks) {
			if (!strtrks.empty()) strtrks += ',';
			strtrks += seltrk;
			if (!smsg.empty()) smsg += '\n';
			smsg += seltrk;
		}
		if (strtrks.empty()) {
			AfxMessageBox("No tracks are selected");
			return;
		}
		if (IDYES != AfxMessageBox(("Are you sure to delete these trackes?\n" + smsg).c_str(), MB_YESNO)) {
			return;
		}
		spost += "\"" + strtrks + "\"";
	} else if (m_ctrlSelRenameTrks.GetCheck()) { // Ʈ�� �̸� ����
		stool = "\"" + get_module_dir() + "utilities\\vital_edit_trks.exe\"";

		// �̸� ������ Ʈ��
		string smsg;
		m_dlgRename.UpdateData();
		bool bdelsure = false;
		for (int i = 0; i < 30; i++) {
			auto from = m_dlgRename.m_strFrom[i];
			auto to = m_dlgRename.m_strTo[i];
			if (from == to) continue;
			if (from.IsEmpty()) continue;
			if (from.Find(',') != -1) continue;
			if (to.Find(',') != -1) continue;
			if (to.IsEmpty() && !bdelsure) {
				if (IDYES != AfxMessageBox("Tracks witout names will be deleted. Are you sure?\n", MB_YESNO)) {
					m_dlgRename.m_ctrlTo[i].SetFocus();
					return;
				} else {
					bdelsure = true;
				}
			}

			if (!spost.empty()) spost += ',';
			spost += from + "=" + to;

			if (!smsg.empty()) smsg += '\n';
			smsg += from + " -> " + to;
		}
		spost = "\"" + spost + "\"";

		if (IDYES != AfxMessageBox(("Are you sure to change track names?\n" + smsg).c_str(), MB_YESNO)) {
			return;
		}
	} else if (m_ctrlSelRenameDev.GetCheck()) { // ��� �̸� ����
		stool = "\"" + get_module_dir() + "utilities\\vital_edit_devs.exe\"";

		// �̸� ������ ���
		m_dlgRename.UpdateData();
		auto from = m_dlgRename.m_strFrom[0];
		auto to = m_dlgRename.m_strTo[0];
		if (from.IsEmpty()) return;
		if (to.IsEmpty()) return;
		if (from.Find(',') != -1) return;
		if (to.Find(',') != -1) return;

		spost = _T("\"") + from + _T("\" \"") + to + _T("\"");

		if (IDYES != AfxMessageBox("Are you sure to change device name?\n" + from + " -> " + to, MB_YESNO)) {
			return;
		}
	} else {
		AfxMessageBox("not implemented");
		return;
	}

	// ���õ� ���� ���ٸ� ��� �����Ѵ�
	if (m_ctrlFileList.GetSelectedCount() == 0) {
		OnBnClickedFileAll();
	}

	m_ntotal = 0;
	m_dtstart = time(nullptr);

	auto idir = rtrim((LPCTSTR)m_strIdir, '\\');
	auto odir = rtrim((LPCTSTR)m_strOdir, '\\');
	if (odir.empty()) {
		odir = idir;
	}

	// �ּ��� ��� ���丮�� �����ؾ�
	if (!filesystem::is_directory(odir)) {
		filesystem::create_directories(odir);
		theApp.WriteProfileString(g_name, _T("odir"), odir.c_str());
	}

	// recs �� ���� piped �̰� �� ��쿡�� tool spre ifile spost > ofile ���°� �ȴ�.
	// �� �ܿ��� tool spre ifile ofile spost ���°� �ȴ�.
	bool is_recs = (stool.find("vital_recs") != -1);
	if (m_dlgRecs.m_bLong) { // longitudinal �ε� ������ �����ϸ�?
		if (m_dlgRecs.m_strOutputFile.IsEmpty()) {
			AfxMessageBox("Please enter output file name");
			m_dlgRecs.m_ctrlOutputFile.SetFocus();
			return;
		}
		auto ofile = odir + "\\" + string(m_dlgRecs.m_strOutputFile);
		if (file_exists(ofile)) {
			if (IDYES != AfxMessageBox("Are you sure to overwrite previous file?\n", MB_YESNO)) {
				m_dlgRecs.m_ctrlOutputFile.SetFocus();
				return;
			}
		}

		auto seltrks = get_selected_trks();
		if (seltrks.empty()) {
			AfxMessageBox("Please select trks");
			return;
		}

		// ���� ������ ���
		auto fa = fopen(ofile.c_str(), "wb");
		fprintf(fa, "Filename,Time");
		bool bfirst = true;
		for (const auto seltrk : seltrks) {
			fputc(',', fa);
			fputstr(seltrk, fa);
		}
		fputstr("\r\n", fa);
		fclose(fa);
	}

	m_ctrLog.SetWindowText(_T(""));
	m_btnRun.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);

	m_ctrlSelRun.EnableWindow(FALSE);
	m_ctrlSelCopyFiles.EnableWindow(FALSE);
	m_ctrlSelRecs.EnableWindow(FALSE);
	m_ctrlSelDelTrks.EnableWindow(FALSE);
	m_ctrlSelRenameTrks.EnableWindow(FALSE);
	m_ctrlSelRenameDev.EnableWindow(FALSE);

	// ���� �� ���� ����� job ����Ʈ�� �߰���
	for (auto pos = m_ctrlFileList.GetFirstSelectedItemPosition(); pos;) {
		auto i = m_ctrlFileList.GetNextSelectedItem(pos);

		auto ifile = rtrim(m_shown[i]->path, '\\');

		// sub directory �����̸�?
		string subdir;
		if (m_bMakeSubDir && !m_dlgRecs.m_bLong) {
			subdir = trim(substr(dirname(ifile), idir.size()), '\\');
			filesystem::create_directories(odir + '\\' + subdir + '\\');
		}

		string ofile = basename(ifile);
		if (is_recs) {
			ofile = substr(ofile, 0, ofile.rfind('.')) + ".csv"; // change ext to csv
		}

		auto opath = odir + '\\';
		if (!subdir.empty())
			opath += subdir + '\\';
		opath += ofile;
		if (is_recs && m_dlgRecs.m_bLong) {
			opath = odir + '\\' + string(m_dlgRecs.m_strOutputFile);
		}

		// ���� ����� �����̸�?
		if (m_bSkip && !m_dlgRecs.m_bLong) {
			if (file_exists(opath)) {
				theApp.log(opath + " already exists.");
				continue;
			}
		}

		// ���� ��ɹ�
		string strJob;
		if (is_recs) {
			strJob = stool + " " + spre + " \"" + ifile + _T("\" ") + spost + " >";
			if (is_recs && m_dlgRecs.m_bLong) strJob += '>'; // longitudinal
			strJob += "\"" + opath + "\"";
		} else {
			strJob = stool +" " + spre + _T(" \"") + ifile + _T("\" \"") + opath + _T("\" ") + spost;
		}

		// ���� ť�� �߰�
		m_jobs.Push(strJob);
		m_ntotal++;
	}

	m_nJob = JOB_RUNNING;

	UpdateData(FALSE);
}

int CVitalUtilsDlg::GetNumVisibleLines() {
	CRect rect;
	long nFirstChar, nLastChar;
	long nFirstLine, nLastLine;

	// Get client rect of rich edit control
	m_ctrLog.GetClientRect(rect);

	// Get character index close to upper left corner
	nFirstChar = m_ctrLog.CharFromPos(CPoint(0, 0));

	// Get character index close to lower right corner
	nLastChar = m_ctrLog.CharFromPos(CPoint(rect.right, rect.bottom));
	if (nLastChar < 0) {
		nLastChar = m_ctrLog.GetTextLength();
	}

	// Convert to lines
	nFirstLine = m_ctrLog.LineFromChar(nFirstChar);
	nLastLine = m_ctrLog.LineFromChar(nLastChar);

	return (nLastLine - nFirstLine);
}

CString SpanToStr(double dtSpan) {
	CString str;
	if (dtSpan > 3600 * 24) {
		int sec = (int)dtSpan;
		int mins = (int)(sec / 60); sec %= 60;
		int hrs = mins / 60; mins %= 60;
		int days = hrs / 24; hrs %= 24;
		str.Format(_T("%d d %d h"), days, hrs);
	} else if (dtSpan > 3600) {
		int sec = (int)dtSpan;
		int mins = (int)(sec / 60); sec %= 60;
		int hrs = mins / 60; mins %= 60;
		str.Format(_T("%d h %d m"), hrs, mins);
	} else if (dtSpan > 60) {
		int sec = (int)dtSpan;
		int mins = sec / 60; sec %= 60;
		str.Format(_T("%d m %d s"), mins, sec);
	} else {
		if (dtSpan == (int)dtSpan) str.Format(_T("%d s"), (int)dtSpan);
		else str.Format(_T("%.3f s"), dtSpan);
	}
	return str;
}

CString basename(CString path, bool withext) {
	auto str = path.Mid(max(path.ReverseFind('/'), path.ReverseFind('\\')) + 1);
	if (withext) return str;
	return str.Left(str.ReverseFind('.'));
}

string format_number(__int64 dwNumber) {
	string str = str_format("%u", dwNumber);
	for (int i = (int)str.size() - 3; i > 0; i -= 3)
		str.insert(i, _T(","));
	return str;
}

string format_size(__int64 dwFileSize) {
	if (dwFileSize < 1048576) {
		return format_number(dwFileSize / 1024) + " KB";
	}

	int dwNumber = (int)floor(dwFileSize / 1048576.0);
	return format_number(dwNumber) + " MB";
}

void CVitalUtilsDlg::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == 0) { // ���¸� ������Ʈ
		string str;

		pair<time_t, string> msg;
		while (theApp.m_msgs.Pop(msg)) {
			str += "[" + substr(dt_to_str(msg.first), 11) + "] " + msg.second + _T("\r\n");
		}

		if (!str.empty()) {
			int nLength = m_ctrLog.GetWindowTextLength();
			// put the selection at the end of text
			m_ctrLog.SetSel(nLength, nLength);
			// replace the selection
			m_ctrLog.ReplaceSel(str.c_str());

			// scroll to last
			int nVisible = GetNumVisibleLines();
			m_ctrLog.LineScroll(m_ctrLog.GetLineCount());
			m_ctrLog.LineScroll(1 - nVisible);
		}

		if (m_nJob == JOB_SCANNING) {
			if (m_scans.IsEmpty() && !m_nrunning.Get()) { // ���� ��� ��ĳ���� ������
				if (m_bStopping) {
					m_bStopping = false;
					m_nJob = JOB_NONE;
					theApp.log("Scanning stopped");
				} else {
					m_nJob = JOB_PARSING;
					m_dtstart = time(nullptr);
					m_ntotal = m_parses.GetSize();
					theApp.log("Scanning done");
				}
			}
		} else if (m_nJob == JOB_PARSING) {
			if (m_parses.IsEmpty() && !m_nrunning.Get()) { // ���� ��� ��¡�� ������
				if (m_bStopping) {
					m_bStopping = false;
					m_nJob = JOB_NONE;
					theApp.log("Parsing stopped");
				} else {
					m_nJob = JOB_NONE;
					m_dtstart = 0;
					m_ntotal = 0;
					SaveCaches();

					theApp.log("Parsing done");

					m_btnScan.SetWindowText("Scan");
					m_ctrIdir.EnableWindow(TRUE);
					m_btnIdir.EnableWindow(TRUE);

					// �Ľ� ����κ��� ��� ������ ���� �ð�, ���� �ð�, ���̸� ����
					for (auto p : m_files) {
						auto& tl = m_path_trklist[p->path].second;
						auto pos = tl.find("#dtstart=");
						if (pos != -1) p->dtstart = strtoul(substr(tl, pos + 9).c_str(), nullptr, 10);

						pos = tl.find("#dtend=");
						if (pos != -1) p->dtend = strtoul(substr(tl, pos + 7).c_str(), nullptr, 10);

						if (p->dtstart && p->dtend) p->dtlen = p->dtend - p->dtstart;
					}

					// ��� ���ϵ��� m_path_trklist�� ����
					// �Ľ� �� ��� Ʈ�� ������ ��ħ
					set<string> dtnames;
					for (const auto& it : m_path_trklist) {
						auto file_dtnames = explode(it.second.second, ',');
						for (auto& dtname : file_dtnames) {
							dtname = trim(dtname);
							if (dtname.empty()) continue;
							if (dtname[0] == '#') continue;
							if (dtnames.find(dtname) != dtnames.end()) continue;
							//TRACE("%s %s\n", it.first.c_str(), dtname.c_str());

							// ����� �� ���� Ʈ������ ����
							bool isprintable = true;
							for (auto& c : dtname) {
								if (!::isalnum(c) && c != '-' && c != '_' && c != '/') {
									isprintable = false;
									break;
								}
							}
							if (!isprintable) continue;

							dtnames.insert(dtname);
						}
					}

					{
						lock_guard<shared_mutex> lk(m_mutex_dtnames);
						swap(dtnames, m_dtnames);
					}

					UpdateTrkList();

					// ���� �ð��� ������Ʈ �Ǿ����Ƿ� ���� ����Ʈ�� ���� �׷��� ��
					m_ctrlFileList.Invalidate();

					m_btnRun.EnableWindow(TRUE);
					m_btnStop.EnableWindow(FALSE);

					m_ctrlSelRun.EnableWindow(TRUE);
					m_ctrlSelCopyFiles.EnableWindow(TRUE);
					m_ctrlSelRecs.EnableWindow(TRUE);
					m_ctrlSelDelTrks.EnableWindow(TRUE);
					m_ctrlSelRenameTrks.EnableWindow(TRUE);
					m_ctrlSelRenameDev.EnableWindow(TRUE);
				}
			}
		} else if (m_nJob == JOB_RUNNING) {
			if (m_jobs.IsEmpty() && !m_nrunning.Get()) {// ��� �۾��� ������
				if (m_bStopping) {
					m_bStopping = false;
					m_nJob = JOB_NONE;
					theApp.log("Running stopped");
				} else {
					m_nJob = JOB_NONE;
					m_ntotal = 0;
					theApp.log("Running done");
				}
				m_btnRun.EnableWindow(TRUE);
				m_btnStop.EnableWindow(FALSE);

				m_ctrlSelRun.EnableWindow(TRUE);
				m_ctrlSelCopyFiles.EnableWindow(TRUE);
				m_ctrlSelRecs.EnableWindow(TRUE);
				m_ctrlSelDelTrks.EnableWindow(TRUE);
				m_ctrlSelRenameTrks.EnableWindow(TRUE);
				m_ctrlSelRenameDev.EnableWindow(TRUE);
			}
		}

		// update progress
		if (m_nrunning.Get()) {
			size_t nremain = 0;
			if (m_nJob == JOB_PARSING) nremain = m_parses.GetSize();
			else if (m_nJob == JOB_SCANNING) nremain = m_scans.GetSize();
			else if (m_nJob == JOB_RUNNING) nremain = m_jobs.GetSize();
			size_t ndone = 0;
			if (m_ntotal > nremain) ndone = m_ntotal - nremain;
			auto tspan = difftime(time(nullptr), m_dtstart);
			auto speed = tspan ? (ndone / tspan) : 1.0;
			auto eta = int(nremain / speed);

			CString str;
			if (m_nJob == JOB_PARSING) str = "Parsing";
			else if (m_nJob == JOB_SCANNING) str = "Scanning";
			else if (m_nJob == JOB_RUNNING) str = "Running";
			if (m_ntotal) {
				CString s; s.Format(_T(" %d/%d (%d%%) @ %d threads, ETA %s"), ndone, m_ntotal, (ndone * 100) / m_ntotal, m_nrunning.Get(), SpanToStr(eta));
				str += s;
				m_ctrProgress.SetWindowText(str);
			}
		} else { // ���� ���� ���̴� �۾��� �����
			CString str;
			m_ctrProgress.GetWindowText(str);
			if (str != "") {
				str = _T("");
				m_ctrProgress.SetWindowText(str);
			}
		}
	} else if (nIDEvent == 1) {
		// �߰��� ������ adding �Ǿ��� ��
		if (m_shown.size() != m_files.size()) {
			if (!m_ctrlTrkList.GetSelCount()) {
				m_shown = m_files; // m_shown �� ������Ʈ
				// ������ ������ sorting ���ؿ� ���� ������ �ؾ��Ѵ�
			}
		}

		if (m_shown.size() != m_ctrlFileList.GetItemCount()) {
			m_ctrlFileList.SetItemCountEx((int)m_shown.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
		}

		// Ʈ�� ����Ʈ ����� ������Ʈ
		BOOL bparsing = (m_nJob == JOB_PARSING);
		if (m_ctrlTrkList.IsWindowEnabled() == bparsing)
			m_ctrlTrkList.EnableWindow(bparsing);

		CString oldstr, newstr;
		m_ctrlFileCnt.GetWindowText(oldstr);
		newstr.Format("%d/%d", m_ctrlFileList.GetSelectedCount(), m_shown.size());
		if (oldstr != newstr) {
			m_ctrlFileCnt.SetWindowText(newstr);
		}

		m_ctrlTrkCnt.GetWindowText(oldstr);
		newstr.Format("%d/%d", m_ctrlTrkList.GetSelCount(), m_ctrlTrkList.GetCount());
		if (oldstr != newstr) {
			m_ctrlTrkCnt.SetWindowText(newstr);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CVitalUtilsDlg::OnBnClickedClear() {
	m_ctrLog.SetWindowText(_T(""));
}

void CVitalUtilsDlg::OnSize(UINT nType, int cx, int cy) {
	CDialogEx::OnSize(nType, cx, cy);

	if (cx == 0 || cy == 0) return;

	if (m_nOldCx == 0 && m_nOldCy == 0) {
		m_nOldCx = cx;
		m_nOldCy = cy;
		return;
	}

	// filelist ���� �Ʒ��� �ִ� �͵��� ����� ����
	if (IsWindow(m_ctrlFileList)) {
		CRect rcw;
		m_ctrlFileList.GetWindowRect(rcw);
		ScreenToClient(rcw);
		int splity = rcw.bottom;

		// ���� ����Ʈ ũ�⸦ Ű��
		m_ctrlFileList.SetWindowPos(nullptr, 0, 0, rcw.Width() + cx - m_nOldCx, rcw.Height() + cy - m_nOldCy, SWP_NOZORDER | SWP_NOMOVE);
		
		// Ʈ�� ����â ũ�⸦ Ű��
		m_ctrlSelTrks.GetWindowRect(rcw);
		m_ctrlSelTrks.SetWindowPos(nullptr, 0, 0, rcw.Width(), rcw.Height() + cy - m_nOldCy, SWP_NOZORDER | SWP_NOMOVE);

		// ���η� ��ġ �̵�
		vector<HWND> children;
		EnumChildWindows(m_hWnd, [](HWND hwnd, LPARAM lParam)->BOOL {
			auto pvec = (vector<HWND>*)lParam;
			pvec->push_back(hwnd);
			return TRUE;
		}, (LPARAM)&children);

		for (auto hchild : children) {
			::GetWindowRect(hchild, &rcw);
			if (rcw.top > splity && ::GetParent(hchild) == m_hWnd) {
				ScreenToClient(rcw);
				::SetWindowPos(hchild, nullptr, rcw.left, rcw.top + cy - m_nOldCy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
		}

		// ũ�� ����
		m_ctrLog.GetWindowRect(rcw);
		m_ctrLog.SetWindowPos(nullptr, 0, 0, rcw.Width() + cx - m_nOldCx, rcw.Height(), SWP_NOZORDER | SWP_NOMOVE);

		m_ctrlFilter.GetWindowRect(rcw);
		m_ctrlFilter.SetWindowPos(nullptr, 0, 0, rcw.Width() + cx - m_nOldCx, rcw.Height(), SWP_NOZORDER | SWP_NOMOVE);

		// ���η� ��ġ �̵�
		m_btnRun.GetWindowRect(rcw);
		ScreenToClient(rcw);
		m_btnRun.SetWindowPos(nullptr, rcw.left + cx - m_nOldCx, rcw.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		m_btnStop.GetWindowRect(rcw);
		ScreenToClient(rcw);
		m_btnStop.SetWindowPos(nullptr, rcw.left + cx - m_nOldCx, rcw.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		m_ctrlFileCnt.GetWindowRect(rcw);
		ScreenToClient(rcw);
		m_ctrlFileCnt.SetWindowPos(nullptr, rcw.left + cx - m_nOldCx, rcw.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		m_ctrlExact.GetWindowRect(rcw);
		ScreenToClient(rcw);
		m_ctrlExact.SetWindowPos(nullptr, rcw.left + cx - m_nOldCx, rcw.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

		m_btnSelect.GetWindowRect(rcw);
		ScreenToClient(rcw);
		m_btnSelect.SetWindowPos(nullptr, rcw.left + cx - m_nOldCx, rcw.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	m_nOldCx = cx;
	m_nOldCy = cy;

	/*CRect rwTrkList;
	m_ctrlTrkList.GetWindowRect(rwTrkList);
	ScreenToClient(rwTrkList);
//	m_ctrlTrkList.SetWindowPos(nullptr, 0, 0, rwTrkList.Width(), cy - rwTrkList.top - 10, SWP_NOZORDER | SWP_NOMOVE);

	CRect rwLog;
	m_ctrLog.GetWindowRect(rwLog);
	ScreenToClient(rwLog);
//	m_ctrLog.SetWindowPos(nullptr, 0, 0, cx - rwLog.left - 10, cy - rwLog.top - 10, SWP_NOZORDER | SWP_NOMOVE);
*/
}

void CVitalUtilsDlg::OnOK() {
}

void CVitalUtilsDlg::OnCancel() {
}

void CVitalUtilsDlg::OnClose() {
	CDialogEx::OnCancel();
}

void CVitalUtilsDlg::OnDestroy() {
	KillTimer(0);
	KillTimer(1);

	// ��� �۾��� ����
	m_bExiting = true;
	OnBnClickedCancel();
	for (int i = 0; i < 16; i++) {
		if (m_thread_worker[i].joinable()) 
			m_thread_worker[i].join();
	}

	// ���� ����� ����
	decltype(m_files) copy;
	{
		unique_lock<shared_mutex> lk(m_mutex_file);
		swap(copy, m_files);
	}
	for (auto& p : copy) delete p;

	CDialogEx::OnDestroy();
}

void CVitalUtilsDlg::OnGetdispinfoFilelist(NMHDR *pNMHDR, LRESULT *pResult) {
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	LVITEM* pItem = &(pDispInfo->item);
	
	VITAL_FILE_INFO* p = nullptr;
	{
		shared_lock<shared_mutex> lk(m_mutex_file);
		if (pItem->iItem >= (int)m_shown.size()) return;
		p = m_shown[pItem->iItem];
	}
	if (!p) return;

	if (pItem->mask & LVIF_TEXT) {
		switch (pItem->iSubItem) {
		case 0: strncpy(pItem->pszText, p->filename.c_str(), pItem->cchTextMax - 1); break;
		case 1: strncpy(pItem->pszText, p->dirname.c_str(), pItem->cchTextMax - 1); break;
		case 2: strncpy(pItem->pszText, substr(dt_to_str(p->mtime), 0, 16).c_str(), pItem->cchTextMax - 1); break;
		case 3: strncpy(pItem->pszText, format_size(p->size).c_str(), pItem->cchTextMax - 1); break;
		case 4: 
			if (p->dtstart) strncpy(pItem->pszText, substr(dt_to_str(p->dtstart), 0, 16).c_str(), pItem->cchTextMax - 1); 
			break;
		case 5: 
			if (p->dtend) strncpy(pItem->pszText, substr(dt_to_str(p->dtend), 0, 16).c_str(), pItem->cchTextMax - 1); 
			break;
		case 6: 
			if (p->dtlen) { 
				auto s = str_format("%.1f", p->dtlen / 3600.0);
				strncpy(pItem->pszText, s.c_str(), pItem->cchTextMax - 1);
			}
			break;
		}
	} 

/*	if (pItem->mask & LVIF_IMAGE) {
		// LVIS_STATEIMAGEMASK�� ���� 15���� ���°��� ������ �� �ִ�
		pItem->iImage = (pItem->state & LVIS_CHECKED)?1:0;
	}
	
	if (pItem->mask & LVIF_STATE) {
		if (m_ctrlFileList.GetItemState(pItem->iItem, LVIS_SELECTED)) {
			pItem->state |= INDEXTOSTATEIMAGEMASK(2);
		} else {
			pItem->state |= INDEXTOSTATEIMAGEMASK(1);
		}
		pItem->stateMask |= LVIS_STATEIMAGEMASK;
	}
	*/
	*pResult = 0;
}

BOOL CVitalUtilsDlg::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->message == WM_KEYDOWN) {
		if (pMsg->wParam == VK_DELETE) {
			if (GetFocus()->m_hWnd == m_ctrlFileList.m_hWnd) {
				if (IDYES == AfxMessageBox("Are you sure to delete these files?", MB_YESNO)) {
					for (int i = m_ctrlFileList.GetItemCount()-1; i >= 0; i--)
						if (m_ctrlFileList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED) {
							if (DeleteFile(m_shown[i]->path.c_str())) {
								theApp.log("deleted " + m_shown[i]->path);
								m_ctrlFileList.DeleteItem(i);
							}
						}
					return TRUE;
				}
			}
		}
		if (::GetKeyState(VK_CONTROL) < 0) {
			switch (pMsg->wParam) {
			case 'A': // ctrl+a
				if (GetFocus()->m_hWnd == m_ctrlFileList.m_hWnd) {
					m_ctrlFileList.SetRedraw(FALSE);
					for (int i = 0; i < m_ctrlFileList.GetItemCount(); i++) {
						m_ctrlFileList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
					}
					m_ctrlFileList.SetRedraw(TRUE);
					m_ctrlFileList.RedrawWindow();
				} else if (GetFocus()->m_hWnd == m_ctrlTrkList.m_hWnd) {
					m_ctrlTrkList.SelItemRange(TRUE, 0, m_ctrlTrkList.GetCount());
					m_ctrlTrkList.Invalidate();
				}
				return TRUE;
			case 'C': // ctrl+c
				if (GetFocus()->m_hWnd == m_ctrlFileList.m_hWnd) {
					vector<string> paths;
					for (auto pos = m_ctrlFileList.GetFirstSelectedItemPosition(); pos;)
						paths.push_back(m_shown[m_ctrlFileList.GetNextSelectedItem(pos)]->path);

					size_t buflen = sizeof(DROPFILES) + 2;
					for (auto path : paths) {
						buflen += path.size() + 1;
					}
					buflen = (buflen / 32 + 1) * 32;

					HGLOBAL hMemFile = ::GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, buflen);
					auto pbuf = (char*)::GlobalLock(hMemFile);
					((DROPFILES*)pbuf)->pFiles = sizeof(DROPFILES);
					pbuf += sizeof(DROPFILES);
					for (auto& path : paths) {
						strcpy(pbuf, path.c_str());
						pbuf += path.size() + 1;
					}
					::GlobalUnlock(hMemFile);

					buflen += paths.size(); // '\r\n'�� �Ұ��̹Ƿ�
					HGLOBAL hMemText = ::GlobalAlloc(GMEM_MOVEABLE, buflen);
					pbuf = (char*)::GlobalLock(hMemText);
					for (auto& s : paths) {
						strcpy(pbuf, s.c_str());
						pbuf += s.size();
						(*pbuf++) = '\r';
						(*pbuf++) = '\n';
					}
					::GlobalUnlock(hMemText);

					OpenClipboard();
					EmptyClipboard();
					SetClipboardData(CF_HDROP, hMemFile);
					SetClipboardData(CF_TEXT, hMemText);
					CloseClipboard();
				}
				else if (GetFocus()->m_hWnd == m_ctrLog.m_hWnd) {
					m_ctrLog.Copy();
				}
				return TRUE;
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CVitalUtilsDlg::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem < (int)m_shown.size()) {
		auto path = m_shown[pNMItemActivate->iItem]->path;
		ShellExecute(NULL, "open", (get_module_dir() + "\\vital.exe").c_str(), ("\"" + path + "\"").c_str(), nullptr, SW_SHOW);
	}
	*pResult = 0;
}

void CVitalUtilsDlg::OnBnClickedTrkNone() {
	m_ctrlTrkList.SelItemRange(FALSE, 0, m_ctrlTrkList.GetCount());
	m_ctrlSelTrks.SetWindowText("");
}

void CVitalUtilsDlg::OnBnClickedFileAll() {
	m_ctrlFileList.SetRedraw(FALSE);
	for (int i = 0; i < m_ctrlFileList.GetItemCount(); i++) {
		m_ctrlFileList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
	m_ctrlFileList.SetRedraw(TRUE);
	m_ctrlFileList.SetFocus();
	m_ctrlFileList.RedrawWindow();
}

void CVitalUtilsDlg::OnBnClickedFileNone() {
	m_ctrlFileList.SetRedraw(FALSE);
	for (auto pos = m_ctrlFileList.GetFirstSelectedItemPosition(); pos;)
		m_ctrlFileList.SetItemState(m_ctrlFileList.GetNextSelectedItem(pos), ~LVIS_SELECTED, LVIS_SELECTED);
	m_ctrlFileList.SetRedraw(TRUE);
	m_ctrlFileList.RedrawWindow();
}

void CVitalUtilsDlg::OnBnClickedSaveList() {
	CString szFilter = _T("Comma-Seperated Values (*.csv)|*.csv|");
	CFileDialog dlg(FALSE, "CSV", NULL, OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilter, this);
	auto result = dlg.DoModal();
	if (result != IDOK) return;

	theApp.log("Saving list");

	auto showns = m_shown;
	vector<string> dtnames;
	for (int i = 0; i < m_ctrlTrkList.GetCount(); i++) {
		CString s; m_ctrlTrkList.GetText(i, s);
		dtnames.push_back((LPCTSTR)s);
	}

	auto filepath = dlg.GetPathName();
	auto fo = fopen(filepath, "wt");
	if (!fo) {
		theApp.log("Saving list failed! " + get_last_error_string());
		return;
	}

	// ����� ���
	fputs("Filename,Path,Size,Start Time,End Time,Length (hr)", fo);
	for (const auto& trkname : dtnames) {
		fputc(',', fo);
		fputstr(trkname, fo);
	}
	fputc('\n', fo);

	for (auto& it : showns) {
		auto tl = explode(m_path_trklist[it->path].second, ',');

		DWORD dtstart = 0;
		DWORD dtend = 0;
		set<string> trk_exists;
		for (const auto& trkname : tl) {
			if (substr(trkname, 0, 9) == "#dtstart=") dtstart = strtoul(substr(trkname, 9).c_str(), nullptr, 10);
			else if (substr(trkname, 0, 7) == "#dtend=") dtend = strtoul(substr(trkname, 7).c_str(), nullptr, 10);
			else trk_exists.insert(trkname);
		}

		fputstr(basename(it->path) + ',', fo);
		fputstr(it->path + ',', fo);
		fprintf(fo, "%zu,", it->size);
		fputstr(dt_to_str(dtstart) + ',', fo);
		fputstr(dt_to_str(dtend) + ',', fo);
		fprintf(fo, "%f", (dtend - dtstart) / 3600.0);
		
		for (const auto& trkname : dtnames) {
			putc(',', fo);
			putc((trk_exists.find(trkname) != trk_exists.end())?'1':'0', fo);
		}
		putc('\n', fo);
	}
	fclose(fo);

	theApp.log("Saving list done");
}

void CVitalUtilsDlg::OnBnClickedSelRunScript() {
	m_dlgRun.ShowWindow(SW_SHOW);
	m_dlgCopy.ShowWindow(SW_HIDE);
	m_dlgRecs.ShowWindow(SW_HIDE);
	m_dlgRename.ShowWindow(SW_HIDE);
	m_dlgDelTrks.ShowWindow(SW_HIDE);
}

void CVitalUtilsDlg::OnBnClickedSelCopyFiles() {
	m_dlgRun.ShowWindow(SW_HIDE);
	m_dlgCopy.ShowWindow(SW_SHOW);
	m_dlgRecs.ShowWindow(SW_HIDE);
	m_dlgRename.ShowWindow(SW_HIDE);
	m_dlgDelTrks.ShowWindow(SW_HIDE);
}

void CVitalUtilsDlg::OnBnClickedSelRecs() {
	m_dlgRun.ShowWindow(SW_HIDE);
	m_dlgCopy.ShowWindow(SW_HIDE);
	m_dlgRecs.ShowWindow(SW_SHOW);
	m_dlgRename.ShowWindow(SW_HIDE);
	m_dlgDelTrks.ShowWindow(SW_HIDE);
}

void CVitalUtilsDlg::OnBnClickedSelRenameDev() {
	m_dlgRun.ShowWindow(SW_HIDE);
	m_dlgCopy.ShowWindow(SW_HIDE);
	m_dlgRecs.ShowWindow(SW_HIDE);
	m_dlgRename.ShowWindow(SW_SHOW);
	m_dlgRename.UpdateForm();
	m_dlgDelTrks.ShowWindow(SW_HIDE);
}

void CVitalUtilsDlg::OnBnClickedSelDelTrks() {
	m_dlgRun.ShowWindow(SW_HIDE);
	m_dlgCopy.ShowWindow(SW_HIDE);
	m_dlgRecs.ShowWindow(SW_HIDE);
	m_dlgRename.ShowWindow(SW_HIDE);
	m_dlgDelTrks.ShowWindow(SW_SHOW);
}

void CVitalUtilsDlg::OnBnClickedSelRenameTrks() {
	m_dlgRun.ShowWindow(SW_HIDE);
	m_dlgCopy.ShowWindow(SW_HIDE);
	m_dlgRecs.ShowWindow(SW_HIDE);
	m_dlgRename.ShowWindow(SW_SHOW);
	m_dlgRename.UpdateForm();
	m_dlgDelTrks.ShowWindow(SW_HIDE);
}

void CVitalUtilsDlg::OnLvnItemchangedFilelist(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	bool shiftpressed = ::GetKeyState(VK_SHIFT) < 0;
	auto iItem = pNMLV->iItem;
	//TRACE("item %d, old %d, new %d\n", iItem, pNMLV->uOldState, pNMLV->uNewState);
	if (iItem == -1 && (pNMLV->uOldState & LVIS_SELECTED) && ((pNMLV->uNewState & LVIS_SELECTED) == 0)) { // deselect all
//		auto iItem = pNMLV->iItem;
//		TRACE("item:%d, old:%x, new:%x\n", iItem, pNMLV->uOldState, pNMLV->uNewState);
//		m_ctrlFileList.SetItemState(m_nLastFileSel, LVIS_SELECTED, LVIS_SELECTED);
		//TRACE("selecting: %d\n", m_nLastFileSel);
	}
	if ((pNMLV->uNewState & LVIS_SELECTED) && ((pNMLV->uOldState & LVIS_SELECTED) == 0)) {// �����̸�?
		m_nLastFileSel = iItem; // ���� ���� ������ ����
		//TRACE("saving lastsel: %d\n", iItem);
	}
	/*
	if (pNMLV->uNewState & LVIS_SELECTED) {// �����̸�?
		if (shiftpressed) {
			if (m_nLastFileSel >= 0 && m_nLastFileSel != iItem) {
				for (int i = min(iItem, m_nLastFileSel); i < max(iItem, m_nLastFileSel); i++) {
					m_ctrlFileList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				}
			}
		} else {
			m_nLastFileSel = iItem; // ���� ���� ������ ����
		}
		*pResult = 1;
		return;
	} 
	
	if ((pNMLV->uOldState & LVIS_SELECTED) && (0 == (pNMLV->uNewState & (LVIS_SELECTED | LVIS_FOCUSED)))) { // focus�� ���µ� deselect �̸�?
		m_ctrlFileList.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED); // �ٽ� ��������
		*pResult = 1;
		return;
	}*/

	// �ƹ��� ���� ����
	*pResult = 0;
}

void CVitalUtilsDlg::OnBnClickedSelect() {
	// ���ϸ� ��Ȯ�� ��ġ
	BOOL exact = m_ctrlExact.GetCheck();
	
	// �˻�� ���� �����͸� ����. ������ �˻������ OR �̴�. �ʿ��ϸ� �� ��Ȯ�� (���� ��� ���ϸ�) �˻�� ������ �ǹǷ�
	CString str; m_ctrlFilter.GetWindowText(str);

	auto lines = explode((LPCTSTR)str, '\n');
	for (auto& line : lines) {
		line = make_lower(trim(line));
	}
	m_ctrlFileList.SetRedraw(FALSE);
	for (int i = 0; i < m_ctrlFileList.GetItemCount(); i++) {
		auto path = make_lower(m_shown[i]->path);
		bool shown = false;
		for (auto& line : lines) {
			if (line.empty()) continue;
			if (!exact) {
				if (path.find(line) != -1) {
					shown = true;
					break;
				}
			} else {
				if (path == line) {
					shown = true;
					break;
				}
				auto filename = basename(path);
				if (filename == line) {
					shown = true;
					break;
				}
				// Ȯ���ڴ� �׻� .vital ��
				string casename = substr(filename, 0, filename.size() - 6);
				if (casename == line) {
					shown = true;
					break;
				}
			}
		}
		m_ctrlFileList.SetItemState(i, shown?LVIS_SELECTED:0, LVIS_SELECTED);
	}
	m_ctrlFileList.SetRedraw(TRUE);
	m_ctrlFileList.RedrawWindow();
	m_ctrlFileList.SetFocus();
}

void CVitalUtilsDlg::OnHdnItemclickFilelist(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	int olditem = m_nSortItem;

	if (m_nSortItem == phdr->iItem) {
		m_bSortAsc = !m_bSortAsc;
	} else {
		m_nSortItem = phdr->iItem;
		m_bSortAsc = true;
	}

	HDITEM hi;
	hi.mask = HDI_FORMAT;
	m_ctrlFileList.GetHeaderCtrl()->GetItem(m_nSortItem, &hi);
	if (m_bSortAsc) {
		hi.fmt |= HDF_SORTUP;
		hi.fmt &= ~HDF_SORTDOWN;
	} else {
		hi.fmt |= HDF_SORTDOWN;
		hi.fmt &= ~HDF_SORTUP;
	}
	m_ctrlFileList.GetHeaderCtrl()->SetItem(m_nSortItem, &hi);

	if (olditem != m_nSortItem) { // ���� �������� ȭ��ǥ�� ����
		m_ctrlFileList.GetHeaderCtrl()->GetItem(olditem, &hi);
		hi.fmt &= ~HDF_SORTDOWN;
		hi.fmt &= ~HDF_SORTUP;
		m_ctrlFileList.GetHeaderCtrl()->SetItem(olditem, &hi);
	}

	m_ctrlFileList.SetRedraw(FALSE);
	switch (m_nSortItem) {
	case 0:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->filename < b->filename; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->filename > b->filename; });
		break;
	case 1:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dirname < b->dirname; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dirname > b->dirname; });
		break;
	case 2:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->mtime < b->mtime; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->mtime > b->mtime; });
		break;
	case 3:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->size < b->size; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->size > b->size; });
		break;
	case 4:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dtstart < b->dtstart; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dtstart > b->dtstart; });
		break;
	case 5:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dtend < b->dtend; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dtend > b->dtend; });
		break;
	case 6:
		if (m_bSortAsc) sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dtlen < b->dtlen; });
		else sort(m_shown.begin(), m_shown.end(), [](const VITAL_FILE_INFO* a, const VITAL_FILE_INFO* b) -> bool { return a->dtlen > b->dtlen; });
		break;
	}
	m_ctrlFileList.SetRedraw(TRUE);
	m_ctrlFileList.Invalidate();

	*pResult = 0;
}

void CVitalUtilsDlg::OnLvnBegindragFilelist(NMHDR *pNMHDR, LRESULT *pResult) {
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// ���õ� ���� ����� �޾ƿ�
	vector<string> paths;
	for(auto pos = m_ctrlFileList.GetFirstSelectedItemPosition(); pos;)
		paths.push_back(m_shown[m_ctrlFileList.GetNextSelectedItem(pos)]->path);

	size_t buflen = sizeof(DROPFILES) + 2;
	for (auto path : paths)
		buflen += path.size() + 1;
	buflen = (buflen / 32 + 1) * 32;

	// Allocate memory from the heap for the DROPFILES struct
	auto hDrop = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, buflen);
	if (!hDrop) return;

	auto pDrop = (DROPFILES*)GlobalLock(hDrop);
	if (!pDrop) {GlobalFree(hDrop); return; }

	// Copy all the filenames into memory after the end of the DROPFILES struct.
	pDrop->pFiles = sizeof(DROPFILES); // offset to the file list
	auto pstr = (char*)pDrop + sizeof(DROPFILES);
	for (auto path : paths) {
		strcpy(pstr, path.c_str());
		pstr += path.size() + 1;
	}
	GlobalUnlock(hDrop);

	COleDataSource datasrc;
	datasrc.CacheGlobalData(CF_HDROP, hDrop);

	auto dwEffect = datasrc.DoDragDrop(DROPEFFECT_COPY | DROPEFFECT_MOVE, NULL);
	if (dwEffect == DROPEFFECT_NONE) {
		GlobalFree(hDrop);
	}

	*pResult = 0;
}

void CVitalUtilsDlg::OnEnKillfocusIdir() {
	auto olddir = m_strIdir;
	UpdateData();
	if (olddir == m_strIdir) return;
	if (!filesystem::is_directory((LPCTSTR)m_strIdir)) {
		AfxMessageBox("Folder not exists!");
		m_ctrIdir.SetFocus();
		m_ctrIdir.SetSel(0, -1);
	} else {
		theApp.WriteProfileString(g_name, _T("idir"), m_strIdir);
		OnBnClickedRescan();
	}
}


void CVitalUtilsDlg::OnEnKillfocusOdir() {
	auto olddir = m_strIdir;
	UpdateData();
	if (olddir == m_strIdir) return;
	if (!filesystem::is_directory((LPCTSTR)m_strOdir)) {
		AfxMessageBox("Folder not exists!");
		m_ctrOdir.SetFocus();
		m_ctrOdir.SetSel(0, -1);
	} else {
		theApp.WriteProfileString(g_name, _T("odir"), m_strOdir);
		OnBnClickedRescan();
	}
}

void CVitalUtilsDlg::OnBnClickedIdirOpen() {
	UpdateData();
	ShellExecute(NULL, "open", m_strIdir, nullptr, nullptr, SW_SHOW);
}

void CVitalUtilsDlg::OnEnChangeTrkFilter() {
	UpdateTrkList();
	
	// �� �ڷ� ��ũ��
	m_ctrlTrkList.SetScrollPos(SB_VERT, -1, 1);
}

void CVitalUtilsDlg::OnBnClickedTrkAll() {
	m_ctrlTrkList.SelItemRange(TRUE, 0, m_ctrlTrkList.GetCount());
	AddSelTrks();
}

void CVitalUtilsDlg::OnBnClickedTrkAdd() {
	AddSelTrks();
}

void CVitalUtilsDlg::AddSelTrks() {
	// Ʈ�� ����Ʈ���� ���� ���õ� Ʈ������ ����â�� �߰���
	CString str; m_ctrlSelTrks.GetWindowText(str);
	str.Trim();
	if (str.GetLength() > 2) str += "\r\n";

	// ���� ���� Ʈ�� ����� ������
	auto seltrks = get_selected_trks();

	// Ʈ�� ����Ʈ�� ���� �׸��� �߰�
	for (int i = 0; i < m_ctrlTrkList.GetCount(); i++) {
		if (!m_ctrlTrkList.GetSel(i)) continue;

		CString s; m_ctrlTrkList.GetText(i, s);

		// �̹� ������ �߰� ����
		if (find(seltrks.begin(), seltrks.end(), (LPCTSTR)s) != seltrks.end()) continue;
		str += s + "\r\n";
	}

	// ��ũ�� �� �ڷ� �̵� �ϸ鼭 �ٿ��ֱ�
	m_ctrlSelTrks.SetSel(0, -1);  //select position after last char in editbox
	m_ctrlSelTrks.ReplaceSel(str); // replace selection with new text
}

void CVitalUtilsDlg::UpdateTrkList() {
	// �˻������ ������
	CString str; m_ctrlTrkFilter.GetWindowText(str);
	auto keywords = explode((LPCTSTR)str, '\n');
	for (auto& keyword : keywords) {
		keyword = make_lower(trim(keyword));
	}

	// �˻�� �ɸ��� �ʴ� Ʈ������ ����
	decltype(m_dtnames) dtnames;
	{
		shared_lock<shared_mutex> lk(m_mutex_dtnames);
		if (keywords.empty()) {
			dtnames = m_dtnames;
		}
		else {
			for (auto& dtname : m_dtnames) {
				bool matched = false;
				for (auto& keyword : keywords) {
					if (keyword.empty()) continue;
					// Ʈ���� keyword�� ��Ī �Ǵ� �͸� �߰��Ѵ�
					if (make_lower(dtname).find(keyword) != -1) {
						matched = true;
						break;
					}
				}
				if (matched) dtnames.insert(dtname);
			}
		}
	}

	// Ʈ�� ����Ʈ ��Ʈ�ѿ� �������� �߰�
	m_ctrlTrkList.SetRedraw(FALSE);
	auto nscr = m_ctrlTrkList.GetTopIndex();
	m_ctrlTrkList.ResetContent();
	for (const auto& s : dtnames) {
		m_ctrlTrkList.AddString(s.c_str());
	}
	m_ctrlTrkList.SetTopIndex(nscr);
	m_ctrlTrkList.SetRedraw(TRUE);
	m_ctrlTrkList.Invalidate();
}

vector<string> CVitalUtilsDlg::get_selected_trks(bool checksel) {
	CString str; 
	m_ctrlSelTrks.GetWindowText(str);
	
	// Ʈ�� ����Ʈ�� ���õ� �׸��� �߰�
	if (str.Trim().IsEmpty() && checksel) { // Ʈ�� ����Ʈ�� ���� �׸��� �߰�
		for (int i = 0; i < m_ctrlTrkList.GetCount(); i++) {
			if (!m_ctrlTrkList.GetSel(i)) continue;
			CString s; m_ctrlTrkList.GetText(i, s);
			str += s + "\r\n";
		}
		// ��ũ�� �� �ڷ� �̵� �ϸ鼭 �ٿ��ֱ�
		m_ctrlSelTrks.SetSel(0, -1);  //select position after last char in editbox
		m_ctrlSelTrks.ReplaceSel(str); // replace selection with new text
	}

	return explode((LPCTSTR)str, "\r\n");
}

void CVitalUtilsDlg::OnLbnDblclkTrklist() {
	OnBnClickedTrkAdd();
}

void CVitalUtilsDlg::OnCbnSelchangeTrkOper() {
	UpdateFileList();
}

void CVitalUtilsDlg::OnEnChangeSelectedTrks() {
	UpdateFileList();
}

void CVitalUtilsDlg::UpdateFileList() {
	CString str; m_ctrlSelOper.GetWindowText(str);
	bool is_or = (str == "OR");

	// ���� ���� Ʈ�� ����� ������
	auto seltrks = get_selected_trks();

	// ������ ���� ����� ������Ʈ
	{
		vector<VITAL_FILE_INFO*> newshown;
		shared_lock<shared_mutex> lk(m_mutex_file);
		if (seltrks.empty()) {
			newshown = m_files;
		}
		else {
			for (auto p : m_files) {
				auto filedevtrk = m_path_trklist[p->path].second;
				if (substr(filedevtrk, 0, 1) != ",") filedevtrk = ',' + filedevtrk;
				if (substr(filedevtrk, filedevtrk.size() - 1) != ",") filedevtrk += ',';

				bool shown = false;
				if (is_or) { // OR: ���õ� Ʈ�� �� �� �ϳ��� ������ ������
					for (const auto& dtname : seltrks) {
						if (filedevtrk.find("," + dtname + ",") != -1) {
							shown = true;
							break;
						}
					}
				} else { // AND: �ϳ��� ������ �Ⱥ���
					shown = true;
					for (const auto& dtname : seltrks) {
						if (filedevtrk.find("," + dtname + ",") == -1) { // Ʈ���� �������� ����
							shown = false;
							break;
						}
					}
				}
				if (shown) newshown.push_back(p);
			}
		}

		m_shown = newshown; // �ڵ����� ������ �ݿ��� ���̴�
	}
	m_ctrlFileList.SetItemCountEx((int)m_shown.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

	// ������ ���� ����� ������Ʈ
	OnBnClickedFileNone();
	//m_ctrlFileList.Invalidate();

	UpdateData(FALSE);
}

void CVitalUtilsDlg::worker_thread_func() {
	while (!m_bExiting) {
		if (m_nJob == JOB_SCANNING) {
			string dir;
			if (m_scans.Pop(dir)) { // ��ĳ�� ����
				m_nrunning.Inc();
				LoadCache(dir);

				WIN32_FIND_DATA fd;
				ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));

				// c++ ǥ������ �ߴٰ� �ð� �������ͼ� ����
				HANDLE hFind = FindFirstFile((dir + "*.*").c_str(), &fd);
				for (BOOL ret = (hFind != INVALID_HANDLE_VALUE); ret; ret = FindNextFile(hFind, &fd)) {
					if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { // directory
						if (strcmp(fd.cFileName, ".") == 0) continue;
						if (strcmp(fd.cFileName, "..") == 0) continue;

						theApp.log("Scanning " + dir + fd.cFileName);
						m_scans.Push(dir + fd.cFileName + "\\");
						m_ntotal++;
					}
					else { // vital ���� ����
						if (extname(fd.cFileName) != "vital") continue;

						auto p = new VITAL_FILE_INFO();
						p->path = dir + fd.cFileName;
						p->filename = fd.cFileName;
						p->dirname = dir; // �����ִ� �������θ� ����Ѵ�.
						p->mtime = filetime_to_unixtime(fd.ftLastWriteTime);
						// 4GB�� �Ѵ� vital ������ ó�� ����
						p->size = (size_t)(fd.nFileSizeLow + fd.nFileSizeHigh * ((ULONGLONG)MAXDWORD + 1));

						{
							unique_lock<shared_mutex> lk(m_mutex_file);
							m_files.push_back(p);
						}

						// ĳ���� ���� ���� ����� m_parses �� ����
						bool hascache = false;
						auto it = m_path_trklist.find(p->path);
						if (it != m_path_trklist.end()) { // ��ΰ� �����ϰ�
							if (it->second.first == p->mtime) {// �ð��� ������
								hascache = true;
							}
						}

						if (!hascache) { // ĳ���� ����. ��¡ ��Ͽ� �߰�
							//TRACE("no cache: " + p->path + '\n');
							m_parses.Push(make_pair(p->mtime, p->path));
							m_ntotal++;
							// ��¡�� ������ �� �����忡�� m_path_trklist �� �Ű� �ְ� ĳ���� ������
						}
					}
				}
				FindClose(hFind);

				m_nrunning.Dec();
			}
			else {
				Sleep(100);
			}
		}
		else if (m_nJob == JOB_PARSING) {
			static time_t tlast = 0;
			time_t tnow = time(nullptr);
			if (!m_cache_updated.empty() && tlast + 30 < tnow) { // ���������� ������ ĳ�� ������ �ְ�, 30�� ��������
				tlast = tnow; // �Ѱ� �����常 ������ ��
				SaveCaches();
			}

			timet_string dstr;
			if (m_parses.Pop(dstr)) { // ��¡ �ؾ��� ������ �ִٸ�
				m_nrunning.Inc();

				auto mtime = dstr.first;
				auto path = dstr.second;
				auto dir = dirname(path);
				auto filename = basename(path);
				auto cmd = "\"" + get_module_dir() + "utilities\\vital_trks.exe\" -s \"" + path + "\"";
				auto res = exec_cmd_get_output(cmd); // �̰� ���� �ɸ�

				// ��¡�� �Ϸ��
				{
					lock_guard<mutex> lk(m_mutex_cache);
					m_path_trklist[path] = make_pair(mtime, res); // m_path_trklist�� �����͸� �߰�
					m_cache_updated.insert(dir); // ���ο� ��¡�� �Ϸ�Ǿ����Ƿ� ĳ���� ������Ʈ �ؾ���
				}

				m_nrunning.Dec();
			}
			else {
				Sleep(100);
			}
		}
		else if (m_nJob == JOB_RUNNING) { // �����ؾ��� ������ �ִٸ�
			string cmd;
			if (m_jobs.Pop(cmd)) {
				m_nrunning.Inc();

				auto jobid = GetThreadId(GetCurrentThread());

				theApp.log(str_format("[%d] %s", jobid, cmd.c_str()));

				time_t tstart = time(nullptr);

				// ���� ���α׷��� ����
				auto dred = cmd.find(">>");
				auto red = cmd.find('>');
				if (dred != -1) { // Longitudinal file
					auto ofile = substr(cmd, dred + 2);
					ofile = trim(ofile, "\t \"");
					cmd = substr(cmd, 0, dred);
					auto s = exec_cmd_get_output(cmd);

					// lock and save the result
					lock_guard<mutex> lk(m_mutex_long);
					auto fa = fopen(ofile.c_str(), "ab"); // append file
					s = rtrim(s, "\r\n");
					fwrite(s.c_str(), 1, s.size(), fa);
					if (!s.empty()) fputs("\r\n", fa);
					fclose(fa);
				}
				else if (red != -1) { // ������ ����
					auto ofile = substr(cmd, red + 1);
					ofile = trim(ofile, "\t \"");
					cmd = substr(cmd, 0, red);
					exec_cmd(cmd, ofile);
				}
				else { // �ܼ� ����
					theApp.log(exec_cmd_get_error(cmd));
				}

				theApp.log(str_format("[%d] finished in %d sec", jobid, (int)difftime(time(nullptr), tstart)));

				m_nrunning.Dec();
			}
			else {
				Sleep(100);
			}
		}
		else {
			Sleep(100);
		}
	}
}

void CVitalUtilsDlg::LoadCache(string dir) {
	// ������ �ִ� trks ������ �о��
	dir = rtrim(dir, '\\');
	auto cachepath = dir + "\\trks.tsv";
	vector<BYTE> buf;
	if (!get_file_contents(cachepath.c_str(), buf)) return;

	// ���ϸ�\t�ð�\tƮ�����
	int icol = 0;
	int num = 0;
	string tabs[3];
	for (auto c : buf) {
		switch (c) {
		case '\t':
			icol++;
			if (icol < 3) tabs[icol] = "";
			break;
		case '\n':
			if (tabs[0].find('\\') == -1) { // old version �� ���� ������ ���ԵǾ��־���. �̰��� ����
				DWORD mtime = strtoul(tabs[1].c_str(), nullptr, 10);
				auto path = dir + '\\' + tabs[0];
				if (!path.empty() && mtime) {
					lock_guard<mutex> lk(m_mutex_cache);
					m_path_trklist[path] = make_pair(mtime, tabs[2]);
					num++;
				}
			}
			icol = 0;
			tabs[0] = "";
			break;
		case '\r':
			break;
		default:
			if (icol < 3) tabs[icol] += c;
		}
	}

	auto str = str_format("Cache Loaded %s (%d records)", dir.c_str(), num);
	theApp.log(str);
}

void CVitalUtilsDlg::SaveCaches() {
	decltype(m_cache_updated) need_updated;
	decltype(m_path_trklist) copyed;

	{
		lock_guard<mutex> lk(m_mutex_cache);
		need_updated = m_cache_updated; // ĳ�� ������Ʈ�� �ʿ��� ������
		m_cache_updated.clear(); // �ٸ� �����忡�� �߰��� �� �ֵ��� ��
		copyed = m_path_trklist;
	}

	for (auto dir : need_updated) {
		theApp.log("Saving tmp file: " + dir);

		string str;
		int num = 0;
		for (const auto& it : copyed) { // �ٸ� �����忡�� ��� �߰����̴�
			if (dirname(it.first) != dir) continue;
			if (it.second.first == 0) continue; // mtime �� 0�̸�
			str += basename(it.first) + '\t';
			str += str_format("%u", it.second.first) + '\t' + it.second.second + '\n';
			num++;
		}

		lock_guard<mutex> lk(m_mutex_cache); // �Ǽ��� �ΰ� �����尡 �������� �ѹ��� ����ǰ� ��

		auto cachefile = dir + "trks.tsv.tmp";
		auto fo = fopen(cachefile.c_str(), "wb");
		if (fo) {
			theApp.log(str_format("Cache file saved: %s (%d records)", dir.c_str(), num));
			fwrite(str.c_str(), 1, str.size(), fo);
			fclose(fo);

			// hidden file�� ����
			auto newfile = substr(cachefile, 0, cachefile.size() - 4);
			//SetFileAttributes(cachefile.c_str(), GetFileAttributes(cachefile.c_str()) | FILE_ATTRIBUTE_HIDDEN);
			MoveFileEx(cachefile.c_str(), newfile.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
		}
		else {
			theApp.log(str_format("Cache file open failed: %s (%d records)", dir.c_str(), num));
		}
	}
}
