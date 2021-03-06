#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h> // for EXIT_ codes
#include <vector>

#include "resource.h"

#define WND_CLASS_NAME		L"Zadacha1Sem5"
#define WND_TITLE_NAME		L"Zadacha 1 - Sem 5"

#define DOT_RADIUS			4

#define CCS_LINES_COLOR		RGB(0, 0, 0)
#define DOT_LINES_COLOR		RGB(255, 0, 0)
#define DOT_BKG_COLOR		RGB(255, 0, 0)
#define DOT_LINES_SEL_COLOR RGB(0, 255, 0)
#define DOT_BKG_SEL_COLOR	RGB(0, 255, 0)

#define CCS_LINES_WIDTH		2
#define DOT_LINES_WIDTH		1

std::vector<POINT> dots;

POINT cursorPositionTracked = { 0 };

// show system error helper function
void showLastErrorAsMessageBox();

// delare procedure callback for dialogs
BOOL CALLBACK AboutDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// declaration of Windows Procedure callback
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
		showLastErrorAsMessageBox();
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
		showLastErrorAsMessageBox();
		return EXIT_FAILURE;
	}

	// query OS to get system metrics for screen sizes in order to center it
	int xOffset = (::GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
	int yOffset = (::GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;

	// create the window
	HWND hWnd = ::CreateWindowExW(0L, WND_CLASS_NAME, WND_TITLE_NAME, wndStyle, xOffset, yOffset, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);
	if (!hWnd) {
		// handle error code and return failure
		showLastErrorAsMessageBox();
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
	HPEN hPenSelDots = {};
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
			hPenCoordSys = ::CreatePen(PS_SOLID, CCS_LINES_WIDTH, CCS_LINES_COLOR);

			// create pen for Dots drawing
			hPenDots = ::CreatePen(PS_SOLID, DOT_LINES_WIDTH, DOT_LINES_COLOR);

			// create pen for selected Dots drawing
			hPenSelDots = ::CreatePen(PS_SOLID, DOT_LINES_WIDTH, DOT_LINES_SEL_COLOR);

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
			::SetDCBrushColor(hDc, DOT_BKG_COLOR);

			// draw all dots from vector<dots>
			for (std::vector<POINT>::iterator iterator = dots.begin(); iterator != dots.end(); ++iterator) {
				// draw text for coordinates
				if (cursorPositionTracked.x > 0 && cursorPositionTracked.y > 0 && 
					(cursorPositionTracked.x > iterator->x - DOT_RADIUS && cursorPositionTracked.x < iterator->x + DOT_RADIUS) &&
					(cursorPositionTracked.y > iterator->y - DOT_RADIUS && cursorPositionTracked.y < iterator->y + DOT_RADIUS)) {
					RECT invalidRect = { 0, 0, 100, 20 };
					wchar_t coords[256];
					// calculate dot position according to coord sys
					POINT coordPos = { 0 };
					coordPos.x = iterator->x - (clientRect.right / 2);
					coordPos.y = (iterator->y - (clientRect.bottom / 2)) * -1;
					wsprintf(coords, L"X: %d, Y: %d", coordPos.x, coordPos.y);
					::DrawTextW(hDc, coords, -1, &invalidRect, DT_SINGLELINE | DT_NOCLIP);
				}
				
				// draw -2 for left and top, +DOT_RADIUS for right and bottom
				::Ellipse(hDc, iterator->x - DOT_RADIUS, iterator->y - DOT_RADIUS, iterator->x + DOT_RADIUS, iterator->y + DOT_RADIUS);
			}

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

		case WM_LBUTTONDOWN: {
			POINT cursorPosition = { 0 };
			cursorPosition.x = (LONG)LOWORD(lParam);
			cursorPosition.y = (LONG)HIWORD(lParam);

			// add current dot position to all dots vector<> as two component vector
			dots.push_back(cursorPosition);
		}
		break;

		case WM_LBUTTONUP:
			// ask windows to redraw the client area
			if (!dots.empty()) {
				POINT dot = dots.back();
				RECT invalidRect = { dot.x - DOT_RADIUS, dot.y - DOT_RADIUS, dot.x + DOT_RADIUS, dot.y + DOT_RADIUS };
				::RedrawWindow(hWnd, &invalidRect, NULL, RDW_ERASE | RDW_INTERNALPAINT | RDW_INVALIDATE);
			}
		break;

		case WM_MOUSEMOVE: {
			if (wParam & MK_SHIFT) {
				cursorPositionTracked.x = (LONG)LOWORD(lParam);
				cursorPositionTracked.y = (LONG)HIWORD(lParam);
			} else {
				cursorPositionTracked = { 0 };
			}

			RECT invalidRect = { 0, 0, 100, 20 };
			::RedrawWindow(hWnd, &invalidRect, NULL, RDW_ERASE | RDW_INTERNALPAINT | RDW_INVALIDATE);
		}
		break;

		case WM_COMMAND:
			if (HIWORD(wParam) == 0) { // if high word of wParam is 0, message source is MENU
				switch (LOWORD(wParam)) {
					case IDM_OPERATIONS_NEW:
						dots.clear();
						::RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INTERNALPAINT | RDW_INVALIDATE);
					break;

					case IDM_OPERATIONS_EXIT:
						::PostMessageW(hWnd, WM_CLOSE, 0, 0);
					break;

					case IDM_HELP_ABOUT: {
						::DialogBox(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDialogProc);
					}
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

BOOL CALLBACK AboutDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG:
			return TRUE;
		break;

		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDOK:
					::EndDialog(hWnd, IDOK);
				break;
			}
		}
		break;

		case WM_CLOSE:
			::EndDialog(hWnd, IDOK);
		break;

		default:
			return FALSE;
		break;
	}

	return TRUE;
}

void showLastErrorAsMessageBox() {
	wchar_t errorBuff[256];
	::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, ::GetLastError(), 0l, errorBuff, 256, NULL);
	::MessageBoxExW(NULL, errorBuff, L"Fatal Error", MB_OK, 0l);
}
