#include"fn.h"

BOOL __stdcall StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService);

RetInfo GetLastErrorString(RetInfo stPreviousInfo , DWORD dwError) 
{
    RetInfo stRetInfo = {0};
    if (dwError == 0) {
        stRetInfo.retinfo = "Error Not Found";
        stRetInfo.status = STATUS_FAILED;
        return stRetInfo;
    }

    LPVOID lpMsgBuf;
    DWORD dwBufferLength = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0,
        NULL
    );

    if (dwBufferLength > 0) {
        CString errorString = (LPCTSTR)lpMsgBuf; 
        LocalFree(lpMsgBuf); // ÊÍ·Å·ÖÅäµÄ»º³åÇø

        stRetInfo.retinfo = stPreviousInfo.retinfo + errorString;
        stRetInfo.status = STATUS_SUCCESS;
        return stRetInfo;
    }
    else {
        stRetInfo.retinfo = " No Found Msg" + dwError;
        stRetInfo.status = STATUS_FAILED;
        return stRetInfo;
    }
}


RetInfo SvcInstall(TCHAR szPath[MAX_PATH],WCHAR* szSvcName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    RetInfo pretInfo;

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        pretInfo.retinfo = "OpenSCManager´íÎó£º";
        DWORD test = GetLastError();
        pretInfo=GetLastErrorString(pretInfo,GetLastError());
        pretInfo.status = STATUS_FAILED;   
        return pretInfo;
    }

    // Create the service

    schService = CreateService(
        schSCManager,              // SCM database 
        szSvcName,                 // name of service 
        szSvcName,                 // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_KERNEL_DRIVER,     // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        pretInfo.retinfo = "CreateService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;   //Note:We must do this last
        return pretInfo;
    }
    else {
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        pretInfo.status = STATUS_SUCCESS;
        pretInfo.retinfo = "³É¹¦°²×°£º";
        return pretInfo;
    }
}

RetInfo DoStartSvc(WCHAR* szSvcName)
{
    RetInfo pretInfo;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // servicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        pretInfo.retinfo = "OpenSCManager´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        return pretInfo;
    }

    // Get a handle to the service.

    schService = OpenService(
        schSCManager,         // SCM database 
        szSvcName,            // name of service 
        SERVICE_ALL_ACCESS);  // full access 

    if (schService == NULL)
    {
        pretInfo.retinfo = "OpenService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    // Check the status in case the service is not stopped. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // information level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded))              // size needed if buffer is too small
    {
        pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
       
        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }


    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
    {
        pretInfo.retinfo = "Çý¶¯ÕýÔÚÔËÐÐ\n ´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        
        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    // Wait for the service to stop before attempting to start it.

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status until the service is no longer stop pending. 

        if (!QueryServiceStatusEx(
            schService,                     // handle to service 
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))              // size needed if buffer is too small
        {
            pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
            pretInfo = GetLastErrorString(pretInfo, GetLastError());
            pretInfo.status = STATUS_FAILED;
            
            CloseServiceHandle(schSCManager);
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return pretInfo;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                pretInfo.retinfo = "³¬Ê±´íÎó£º";
                pretInfo = GetLastErrorString(pretInfo, GetLastError());
                pretInfo.status = STATUS_FAILED;
                
                CloseServiceHandle(schSCManager);
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return pretInfo;
            }
        }
    }

    // Attempt to start the service.

    if (!StartService(
        schService,  // handle to service 
        0,           // number of arguments 
        NULL))      // no arguments 
    {
        pretInfo.retinfo = "StartService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        
        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    // Check the status until the service is no longer start pending. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // info level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded))              // if buffer too small
    {
        pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        
        CloseServiceHandle(schSCManager);
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
    {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status again. 

        if (!QueryServiceStatusEx(
            schService,             // handle to service 
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded))              // if buffer too small
        {
            pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
            pretInfo = GetLastErrorString(pretInfo, GetLastError());
            pretInfo.status = STATUS_FAILED;
            
            CloseServiceHandle(schSCManager);
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint)
        {
            // Continue to wait and check.

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        }
        else
        {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
            {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    // Determine whether the service is running.

    if (ssStatus.dwCurrentState == SERVICE_RUNNING)
    {
        pretInfo.retinfo = "¼ÓÔØ³É¹¦£º";
        pretInfo.status = STATUS_SUCCESS;
        
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }
    else
    {
        pretInfo.status = STATUS_FAILED;
        pretInfo.retinfo.Format(_T("%s"), ssStatus.dwCurrentState);
        pretInfo.retinfo += "×´¿ö";
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}




RetInfo DoStopSvc(WCHAR* szSvcName)
{
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out
    DWORD dwWaitTime;
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    RetInfo pretInfo;

    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        pretInfo.retinfo = "OpenSCManager´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;   
        return pretInfo;
     }

    // Get a handle to the service.

    schService = OpenService(
        schSCManager,         // SCM database 
        szSvcName,            // name of service 
        SERVICE_STOP |
        SERVICE_QUERY_STATUS |
        SERVICE_ENUMERATE_DEPENDENTS);

    if (schService == NULL)
    {
        pretInfo.retinfo = "OpenService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    // Make sure the service is not already stopped.

    if (!QueryServiceStatusEx(
        schService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded))
    {
        pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        goto stop_cleanup;  // Remember to return.
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED)
    {
        pretInfo.retinfo = "ÒÑÍ£Ö¹Çý¶¯";
        
        pretInfo.status = STATUS_SUCCESS;
        goto stop_cleanup;
    }

    // If a stop is pending, wait for it.

    while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
    {
        
        dwWaitTime = ssp.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
            pretInfo = GetLastErrorString(pretInfo, GetLastError());
            pretInfo.status = STATUS_FAILED;
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
        {
            pretInfo.retinfo = "³É¹¦Í£Ö¹Çý¶¯£º";
            pretInfo.status = STATUS_SUCCESS;
            goto stop_cleanup;
        }

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            pretInfo.retinfo = "³¬Ê±´íÎó£º";
            pretInfo = GetLastErrorString(pretInfo, GetLastError());
            pretInfo.status = STATUS_FAILED;
            goto stop_cleanup;
        }
    }

    // If the service is running, dependencies must be stopped first.

    if(!StopDependentServices(schSCManager, schService)) {
        pretInfo.retinfo = "StopDependentServices´íÎó";
        pretInfo.status = STATUS_SUCCESS;
    }

    // Send a stop code to the service.

    if (!ControlService(
        schService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&ssp))
    {
        pretInfo.retinfo = "ControlService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        goto stop_cleanup;
    }

    // Wait for the service to stop.

    while (ssp.dwCurrentState != SERVICE_STOPPED)
    {
        Sleep(ssp.dwWaitHint);
        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded))
        {
            pretInfo.retinfo = "QueryServiceStatusEx´íÎó£º";
            pretInfo = GetLastErrorString(pretInfo, GetLastError());
            pretInfo.status = STATUS_FAILED;
            goto stop_cleanup;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout)
        {
            pretInfo.retinfo = "³¬Ê±´íÎó£º";
            pretInfo = GetLastErrorString(pretInfo, GetLastError());
            pretInfo.status = STATUS_FAILED;
            goto stop_cleanup;
        }
    }
    pretInfo.retinfo = "³É¹¦Í£Ö¹Çý¶¯£º";
    pretInfo.status = STATUS_SUCCESS;

stop_cleanup:
    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return pretInfo;
}

BOOL __stdcall StopDependentServices(SC_HANDLE schSCManager, SC_HANDLE schService)
{
    DWORD i;
    DWORD dwBytesNeeded;
    DWORD dwCount;

    LPENUM_SERVICE_STATUS   lpDependencies = NULL;
    ENUM_SERVICE_STATUS     ess;
    SC_HANDLE               hDepService;
    SERVICE_STATUS_PROCESS  ssp;

    DWORD dwStartTime = GetTickCount();
    DWORD dwTimeout = 30000; // 30-second time-out

    
    // Pass a zero-length buffer to get the required buffer size.
    if (EnumDependentServices(schService, SERVICE_ACTIVE,
        lpDependencies, 0, &dwBytesNeeded, &dwCount))
    {
        // If the Enum call succeeds, then there are no dependent
        // services, so do nothing.
        return TRUE;
    }
    else
    {
        if (GetLastError() != ERROR_MORE_DATA)
            return FALSE; // Unexpected error

        // Allocate a buffer for the dependencies.
        lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
            GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

        if (!lpDependencies)
            return FALSE;

        __try {
            // Enumerate the dependencies.
            if (!EnumDependentServices(schService, SERVICE_ACTIVE,
                lpDependencies, dwBytesNeeded, &dwBytesNeeded,
                &dwCount))
                return FALSE;

            for (i = 0; i < dwCount; i++)
            {
                ess = *(lpDependencies + i);
                // Open the service.
                hDepService = OpenService(schSCManager,
                    ess.lpServiceName,
                    SERVICE_STOP | SERVICE_QUERY_STATUS);

                if (!hDepService)
                    return FALSE;

                __try {
                    // Send a stop code.
                    if (!ControlService(hDepService,
                        SERVICE_CONTROL_STOP,
                        (LPSERVICE_STATUS)&ssp))
                        return FALSE;

                    // Wait for the service to stop.
                    while (ssp.dwCurrentState != SERVICE_STOPPED)
                    {
                        Sleep(ssp.dwWaitHint);
                        if (!QueryServiceStatusEx(
                            hDepService,
                            SC_STATUS_PROCESS_INFO,
                            (LPBYTE)&ssp,
                            sizeof(SERVICE_STATUS_PROCESS),
                            &dwBytesNeeded))
                            return FALSE;

                        if (ssp.dwCurrentState == SERVICE_STOPPED)
                            break;

                        if (GetTickCount() - dwStartTime > dwTimeout)
                            return FALSE;
                    }
                }
                __finally
                {
                    // Always release the service handle.
                    CloseServiceHandle(hDepService);
                }
            }
        }
        __finally
        {
            // Always free the enumeration buffer.
            HeapFree(GetProcessHeap(), 0, lpDependencies);
        }
    }
    return TRUE;
}

RetInfo DoDeleteSvc(WCHAR* szSvcName)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    RetInfo pretInfo;
    // Get a handle to the SCM database. 

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        pretInfo.retinfo = "OpenSCManager´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        return pretInfo;
    }

    // Get a handle to the service.

    schService = OpenService(
        schSCManager,       // SCM database 
        szSvcName,          // name of service 
        DELETE);            // need delete access 

    if (schService == NULL)
    {
        pretInfo.retinfo = "OpenService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        
        CloseServiceHandle(schSCManager);
        return pretInfo;
    }

    // Delete the service.

    if (!DeleteService(schService))
    {
        pretInfo.retinfo = "DeleteService´íÎó£º";
        pretInfo = GetLastErrorString(pretInfo, GetLastError());
        pretInfo.status = STATUS_FAILED;
        return pretInfo;
    }
    else {
        pretInfo.retinfo = "³É¹¦É¾³ýÇý¶¯·þÎñ";
        pretInfo.status = STATUS_SUCCESS;
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return pretInfo;
}