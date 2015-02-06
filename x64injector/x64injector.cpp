
#include <Windows.h>
#include <iostream>

using namespace std;

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd );

int wmain ( int argc, wchar_t* argv[] )

{
	wchar_t search[24];

	if ( argc > 2 )
		wcscpy ( search, argv[2] );

	DWORD dwPid;
	HANDLE hHandle;
	void* lpRemoteString;
	wchar_t szPath[ MAX_PATH ];
	HWND hWnd = NULL;

	while (	( hWnd = FindWindow ( NULL, search ) ) == NULL )
		{
			wcout << "\n\nSearch Window:\n";
			//MessageBox( NULL, L"Window could not be found.", NULL, MB_OK );
			wcin.getline ( search, 24 );
		}

	GetWindowThreadProcessId ( hWnd, &dwPid );
	hHandle = OpenProcess ( PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, dwPid );
	lpRemoteString = VirtualAllocEx ( hHandle, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	GetCurrentDirectory ( sizeof ( szPath ), szPath );
	wcscat_s ( szPath, L"\\translate.dll" );
	WriteProcessMemory ( hHandle, lpRemoteString, ( void* ) szPath, sizeof ( szPath ) * 2, NULL );
//MessageBox( NULL, szPath, L"Press OK to inject", MB_OK );
	HANDLE hThread = CreateRemoteThread ( hHandle, NULL, 0, ( LPTHREAD_START_ROUTINE ) LoadLibraryW, lpRemoteString, 0, NULL );

	if ( hThread == NULL )
		{
			void* lpBuffer;
			FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			                NULL,
			                ::GetLastError(),
			                MAKELANGID ( LANG_NEUTRAL, SUBLANG_DEFAULT ), // Default language
			                ( LPTSTR ) &lpBuffer,
			                0,
			                NULL );
			MessageBox ( NULL, ( LPCTSTR ) lpBuffer, L"LastError", MB_OK );
			LocalFree ( lpBuffer );
		}

	WaitForSingleObject ( hThread, INFINITE );
	DWORD dwModule;
	GetExitCodeThread ( hThread, &dwModule );
	CloseHandle ( hThread );
	VirtualFreeEx ( hHandle, lpRemoteString, 0, MEM_FREE );
	MessageBox ( NULL, szPath, L"Unload?", MB_OK );
	hThread = CreateRemoteThread ( hHandle, NULL, 0, ( LPTHREAD_START_ROUTINE ) FreeLibrary, ( void* ) dwModule, 0, NULL );
	WaitForSingleObject ( hThread, INFINITE );
	CloseHandle ( hThread );
	CloseHandle ( hHandle );
	return 0;
}
