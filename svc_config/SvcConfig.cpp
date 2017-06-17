
#include "Main.hpp"


VOID WINAPI DisplayUsage()
{
	printf("Description: \n");
	printf("\tCommand-line tool that configures a service.\n\n");
	printf("Usage:\n");
	printf("\tsvcconfig [command] [service_name]\n\n");
	printf("\t[command]\n");
	printf("\t  --query\n");
	printf("\t  --describe\n");
	printf("\t  --disable\n");
	printf("\t  --enable\n");
	printf("\t  --delete\n");
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

	LPQUERY_SERVICE_CONFIG	lpsc = nullptr;
	LPSERVICE_DESCRIPTION	lpsd = nullptr;

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

	
	// Make sure uninitialized pointers are not dereferenced
	if ((lpsc == nullptr) || (lpsd == nullptr)) {
		printf("[ERROR]: Attempting to dereference a null pointer!");

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

VOID WINAPI DoDisableSvc(const TCHAR *svcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;


	// Get a handle to the SCM database.
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ServicesActive database
		SC_MANAGER_ALL_ACCESS);			// Full access rights

	if (schSCManager == NULL) {
		printf("OpenSCManager failed (%d) \n", GetLastError());

		return;
	}


	// Get a handle to the service
	schService = OpenService(
		schSCManager,					// SCM database
		svcName,						// Name of service
		SERVICE_CHANGE_CONFIG);			// Need change config access

	if (schService == NULL) {
		printf("OpenService failed (%d) \n", GetLastError());
		CloseServiceHandle(schSCManager);

		return;
	}


	// Change the service start type
	if (!ChangeServiceConfig(
		schService,						// Handle of service
		SERVICE_NO_CHANGE,				// Service type: no change
		SERVICE_DISABLED,				// Service start type
		SERVICE_NO_CHANGE,				// Error control: no change
		NULL,							// Binary path: no change
		NULL,							// Load order group: no change
		NULL,							// Tag ID: no change
		NULL,							// Dependencies: no change
		NULL,							// Account name: no change
		NULL,							// Password: no change
		NULL)) {						// Display name: no change

		printf("ChangeServiceConfig failed (%d) \n", GetLastError());
	} else {
		printf("Service disabled successfully. \n");
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}



/** Purpose:
 *		Enables the service.
 *
 *	Parameters:
 *		none
 *
 *	Return value:
 *		none
 *
 *
 */

VOID WINAPI DoEnableSvc(const TCHAR *svcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	
	// Get a handle to the SCM database
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ServicesActive database
		SC_MANAGER_ALL_ACCESS);			// Full access rights

	if (schSCManager == NULL) {
		printf("OpenSCManager failed (%d) \n", GetLastError());

		return;
	}


	// Get a handle to the service
	schService = OpenService(
		schSCManager,					// SCM database
		svcName,						// Name of service
		SERVICE_CHANGE_CONFIG);			// Need change config access

	if (schService == NULL) {
		printf("OpenService failed (%d) \n", GetLastError());
		CloseServiceHandle(schSCManager);

		return;
	}


	// Change the service start type
	if (!ChangeServiceConfig(
		schService,						// Handle of service
		SERVICE_NO_CHANGE,				// Service type: no change
		SERVICE_DEMAND_START,			// Service start type
		SERVICE_NO_CHANGE,				// Error control: no change
		NULL,							// Binary path: no change
		NULL,							// Load order group: no change
		NULL,							// Tag ID: no change
		NULL,							// Dependencies: no change
		NULL,							// Account name: no change
		NULL,							// Password: no change
		NULL)) {						// Display name: no change

		printf("ChangeServiceConfig failed (%d) \n", GetLastError());
	} else {
		printf("Service enabled successfully.\n");
	}

	
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}



/** Purpose
 *		Updates the service description to "This is a test description".
 *
 *	Parameters:
 *		none
 *
 *	Return value:
 *		none
 *
 */

VOID WINAPI DoUpdateSvcDesc(const TCHAR *svcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	SERVICE_DESCRIPTION sd;
	LPTSTR szDesc = TEXT("This is a test description");


	// Get a handle to the SCM database
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ServicesActive database
		SC_MANAGER_ALL_ACCESS);			// Full access rights

	if (schSCManager == NULL) {
		printf("OpenSCManager failed (%d) \n", GetLastError());

		return;
	}



	// Get a handle to the service
	schService = OpenService(
		schSCManager,
		svcName,
		SERVICE_CHANGE_CONFIG);

	if (schService == NULL) {
		printf("OpenService failed (%d) \n", GetLastError());
		CloseServiceHandle(schSCManager);

		return;
	}



	// Change the service description
	sd.lpDescription = szDesc;

	if (!ChangeServiceConfig2(
		schService,
		SERVICE_CONFIG_DESCRIPTION,
		&sd)) {

		printf("ChangeServiceConfig2 failed\n");
	} else {
		printf("Service description updated successfully.\n");
	}


	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}



/** Purpose:
 *		Deletes a service from the SCM database.
 *
 *	Parameters:
 *		none
 *
 *	Return value:
 *		none
 *
 */

VOID WINAPI DoDeleteSvc(const TCHAR *svcName)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;


	// Get a handle to the SCM database
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ActiveServices database
		SC_MANAGER_ALL_ACCESS);			// Full access rights

	if (schSCManager == NULL) {
		printf("OpenSCManager failed (%d) \n", GetLastError());

		return;
	}


	// Get a handle to the service
	schService = OpenService(
		schSCManager,					// SCM database
		svcName,						// Name of service
		DELETE);						// Need delete access

	if (schService == NULL) {
		printf("OpenService failed (%d) \n", GetLastError());
		CloseServiceHandle(schSCManager);

		return;
	}



	// Delete the service
	if (!DeleteService(schService)) {
		printf("DeleteService failed (%d) \n", GetLastError());
	} else {
		printf("Service deleted successfully.\n");
	}


	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}