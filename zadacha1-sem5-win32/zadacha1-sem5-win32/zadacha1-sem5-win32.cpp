#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h> // for EXIT_ codes

#include "resource.h"

#define WND_CLASS_NAME		L"Zadacha1Sem5"
#define WND_TITLE_NAME		L"Zadacha 1 - Sem 5"

// declaration of Windows Procedure callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(pCmdLine);

	// initiate window class structure and create the window
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDM_APP_MENU);
	wcex.lpszClassName = WND_CLASS_NAME;
	wcex.hIconSm = 0;
	if (!::RegisterClassExW(&wcex)) {
		// handle error code and return failure
		return EXIT_FAILURE;
	}

	// rect structure to hold width size and x,y offsets
	RECT rect = { 0, 0, 800, 600 }; // initial values for window size

	// query OS to get system metrics for screen sizes
	rect.left = (::GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
	rect.top  = (::GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;

	// create the window
	HWND hWnd = ::CreateWindowExW(0L, WND_CLASS_NAME, WND_TITLE_NAME, WS_OVERLAPPEDWINDOW, rect.left, rect.top, rect.right, rect.bottom, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		// handle error code and return failure
		return EXIT_FAILURE;
	}

	// make sure our windows is frontmost
	::SetWindowPos(hWnd, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);

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

	// return whatever code wParam has
	return static_cast<int>(pMsg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps = { };
	HDC hDc = { };

	// Check any available messages from the queue
	switch (uMsg) {
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					::PostQuitMessage(0);
				break;
			}
		break;

		case WM_PAINT:
			hDc = ::BeginPaint(hWnd, &ps);
			::EndPaint(hWnd, &ps);
		break;

		case WM_COMMAND:
			if (HIWORD(wParam) == 0) { // if high word of wParam is 0, message source is MENU
				switch (LOWORD(wParam)) {
					case IDM_OPERATIONS_EXIT:
						::PostMessageW(hWnd, WM_CLOSE, 0, 0);
					break;
				}
			}
		break;

		case WM_DESTROY:
			::PostQuitMessage(0);
		break;
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
