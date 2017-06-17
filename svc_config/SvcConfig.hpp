#pragma once

VOID WINAPI DisplayUsage();

VOID WINAPI DoQuerySvc(const TCHAR *svcName);
VOID WINAPI DoUpdateSvcDesc();
VOID WINAPI DoDisableSvc();
VOID WINAPI DoEnableSvc();
VOID WINAPI DoDeleteSvc();

