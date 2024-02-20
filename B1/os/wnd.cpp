#include "Window.h"

#ifdef _WIN32
#include <windows.h>
#include <WinUser.h>
#include <wrl.h>

#include <iostream>
#include "wnd.h"


class wndWindow : public Window {
public:
	bool Create(const std::string& title, int width, int height) override;
	void Destroy() override;

	HWND hWnd;
};


Window* osCreateWindow(const std::string& title, int width, int height) 
{
	auto window = new wndWindow();
	if (!window->Create(title, width, height)) {
		return nullptr;
	}
	return window;
}


void osDestroyWindow(Window* window)
{
	window->Destroy();
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage) {
	case WM_KEYDOWN: {
		// If a key is pressed send it to the input object so it can record that state.
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default: {
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}


bool wndWindow::Create(const std::string& title, int _width, int _height)
{
	width = _width;
	height = _height;

	std::wstring appName(title.begin(), title.end());
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	WNDCLASSEX wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = appName.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	RECT windowRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, appName.c_str(), appName.c_str(),
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	if (hWnd == nullptr) {
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	ShowCursor(true);

	return true;
}

void wndWindow::Destroy()
{
	DestroyWindow(hWnd);
}

HWND wndGetHWND(Window* window)
{
	wndWindow* wndW = static_cast<wndWindow*>(window);
	return wndW->hWnd;
}
#endif _WIN32
