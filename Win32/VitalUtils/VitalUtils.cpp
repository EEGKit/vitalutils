#include "stdafx.h"
#include "VitalUtils.h"
#include "VitalUtilsDlg.h"
#include <time.h>
#include <thread>
#include <cctype>
#include <algorithm>
#include "dlgdownload.h"
#include "dlgunzip.h"
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;

#pragma comment(lib, "Version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CVitalUtilsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CVitalUtilsApp::CVitalUtilsApp() {
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

CVitalUtilsApp theApp;

BOOL CVitalUtilsApp::InitInstance() {
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if ( !fs::exists(get_module_dir() + "utilities\\vital_trks.exe") || 
		(!fs::exists(get_module_dir() + "utilities\\vital_recs.exe") && !fs::exists(get_module_dir() + "utilities\\vital_recs_x64.exe"))) {
		AfxMessageBox(_T("vital_recs.exe and vital_trks.exe should be exist in utilities folder"));
		return FALSE;
	}

	// initialize winsock
	WSADATA wd;
	if (WSAStartup(MAKEWORD(2, 2), &wd) != 0) return FALSE;

	AfxOleInit();
	AfxEnableControlContainer(); //���� ������Ʈ���� ActiveX��Ʈ���� ����� �� �ְ� ����
	AfxInitRichEdit();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// MFC ��Ʈ���� �׸��� ����ϱ� ���� "Windows ����" ���־� ������ Ȱ��ȭ
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("VitalUtils"));

	CVitalUtilsDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	} else if (nResponse == IDCANCEL) {
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	} else if (nResponse == -1) {
	}

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL) {
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

void CVitalUtilsApp::log(string msg) {
	// ���뱸 ����
	auto trivial = get_module_dir();
	msg = replace_all(msg, trivial, "");
	if (msg.empty()) return;
	
	m_msgs.Push(make_pair((DWORD)time(nullptr), msg));
}

bool CVitalUtilsApp::install_python() {
	auto python_path = get_python_path();

	// C:\Users\lucid\AppData\Roaming\VitalRecorder\python
	string odir = dirname(python_path);

	// download setup program to tmp dir
	error_code ec;
	auto tmpdir = fs::temp_directory_path(ec);
	auto localpath = tmpdir.string() + str_format("python_%u.zip", time(nullptr));
	CDlgDownload dlg_download("vitaldb.net:443", "/python.zip", localpath);
	if (IDOK != dlg_download.DoModal()) {
		AfxMessageBox("Download error");
		return false;
	}
	if (fs::file_size(localpath, ec) < 1000 * 1000) {  // �뷮�� �ʹ� ������ ������ Ȯ��
		TRACE("python < 1MB\n");
		return false;
	}

	//string localpath = "C:\\Users\\lucid\\OneDrive\\Desktop\\python_1646734847.zip";

	// �⺻ ��ġ ����
	CDlgUnzip dlg_unzip(localpath, odir);
	if (IDOK != dlg_unzip.DoModal()) return false;

	AfxMessageBox("filter server installed successfully!");
	return true;
}

bool get_file_contents(LPCTSTR path, vector<BYTE>& ret) {
	auto f = fopen(path, "rb");
	if (!f) return false;
	fseek(f, 0, SEEK_END);
	auto len = ftell(f);
	rewind(f);
	ret.resize(len);
	fread(&ret[0], 1, len, f);
	fclose(f);
	return true;
}

string ltrim(string s, char c) {
	return s.erase(0, s.find_first_not_of(c));
}

// trim from end (in place)
string rtrim(string s, char c) {
	return s.erase(s.find_last_not_of(c) + 1);
}

// trim from both ends (in place)
string trim(string s, char c) { return rtrim(ltrim(s, c), c); }

string ltrim(string s, const char* c) {
	return s.erase(0, s.find_first_not_of(c));
}

// trim from end (in place)
string rtrim(string s, const char* c) {
	return s.erase(s.find_last_not_of(c) + 1);
}

// trim from both ends (in place)
string trim(string s, const char* c) { return rtrim(ltrim(s, c), c); }

