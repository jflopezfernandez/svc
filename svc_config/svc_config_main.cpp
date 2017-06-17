/** SERVICE CONFIGURATION PROGRAM
 *
 *  Author: Jose Lopez
 *	Date: 17-June-2017
 *
 *	Source: (https://msdn.microsoft.com/en-us/library/bb540473(v=vs.85).aspx)
 *
 *
 *	TODO:	Since many of the functions can fail, converting them to return-
 *			type int and checking for the result might be better than the
 *			current system, although I guess in a way I'm already testing by
 *			seeing if their respective variables come back NULL. I'll mull it
 *			over. 
 *
 */

#include "Main.hpp"


/** Purpose:
 *		Entry point function. Executes specified command from the user.
 *
 *	Parameters:
 *		Command-line syntax is: svcconfig [command] [service_path]
 *
 *	Return values:
 *		none
 */

INT WINAPI _tmain(INT argc, TCHAR *argv[])
{
	TCHAR szCommand[13];
	TCHAR szSvcName[80];

	printf("\n");
	
	if (argc != 3) {
		printf("\t[Error]: Incorrect number of arguments\n\n");
		DisplayUsage();

		return EXIT_FAILURE;
	}

	StringCchCopy(szCommand, 12, argv[1]);
	StringCchCopy(szSvcName, 80, argv[2]);


	if (lstrcmpi(szCommand, TEXT("--query")) == 0)
		DoQuerySvc(szSvcName);
	else if (lstrcmpi(szCommand, TEXT("--describe")) == 0)
		DoUpdateSvcDesc(szSvcName);
	else if (lstrcmpi(szCommand, TEXT("--disable")) == 0)
		DoDisableSvc(szSvcName);
	else if (lstrcmpi(szCommand, TEXT("--enable")) == 0)
		DoEnableSvc(szSvcName);
	else if (lstrcmpi(szCommand, TEXT("--delete")) == 0)
		DoDeleteSvc(szSvcName);
	else if (lstrcmpi(szCommand, TEXT("--help")) == 0)
		DisplayUsage();
	else {
		_tprintf(TEXT("Unknown command (%s) \n\n"), szCommand);
		DisplayUsage();
	}
	
	return EXIT_SUCCESS;
}