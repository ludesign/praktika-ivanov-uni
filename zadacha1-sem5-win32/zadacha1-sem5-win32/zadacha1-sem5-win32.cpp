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

	// define window style
	DWORD wndStyle = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);
	DWORD wndExStyle = 0;

	// rect structure to hold width size and x,y offsets
	RECT rect = { 0, 0, 802, 802 }; // initial values for window size

	// calculate adjusted size for our window to fix the required client size + the additional elements (frame, menu, titlebar...)
	if (!::AdjustWindowRectEx(&rect, wndStyle, TRUE, wndExStyle)) {
		// handle error code and return failure
	}

	// query OS to get system metrics for screen sizes in order to center it
	int xOffset = (::GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
	int yOffset = (::GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;

	// create the window
	HWND hWnd = ::CreateWindowExW(0L, WND_CLASS_NAME, WND_TITLE_NAME, wndStyle, xOffset, yOffset, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		// handle error code and return failure
		return EXIT_FAILURE;
	}

	// make sure our windows is visible, frontmost and topmost
	::SetWindowPos(hWnd, HWND_TOPMOST, xOffset, yOffset, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW);
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
	HPEN hPenCoordSys = { };
	HPEN hPenDots = {};
	RECT clientRect = { 0 };

	// Check any available messages from the queue
	switch (uMsg) {
		case WM_KEYDOWN:
			switch (wParam) {
				case VK_ESCAPE:
					::PostQuitMessage(0);
				break;
			}
		break;

		case WM_PAINT: {
			// retrieve client size for furture drawing calculations
			GetClientRect(hWnd, &clientRect);

			// begin painting
			hDc = ::BeginPaint(hWnd, &ps);

			// create pen for CCS drawing
			hPenCoordSys = ::CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

			// create pen for Dots drawing
			hPenDots = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

			// select ccs pen and retrieve the original pen
			HGDIOBJ hOriginalPen = ::SelectObject(hDc, hPenCoordSys);

			// draw horizontal line
			::MoveToEx(hDc, 0, clientRect.bottom / 2, (LPPOINT) NULL);
			::LineTo(hDc, clientRect.right, clientRect.bottom / 2);
			
			// draw vertical line
			::MoveToEx(hDc, clientRect.right / 2, 0, (LPPOINT)NULL);
			::LineTo(hDc, clientRect.right / 2, clientRect.bottom);

			// select dots pen
			::SelectObject(hDc, hPenDots);

			// retrieve stock brush, change to dot color
			HGDIOBJ hStockBrush = ::GetStockObject(DC_BRUSH);
			HGDIOBJ hOriginalBrush = ::SelectObject(hDc, hStockBrush);
			::SetDCBrushColor(hDc, RGB(255, 0, 0));

			// draw a sample dot
			::Ellipse(hDc, 10, 10, 14, 14);

			// select default/origin pen before deleting our custom pens
			::SelectObject(hDc, hOriginalPen);
			::SelectObject(hDc, hOriginalBrush);
			
			// end painting and clean up pens
			::DeleteObject(hStockBrush);
			::DeleteObject(hPenCoordSys);
			::DeleteObject(hPenDots);
			::EndPaint(hWnd, &ps);
		}
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
