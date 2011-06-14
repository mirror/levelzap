// LevelZapContextMenuExt.cpp
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
#include "LevelZapContextMenuExt.h"

#include <StStgMedium.h>
#include <ArrayAutoPtr.h>
#include <GuidString.h>

#include <assert.h>


// CLevelZapContextMenuExt

//
// Constructor.
//
CLevelZapContextMenuExt::CLevelZapContextMenuExt()
    : m_vFolders(),
      m_FirstCmdId(),
      m_ZapCmdId()
{
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
                        m_vFolders.push_back(std::wstring(buffer, copiedCount));
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
                    hRes = ZapAllFolders();
                } else {
                    // Invalid command ID.
                    hRes = E_INVALIDARG;
                }
            }
        }
    } catch (...) {
        hRes = E_UNEXPECTED;
    }

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
                    CStringW zapItemHint(MAKEINTRESOURCEW(IDS_ZAP_MENU_ITEM_HINT));
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
// @return Result code.
//
HRESULT CLevelZapContextMenuExt::ZapAllFolders() const
{
    HRESULT hRes = S_OK;
    FolderV::const_iterator it, end = m_vFolders.end();
    for (it = m_vFolders.begin(); SUCCEEDED(hRes) && it != end; ++it) {
        hRes = ZapFolder(*it);
    }
    return hRes;
}

//
// ZapFolder
//
// Copies the entire content of the given directory up one level and then "zaps" the directory.
//
// @param p_Folder Folder path.
// @return Result code.
//
HRESULT CLevelZapContextMenuExt::ZapFolder(const std::wstring& p_Folder) const
{
    HRESULT hRes = E_UNEXPECTED;

    // First rename the given folder to a random name so that the move will work
    // if it contains another folder with the same name. (You know, like those ZIP files do.)
    std::wstring::size_type lastDelim = p_Folder.find_last_of(L"\\/");
    if (lastDelim != std::wstring::npos) {
        GuidString randomizedName;
        std::wstring randomizedPath(p_Folder);
        randomizedPath.replace(lastDelim + 1, (randomizedPath.size() - lastDelim) - 1, randomizedName.String());
        if (::MoveFileExW(p_Folder.c_str(), randomizedPath.c_str(), 0)) {
            // Move all files and folders in the randomized folder up one level. This is easier than it actually sounds...
            std::wstring randomizedFrom(randomizedPath);
            randomizedFrom.append(L"\\*\0", 3);
            assert(randomizedFrom.size() == randomizedPath.size() + 3);
            std::wstring randomizedTo(randomizedPath, 0, lastDelim);
            randomizedTo.append(L"\\\0", 2);
            assert(randomizedTo.size() == lastDelim + 2);
            SHFILEOPSTRUCTW fileOpStruct = { 0 };
            fileOpStruct.wFunc = FO_MOVE;
            fileOpStruct.pFrom = randomizedFrom.c_str();
            fileOpStruct.pTo = randomizedTo.c_str();
            fileOpStruct.fFlags = FOF_SILENT;
            if (::SHFileOperationW(&fileOpStruct) == 0) {
                if (!fileOpStruct.fAnyOperationsAborted) {
                    // We can now zap the directory itself!
                    randomizedFrom = randomizedPath;
                    randomizedFrom.append(L"\0", 1);
                    assert(randomizedFrom.size() == randomizedPath.size() + 1);
                    ::ZeroMemory(&fileOpStruct, sizeof(fileOpStruct));
                    fileOpStruct.wFunc = FO_DELETE;
                    fileOpStruct.pFrom = randomizedFrom.c_str();
                    fileOpStruct.pTo = 0;
                    fileOpStruct.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
                    if (::SHFileOperationW(&fileOpStruct) == 0) {
                        if (!fileOpStruct.fAnyOperationsAborted) {
                            // All is well.
                            hRes = S_OK;
                        } else {
                            // User cancelled the delete. What to do?
                            // TODO THINK ABOUT IT
                        }
                    } else {
                        // Failed to zap: what to do?
                        // TODO THINK ABOUT IT
                    }
                } else {
                    // User cancelled at some point. What to do?
                    // TODO THINK ABOUT IT
                }
            } else {
                // Failed to move: cancel the random renaming?
                // TODO THINK ABOUT IT
                hRes = E_FAIL;
            }
        } else {
            // Error renaming.
            hRes = E_FAIL;
        }
    } else {
        // Wrong path.
        hRes = E_FAIL;
    }

    return hRes;
}
