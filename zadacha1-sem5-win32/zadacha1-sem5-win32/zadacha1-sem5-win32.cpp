#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h> // for EXIT_ codes

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);

	// handle messages loop
	BOOL bMsgFlag = 0;
	MSG pMsg = { 0 };
	while ((bMsgFlag = ::GetMessageW(&pMsg, NULL, 0, 0)) != 0) {
		if (bMsgFlag > 0) { // GetMessageW returned no error nor processed WM_QUIT message
			::TranslateMessage(&pMsg);
			::DispatchMessageW(&pMsg);
		} else { // handle error returned by GetMessageW
			// TODO: add a helper function to use GetLastError with FormatMessage for retrieve the error string
		}
	}

	return static_cast<int>(pMsg.wParam);
}
