#include "stdafx.h"
#include "OneTouchSearchAction.h"
#include "ClipboardBackup.h"
#include "URLEncode.h"

CStringW getClipboard() {
	// Return clipboard text in Unicode format
	CStringW strData;

	if (OpenClipboard(NULL)) {
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData) {
			WCHAR *pchData = (WCHAR*) GlobalLock(hData);
			if (pchData) {
				strData = pchData;
				GlobalUnlock(hData);
			}
		}
		CloseClipboard();
	}
	return strData;
}

bool oneTouchSearch(const wchar_t* search_engine_url) {

	// Backup current clipboard (text & images should work)
	CClipboardBackup cbbackup;

	// Empty clipboard to check whether the selection doesn't exist
	::OpenClipboard(NULL);
	::EmptyClipboard();
	::CloseClipboard();

	// Get currently selected text in any application by sending CTRL+INS
	// https://stackoverflow.com/questions/2251578/how-do-i-get-the-selected-text-from-the-focused-window-using-native-win32-api
	// ^^ does not work, but the code to get selection in Artha sources does:
	// https://sourceforge.net/p/artha/code/HEAD/tree/trunk/src/gui.c

	// Prepare the keys
	const int key_count = 4;
	INPUT input[key_count] = {{INPUT_KEYBOARD}, {INPUT_KEYBOARD}, {INPUT_KEYBOARD}, {INPUT_KEYBOARD}};
	const WORD inputKey[2] = {VK_CONTROL, VK_INSERT};

	// Prepare ALT release
	input[0].ki.wVk = VK_MENU;
	input[0].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, input, sizeof(INPUT));
	input[0].ki.dwFlags = 0;

	// Prepare CTRL+INS
	input[0].ki.wVk = input[2].ki.wVk = inputKey[0];
	input[1].ki.wVk = input[3].ki.wVk = inputKey[1];
	input[2].ki.dwFlags = (input[3].ki.dwFlags |= KEYEVENTF_KEYUP);	

	// Send the keys
	if (SendInput(key_count, (LPINPUT) input, sizeof(INPUT))) {

		// Wait 200 ms
		Sleep(200);

		// Get the current clipboard text in Unicode format
		CStringW clipText = getClipboard();

		// Check that the returned string is not empty (= empty clipboard)
		if (clipText.Trim().GetLength() > 0) {

			// Encode the URL
			CURLEncode cEncoder;
			CString clipTextURL = cEncoder.Encode(clipText);

			// Prepends the query URL
			CString finalURL;
			finalURL.Append(search_engine_url);
			finalURL.Append(clipTextURL);

			// Open the default browser
			ShellExecute(NULL, _T("open"), finalURL, NULL, NULL, SW_SHOWNORMAL);

			// Restore the clipboard
			cbbackup.Restore();
			return TRUE;

		} else {

			// Restore the clipboard
			cbbackup.Restore();

			// No text to search
			return FALSE;
		}
	} else {

		// Unable to send keys to copy the text
		return FALSE;
	}
}
