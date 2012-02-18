// LevelZap.cpp
// (c) 2011, Charles Lechasseur
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "stdafx.h"
#include "resource.h"
#include "LevelZap_i.h"
#include "LevelZapTypes.h"
#include "dllmain.h"
#include "xdlldata.h"
#include <GuidString.h>

// Used to determine whether the DLL can be unloaded by OLE.
STDAPI DllCanUnloadNow(void)
{
	#ifdef _MERGE_PROXYSTUB
	HRESULT hr = PrxDllCanUnloadNow();
	if (hr != S_OK)
		return hr;
#endif
			return _AtlModule.DllCanUnloadNow();
	}

// Returns a class factory to create an object of the requested type.
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	#ifdef _MERGE_PROXYSTUB
	if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
		return S_OK;
#endif
		return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - Adds entries to the system registry.
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _AtlModule.DllRegisterServer();
	#ifdef _MERGE_PROXYSTUB
	if (FAILED(hr))
		return hr;
	hr = PrxDllRegisterServer();
#endif
		return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer();
	#ifdef _MERGE_PROXYSTUB
	if (FAILED(hr))
		return hr;
	hr = PrxDllRegisterServer();
	if (FAILED(hr))
		return hr;
	hr = PrxDllUnregisterServer();
#endif
		return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}

// OutputDebugString
void OutputDebugStringEx(const wchar_t* format, ...) {
	wchar_t buffer[1024*8];
	va_list argptr;
	va_start(argptr, format);
	_vsnwprintf(buffer, 1024*8, format, argptr);
	va_end(argptr);
	OutputDebugString(buffer);
}

// Find current folder name
CString PathFindFolderName(CString szPath) {
	szPath = szPath.Right(szPath.GetLength()-szPath.ReverseFind(L'\\')-1);
	return szPath;
}

// Find previous level path
CString PathFindPreviousComponent(CString szPath) {
	szPath = szPath.Left(szPath.ReverseFind(L'\\'));
	return szPath;
}

// Rename folder
HRESULT MoveFolderEx(CString& szFrom, CString& szTo) {
	if (szTo.IsEmpty()) {
		GuidString szGUID;
		szTo = PathFindPreviousComponent(szFrom) + L"\\" + CString(szGUID.String().c_str());
	}
	if (!(GetFileAttributes(szFrom) & FILE_ATTRIBUTE_DIRECTORY)) {
		szTo = szFrom;
		return S_OK;
	}
	if (!MoveFileEx(szFrom, szTo, 0)) {
		OutputDebugStringEx(L"MOVE_FAILED: %s -> %s\n", szFrom, szTo);
		return E_FAIL;
	}
	return S_OK;
}

// Find file recursively
BOOL PathFindFile(CString _szPath, CString _szFile) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind;

	hFind = FindFirstFile(_szPath + L"\\*", &ffd);
	if (INVALID_HANDLE_VALUE == hFind) {
		OutputDebugStringEx(L"INVALID_HANDLE_VALUE: %s\n", _szPath);
		FindClose(hFind);
		return false;
	}	
	do {
		CString szFileName(ffd.cFileName);
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (szFileName.Compare(L".") && szFileName.Compare(L"..")) {
				if(PathFindFile(_szPath + L"\\" + szFileName, _szFile)) { FindClose(hFind); return true; }
			}			
		} else {
			if (!szFileName.CompareNoCase(_szFile) && !IsMetaFile(PathFindExtension(szFileName))) { FindClose(hFind); return true; }			
		}
	} while (FindNextFile(hFind, &ffd));

	FindClose(hFind);
	return false;
}

// Is directory empty
BOOL PathIsDirectoryEmptyEx(CString _szPath) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind;

	hFind = FindFirstFile(_szPath + L"\\*", &ffd);
	if (INVALID_HANDLE_VALUE == hFind) {
		OutputDebugStringEx(L"INVALID_HANDLE_VALUE: %s\n", _szPath);
		FindClose(hFind);
		return false;
	}
	do {
		CString szFileName(ffd.cFileName);
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (szFileName.Compare(L".") && szFileName.Compare(L"..")) {
				if(!PathIsDirectoryEmptyEx(_szPath + L"\\" + szFileName)) { FindClose(hFind); return false; }
			}
		} else { FindClose(hFind); return false; }
	} while (FindNextFile(hFind, &ffd));

	FindClose(hFind);
	return true;
}

// Indentify files by file ending
BOOL IsMetaFile(CString fileEnding) {
	if (fileEnding.IsEmpty()) return true;
	CAtlList<CString> sl;
	QueryMultiStringValueEx(L"metaFiles", sl);
	fileEnding = fileEnding.MakeLower().Right(fileEnding.GetLength()-1);
	POSITION pos = sl.GetHeadPosition();
	while(pos) {
		if (!sl.GetNext(pos).Compare(fileEnding)) return true;
	}
	return false;
}

// Read registry string
CString QueryStringValueEx(CString szValue) {
	CRegKey RegKey;
	TCHAR pszValue[1024];
	ULONG pnChars = 1024;
	if(RegKey.Open(HKEY_CURRENT_USER, L"Software\\LevelZap") != ERROR_SUCCESS) return L"";
	if(RegKey.QueryStringValue(szValue, pszValue, &pnChars) != ERROR_SUCCESS) return L"";
	if(RegKey.Close() != ERROR_SUCCESS) return L"";
	return CString(pszValue);
}

// Read registry dword
DWORD QueryDWORDValueEx(CString szValue) {
	CRegKey RegKey;
	DWORD dwValue = 0;
	if(RegKey.Open(HKEY_CURRENT_USER,  L"Software\\LevelZap") != ERROR_SUCCESS) return 0;
	if(RegKey.QueryDWORDValue(szValue, dwValue) != ERROR_SUCCESS) return 0;
	if(RegKey.Close() != ERROR_SUCCESS) return 0;
	return dwValue;
}

// Read registry array
LONG QueryMultiStringValueEx(CString szValue, CAtlList<CString>& szArr) {
	CRegKey RegKey;
	LONG lRet;
	CAtlArray<WCHAR> pszValue;
	ULONG pnChars = 0;

	if (lRet = RegKey.Open(HKEY_CURRENT_USER,  L"Software\\LevelZap") != ERROR_SUCCESS) return lRet;
	if (lRet = RegKey.QueryMultiStringValue(szValue, NULL, &pnChars) != ERROR_SUCCESS) return lRet;
	pszValue.SetCount(pnChars);
	if (lRet = RegKey.QueryMultiStringValue(szValue, pszValue.GetData(), &pnChars) != ERROR_SUCCESS) return lRet;
	if (lRet = RegKey.Close() != ERROR_SUCCESS) return lRet;

    LPCWSTR lpEnd  = pszValue.GetData() + pszValue.GetCount();
    LPCWSTR lpNext = pszValue.GetData();
    while (lpNext && lpNext < lpEnd && *lpNext) {
        szArr.AddTail(lpNext);
        lpNext = std::find(lpNext, lpEnd, L'\0');
        if (lpNext == lpEnd) break;
        ++lpNext;
    }
    return ERROR_SUCCESS;
}