#include "stdafx.h"
#include "resource.h"
#include <vector>
#pragma comment(lib,"Version.lib")

// Define handles
HINSTANCE g_hInstance = (HINSTANCE)GetModuleHandle(NULL);
HWND g_hMainWnd = NULL;

// Define tray icon properties
// TODO: The tray icon class used doesn't support Unicode
CTrayIcon g_TrayIconOTS("One Touch Search", true, LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ONETOUCHSEARCH)));

// Define OneTouchSearch function (implemented in OneTouchSearch.cpp)
bool oneTouchSearch(const wchar_t* search_engine_url);

// Get app version info from resource
bool GetAppVersion(CStringW &ver) {
	// Get the filename of the app
	TCHAR szFileName[MAX_PATH + 1] = {0};
	if (GetModuleFileName(NULL, szFileName, MAX_PATH) == 0)
		return false;

	// Get size of version resource
	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSize(szFileName, &dwHandle);
	if (dwSize == 0)
		return false;

	// Get version info
	std::vector<BYTE> data(dwSize);
	if (!GetFileVersionInfo(szFileName, NULL, dwSize, &data[0]))
		return false;

	// Get the language-independent resource
	UINT uiVerLen = 0;
	VS_FIXEDFILEINFO* pFixedInfo = 0;
	if (VerQueryValue(&data[0], _T("\\"), (void**)&pFixedInfo, (UINT *)&uiVerLen) == 0)
		return false;

	// Return the string
	ver.Format(_T("%u.%u.%u"),
		HIWORD (pFixedInfo->dwProductVersionMS),
		LOWORD (pFixedInfo->dwProductVersionMS),
		HIWORD (pFixedInfo->dwProductVersionLS),
		LOWORD (pFixedInfo->dwProductVersionLS));
	return true;
}

// Handle tray icon events
void g_TrayIconOTS_OnMessage(CTrayIcon* pTrayIcon, UINT uMsg)
{
	switch (uMsg) {
		case WM_LBUTTONUP:
			// Disable left click or double click (no Windows will be visible)
			// TODO: Add configuration dialog on double click, to specify
			//       search engine to open and hotkey to use
			//       https://docs.microsoft.com/en-us/windows/win32/controls/create-a-hot-key-control
			break;
		case WM_RBUTTONUP:
			{
				POINT pt;
				if (GetCursorPos(&pt))
				{
					// Create tray popup menu
					HMENU menu = CreatePopupMenu();
					AppendMenu(menu, MF_STRING, 1, _T("About"));
					AppendMenu(menu, MF_SEPARATOR, 2, NULL);;
					AppendMenu(menu, MF_STRING, 2, _T("Exit"));

					// Wait popup menu click
					UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD|TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hMainWnd, NULL);
					if (cmd == 2)
						// Exit
						PostMessage(g_hMainWnd, WM_CLOSE, 0, 0);
					else if (cmd == 1) {
						// About
						CStringW appVer;
						std::wstring aboutMessage;
						aboutMessage.append(_T("One Touch Search"));
						if (GetAppVersion(appVer)) {
							aboutMessage.append(_T(" v"));
							aboutMessage.append(appVer);
						}
						aboutMessage.append(_T("\n\nAllows to open the default web browser and search the currently selected text in any program when pressing CTRL+ALT+SHIFT+K (default hotkey).\n\nBind this shortcut to one of your mouse buttons with Logitech Options and you'll get the old OneTouchSearch feature back! Consider also running this program as Administrator to catch the input in both admin and non-admin apps."));
						MessageBox(g_hMainWnd, aboutMessage.c_str(), _T("One Touch Search"), MB_ICONINFORMATION | MB_OK);
					}
				}
			}
			break;
	}
}

// Handle tray icon messages
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_DESTROY:
			// Quit the application
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Create the message-only window
bool CreateMainHiddenWnd() {
	
	static const wchar_t CLASS_NAME[] = _T("OneTouchSearchWndClass");

	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = g_hInstance;
	wc.lpfnWndProc = &MainWndProc;
	wc.lpszClassName = CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	if (!RegisterClass(&wc))
		return false;

	g_hMainWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		_T("One Touch Search"),
		WS_OVERLAPPED | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
		HWND_MESSAGE, // message-only
		NULL, g_hInstance, NULL);

	return true;
}

// Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Create a message-only window to handle the tray icon
	if (!CreateMainHiddenWnd())
		return -1;

	// Define registry values
	static const HKEY REG_HK = HKEY_CURRENT_USER;
	static const wchar_t* OTS_REG_KEY = L"Software\\OneTouchSearch";
	static const wchar_t* OTS_REG_VALUE_URL = L"SearchEngineURL";
	static const wchar_t* OTS_REG_VALUE_HOTKEY = L"Hotkey";
	
	UINT hkModifiers;
	UINT hkKey;
	std::wstring searchEngineURL;

	// Read search engine URL from the registry
	searchEngineURL.resize(1023);
	DWORD searchEngineURL_size = searchEngineURL.size();
	if (RegGetValue(REG_HK, OTS_REG_KEY, OTS_REG_VALUE_URL, RRF_RT_REG_SZ, NULL, &searchEngineURL[0], &searchEngineURL_size) == ERROR_SUCCESS) {
		// Check size of the returned string
		if (searchEngineURL_size > 4) {
			// Remove null char from string
			searchEngineURL.resize((searchEngineURL_size / sizeof(wchar_t))-1);
		} else {
			// String empty
			MessageBox(g_hMainWnd, _T("Invalid/empty URL for the search engine, cannot continue."), _T("One Touch Search error"), MB_ICONEXCLAMATION | MB_OK);
			return -2;
		}
	} else {
		// Default
		searchEngineURL.assign(_T("http://www.google.com/search?q="));

		// Save setting to registry for next time
		HKEY hSaveKey;
		if (RegCreateKeyEx(REG_HK, OTS_REG_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hSaveKey, NULL) == ERROR_SUCCESS) {
			RegSetValueEx(hSaveKey, OTS_REG_VALUE_URL, 0, REG_SZ, (LPBYTE)(searchEngineURL.c_str()), (searchEngineURL.size() + 1) * sizeof(wchar_t));
		}
		RegCloseKey(hSaveKey);
	}

	// Read the hotkey from the registry
	DWORD hotkey_data;
	DWORD hotkey_data_size = sizeof(hotkey_data);
	if (RegGetValue(REG_HK, OTS_REG_KEY, OTS_REG_VALUE_HOTKEY, RRF_RT_REG_DWORD, NULL, &hotkey_data, &hotkey_data_size) == ERROR_SUCCESS) {
		// Registry value exists
		hkModifiers = HIWORD(hotkey_data);
		hkKey = LOWORD(hotkey_data);
	} else {
		// Default
		hkModifiers = MOD_CONTROL | MOD_ALT | MOD_SHIFT; // CTRL+ALT+SHIFT (high word)
		hkKey = 0x4B; // VK_KEY_K (low word)

		// Save setting to registry for next time
		DWORD hkValue = MAKELONG(hkKey, hkModifiers);
		HKEY hSaveKey;
		if (RegCreateKeyEx(REG_HK, OTS_REG_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hSaveKey, NULL) == ERROR_SUCCESS) {
			RegSetValueEx(hSaveKey, OTS_REG_VALUE_HOTKEY, 0, REG_DWORD, (const BYTE*)&hkValue, sizeof(hkValue));
		}
		RegCloseKey(hSaveKey);
	}

	// Check that at least a modifier is present
	if (((hkModifiers & MOD_CONTROL) == MOD_CONTROL) || 
		((hkModifiers & MOD_WIN) == MOD_WIN)) {
		// A modifier is present, OK!

		// Register the system-wide hotkey
		if (RegisterHotKey(NULL, 1, hkModifiers | MOD_NOREPEAT, hkKey)) {
			
			// Set the listener for the tray icon
			g_TrayIconOTS.SetListener(g_TrayIconOTS_OnMessage);

			// Start the message pump
			MSG msg;
			while (GetMessage(&msg, NULL, 0, 0))
			{
				if (msg.message == WM_HOTKEY) {
					// Run One Touch Search code when the hotkey is detected
					bool success = oneTouchSearch(searchEngineURL.c_str());

					// Check if not successfull
					if (success != TRUE) {

						// Display a notification
						g_TrayIconOTS.ShowBalloonTooltip("One Touch Search", "No selected text found!", CTrayIcon::eTI_Info);
					}

				} else {
					// Dispatch other messages to the main window
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			return (int)msg.wParam;

		} else {
			// Unable to register hotkey (already in use?)
			MessageBox(g_hMainWnd, _T("The hotkey is already in use, cannot continue."), _T("One Touch Search error"), MB_ICONEXCLAMATION | MB_OK);
			return -4;
		}
	} else {
		// A modifier is not present
		MessageBox(g_hMainWnd, _T("No valid modifier (CTRL, WIN, ...) found for the hotkey, cannot continue."), _T("One Touch Search error"), MB_ICONEXCLAMATION | MB_OK);
		return -3;
	}
}
