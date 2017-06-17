#ifndef _SERVICE_MAIN_HPP
#define _SERVICE_MAIN_HPP


#define SVCNAME TEXT("svcName")


VOID svcInstall();

VOID WINAPI svcCtrlHandler(DWORD);
VOID WINAPI svcMain(DWORD, LPTSTR *);

VOID reportSvcStatus(DWORD, DWORD, DWORD);
VOID svcInit(DWORD, LPTSTR *);
VOID svcReportEvent(LPTSTR);



#endif /* _SERVICE_MAIN_HPP */