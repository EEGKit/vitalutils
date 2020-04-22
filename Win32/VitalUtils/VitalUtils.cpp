#include "stdafx.h"
#include "VitalUtils.h"
#include "VitalUtilsDlg.h"
#include <time.h>
#include <thread>
#include <cctype>
#include <algorithm>
#include "dlgdownload.h"
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

string dt_to_str(time_t t) {
	tm* ptm = localtime(&t); // simple conversion
	SYSTEMTIME st;
	st.wYear = (WORD)(1900 + ptm->tm_year);
	st.wMonth = (WORD)(1 + ptm->tm_mon);
	st.wDayOfWeek = (WORD)ptm->tm_wday;
	st.wDay = (WORD)ptm->tm_mday;
	st.wHour = (WORD)ptm->tm_hour;
	st.wMinute = (WORD)ptm->tm_min;
	st.wSecond = (WORD)ptm->tm_sec;
	return str_format("%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

// create dir recursively
string get_module_path() {
	char temp_path[MAX_PATH];
	GetModuleFileName(AfxGetInstanceHandle(), temp_path, sizeof(temp_path));
	return temp_path;
}

string get_module_dir() {
	return dirname(get_module_path());
}

string get_last_error_string() {
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) return ""; //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

// output�� error�� ��� ����
string exec_cmd_get_error(string cmd) {
	HANDLE hPipeRead, hPipeWrite;

	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE;   //Pipe handles are inherited by child process.
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe to get results from child's stdout.
	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
		return "";

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	si.hStdError = hPipeWrite;
	si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

	PROCESS_INFORMATION pi = { 0 };

	BOOL fSuccess = CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!fSuccess) {
		theApp.log(cmd + " " + get_last_error_string());
		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);
		return "";
	}

	string ret;
	bool bProcessEnded = false;
	for (; !bProcessEnded;) {
		// Give some timeslice (50ms), so we won't waste 100% cpu.
		bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

		// Even if process exited - we continue reading, if there is some data available over pipe.
		for (;;) {
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;
			if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
				break;

			if (!dwAvail) // no data available, return
				break;

			if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				// error, the child process might ended
				break;

			ret.append(buf, dwRead);
		}
	} //for

	CloseHandle(hPipeWrite);
	CloseHandle(hPipeRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return ret;
}

string exec_cmd_get_output(string cmd) {
	string strResult;
	HANDLE hPipeRead, hPipeWrite;

	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE;   //Pipe handles are inherited by child process.
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe to get results from child's stdout.
	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0))
		return strResult;

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite;
	si.hStdError = NULL;
	si.wShowWindow = SW_HIDE;       // Prevents cmd window from flashing. Requires STARTF_USESHOWWINDOW in dwFlags.

	PROCESS_INFORMATION pi = { 0 };

	BOOL fSuccess = CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!fSuccess) {
		theApp.log(cmd + " " + get_last_error_string());
		CloseHandle(hPipeWrite);
		CloseHandle(hPipeRead);
		return strResult;
	}

	bool bProcessEnded = false;
	for (; !bProcessEnded;) {
		// Give some timeslice (50ms), so we won't waste 100% cpu.
		bProcessEnded = WaitForSingleObject(pi.hProcess, 50) == WAIT_OBJECT_0;

		// Even if process exited - we continue reading, if there is some data available over pipe.
		for (;;) {
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;
			if (!::PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwAvail, NULL))
				break;

			if (!dwAvail) // no data available, return
				break;

			if (!::ReadFile(hPipeRead, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				// error, the child process might ended
				break;

			buf[dwRead] = 0;
			strResult += buf;
		}
	} //for

	CloseHandle(hPipeWrite);
	CloseHandle(hPipeRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return strResult;
}

// pipe �� ������ �ϰ� ��� ����� ofile �� ������
bool exec_cmd(string cmdLine, string ofile) {
	auto jobid = GetThreadId(GetCurrentThread());

	// ���� �ڵ� https://stackoverflow.com/questions/7018228/how-do-i-redirect-output-to-a-file-with-createprocess
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// �������� ����. // temp file �� ����.
	bool ret = false;
	HANDLE fo = CreateFile(ofile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fo == INVALID_HANDLE_VALUE) {
		theApp.log(str_format("[%d] cannot open file", jobid));
		return false;
	}

	// ������ ����� �������� ����
	// https://msdn.microsoft.com/ko-kr/library/windows/desktop/ms682499(v=vs.85).aspx
	HANDLE stder, stdew;
	if (!CreatePipe(&stder, &stdew, &sa, 0)) {
		theApp.log("cannot create pipe");
		return false;
	}
	if (!SetHandleInformation(stder, HANDLE_FLAG_INHERIT, 0)) {
		theApp.log("set handle information");
		return false;
	}

	STARTUPINFO si = {};
	si.cb = sizeof(si);
	si.wShowWindow = SW_HIDE;
	si.hStdOutput = fo;
	si.hStdError = stdew;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi = {};

	if (!CreateProcess(NULL, (LPSTR)cmdLine.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		theApp.log("createprocess error " + get_last_error_string());
		goto clean;
	}
	
	char buf[4096];
	while (1) {
		if (WAIT_TIMEOUT != WaitForSingleObject(pi.hProcess, 500)) break;
		for (DWORD dwAvail = 0; PeekNamedPipe(stder, 0, 0, 0, &dwAvail, 0) && dwAvail; dwAvail = 0) {
			DWORD dwRead = 0;
			ReadFile(stder, buf, min(4095, dwAvail), &dwRead, 0);
			theApp.log(string(buf, dwRead));
		}
	}
	// ���μ��� ����
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	ret = true;

clean:
	if (fo) CloseHandle(fo);

	if (stder) CloseHandle(stder);
	if (stdew) CloseHandle(stdew); // �������ٰ� �츮�� �� �� ����

	return ret;
}

string substr(const string& s, size_t pos, size_t len) {
	try {
		return s.substr(pos, len);
	}
	catch (...) {
	}
	return "";
}

string make_lower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	return s;
}

string extname(string path) {
	auto pos = path.find_last_of('.');
	if (pos == string::npos) return "";
	return make_lower(substr(path, pos + 1));
}

string dirname(string path) {
	return substr(path, 0, path.find_last_of("/\\") + 1);
}

string basename(string path) {
	return substr(path, path.find_last_of("/\\") + 1);
}

time_t filetime_to_unixtime(const FILETIME &ft) {
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return (time_t)(ull.QuadPart / 10000000ULL - 11644473600ULL);
}

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

	if ( !file_exists(get_module_dir() + "utilities\\vital_trks.exe") || 
		(!file_exists(get_module_dir() + "utilities\\vital_recs.exe") && !file_exists(get_module_dir() + "utilities\\vital_recs_x64.exe"))) {
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

string replace_all(string res, const string& pattern, const string& replace) {
	string::size_type pos = 0;
	string::size_type offset = 0;
	while ((pos = res.find(pattern, offset)) != string::npos) {
		res.replace(res.begin() + pos, res.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}
	return res;
}

void CVitalUtilsApp::log(string msg) {
	// ���뱸 ����
	auto trivial = get_module_dir();
	msg = replace_all(msg, trivial, "");
	if (msg.empty()) return;
	
	m_msgs.Push(make_pair((DWORD)time(nullptr), msg));
}

vector<string> explode(string str, string sep) {
	vector<string> ret;
	if (str.empty()) return ret;
	if (str.size() < sep.size()) str += sep;
	else if (substr(str, str.size() - sep.size()) != sep) str += sep;

	for (size_t i = 0, j = 0; (j = str.find(sep, i)) != string::npos; i = j + sep.size()) {
		ret.push_back(substr(str, i, j - i));
	}
	if (ret.empty()) {
		ret.push_back(str);
	}
	return ret;
}

vector<string> explode(const string& s, const char c) {
	string buf;
	vector<string> ret;
	for (auto n : s) {
		if (n != c)
			buf += n;
		else if (n == c) {
			ret.push_back(buf);
			buf.clear();
		}
	}
	if (!buf.empty()) ret.push_back(buf);
	return ret;
}

void CVitalUtilsApp::InstallPython() {
	char tmpdir[MAX_PATH];
	GetTempPath(MAX_PATH, tmpdir);

	CString strSetupUrl = "https:/""/vitaldb.net/python_setup.exe";
	CString tmppath; tmppath.Format("%spysetup_%u.exe", tmpdir, time(nullptr));
	CDlgDownload dlg(nullptr, strSetupUrl, tmppath);
	if (IDOK != dlg.DoModal()) {
		AfxMessageBox("Cannot download python setup file.\nPlease download it from " + strSetupUrl + "\nand run it in the Vital Recorder installation folder");
		return;
	}

	SHELLEXECUTEINFO shExInfo = { 0 };
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = 0;
	shExInfo.lpVerb = _T("runas"); // Operation to perform
	shExInfo.lpFile = tmppath; // Application to start    
	shExInfo.lpParameters = ""; // Additional parameters
	shExInfo.lpDirectory = get_module_dir().c_str(); // ���� ���α׷� ���丮�� ������ Ǯ�����
	shExInfo.nShow = SW_SHOW;
	shExInfo.hInstApp = 0;

	if (!ShellExecuteEx(&shExInfo)) {
		AfxMessageBox("cannot start installer");
		return;
	}

	if (WAIT_OBJECT_0 != WaitForSingleObject(shExInfo.hProcess, INFINITE)) {
		AfxMessageBox("cannot install python");
		return;
	}

	CloseHandle(shExInfo.hProcess);

	theApp.log("Python installed");
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

