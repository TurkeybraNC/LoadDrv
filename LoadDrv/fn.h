#ifndef _FN_H_
#define _FN_H_

#include<Windows.h>
#include<winsvc.h>
#include<stdio.h>
#include <tchar.h>
#include <atlstr.h>
#include<stdlib.h>
#include<string>
#include<iostream>


#define STATUS_FAILED 1
#define STATUS_SUCCESS 0


struct RetInfo {
	INT status;
	CString retinfo;
};

RetInfo GetLastErrorString(RetInfo stPreviousInfo, DWORD dwError);
RetInfo SvcInstall(TCHAR szPath[MAX_PATH], WCHAR* szSvcName);
RetInfo DoStartSvc(WCHAR* szSvcName);
RetInfo DoStopSvc(WCHAR* szSvcName);
RetInfo DoDeleteSvc(WCHAR* szSvcName);
#endif