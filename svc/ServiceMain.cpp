
#include "Main.hpp"


SERVICE_STATUS			gSvcStatus;
SERVICE_STATUS_HANDLE	gSvcStatusHandle;
HANDLE					ghSvcStopEvent = NULL;


/** Purpose: 
 *		Installs the service in the SCM database.
 *
 *  Paramters:
 *		none
 *
 *	Return value:
 *		none
 *
 */

VOID svcInstall()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	TCHAR szPath[MAX_PATH];

	// Jose Lopez (5:20AM) 17-June-2017
	// FIXED: const char * is incompatible with *HINSTANCE
	// Passing NULL instead of a handle to the loaded module. If this parameter
	// is NULL, GetModuleFileName retrieves the path of the executable of the
	// current process.

	if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
		printf("Cannot install service %d) \n", GetLastError());

		return;
	}


	// Get a handle to the SCM Database
	schSCManager = OpenSCManager(
		NULL,							// Local computer
		NULL,							// ServicesActive database
		SC_MANAGER_ALL_ACCESS			// Full access rights
	);

	if (NULL == schSCManager) {
		printf("OpenSCManager failed (%d) \n", GetLastError());

		return;
	}


	// Create the service
	schService = CreateService(
		schSCManager,					// SCM database
		SVCNAME,						// Name of the service
		SVCNAME,						// Service name to display
		SERVICE_ALL_ACCESS,				// Desired access
		SERVICE_WIN32_OWN_PROCESS,		// Service type
		SERVICE_DEMAND_START,			// Start type
		SERVICE_ERROR_NORMAL,			// Error control type
		szPath,							// Path to service binary
		NULL,							// No load ordering group
		NULL,							// No tag identifier
		NULL,							// No dependencies
		NULL,							// LocalSystem account
		NULL							// No password
	);


	if (schService == NULL) {
		printf("CreateService failed (%d) \n", GetLastError());
		CloseServiceHandle(schSCManager);

		return;
	} else {
		printf("Service installed successfully. \n");
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}


/** Purpose:
 *		Entry point for the service.
 *
 *  Paramters:
 *		dwArgc		Number of arguments in the lpszArgv array
 *
 *		lpszArgv	Array of strings. The first string is the name of
 *					the service and subsequent strings are passed by the process
 *					that called the StartService function to start the service.
 *
 *	Return value:
 *		none
 *
 */

VOID WINAPI svcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// Register the handler function for the service
	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		svcCtrlHandler
	);

	if (!gSvcStatusHandle) {
		svcReportEvent(TEXT("RegisterServiceCtrolHandler"));

		return;
	}


	// These SERVICE_STATUS members remain as set here
	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM
	reportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.
	svcInit(dwArgc, lpszArgv);
}


/** Purpose:
 *		The service code.
 *
 *	Parameters:
 *		dwArgc		Number of arguments in the lpszArgv array
 *
 *		lpszArgv	Array of strings. The first string is the name of
 *					the service and subsequent strings are passed by the
 *					process that called the StartService function to start
 *					the service.
 *
 */

VOID svcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// TODO: Declare and set any required variables.
	//		 Be sure to periodically call ReportSvcStatus() with
	//		 SERVICE_START_PENDING. If initialization fails, call
	//		 ReportSvcStatus with SERVICE_STOPPED.

	// Create an event. The control handler function, svcCtrlHandler,
	// signals this event when it receives the stop control code.

	ghSvcStopEvent = CreateEvent(
		NULL,							// Default security attributes
		TRUE,							// Manual reset event
		FALSE,							// Not signaled
		NULL							// No name
	);

	if (ghSvcStopEvent == NULL) {
		reportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);

		return;
	}


	// Report running status when initialization is complete
	reportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);


	// TODO: Perform work until service stops.

	while (true) {
		// Check whether to stop the service.
		WaitForSingleObject(ghSvcStopEvent, INFINITE);
		reportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);

		return;
	}
}



/** Purpose:
 *		Sets the current service status and reports it to the SCM.
 *
 *	Parameters:
 *		dwCurrentState		The current state (see SERVICE_STATUS)
 *
 *		dwWin32ExitCode		The system error code
 *
 *		dwWaitHint			Estimated time for pending operation (in ms)
 *
 *	Return value:
 *		none
 *
 */

VOID reportSvcStatus(DWORD dwCurrentState,
					 DWORD dwWin32ExitCode,
					 DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else
		gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;


	if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else
		gSvcStatus.dwCheckPoint = ++dwCheckPoint;


	// Report the status of the service to the SCM.
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}



/** Purpose:
 *		Called by SCM whenever a control code is sent to the service
 *		using the ControlService function.
 *
 *	Parameters:
 *		dwCtrl		control code
 *
 *	Return value:
 *		none
 *
 */

VOID WINAPI svcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code.
	switch (dwCtrl)
	{
		case SERVICE_CONTROL_STOP:
			reportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

			// Signal the service to stop
			SetEvent(ghSvcStopEvent);
			reportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

			return;

		case SERVICE_CONTROL_INTERROGATE:
			break;

		default:
			break;
	}
}



/** Purpose:
 *		Logs messages to the event log.
 *
 *	Parameters:
 *		szFunction		name of function that failed
 *
 *	Return value:
 *		none
 *
 *  Remarks:
 *		The service must have an entry in the Application event log.
 *
 */

VOID svcReportEvent(LPTSTR szFunction)
{
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];

	hEventSource = RegisterEventSource(NULL, SVCNAME);

	if (NULL != hEventSource) {
		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = SVCNAME;
		lpszStrings[1] = Buffer;


		ReportEvent(hEventSource,		// Event log handle
			EVENTLOG_ERROR_TYPE,		// Event type
			0,							// Event category
			SVC_ERROR,					// Event identifier
			NULL,						// No security identifier
			2,							// Size of lpszStrings array
			0,							// No binary data
			lpszStrings,				// Array of strings
			NULL);						// No binary data

		DeregisterEventSource(hEventSource);
	}
}