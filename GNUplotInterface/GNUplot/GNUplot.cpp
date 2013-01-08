/************************************************************************
*																																				*
*					GNUplot.cpp -- Class file for gnuplot interface								*
*                                                                       *
*					Created by: Andrew Loblaw	on September 26, 2011								*
*					Modified on: October 2, 2011																*
*                                                                       *
************************************************************************/

/* requirements:
* gnuplot has to be installed (http://www.gnuplot.info/download.html)
* for Windows: set Path-Variable for Gnuplot path (e.g. C:/gnuplot/binary)
* or set Gnuplot path with: Gnuplot::set_GNUPlotPath(const std::string &path); */

#include "GNUplot.h"

#if defined(DEBUG) || defined (_DEBUG)
//include or whatever
#else if defined(NDEBUG)
	//Do this
#endif

const TCHAR GNUPLOTEXE[] = TEXT("C:\\Gnuplot\\binary\\gnuplot.exe");

/* CHAR * GNUPlotInterface::Init(...)
*	This function is used to initialize a "real-time" plot. It sets the plot up
*	for plotting data from individual values, as well as other formatting issues
*	like the title name and line type, etc.
*	"gnuplotCommandLine" specifies the entire initialization sequence. For 
* clarification on command arguments see "gnuplot.pdf" in C:\Gnuplot\docs.
*/
std::string GNUPlotInterface::Init(const CHAR *gnuplotLINETYPE, const CHAR *gnuplotTITLE){
	DWORD dwToWrite;
	DWORD dwWritten = 0;
	BOOL bSuccess = FALSE;
	std::string gnuplotCommand;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) 
	gnuplotCommand = "set term windows\n";
#else
	/*This section only for non-windows terminal users. 
	*	gnuplotCommand = "set term &&&&&" depending on your terminal type (i.e. "set
	*	term macintosh*/
	//std::cout<<"Add definition for gnuplotCommand here. (depends on your terminal type)"<<std::endl;
	//gnuplotCommand = "set term macintosh\n"; //Change 'macintosh' to your terminal type
#endif
	gnuplotCommand.append("plot \"-\" with ");	//"-" for realtime plotting
	gnuplotCommand.append(gnuplotLINETYPE);	//Linetype
	gnuplotCommand.append(" title \""); 
	gnuplotCommand.append(gnuplotTITLE); 	gnuplotCommand.append("\"\n"); //Title
	gnuplotCommand.append("1 1\n2 2\ne\n");	//Some initial plot (So we don't get warning messages)
	dwToWrite = gnuplotCommand.length();
	bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotCommand.c_str(), dwToWrite, &dwWritten, NULL);
	if ( !bSuccess || dwWritten != dwToWrite )
		ErrorExit(TEXT("In GNUPlotInterface::Init StdIN write"));
	/* Read reply from gnuplot.exe (Should be none with proper command line) */
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd,chBuf,dwToRead,&dwRead,NULL);
		if ( !bSuccess)
			ErrorExit(TEXT("In GNUPlotInterface::Init StdOUT read"));
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	std::string returnString(chBuf);
	return returnString;
}
/* GNUPlotInterface::Write(...)
*	This is used to send specific commands AS-IS to gnuplot. Examples include 
* setting the xrange, yrange or other "set" commands. Make sure to include the
*	newline character \n at the end of this command if required.
*	"gnuplotCommandLine" specifies the arguments to be sent to gnuplot.exe
*/
std::string GNUPlotInterface::Write(const CHAR *gnuplotCommandLine){
	DWORD dwToWrite = (DWORD) strlen(gnuplotCommandLine);
	DWORD dwWritten = 0;
	BOOL bSuccess = FALSE;
	bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotCommandLine, dwToWrite, &dwWritten, NULL);
	if ( !bSuccess || dwWritten != dwToWrite )
		ErrorExit(TEXT("In GNUPlotInterface::Plot(...,1) StdIN write"));
	
	/* Now read the reply from gnuplot.exe */
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd,chBuf,dwToRead,&dwRead,NULL);
		if(!bSuccess)
			ErrorExit(TEXT("In GNUPlotInterface::Plot(...) StdOUT read"));	
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	std::string returnString(chBuf);
	return returnString;	// Return the char array sent from gnuplot.exe
}
/* GNUPlotInterface::Plot(...)
*	To be used only with "Real-time" plotting of data (individual points)
*	This is the first (with minimum overhead) of the overloaded Plot(...) 
*	functions. 
*	"gnuplotCommandLine" specifies the arguments to be sent to gnuplot.exe. 
* "commandArg" specifies whether certain monotonous necessary arguments were 
*	ommitted from commandLine. Typical arguments that might have been ommitted
*	include "replot" or the "e" character. Currently commandArg only supports 
* value of 0. 
*/
std::string GNUPlotInterface::Plot(const CHAR *gnuplotCommandLine, int commandArg){
	DWORD dwToWrite;
	DWORD dwWritten = 0;
	BOOL bSuccess = FALSE;

	if(commandArg == 0){
		/* Only the plot points are supplied in commandLine, so add "replot" and "e"
		* to the commandLine string (prepend "replot", append "e") */
		std::string gnuplotCommand = "replot\n";
		gnuplotCommand.append(gnuplotCommandLine);
		gnuplotCommand.append("e\n");

		dwToWrite = gnuplotCommand.length();
		bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotCommand.c_str(), dwToWrite, &dwWritten, NULL);
		if ( !bSuccess || dwWritten != dwToWrite )
			ErrorExit(TEXT("In GNUPlotInterface::Plot(...,1) StdIN write"));
	}
	else{
		SetLastError(ERROR_INVALID_DATA);
		ErrorExit(TEXT("GNUPlotInterface::Plot(...,#) commandArg"));
	}
	/* Now read the reply from gnuplot.exe */
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd,chBuf,dwToRead,&dwRead,NULL);
		if(!bSuccess)
			ErrorExit(TEXT("In GNUPlotInterface::Plot(...) StdOUT read"));	
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	std::string returnString(chBuf);
	return returnString;	// Return the char array sent from gnuplot.exe
}
/* GNUPlotInterface::Plot(...) [overloaded]
*	To be used only with "Real-time" plotting of data (individual points)
*	This is the second of the overloaded Plot(...) functions. 
*	"data" specifies the data points to be sent to gnuplot.exe. The data should
*	be organized as follows: 
*	data[0][0] = x[0], data[1][0] = y[0]; 
*	data[0][1] = x[1], data[1][1] = y[1];
*	data[0][2] = x[2], data[1][2] = y[2];
* "dims" specifies how many dimensions in the data array (currently only 1 or 2
*	dimensions are supported)
*	"length" specifies how long the data array is (i.e. how many data points)
*/
std::string GNUPlotInterface::Plot(std::vector<double> data[], int dims, int length){
	DWORD dwToWrite;
	DWORD dwWritten = 0;
	BOOL bSuccess = FALSE;

	std::string gnuplotCommand = "replot\n";
	CHAR doubleVal[20];
	if(dims == 1)
		for(int i = 0; i<length; i++){
			sprintf_s(doubleVal,"%.5f",data[0][i]);
			gnuplotCommand.append(doubleVal);	//y-value
			gnuplotCommand.append("\n");
		}
	else if(dims == 2)
		for(int i = 0; i<length; i++){
			sprintf_s(doubleVal,"%.5f",data[0][i]);	//x-value
			gnuplotCommand.append(doubleVal); gnuplotCommand.append(" ");
			sprintf_s(doubleVal,"%.5f",data[1][i]);	//y-value
			gnuplotCommand.append(doubleVal);	gnuplotCommand.append("\n");
		}
	else{
		SetLastError(ERROR_INVALID_DATA);
		ErrorExit(TEXT("GNUPlotInterface::Plot(...,#,...) Invalid #"));
	}
	gnuplotCommand.append("e\n");

	dwToWrite = gnuplotCommand.length();
	bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotCommand.c_str(), dwToWrite, &dwWritten, NULL);
	if ( !bSuccess || dwWritten != dwToWrite )
			ErrorExit(TEXT("In GNUPlotInterface::Plot(data) StdIN write"));

	/* Now read the reply from gnuplot.exe */
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd,chBuf,dwToRead,&dwRead,NULL);
		if(!bSuccess)
			ErrorExit(TEXT("In GNUPlotInterface::Plot(data) StdOUT read"));	
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	std::string returnString(chBuf);
	return returnString;
}
/* GNUPlotInterface::Plot(...) [overloaded]
*	To be used only with "Real-time" plotting of data (individual points)
*	This is the third of the overloaded Plot(...) functions and can be used 
*	in a way very familiar to Matlab plotting.
*	"xValues"	specifies the x array of values for plotting.
*	"yValues" specifies the y array of values for plotting.
*	"length" specifies how long the data arrays are (i.e. how many data points)
*/
std::string GNUPlotInterface::Plot(double xValues[], double yValues[], int length){
	DWORD dwToWrite;
	DWORD dwWritten = 0;
	BOOL bSuccess = FALSE;

	std::string gnuplotCommand = "replot\n";
	CHAR doubleVal[20];
	for(int i = 0; i<length; i++){
		sprintf_s(doubleVal,20,"%.5f",xValues[i]);	//x-value 
		gnuplotCommand.append(doubleVal); gnuplotCommand.append(" ");
		sprintf_s(doubleVal,20,"%.5f",yValues[i]);	//y-value
		gnuplotCommand.append(doubleVal);	gnuplotCommand.append("\n");
	}
	gnuplotCommand.append("e\n");

	dwToWrite = gnuplotCommand.length();
	bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotCommand.c_str(), dwToWrite, &dwWritten, NULL);
	if ( !bSuccess || dwWritten != dwToWrite )
			ErrorExit(TEXT("In GNUPlotInterface::Plot(data) StdIN write"));

	/* Now read the reply from gnuplot.exe */
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd,chBuf,dwToRead,&dwRead,NULL);
		if(!bSuccess)
			ErrorExit(TEXT("In GNUPlotInterface::Plot(data) StdOUT read"));	
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	std::string returnString(chBuf);
	return returnString;
}
/* void GNUPlotInterface::Plot(...)
*	To be used only with "Real-time" plotting of data (individual points)
*	This is the fourth of the overloaded Plot(...) functions and can be used 
*	in a way very familiar to Matlab plotting. Only the y values are specified;
* the x values are assigned based on the index of each y value (like Matlab)
*	"yValues" specifies the y array of values for plotting.
*	"length" specifies how long the data arrays are (i.e. how many data points)
*/
std::string GNUPlotInterface::Plot(double yValues[], int length){
	DWORD dwToWrite;
	DWORD dwWritten = 0;
	BOOL bSuccess = FALSE;

	std::string gnuplotCommand = "replot\n";
	CHAR doubleVal[20];
	for(int i = 0; i<length; i++){
		sprintf_s(doubleVal,20,"%d",i);	//x-value 
		gnuplotCommand.append(doubleVal); gnuplotCommand.append(" ");
		sprintf_s(doubleVal,20,"%.5f",yValues[i]);	//y-value
		gnuplotCommand.append(doubleVal);	gnuplotCommand.append("\n");
	}
	gnuplotCommand.append("e\n");

	dwToWrite = gnuplotCommand.length();
	bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotCommand.c_str(), dwToWrite, &dwWritten, NULL);
	if ( !bSuccess || dwWritten != dwToWrite )
			ErrorExit(TEXT("In GNUPlotInterface::Plot(data) StdIN write"));

	/* Now read the reply from gnuplot.exe */
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd,chBuf,dwToRead,&dwRead,NULL);
		if(!bSuccess)
			ErrorExit(TEXT("In GNUPlotInterface::Plot(data) StdOUT read"));	
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	std::string returnString(chBuf);
	return returnString;
}
/* void GNUPlotInterface::CreatePipe()
*	This function simply creates the pipes used to interface with gnuplot.exe.
*	It MUST be called before StartProcess(), otherwise StartProcess() will not 
* have initialized pipe handles to call gnuplot.exe with, and an error will
*	occur.
*/
void GNUPlotInterface::CreatePipes(void)
{
	SECURITY_ATTRIBUTES saAttr; 
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	/* Create a pipe for the child process's STDIN. */
	if (! CreatePipe(&m_hGNUPlotStd_IN_Rd, &m_hGNUPlotStd_IN_Wr, &saAttr, 0)) 
		ErrorExit(TEXT("Stdin CreatePipe")); 

	// Ensure the write handle to the pipe for STDIN is not inherited. 
	if ( ! SetHandleInformation(m_hGNUPlotStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
		ErrorExit(TEXT("Stdin SetHandleInformation")); 

	/* Create a pipe for the child process's STDOUT. */
	if (! CreatePipe(&m_hGNUPlotStd_OUT_Rd, &m_hGNUPlotStd_OUT_Wr, &saAttr, 0)) 
		ErrorExit(TEXT("Stdout CreatePipe")); 

	// Ensure the read handle to the pipe for STDOUT is not inherited. 
	if ( ! SetHandleInformation(m_hGNUPlotStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		ErrorExit(TEXT("Stdin SetHandleInformation")); 
}
/* void GNUPlotInterface::StartProcess()
*	This function starts the gnuplot.exe process and sets the appropriate pipe 
*	handles for communication. The class member variable m_ProcInfo is returned 
* containing a handle to the process, and the thread it is contained in. The 
*	path to gnuplot.exe is assumed to be "C:\Gnuplot\binary", if it is not, then
* modify the "const TCHAR *GNUPLOT_EXE" declaration.
*/
void GNUPlotInterface::StartProcess(void)
	// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{ 
	const TCHAR *szProcessName = GNUPLOTEXE;
	STARTUPINFO StartInfo;
	DWORD dwCreationFlags;
	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory( &m_ProcInfo, sizeof(PROCESS_INFORMATION) );

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory( &StartInfo, sizeof(STARTUPINFO) );
	StartInfo.cb = sizeof(STARTUPINFO); 
	StartInfo.dwX = 0;	//Starting x-position in pixels
	StartInfo.dwY = 0;	//Starting y-position in pixels
	StartInfo.dwXSize = 800;	//Starting width in pixels
	StartInfo.dwYSize = 800;	//Starting height in pixels
	StartInfo.hStdError = m_hGNUPlotStd_OUT_Wr;
	StartInfo.hStdOutput = m_hGNUPlotStd_OUT_Wr;
	StartInfo.hStdInput = m_hGNUPlotStd_IN_Rd;
	StartInfo.dwFlags |= STARTF_USESTDHANDLES | STARTF_USEPOSITION | 
		STARTF_USESIZE;

	/* Initialize the Creation Flags variable with the appropriate flags set 
	* The DETACHED_PROCESS flag prevents the gnuplot window from forcing it's way
	*	to the front when the command line becomes active. */
	dwCreationFlags = DETACHED_PROCESS;	

	// Create the child process. 
	if(!CreateProcess(szProcessName, 
		NULL,						// command line 
		NULL,						// process security attributes 
		NULL,						// primary thread security attributes 
		TRUE,						// handles are inherited 
		dwCreationFlags,							// creation flags 
		NULL,						// use parent's environment 
		NULL,						// use parent's current directory 
		&StartInfo,		// STARTUPINFO pointer 
		&m_ProcInfo /* receives PROCESS_INFORMATION */))
	{
		// If an error occurs, exit the application. 
		ErrorExit(TEXT("CreateProcess"));
	}
}
/* GNUPlotInterface::CloseAll()
*	A simple cleanup function.
*	This function closes all the relevant pipe, process and thread handles.
*/
std::string GNUPlotInterface::CloseAll(void)
{
	/* Write a quit command to gnuplot.exe */
	CHAR *gnuplotQuitCommand = "quit\n";
	DWORD dwToWrite = strlen(gnuplotQuitCommand);
	DWORD dwWritten = 0;
	DWORD dwToRead = BUFSIZE;
	DWORD dwRead = 0;
	DWORD totalBytesAvail;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	bSuccess = WriteFile(m_hGNUPlotStd_IN_Wr, gnuplotQuitCommand, dwToWrite, &dwWritten, NULL);
	if ( !bSuccess || dwWritten != dwToWrite )
		ErrorExit(TEXT("In GNUPlotInterface::CloseAll StdIN write"));
	/* Peek the pipe to see if data is available to be read. If no data available
	*	then don't try and read (i.e. prevent ReadFile from blocking by not calling
	*	it). */
	PeekNamedPipe(m_hGNUPlotStd_OUT_Rd,NULL,BUFSIZE,NULL,&totalBytesAvail,NULL);
	if(totalBytesAvail>0){
		bSuccess = ReadFile(m_hGNUPlotStd_OUT_Rd, chBuf, dwToRead, &dwRead,NULL);
		if ( !bSuccess )
			ErrorExit(TEXT("In GNUPlotInterface::CloseAll StdOUT read"));
		chBuf[dwRead] = '\0';
	}
	else
		chBuf[0] = '\0';

	/* Close the handle to the process and thread */
	CloseHandle(m_ProcInfo.hProcess);
	CloseHandle(m_ProcInfo.hThread);

	/* Close the child process read-pipe handle */
	if ( ! CloseHandle(m_hGNUPlotStd_IN_Rd) ) 
		ErrorExit(TEXT("StdInWr CloseHandle")); 
	/* Close the parent process write-pipe handle */
	if ( ! CloseHandle(m_hGNUPlotStd_IN_Wr) ) 
		ErrorExit(TEXT("StdInWr CloseHandle")); 
	/* Close the parent process read-pipe handle */
	if ( ! CloseHandle(m_hGNUPlotStd_OUT_Rd) ) 
		ErrorExit(TEXT("StdInWr CloseHandle")); 
	/* Close the child process write-pipe handle */
	if ( ! CloseHandle(m_hGNUPlotStd_OUT_Wr) ) 
		ErrorExit(TEXT("StdInWr CloseHandle")); 

	std::string returnString(chBuf);
	return returnString; // Return the char array sent from gnuplot.exe (reply to "quit")
}
/* GNUPlotInterface::ErrorExit(PTSTR lpszFunction)
*	This function was taken directly from:
*	http://msdn.microsoft.com/en-us/library/windows/desktop/ms682499%28v=vs.85%29.aspx
*	It is an error handling function which displays error messages in a message
* box, with error codes.
*/
void GNUPlotInterface::ErrorExit(PTSTR lpszFunction) 
	// Format a readable error message, display a message box, 
	// and exit from the application.
{ 
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)lpszFunction)+40)*sizeof(TCHAR)); 
	StringCchPrintf((LPTSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(1);
}
