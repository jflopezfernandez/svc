/** Windows Service Test Application
 *  Author: Jose Lopez
 *  Date:	17-June-2017
 *
 *  A >lot< of help and source code came from Microsoft's Developer Network
 *  website.
 *
 *  Source: (https://msdn.microsoft.com/en-us/library/ms687416(v=vs.85).aspx)
 *
 */

#include "Main.hpp"


int __cdecl _tmain(int argc, TCHAR *argv[])
{
	// If command line parameter is "install", install the service.
	// Otherwise, the service is probably being started by the SCM.

	if (lstrcmpi(argv[1], TEXT("install")) == 0) {
		svcInstall();

		return EXIT_SUCCESS;
	}

	// TODO: Add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION) svcMain },
		{ NULL, NULL }
	};

	// This call returns when the service has stopped.
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher(DispatchTable)) {
		svcReportEvent(TEXT("StartSergiceCtrlDispatcher"));
	}

	return EXIT_SUCCESS;
}