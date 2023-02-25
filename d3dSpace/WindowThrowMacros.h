#pragma once

#define MHWND_EXCEPT(hr) Window::HrException(__LINE__, __FILE__, hr)
#define MHWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILE__, GetLastError())
#define MHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILE__)