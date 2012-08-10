// Utilities.cpp
// (c) 2012, John Peterson
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
#include <GuidString.h>
#include "Utilities.h"

CAtlList<CString> Util::m_szMetaFiles;

//
// OutputDebugString
//
// @param format Variable argument list.
//
void Util::OutputDebugStringEx(const wchar_t* format, ...) {
	wchar_t buffer[1024*8];
	va_list argptr;
	va_start(argptr, format);
	_vsnwprintf(buffer, 1024*8, format, argptr);
	va_end(argptr);
	OutputDebugString(buffer);
}

//
// GetLastError
//
void Util::GetLastErrorEx() {
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf,
		0,
		NULL);	
	OutputDebugString((LPWSTR)lpMsgBuf);
}

//
// FormatMessageEx
//
void Util::FormatMessageEx(DWORD dw) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf,
		0,
		NULL);	
	OutputDebugStringEx(L"0x%08x %s", dw, (LPWSTR)lpMsgBuf);
}

//
// GetVersionEx
//
int Util::GetVersionEx2() {
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	if (osvi.dwMajorVersion == 5) return 5;
	else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0) return 6;
	else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) return 7;
	else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) return 8;
	else return 9;
}

//
// Find current folder name
//
// @param szPath Path.
//
CString Util::PathFindFolderName(CString szPath) {
	szPath = szPath.Right(szPath.GetLength()-szPath.ReverseFind(L'\\')-1);
	return szPath;
}

//
// Find previous level path
//
// @param szPath Path.
// @return Previous level path.
//
CString Util::PathFindPreviousComponent(CString szPath) {
	szPath = szPath.Left(szPath.ReverseFind(L'\\'));
	return szPath;
}

//
// Rename folder
//
// @param szFrom Old name.
// @param szTo New name.
// @return Result code.
//
HRESULT Util::MoveFolderEx(CString& szFrom, CString& szTo) {
	if (szTo.IsEmpty()) {
		GuidString szGUID;
		szTo = PathFindPreviousComponent(szFrom) + L"\\" + PathFindFolderName(szFrom) + CString(szGUID.String().c_str());
	}
	if (!(GetFileAttributes(szFrom) & FILE_ATTRIBUTE_DIRECTORY)) {
		szTo = szFrom;
		return S_OK;
	}
	if (!MoveFileEx(szFrom, szTo, 0)) {
		OutputDebugStringEx(L"MOVE_FAILED: %s -> %s\n", szFrom, szTo);
		GetLastErrorEx();
		return E_FAIL;
	}
	return S_OK;
}

//
// Find file recursively
//
// @param _szPath Path.
// @param _szPath Filename to find.
// @return BOOL File found.
//
BOOL Util::PathFindFile(CString _szPath, CString _szFile, BOOL bRecursive) {
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
				if (!szFileName.CompareNoCase(_szFile)) goto ret;
				if (bRecursive)
					if (PathFindFile(_szPath + L"\\" + szFileName, _szFile)) goto ret;
			}			
		} else {
			if (!szFileName.CompareNoCase(_szFile) && !IsMetaFile(PathFindExtension(szFileName))) goto ret;			
		}
	} while (FindNextFile(hFind, &ffd));

	FindClose(hFind);
	return false;
ret:
	FindClose(hFind);
	return true;
}

//
// Is directory empty
//
// @param _szPath Path.
// @return BOOL Directory is empty.
//
BOOL Util::PathIsDirectoryEmptyEx(CString _szPath) {
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

//
// Indentify files by file ending
//
// @param fileEnding File ending.
// @return BOOL Is metafile.
//
BOOL Util::IsMetaFile(CString fileEnding) {
	fileEnding = fileEnding.MakeLower().Right(fileEnding.GetLength()-1);
	POSITION pos = m_szMetaFiles.GetHeadPosition();
	while(pos) {
		CString s = m_szMetaFiles.GetNext(pos);
		if (fileEnding.IsEmpty() && !s.Compare(L"*")) return true;
		if (!s.Compare(fileEnding)) return true;
	}
	return false;
}

//
// Read registry string
//
// @param szValue Registry value name.
// @return CString Registry value data.
//
CString Util::QueryStringValueEx(CString szValue) {
	CRegKey RegKey;
	TCHAR pszValue[1024];
	ULONG pnChars = 1024;
	if(RegKey.Open(HKEY_CURRENT_USER, L"Software\\LevelZap") != ERROR_SUCCESS) return L"";
	if(RegKey.QueryStringValue(szValue, pszValue, &pnChars) != ERROR_SUCCESS) return L"";
	if(RegKey.Close() != ERROR_SUCCESS) return L"";
	return CString(pszValue);
}

//
// Read registry dword
//
// @param szValue Registry value name.
// @return DWORD Registry value data.
//
DWORD Util::QueryDWORDValueEx(CString szValue) {
	CRegKey RegKey;
	DWORD dwValue = 0;
	if(RegKey.Open(HKEY_CURRENT_USER,  L"Software\\LevelZap") != ERROR_SUCCESS) return 0;
	if(RegKey.QueryDWORDValue(szValue, dwValue) != ERROR_SUCCESS) return 0;
	if(RegKey.Close() != ERROR_SUCCESS) return 0;
	return dwValue;
}

//
// Read registry array
//
// @param szValue Registry value name.
// @param szArr Registry value data.
// @return LONG Result code.
//
LONG Util::QueryMultiStringValueEx(CString szValue, CAtlList<CString>& szArr) {
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