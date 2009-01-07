// Copyright (C) 2003-2008 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/



//////////////////////////////////////////////////////////////////////////////////////////
// Includes
// ŻŻŻŻŻŻŻŻŻŻŻŻŻ
#include <string>
#include <stdio.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include "StringUtil.h"

#define HAVE_WX 1
#if defined(HAVE_WX) && HAVE_WX // wxWidgets
	#include <wx/datetime.h> // for the timestamps
#endif
///////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////
// Settings
// ŻŻŻŻŻŻŻŻŻŻŻŻŻ

// On and off
bool g_consoleEnable = true;
bool gSaveFile = true;
#define DEBUG_WIIMOTE // On or off
const int nFiles = 1;

// Create handles
#ifdef DEBUG_WIIMOTE
	FILE* __fStdOut[nFiles];
#endif
#ifdef _WIN32
	HANDLE __hStdOut = NULL;
#endif

//////////////////////////////


// =======================================================================================
/* Get Timestamp */
// -------------
std::string Tm(bool Ms)
{
	#if defined(HAVE_WX) && HAVE_WX
		std::string Tmp;
		if(Ms)
		{
			wxDateTime datetime = wxDateTime::UNow(); // Get timestamp
			Tmp = StringFromFormat("%02i:%02i:%03i",
			datetime.GetMinute(), datetime.GetSecond(), datetime.GetMillisecond());
		}
		else
		{
			wxDateTime datetime = wxDateTime::Now(); // Get timestamp
			Tmp = StringFromFormat("%02i:%02i",
			datetime.GetMinute(), datetime.GetSecond());
		}
		return Tmp;
	#else
		std::string Tmp = "";
		return Tmp;
	#endif
}
// ===========================




// =======================================================================================
/* Start console window - width and height is the size of console window, if you specify
fname, the output will also be written to this file. TODO: Close the file pointer when the app
is closed */
// -------------
void startConsoleWin(int width, int height, char* fname)
{
#if defined(DEBUG_WIIMOTE) && defined(_WIN32)

	AllocConsole();

	SetConsoleTitle(fname);
	__hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD co = {width,height};
	SetConsoleScreenBufferSize(__hStdOut, co);

	SMALL_RECT coo = {0,0,(width - 1),70}; // top, left, right, bottom
	SetConsoleWindowInfo(__hStdOut, TRUE, &coo);

	// ---------------------------------------------------------------------------------------
	// Create a file for this
	if(fname)
	{
		for(int i = 0; i < nFiles; i++)
		{
			// Edit the log file name
			std::string FileEnding = ".log";
			std::string FileName = fname;
			char buffer[33]; _itoa(i, buffer, 10); // convert number to string
			std::string FullFilename = (FileName + buffer + FileEnding);
			__fStdOut[i] = fopen(FullFilename.c_str(), "w");
		}
	}
	// ---------------

#endif
}


// ---------------------------------------------------------------------------------------
// File printf function
int aprintf(int a, char *fmt, ...)
{
#if defined(DEBUG_WIIMOTE) && defined(_WIN32)
	if(gSaveFile)
	{
		char s[500]; // WARNING: mind this value
		va_list argptr;
		int cnt;

		va_start(argptr, fmt);
		cnt = vsnprintf(s, 500, fmt, argptr); // remember to update this value to
		va_end(argptr);

		// ---------------------------------------------------------------------------------------
		if(__fStdOut[a]) // TODO: make this work, we have to set all default values to NULL
			//to make it work
			fprintf(__fStdOut[a], s);
			fflush(__fStdOut[0]); // Write file now, don't wait
		// -------------

		return(cnt);
	}
	else
	{
		return 0;
	}
#else
	return 0;
#endif
}



// ---------------------------------------------------------------------------------------
// Printf to screen function
int wprintf(const char *fmt, ...)
{
#if defined(DEBUG_WIIMOTE) && defined(_WIN32)
	char s[500]; // WARNING: mind this value
	va_list argptr;
	int cnt;

	va_start(argptr, fmt);
	cnt = vsnprintf(s, 500, fmt, argptr);
	va_end(argptr);

	DWORD cCharsWritten; // We will get a value back here

	// ------------------------------------------
	// Write to console
	// ----------------
	if(__hStdOut)
	{
		WriteConsole(__hStdOut, s, strlen(s), &cCharsWritten, NULL);
	}

	// ----------------------------------------
	// Write to file
	// ----------------
	aprintf(0, s);

	return(cnt);
#else
	return 0;
#endif
}


// ---------------------------------------------------------------------------------------
// Clear console screen
void ClearScreen() 
{ 
#if defined(_WIN32)
	if(g_consoleEnable)
	{
		COORD coordScreen = { 0, 0 }; 
		DWORD cCharsWritten; 
		CONSOLE_SCREEN_BUFFER_INFO csbi; 
		DWORD dwConSize; 

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
	    
		GetConsoleScreenBufferInfo(hConsole, &csbi); 
		dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
		FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, 
			coordScreen, &cCharsWritten); 
		GetConsoleScreenBufferInfo(hConsole, &csbi); 
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, 
			coordScreen, &cCharsWritten); 
		SetConsoleCursorPosition(hConsole, coordScreen); 
	}
#endif
}


// ---------------------------------------------------------------------------------------
// Get window handle of console window to be able to resize it
#if defined(_WIN32)
HWND GetConsoleHwnd(void)
{

   #define MY_BUFSIZE 1024 // Buffer size for console window titles.
   HWND hwndFound;         // This is what is returned to the caller.
   char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
                                       // WindowTitle.
   char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
                                       // WindowTitle.

   // Fetch current window title.

   GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

   // Format a "unique" NewWindowTitle.

   wsprintf(pszNewWindowTitle,"%d/%d",
               GetTickCount(),
               GetCurrentProcessId());

   // Change current window title.

   SetConsoleTitle(pszNewWindowTitle);

   // Ensure window title has been updated.

   Sleep(40);

   // Look for NewWindowTitle.

   hwndFound = FindWindow(NULL, pszNewWindowTitle);

   // Restore original window title.

   SetConsoleTitle(pszOldWindowTitle);

   return(hwndFound);

}
#endif // win32
