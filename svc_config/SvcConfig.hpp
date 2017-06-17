#pragma once

VOID WINAPI DisplayUsage();

VOID WINAPI DoQuerySvc(const TCHAR *svcName);
VOID WINAPI DoUpdateSvcDesc(const TCHAR *svcName);
VOID WINAPI DoDisableSvc(const TCHAR *svcName);
VOID WINAPI DoEnableSvc(const TCHAR *svcName);
VOID WINAPI DoDeleteSvc(const TCHAR *svcName);

