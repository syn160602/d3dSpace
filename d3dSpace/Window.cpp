#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowThrowMacros.h"
#include "imgui/imgui_impl_win32.h"

#include "helpers.h"


// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON, 128, 128, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = _string2LPCWSTR(GetName());
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(_string2LPCWSTR(wndClassName), GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


// Window Stuff
Window::Window(int width, int height, const char* name)
	:
	width(width),
	height(height)
{
	// calculate window size based on desired client region size
	RECT wr;
	wr.left = 10;
	wr.right = width + wr.left;
	wr.top = 10;
	wr.bottom = height + wr.top;
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw MHWND_LAST_EXCEPT();
	}

	// create window & get hWnd
	hWnd = CreateWindow(
		_string2LPCWSTR(WindowClass::GetName()), _string2LPCWSTR(name),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	// check error
	if (hWnd == nullptr)
		throw MHWND_LAST_EXCEPT();
	// newly created windows start off as hidden
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	// Init ImGui Win32 Impl
	ImGui_ImplWin32_Init(hWnd);
	// create graphics object
	pGfx = std::make_unique<Graphics>(hWnd, width, height);
	// register mouse raw input device
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw MHWND_LAST_EXCEPT();
	}
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowText(hWnd, _string2LPCWSTR(title.c_str())) == 0)
	{
		throw MHWND_LAST_EXCEPT();
	}
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

bool Window::CursorEnabled() const noexcept
{
	return cursorEnabled;
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	// while queue has messages, remove and dispatch them (but do not block on empty queue)
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
	{
		// check for quit because peekmessages does not signal this via return
		if (msg.message == WM_QUIT)
		{
			// return optional wrapping int (arg of PostQuitMessage is in wParam) signals quit
			return (int)msg.wParam;
		}

		// TranslateMessage will post auxilliary WM_CHAR message from key msgs
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return empty optional when not quitting app
	return {};
}

Graphics& Window::Gfx()
{
	if (!pGfx) {
		throw MHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

void Window::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::HideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0);
}

void Window::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if (msg == WM_NCCREATE)
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window instance
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward message to window instance handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve ptr to window instance
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward message to window instance handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input getting "stuck"
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	case WM_ACTIVATE:
		// confine/free cursor on window to foreground/background if cursor disabled
		if (!cursorEnabled)
		{
			if (wParam & WM_ACTIVATE)
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;

		/******** KEYBOARD MESSAGES ********/
	case WM_KEYDOWN:
		// syskey commands need to be handled
	case WM_SYSKEYDOWN:
		// strifle this keyboard message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled())	// filter autorepeat
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// strifle this keyboard message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/****** END KEYBOARD MESSAGES ******/

		/********** MOUSE MESSAGES *********/
	case WM_MOUSEMOVE:
	{
		// strifle this mouse message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		const POINTS pt = MAKEPOINTS(lParam);
		// in client region -> log move, and log enter + capture mouse if not previously in window)
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) {
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow()) {
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// not in client region -> log move & maintain capture if button down
		else {
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
				mouse.OnMouseMove(pt.x, pt.y);
			// button up -> release capture & log event for leaving
			else {
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		// bring window to foreground on lclick client region
		SetForegroundWindow(hWnd);
		if (!cursorEnabled)
		{
			ConfineCursor();
			HideCursor();
		}
		// strifle this mouse message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;

		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// strifle this mouse message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		// strifle this mouse message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		// strifle this mouse message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// strifle this mouse message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
			break;
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	/******* END MOUSE MESSAGES ********/

	/******* RAW MOUSE MESSAGES ********/
	case WM_INPUT:
	{
		if (!mouse.RawEnabled())
		{
			break;
		}
		UINT size;
		// first get the size of input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)
		) == -1)
		{
			break;
		}
		rawBuffer.resize(size);
		// read in the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)
		) == -1)
		{
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	/***** END RAW MOUSE MESSAGES ******/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Window Exception Stuff
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Window Exception";
}


HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Window Exception [No Graphics]";
}