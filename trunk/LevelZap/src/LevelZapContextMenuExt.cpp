// LevelZapContextMenuExt.cpp
// (c) 2011, Charles Lechasseur. 2012, John Peterson.
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
#include "LevelZapContextMenuExt.h"

#include <StStgMedium.h>
#include <ArrayAutoPtr.h>
#include <Dbghelp.h>

#include <assert.h>
#include <sstream>

// CLevelZapContextMenuExt

//
// Constructor.
//
CLevelZapContextMenuExt::CLevelZapContextMenuExt()
    : m_vFolders(),
      m_FirstCmdId(),
      m_ZapCmdId()
{
	Util::m_szMetaFiles.RemoveAll();
}

//
// IShellExtInit::Initialize
//
// Called by the shell to initialize our contextual menu extension.
// We need to use this opportunity to look at the folders to act upon.
//
// @param p_pFolderPIDL Pointer to ITEMIDLIST representing selected folder; unused.
// @param p_pDataObject Pointer to data object containing information about selected folders.
// @param p_hKeyFileClass Handle to file class key; unused.
// @return S_OK if successful, otherwise an error code.
//
STDMETHODIMP CLevelZapContextMenuExt::Initialize(
    PCIDLIST_ABSOLUTE /*p_pFolderPIDL*/,
    IDataObject *p_pDataObject,
    HKEY /*p_hKeyFileClass*/)
{
    HRESULT hRes = S_OK;

    try {
        // Make sure we have a data object.
        if (p_pDataObject != 0) {
            // Extract HDROP from data object.
            StStgMedium stgMedium;
            FORMATETC formatEtc = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
            if (SUCCEEDED(p_pDataObject->GetData(&formatEtc, &stgMedium))) {
                // Get number of folders included in the selection.
                UINT folderCount = ::DragQueryFileW(
                    static_cast<HDROP>(stgMedium.Get().hGlobal), 0xFFFFFFFF, 0, 0);
                if (folderCount > 0) {
                    // Pre-allocate space in vector to store folders.
                    m_vFolders.reserve(folderCount);

                    // Get each file in turn.
                    wchar_t buffer[MAX_PATH + 1];
                    for(UINT i = 0; i < folderCount; ++i) {
                        UINT copiedCount = ::DragQueryFileW(static_cast<HDROP>(stgMedium.Get().hGlobal),
                            i, buffer, sizeof(buffer) / sizeof(wchar_t));
                        m_vFolders.push_back(CString(buffer, copiedCount));
                    }
                } else {
                    // It's difficult to display a menu item without files to act upon.
                    hRes = E_FAIL;
                }
            }
        } else {
            hRes = E_POINTER;
        }
    } catch (...) {
        hRes = E_UNEXPECTED;
    }

    return hRes;
}

//
// IContextMenu::QueryContextMenu
//
// Invoked by the shell to populate a contextual menu.
// We need to use this opportunity to add our menu item.
//
// @param p_hMenu Handle to the contextual menu to populate.
// @param p_Index Index in the menu where to add items.
// @param p_FirstCmdId First available command ID for our commands.
// @param p_LastCmdId Last available command ID for our commands.
// @param p_Flags Flags indicating how the menu can be changed; see MSDN for details.
// @return If successful, a success code with code value set to the largest command ID
//         used, plus one; otherwise, an error code.
//
STDMETHODIMP CLevelZapContextMenuExt::QueryContextMenu(
    HMENU p_hMenu,
    UINT p_Index,
    UINT p_FirstCmdId,
    UINT p_LastCmdId,
    UINT p_Flags)
{
    HRESULT hRes = S_OK;

    try {
        if (p_hMenu == 0) {
            hRes = E_INVALIDARG;
        } else {
            // Do not add items if the default action is chosen or if we have no folders.
            if (m_vFolders.empty() || (p_Flags & CMF_DEFAULTONLY) != 0) {
                hRes = E_FAIL;
            } else {
                UINT cmdId = p_FirstCmdId;
                UINT position = p_Index;

                // Insert "zap" menu item. We have only one so it's pretty easy.
                CString zapMenuDesc(MAKEINTRESOURCE(IDS_ZAP_MENU_ITEM_DESCRIPTION));
                if (::InsertMenu(p_hMenu, position, MF_STRING | MF_BYPOSITION, cmdId, zapMenuDesc)) {
                    m_FirstCmdId = cmdId;
                    m_ZapCmdId = cmdId;
                    ++cmdId;
                    ++position;
                } else {
                    hRes = E_FAIL;
                }

                if (SUCCEEDED(hRes)) {
                    // Strange return value requirement... see MSDN for details.
                    hRes = MAKE_HRESULT(SEVERITY_SUCCESS, 0, cmdId - p_FirstCmdId + 1);
                }
            }
        }
    } catch (...) {
        hRes = E_UNEXPECTED;
    }

    return hRes;
}

//
// IContextMenu::InvokeCommand
//
// Invoked by the shell when the user selects one of our menu items.
// We do our stuff here.
//
// @param p_pCommandInfo Pointer to struct containing command information.
// @return S_OK if successful, otherwise an error code.
//
STDMETHODIMP CLevelZapContextMenuExt::InvokeCommand(
    CMINVOKECOMMANDINFO* p_pCommandInfo)
{
    HRESULT hRes = S_OK;
	m_bRecursive = (GetKeyState(VK_CONTROL)&0x80);
	if (m_bRecursive) {
		// Confirm action
		if (!Dialog::doModal(0, Util::GetVersionEx2()>=6?IDS_ZAP_LEVEL:IDS_ZAP_CONFIRM_OLD)) return E_ABORT;
		Util::QueryMultiStringValueEx(L"metaFiles", Util::m_szMetaFiles);
	}
	m_szMetaDir = Util::QueryStringValueEx(L"MetaDir"); if (m_szMetaDir.IsEmpty()) m_szMetaDir = L"_meta";

    try {
        if ((p_pCommandInfo == 0) || (p_pCommandInfo->cbSize < sizeof(CMINVOKECOMMANDINFO))) {
            hRes = E_INVALIDARG;
        } else {
            // Get offset of invoked command.
            UINT cmdOffset = (UINT) p_pCommandInfo->lpVerb;
            if ((cmdOffset & 0xFFFF0000) != 0) {
                // We do not support verb invokation.
                hRes = E_FAIL;
            } else if (!m_FirstCmdId.HasValue()) {
                // We do not have menu items, so we can't invoke anything.
                hRes = E_INVALIDARG;
            } else {
                // Check which command it is that is invoked.
                UINT cmdId = cmdOffset + m_FirstCmdId;
                if (m_ZapCmdId.HasValue() && m_ZapCmdId == cmdId) {
                    // Zap everything.
                    hRes = ZapAllFolders((p_pCommandInfo->fMask & CMIC_MASK_FLAG_NO_UI) == 0 ? p_pCommandInfo->hwnd : 0);
                } else {
                    // Invalid command ID.
                    hRes = E_INVALIDARG;
                }
            }
        }
    } catch (...) {
        hRes = E_UNEXPECTED;
    }

	Util::OutputDebugStringEx(L"RETURN 0x%08x\n", hRes);
    return hRes;
}

//
// IContextMenu::GetCommandString
//
// Invoked by the shell to get a text description for our menu item.
//
// @param p_CmdId Offset of ID of command for which to query text, relative
//                to our first command ID.
// @param p_Flags Type of information or action requested; see MSDN for details.
// @param p_pReserved Reserved; unused.
// @param p_pBuffer Pointer to memory buffer where to copy a null-terminated string.
//                  Note: if GCS_UNICODE is found in p_Flags, the string must
//                  be cast to a LPWSTR and a Unicode string must be stored there.
// @param p_BufferSize Max size of p_pBuffer, in characters. 
// @return S_OK if successful, otherwise an error code.
//
STDMETHODIMP CLevelZapContextMenuExt::GetCommandString(
    UINT_PTR p_CmdId,
    UINT p_Flags,
    UINT* p_pReserved,
    LPSTR p_pBuffer,
    UINT p_BufferSize)
{
    HRESULT hRes = S_OK;

    try {
        // Check what is requested.
        if ((p_Flags == GCS_VERBA) || (p_Flags == GCS_VERBW)) {
            // We do not support verb invokation.
            hRes = E_NOTIMPL;
        } else if ((p_Flags == GCS_VALIDATEA) || (p_Flags == GCS_VALIDATEW)) {
            // We need to validate command ID.
            if (m_FirstCmdId.HasValue() && m_ZapCmdId.HasValue() && m_ZapCmdId == (m_FirstCmdId + p_CmdId)) {
                hRes = S_OK;
            } else {
                hRes = S_FALSE;
            }
        } else if (p_Flags == GCS_HELPTEXTA) {
            // Call this method to get the Unicode version.
            ArrayAutoPtr<wchar_t> wBuffer(new wchar_t[p_BufferSize]);
            hRes = this->GetCommandString(p_CmdId,
                                          GCS_HELPTEXTW,
                                          p_pReserved,
                                          reinterpret_cast<LPSTR>(wBuffer.Get()),
                                          p_BufferSize);
            if (SUCCEEDED(hRes)) {
                // Convert it to a single-byte string and return it.
                CStringA aBuffer(wBuffer.Get());
                if (::strcpy_s(p_pBuffer, p_BufferSize, aBuffer) == 0) {
                    hRes = S_OK;
                } else {
                    hRes = E_FAIL;
                }
            }
        } else if (p_Flags == GCS_HELPTEXTW) {
            // A Unicode help string is requested.
            if (p_pBuffer != 0) {
                if (m_FirstCmdId.HasValue() && m_ZapCmdId.HasValue() && m_ZapCmdId == (m_FirstCmdId + p_CmdId)) {
                    // Return help text for our zap item.
                    CStringW zapItemHint(MAKEINTRESOURCE(IDS_ZAP_MENU_ITEM_HINT));
                    if (::wcscpy_s((LPWSTR) p_pBuffer, p_BufferSize, zapItemHint) != 0) {
                        hRes = E_FAIL;
                    }
                } else {
                    hRes = E_INVALIDARG;
                }
            } else {
                hRes = E_INVALIDARG;
            }
        } else {
            // Unknown, unsupported flag.
            hRes = E_NOTIMPL;
        }
    } catch (...) {
        hRes = E_UNEXPECTED;
    }

    return hRes;
}

//
// ZapAllFolders
//
// Called when the contextual menu item is chosen. We scan all folders
// we found at initialization time and "zap"'em.
//
// @param p_hParentWnd Handle of parent window for dialog boxes.
//                     If this is set to 0, we will not show any UI.
// @return Result code.
//
HRESULT CLevelZapContextMenuExt::ZapAllFolders(const HWND p_hParentWnd) const
{
    HRESULT hRes = S_OK;
    bool yesToAll = !Util::QueryDWORDValueEx(L"PromptUser");
    FolderV::const_iterator it, end = m_vFolders.end();
    for (it = m_vFolders.begin(); it != end; ++it) {
		if (GetFileAttributes(*it)&FILE_ATTRIBUTE_DIRECTORY || m_bRecursive)
			hRes = ZapFolder(p_hParentWnd, *it, yesToAll);
	}
    return hRes;
}

//
// ZapFolder
//
// Moves the entire content of the given directory up one level and then "zaps" the directory.
//
// @param p_hParentWnd Handle of parent window for dialog boxes.
//                     If this is set to 0, we will not show any UI.
// @param p_Folder Folder path.
// @param p_rYesToAll true if user chose to answer "Yes" to all confirmations.
// @return Result code.
//
HRESULT CLevelZapContextMenuExt::ZapFolder(const HWND p_hParentWnd,
                                           CString p_Folder,
                                           bool& p_rYesToAll) const {
	CString folderName = Util::PathFindFolderName(p_Folder);
	// Check folder name
	if (!folderName.Compare(m_szMetaDir) && m_bRecursive) return E_FAIL;
	// Ask for confirmation.
	CString confirmMsg1(MAKEINTRESOURCE(IDS_ZAP_CONFIRM_MESSAGE_1));
	CString confirmMsg2(MAKEINTRESOURCE(IDS_ZAP_CONFIRM_MESSAGE_2));
	CString confirmMsgComplete = confirmMsg1 + folderName + confirmMsg2;
	CString confirmMsgOld1(MAKEINTRESOURCE(IDS_ZAP_CONFIRM_MESSAGE_OLD_1));
	CString confirmMsgOld2(MAKEINTRESOURCE(IDS_ZAP_CONFIRM_MESSAGE_OLD_2));
	CString confirmMsgCompleteOld = confirmMsgOld1 + folderName + confirmMsgOld2;
    if (!p_rYesToAll && !m_bRecursive)
		if (!Dialog::doModal(p_hParentWnd, Util::GetVersionEx2()>=6?confirmMsgComplete.GetBuffer():confirmMsgCompleteOld.GetBuffer())) return E_ABORT;
	// Check for name collission
	BOOL bRename = Util::PathFindFile(p_Folder, Util::PathFindFolderName(p_Folder), m_bRecursive);
	CString _p_Folder(p_Folder);
	if (bRename) p_Folder.Empty();
	if (bRename) if (!SUCCEEDED(Util::MoveFolderEx(_p_Folder, p_Folder))) return E_FAIL;
	CString szlFrom, szlTo;
	if (!SUCCEEDED(FindFiles(p_hParentWnd, Util::PathFindPreviousComponent(p_Folder), p_Folder, szlFrom, szlTo))) {
		if (bRename) Util::MoveFolderEx(p_Folder, _p_Folder);
		return E_FAIL;
	}
	// Move files
	if (SUCCEEDED(MoveFile(p_hParentWnd, szlFrom, szlTo))) {
		// Delete source directory
		DeleteFolder(p_hParentWnd, p_Folder);
		return S_OK;
	} else
        return E_FAIL;
    return S_OK;
}

//
// FindFiles
//
// Populate recursive file list
//
HRESULT CLevelZapContextMenuExt::FindFiles(const HWND p_hParentWnd,
											CString szTo,
											CString szFromPath,
											CString& szlFrom,
											CString& szlTo) const {
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	CString szPath;

   	// File
	hFind = FindFirstFile(szFromPath, &ffd);
	if (INVALID_HANDLE_VALUE == hFind) {
		Util::OutputDebugStringEx(L"INVALID_HANDLE_VALUE: %s\n", szFromPath);
		FindClose(hFind);
		return E_FAIL;
	}
	if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		if (!Util::IsMetaFile(PathFindExtension(szFromPath))) return E_FAIL;
		szlFrom.Append(szFromPath); szlFrom.AppendChar('\0');
		szlTo.Append(szTo); szlTo.Append(L"\\");
		szlTo.Append(m_szMetaDir); szlTo.Append(L"\\");
		szlTo.Append(ffd.cFileName); szlTo.AppendChar('\0');
		FindClose(hFind);
		return S_OK;
	}
	FindClose(hFind);

	// Directory
	szPath.Append(szFromPath);
	szPath.Append(L"\\*");
	hFind = FindFirstFile(szPath, &ffd);
	if (INVALID_HANDLE_VALUE == hFind) {
		Util::OutputDebugStringEx(L"INVALID_HANDLE_VALUE: %s\n", szPath);
		FindClose(hFind);
		return E_FAIL;
	}	
	do {
		CString szFileName(ffd.cFileName);
		szPath.Empty();
		szPath.Append(szPath);
		szPath.Append(szFromPath);
		szPath.Append(L"\\");
		szPath.Append(szFileName);
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (szFileName.Compare(L".") && szFileName.Compare(L"..")) {
				if (m_bRecursive) {
					Util::OutputDebugStringEx(L"Folder %s\n", szPath);
					FindFiles(p_hParentWnd, szTo, szPath, szlFrom, szlTo);					
				} else {
					szlFrom.Append(szPath); szlFrom.AppendChar('\0');
					szlTo.Append(szTo + L"\\" + szFileName); szlTo.AppendChar('\0');
					Util::OutputDebugStringEx(L"    Move %s -> %s\n", szPath, szTo + L"\\" + szFileName);
				}
			}
		}
		else
		{
			szlFrom.Append(szPath); szlFrom.AppendChar('\0');
			CString _szTo;
			_szTo.Append(szTo);
			if (Util::IsMetaFile(PathFindExtension(szPath))) { _szTo.Append(L"\\"); _szTo.Append(m_szMetaDir); }
			_szTo.Append(L"\\"); _szTo.Append(szFileName);
			szlTo.Append(_szTo); szlTo.AppendChar('\0');
			Util::OutputDebugStringEx(L"    Move %s -> %s\n", szPath, _szTo);
		}
	} while (FindNextFile(hFind, &ffd));

	FindClose(hFind);
	return S_OK;
}

//
// MoveFile
//
// Move file(s)
//
HRESULT CLevelZapContextMenuExt::MoveFile(const HWND p_hParentWnd,
											CString p_Path,
											CString p_FolderTo) const {
	if (p_Path.IsEmpty()) return S_OK;
	SHFILEOPSTRUCT fileOpStruct = {0};
	fileOpStruct.hwnd = p_hParentWnd;
	fileOpStruct.wFunc = FO_MOVE;
	p_Path.AppendChar(L'\0'); fileOpStruct.pFrom = p_Path;
	p_FolderTo.AppendChar(L'\0'); fileOpStruct.pTo = p_FolderTo;
	fileOpStruct.fFlags = FOF_MULTIDESTFILES | FOF_ALLOWUNDO | FOF_SILENT;
	if (p_hParentWnd == 0) fileOpStruct.fFlags |= (FOF_NOCONFIRMATION | FOF_NOERRORUI);
	int hRes = SHFileOperation(&fileOpStruct);
	if (fileOpStruct.fAnyOperationsAborted) hRes = E_ABORT;
	Util::OutputDebugStringEx(L"Move 0x%08x | %s -> %s\n", hRes, p_Path, p_FolderTo);
	return hRes;
}

//
// DeleteFolder
//
// Delete folder
//
HRESULT CLevelZapContextMenuExt::DeleteFolder(const HWND p_hParentWnd,
											CString p_Path) const {
	SHFILEOPSTRUCT fileOpStruct = {0};
	fileOpStruct.hwnd = p_hParentWnd;
	fileOpStruct.wFunc = FO_DELETE;	
	fileOpStruct.pTo = 0;
	fileOpStruct.fFlags = FOF_ALLOWUNDO | FOF_WANTNUKEWARNING | FOF_SILENT;
	if (Util::PathIsDirectoryEmptyEx(p_Path)) fileOpStruct.fFlags |= FOF_NOCONFIRMATION;
	p_Path.AppendChar(L'\0'); fileOpStruct.pFrom = p_Path;
	if (p_hParentWnd == 0) fileOpStruct.fFlags |= FOF_NOERRORUI;
	int hRes = SHFileOperation(&fileOpStruct);
	Util::OutputDebugStringEx(L"Delete 0x%08x | %s", hRes, p_Path);
	return hRes;
}