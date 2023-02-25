#include "myWin.h" 
#include "WindowsMessageMap.h"
#include <sstream>
#include "App.h"

#include "helpers.h"


int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpComLine,
	int       nComShow)
{
	try
	{
		return App().Go();
	}
	catch (const myException& e)
	{
		MessageBox(nullptr, _string2LPCWSTR(e.what()), _string2LPCWSTR(e.GetType()), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, _string2LPCWSTR(e.what()), L"Standard Exceprion", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}