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


//
// CLevelZapContextMenuExt
//
// Class implementing the contextual menu handler for LevelZap. Will add an item to
// the contextual menu of folders to zap'em and move their files one level up.
class ATL_NO_VTABLE CLevelZapContextMenuExt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLevelZapContextMenuExt, &CLSID_LevelZapContextMenuExt>,
	public ILevelZapContextMenuExt
{
public:
	CLevelZapContextMenuExt()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LEVELZAPCONTEXTMENUEXT)

DECLARE_NOT_AGGREGATABLE(CLevelZapContextMenuExt)

BEGIN_COM_MAP(CLevelZapContextMenuExt)
	COM_INTERFACE_ENTRY(ILevelZapContextMenuExt)
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



};

OBJECT_ENTRY_AUTO(__uuidof(LevelZapContextMenuExt), CLevelZapContextMenuExt)
