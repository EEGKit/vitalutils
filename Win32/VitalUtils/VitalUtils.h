#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include <list>
#include <set>
#include <map>
#include <vector>
#include "QUEUE.h"
#include <thread>
using namespace std;

struct VITAL_FILE_INFO {
	CString filename;
	CString dirname;
	CString path;
	DWORD mtime;
	size_t size;
};

CString GetLastErrorString();
vector<CString> Explode(CString str, TCHAR sep);
CString ExtName(CString path);
CString DirName(CString path);
void ListFiles(LPCTSTR path, vector<CString>& files, CString ext);
CString BaseName(CString path);
CString GetModulePath();
CString GetModuleDir();
DWORD FileTimeToUnixTime(const FILETIME &ft);
bool FileExists(CString path);
void CreateDir(CString path);

typedef pair<DWORD, CString> DWORD_CString;

class CVitalUtilsApp : public CWinApp {
public:
	CVitalUtilsApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

public:
	DECLARE_MESSAGE_MAP()

public:
	CString m_ver;
	time_t m_dtstart = 0;
	bool m_bparsing = false; // ������ ��¡ ������?
	int m_ntotal = 0; // ���� �������� �۾��� �� ��
	
	struct ThreadCounter {
		ThreadCounter() {
			InitializeCriticalSection(&cs);
		}
		~ThreadCounter() {
			DeleteCriticalSection(&cs);
		}
		void Inc() {
			EnterCriticalSection(&cs);
			cnt++;
			LeaveCriticalSection(&cs);
		}
		void Dec() {
			EnterCriticalSection(&cs);
			cnt--;
			LeaveCriticalSection(&cs);
		}
		int Get() {
			EnterCriticalSection(&cs);
			int ret = cnt;
			LeaveCriticalSection(&cs);
			return ret;
		}
		CRITICAL_SECTION cs;
		int cnt = 0;
	} m_nrunning; // ���� �������� �������� �� ��
	
	vector<VITAL_FILE_INFO*> m_files;
	set<CString> m_devtrks;
	Queue<pair<DWORD, CString>> m_msgs;
	Queue<CString> m_jobs;

	Queue<DWORD_CString> m_parses;
	map<CString, DWORD_CString> m_path_trklist; // ���⿡ Ʈ�� �����Ͱ� ����
	
	CRITICAL_SECTION m_csTrk;
	CRITICAL_SECTION m_csFile;
	CRITICAL_SECTION m_csLong;

	void Log(CString msg);
	void AddJob(CString cmd);
	virtual int ExitInstance();
	void SaveCache(CString path);
};

extern CVitalUtilsApp theApp;
