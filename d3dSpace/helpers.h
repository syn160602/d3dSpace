#pragma once
#include "myWin.h" 
#include <sstream>

inline LPCWSTR _string2LPCWSTR(std::string orig)
{
	wchar_t* wcstring = 0;
	try
	{
		size_t origsize = orig.length() + 1;
		size_t convertedChars = 0;
		if (orig == "")
		{
			wcstring = (wchar_t*)malloc(0);
			mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		}
		else
		{
			wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
			mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		}
	}

	catch (std::exception e)
	{
	}

	return wcstring;
}

inline std::string _LPCWSTR2string(LPCWSTR lpcwszStr)
{
	char* str;
	size_t converted = 0;
	size_t orisize = wcslen(lpcwszStr);
	if (lpcwszStr == L"")
	{
		return "";
	}
	else
	{
		str = (char*)malloc(sizeof(char) * orisize + 1);
		wcstombs_s(&converted, str, orisize + 1, lpcwszStr, orisize);
	}
	return str;
}

inline WCHAR* char2wchar(const char* chr)
{
	WCHAR* wcstring = 0;
	try
	{
		size_t origsize = strlen(chr) + 1;
		size_t convertedChars = 0;
		if (chr == "")
		{
			wcstring = (wchar_t*)malloc(0);
			mbstowcs_s(&convertedChars, wcstring, origsize, chr, _TRUNCATE);
		}
		else
		{
			wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (strlen(chr) - 1));
			mbstowcs_s(&convertedChars, wcstring, origsize, chr, _TRUNCATE);
		}
	}

	catch (std::exception e)
	{
	}

	return wcstring;
}