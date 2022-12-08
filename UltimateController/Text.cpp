#include <windows.h>
#include "Text.h"


namespace cli
{
	std::wstring toWstring(const std::string& str)
	{
		LPCSTR pszSrc = str.c_str();
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, NULL, 0);
		if (nLen == 0)
			return std::wstring(L"");

		wchar_t* pwszDst = new wchar_t[nLen];
		if (!pwszDst)
			return std::wstring(L"");

		MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, pwszDst, nLen);
		std::wstring wstr(pwszDst);
		delete[] pwszDst;
		pwszDst = NULL;

		return wstr;
	}

	std::string toString(const std::wstring& wstr)
	{
		std::string result;
		//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
		int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
		char* buffer = new char[len + 1];
		//宽字节编码转换成多字节编码  
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
		buffer[len] = '\0';
		//删除缓冲区并返回值  
		result.append(buffer);
		delete[] buffer;

		return result;
	}

}