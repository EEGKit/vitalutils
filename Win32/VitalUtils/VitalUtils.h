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
#include <mutex>
#include "util.h"

using namespace std;

typedef pair<time_t, string> time_msg;
typedef tuple<time_t, size_t, string> mtime_filesize_path;
typedef tuple<time_t, size_t, string> mtime_filesize_trklist;

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
	
	Queue<time_msg> m_msgs;

	void log(string msg);
	bool install_python();
};

extern CVitalUtilsApp theApp;
