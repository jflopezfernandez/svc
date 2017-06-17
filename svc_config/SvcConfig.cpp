
#include "Main.hpp"


VOID WINAPI DisplayUsage()
{
	printf("Description: \n");
	printf("\tCommand-line tool that configures a service.\n\n");
	printf("Usage:\n");
	printf("\tsvcconfig [command] [service_name]\n\n");
	printf("\t[command]\n");
	printf("\t  query\n");
	printf("\t  describe\n");
	printf("\t  disable\n");
	printf("\t  enable\n");
	printf("\t  delete\n");
}



/** Purpose:
 *		Retrieves and displays the current service configuration.
 *
 *	Parameters:
 *		none
 *
 *	Return value:
 *		none
 *
 */

VOID WINAPI DoQuerySvc(const TCHAR *svcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	LPQUERY_SERVICE_CONFIG	lpsc;
	LPSERVICE_DESCRIPTION	lpsd;

	DWORD	dwBytesNeeded,
			cbBuffSize,
			dwError;


	// Get a handle to the SCM database.
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ServicesActive database
		SC_MANAGER_ALL_ACCESS			// Full access rights
	);

	if (schSCManager == NULL) {
		printf("OpenSCManager failed (%d) \n", GetLastError());

		return;
	}


	// Get a handle to the service.
	schService = OpenService(
		schSCManager,					// SCM database
		svcName,						// Name of service
		SERVICE_QUERY_CONFIG			// Need query config access
	);

	if (schService == NULL) {
		printf("OpenService failed (%d) \n", GetLastError());
		CloseServiceHandle(schSCManager);

		return;
	}


	// Get the configuration information
	if (!QueryServiceConfig(
		schService,
		NULL,
		0,
		&dwBytesNeeded)) {

		dwError = GetLastError();

		if (dwError == ERROR_INSUFFICIENT_BUFFER) {
			cbBuffSize = dwBytesNeeded;
			lpsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LMEM_FIXED, cbBuffSize);
		} else {
			printf("QueryServiceConfig failed (%d) \n", dwError);

			goto CLEANUP;
		}
	}

	if (!QueryServiceConfig2(
		schService,
		SERVICE_CONFIG_DESCRIPTION,
		NULL,
		0,
		&dwBytesNeeded)) {

		dwError = GetLastError();

		if (dwError == ERROR_INSUFFICIENT_BUFFER) {
			cbBuffSize = dwBytesNeeded;
			lpsd = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, cbBuffSize);
		} else {
			printf("QueryServiceConfig2 failed (%d) \n", dwError);

			goto CLEANUP;
		}
	}

	if (!QueryServiceConfig2(
		schService,
		SERVICE_CONFIG_DESCRIPTION,
		(LPBYTE)lpsd,
		cbBuffSize,
		&dwBytesNeeded)) {

		printf("QueryServiceConfig2 failed (%d) \n", GetLastError());

		goto CLEANUP;
	}

		
	// Print the configuration information
	_tprintf(TEXT("%s configuration: \n"), svcName);
	_tprintf(TEXT("		Type: 0x%x\n"), lpsc->dwServiceType);
	_tprintf(TEXT("		StartType: 0x%x\n"), lpsc->dwStartType);
	_tprintf(TEXT("		Error Control: 0x%x\n"), lpsc->dwErrorControl);
	_tprintf(TEXT("		Binary path: %s\n"), lpsc->lpBinaryPathName);
	_tprintf(TEXT("		Account: %s\n"), lpsc->lpServiceStartName);


	if (lpsd->lpDescription != NULL && lstrcmp(lpsd->lpDescription, TEXT("")) != 0)
		_tprintf(TEXT("\tDescription: %s\n"), lpsd->lpDescription);

	if (lpsc->lpLoadOrderGroup != NULL && lstrcmp(lpsc->lpLoadOrderGroup, TEXT("")) != 0)
		_tprintf(TEXT("\tLoad order group: %s\n"), lpsc->lpLoadOrderGroup);

	if (lpsc->dwTagId != 0)
		_tprintf(TEXT("\tTag ID: %d\n"), lpsc->dwTagId);

	if (lpsc->lpDependencies != NULL && lstrcmp(lpsc->lpDependencies, TEXT("")) != 0)
		_tprintf(TEXT("\tDependencies: %s\n"), lpsc->lpDependencies);


	LocalFree(lpsc);
	LocalFree(lpsd);


CLEANUP:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}



/** Purpose:
 *		Disables the service.
 *
 *	Parameters:
 *		none
 *
 *	Return values:
 *		none
 *
 */

VOID WINAPI DoDisableSvc()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;


	// Get a handle to the SCM database.
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ServicesActive database
		SC_MANAGER_ALL_ACCESS);			// Full access rights
}