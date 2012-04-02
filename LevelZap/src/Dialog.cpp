// Dialog.cpp
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
#include "Dialog.h"
#include "Utilities.h"

#pragma comment(lib, "comctl32.lib")
#ifdef _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

bool Dialog::bSelection = Dialog::B_CANCEL;

HRESULT Dialog::CallbackProc(HWND hwnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData) {
	HRESULT hr = S_OK;
	switch (uNotification) {
		case TDN_BUTTON_CLICKED:
			bSelection = static_cast<int>(wParam) == B_OK;
			break;
		case TDN_CREATED:
			SendMessage(hwnd, WM_SETICON, ICON_BIG, NULL);
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, NULL);
			break;		
	}		
	return hr;
}

bool Dialog::doModal(HWND hwndParent, _U_STRINGorID title) {
	if (Util::GetVersionEx2() >= 6) {
		CAtlArray<TASKDIALOG_BUTTON> m_buttons;
		int id = 0;
		size_t index = m_buttons.Add();
		m_buttons[index].pszButtonText = title.m_lpstr;
		m_buttons[index].nButtonID = B_OK;

		TASKDIALOGCONFIG tdConfig;    
		ZeroMemory(&tdConfig, sizeof(tdConfig));
		tdConfig.hInstance = _AtlBaseModule.GetModuleInstance();
		tdConfig.cbSize = sizeof(TASKDIALOGCONFIG);
		tdConfig.pfCallback = CallbackProc;
		tdConfig.pszMainIcon = TD_WARNING_ICON;
		tdConfig.pszWindowTitle = MAKEINTRESOURCE(IDS_PROJNAME);
		tdConfig.pszMainInstruction = MAKEINTRESOURCE(IDS_ZAP_CONFIRM);
		tdConfig.pszContent = MAKEINTRESOURCE(IDS_ZAP_CONTENT);
		tdConfig.dwFlags = TDF_USE_COMMAND_LINKS|TDF_ALLOW_DIALOG_CANCELLATION|TDF_CAN_BE_MINIMIZED;
		tdConfig.dwCommonButtons = TDCBF_CANCEL_BUTTON;
		tdConfig.pButtons = m_buttons.GetData();
		tdConfig.cButtons = static_cast<UINT>(m_buttons.GetCount());

		TaskDialogIndirect(&tdConfig, NULL, NULL, NULL);
		return bSelection;
	} else {
		CString t(MAKEINTRESOURCE(IDS_PROJNAME));
		CString i(title.m_lpstr);
		return (MessageBox(hwndParent, i, t, MB_OKCANCEL|MB_ICONEXCLAMATION) == IDOK);
	}
}