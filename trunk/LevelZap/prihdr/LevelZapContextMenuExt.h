// LevelZapContextMenuExt.h
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

#pragma once
#include "resource.h"       // main symbols

#include "LevelZap_i.h"
#include "LevelZapTypes.h"

#include <Nullable.h>

//
// CLevelZapContextMenuExt
//
// Class implementing the contextual menu handler for LevelZap. Will add an item to
// the contextual menu of folders to zap'em and move their files one level up.
//
class ATL_NO_VTABLE CLevelZapContextMenuExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLevelZapContextMenuExt, &CLSID_LevelZapContextMenuExt>,
	public ILevelZapContextMenuExt,
    public IShellExtInit,
    public IContextMenu
{
public:
	CLevelZapContextMenuExt();

DECLARE_REGISTRY_RESOURCEID(IDR_LEVELZAPCONTEXTMENUEXT)

DECLARE_NOT_AGGREGATABLE(CLevelZapContextMenuExt)

BEGIN_COM_MAP(CLevelZapContextMenuExt)
	COM_INTERFACE_ENTRY(ILevelZapContextMenuExt)
    COM_INTERFACE_ENTRY(IShellExtInit)
    COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
    // IShellExtInit methods
    STDMETHOD(Initialize)(PCIDLIST_ABSOLUTE p_pFolderPIDL, IDataObject *p_pDataObject, HKEY p_hKeyFileClass);

    // IContextMenu methods
    STDMETHOD(QueryContextMenu)(HMENU p_hMenu, UINT p_Index, UINT p_FirstCmdId,
                                UINT p_LastCmdId, UINT p_Flags);
    STDMETHOD(InvokeCommand)(CMINVOKECOMMANDINFO* p_pCommandInfo);
    STDMETHOD(GetCommandString)(UINT_PTR p_CmdId, UINT p_Flags, UINT* p_pReserved,
                                LPSTR p_pBuffer, UINT p_BufferSize);

private:
    FolderV             m_vFolders;     // List of folders to "zap".
	HRESULT				FindFiles(const HWND p_hParentWnd,
									CString szTo,
									CString szFromPath,
									DWORD& dwLevel,
									CString& szlFrom,
									CString& szlTo) const;

    Nullable<UINT>      m_FirstCmdId;   // ID of first command menu item.
    Nullable<UINT>      m_ZapCmdId;     // ID of our "zap" command.

    HRESULT             ZapAllFolders(const HWND p_hParentWnd) const;
    HRESULT             ZapFolder(const HWND p_hParentWnd,
                                  CString p_Folder,
                                  bool& p_rYesToAll) const;
	HRESULT				MoveFile(const HWND p_hParentWnd,
											CString p_Path,
											CString p_FolderTo) const;
	HRESULT				DeleteFolder(const HWND p_hParentWnd,
											CString p_Path) const;
	CString				m_szMetaDir;
	DWORD				m_dwLevels;
};

OBJECT_ENTRY_AUTO(__uuidof(LevelZapContextMenuExt), CLevelZapContextMenuExt)
