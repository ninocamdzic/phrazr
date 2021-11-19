/*
Copyright(C) 2021 Nino Camdzic

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

// Enable visual styles.
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <commctrl.h>
#include <lmcons.h>
#include <string>
#include "base64.h"
#include "resource.h"

#define APP_NAME "Phrazr"
#define APP_VER "0.1.6"
#define APP_FULL_NAME "Phrazr"
#define APP_COPYRIGHT "Copyright(c) 2017 - 2021 Nino Camdzic"

#define ID_PASSPHRASE_STATIC 101
#define ID_PASSPHRASE_EDIT 102
#define ID_SEPARATOR_STATIC 103
#define ID_USER_STATIC 104
#define ID_USER_EDIT 105
#define ID_PROTECTED_STATIC 106
#define ID_PROTECTED_EDIT 107
#define ID_PROTECT_BUTTON 110
#define ID_RETRIEVE_BUTTON 111
#define ID_INFO_BUTTON 112
#define ID_EXIT_BUTTON 113
#define ID_KEY_STATIC 114
#define ID_KEY_EDIT 115
#define ID_DELETE_BUTTON 116
#define ID_REG_LOC_STATIC 117
#define ID_REG_LOC_EDIT 118
#define ID_UNHIDE_BUTTON 119

#define MAIN_REG_KEY "Software"
#define MAX_EDIT_SIZE 255
#define PASSW_MASK_CHAR '•'
#define HIDE_BUTTON_TEXT "Hide"
#define SHOW_BUTTON_TEXT "Show"

const char className[] = "phrazrw";

bool passwHidden = true;

HWND hKeyStatic;
HWND hKeyEdit;
HWND hValueStatic;
HWND hValueEdit;
HWND hUnhideButton;
HWND hUserStatic;
HWND hUserEdit;
HWND hRegLocStatic;
HWND hRegLocEdit;
HWND hProtectedStatic;
HWND hProtectedEdit;
HWND hProtectButton;
HWND hRetrieveButton;
HWND hDeleteButton;
HWND hInfoButton;
HWND hExitButton;

const int windowWidth = 500;
const int windowHeight = 385;
const int controlBeginX = 10;
const int controlBeginY = 10;

// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms632680(v=vs.85).aspx
// and: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633574(v=vs.85).aspx#system
void initUi(HWND hwnd) {
	int x = controlBeginX;
	int y = controlBeginY;
	int w = windowWidth - 35;
	int h = 20;

	HFONT hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	HFONT hFontCourierNew = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Courier New"));

	hKeyStatic = CreateWindowEx(0, "Static", "Key:",
		WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd, (HMENU)ID_KEY_STATIC,
		GetModuleHandle(NULL), NULL);
	SendMessage(hKeyStatic, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	
	y += h;
	hKeyEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, x, y, w, h, hwnd,
		(HMENU)ID_KEY_EDIT, GetModuleHandle(NULL), NULL);
	SendMessage(hKeyEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	SendMessage(hKeyEdit, EM_LIMITTEXT, MAX_EDIT_SIZE, 0);

	y += h + 10;
	hValueStatic = CreateWindowEx(0, "Static", "Passphrase:",
		WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd, (HMENU)ID_PASSPHRASE_STATIC,
		GetModuleHandle(NULL), NULL);
	SendMessage(hValueStatic, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	y += h;
	w -= 50;
	hValueEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD, x, y, w, h, hwnd,
		(HMENU)ID_PASSPHRASE_EDIT, GetModuleHandle(NULL), NULL);
	SendMessage(hValueEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	SendMessage(hValueEdit, EM_SETPASSWORDCHAR, PASSW_MASK_CHAR, MAKELPARAM(FALSE, 0));

	x += w + 10;
	w = 40;
	hUnhideButton = CreateWindowEx(0, "Button", SHOW_BUTTON_TEXT,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_UNHIDE_BUTTON, GetModuleHandle(NULL), NULL);
	SendMessage(hUnhideButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	y += h + 10;
	x = 10;
	h = 20;
	w = windowWidth - 35;
	hUserStatic = CreateWindowEx(0, "Static", "User:",
		WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd,
		(HMENU)ID_USER_STATIC, GetModuleHandle(NULL), NULL);
	SendMessage(hUserStatic, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	y += h;
	hUserEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_USER_EDIT, GetModuleHandle(NULL), NULL);
	SendMessage(hUserEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	SendMessage(hUserEdit, EM_SETREADONLY, MAKEWPARAM(TRUE, 0), NULL);

	y += h + 10;
	hRegLocStatic = CreateWindowEx(0, "Static", "Registry location:",
		WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd,
		(HMENU)ID_USER_STATIC, GetModuleHandle(NULL), NULL);
	SendMessage(hRegLocStatic, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	y += h;
	hRegLocEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_REG_LOC_EDIT, GetModuleHandle(NULL), NULL);

	SendMessage(hRegLocEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
	SendMessage(hRegLocEdit, EM_SETREADONLY, MAKEWPARAM(TRUE, 0), NULL);
	SetWindowText(hRegLocEdit, std::string("HKEY_CURRENT_USER").append("\\").append(MAIN_REG_KEY).append("\\").append(APP_NAME).c_str());

	y += h + 10;
	h = 20;
	hProtectedStatic = CreateWindowEx(0, "Static", "Protected passphrase:",
		WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd,
		(HMENU)ID_PROTECTED_STATIC, GetModuleHandle(NULL), NULL);
	SendMessage(hProtectedStatic, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	y += h;
	h = 70;
	hProtectedEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "Edit", "",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL |
		ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL, x, y, w, h, hwnd,
		(HMENU)ID_PROTECTED_EDIT, GetModuleHandle(NULL), NULL);
	SendMessage(hProtectedEdit, WM_SETFONT, (WPARAM)hFontCourierNew, MAKELPARAM(FALSE, 0));
	SendMessage(hProtectedEdit, EM_SETREADONLY, MAKEWPARAM(TRUE, 0), NULL);

	y += h + 10;
	w = 60;
	h = 25;
	hProtectButton = CreateWindowEx(0, "Button", "Protect",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_PROTECT_BUTTON, GetModuleHandle(NULL), NULL);
	SendMessage(hProtectButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	x += w + 5;
	hRetrieveButton = CreateWindowEx(0, "Button", "Retrieve",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_RETRIEVE_BUTTON, GetModuleHandle(NULL), NULL);
	SendMessage(hRetrieveButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	x += w + 5;
	hDeleteButton = CreateWindowEx(0, "Button", "Delete",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_DELETE_BUTTON, GetModuleHandle(NULL), NULL);
	SendMessage(hDeleteButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	x = windowWidth - 24 - w;
	hExitButton = CreateWindowEx(0, "Button", "Exit",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_EXIT_BUTTON, GetModuleHandle(NULL), NULL);
	SendMessage(hExitButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));

	h = 25;
	x -= w + 5;
	hInfoButton = CreateWindowEx(0, "Button", "About",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, w, h, hwnd,
		(HMENU)ID_INFO_BUTTON, GetModuleHandle(NULL), NULL);
	SendMessage(hInfoButton, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
}

// Retrieve the executing user.
void getUser(std::string* username) {
	TCHAR buffer[UNLEN + 1];
	DWORD size = UNLEN + 1;

	if (GetUserName((TCHAR*)buffer, &size)) {
		*username = std::string(buffer);
	}
}

void init(HWND hwnd) {
	// Display the logged in user.
	std::string username;

	getUser(&username);

	if (username.size() > 0) {
		SetWindowText(hUserEdit, username.c_str());
	}
	else {
		MessageBox(hwnd, "Failed to determine user.", APP_NAME, MB_ICONERROR | MB_OK);
	}
}

void doShowInfo(HWND hwnd) {
	std::string info;

	info.append(APP_FULL_NAME);
	info.append(" ");
	info.append(APP_VER);
	info.append("\n");
	info.append(APP_COPYRIGHT);

	MessageBox(hwnd, info.c_str(), "Info", MB_ICONINFORMATION | MB_OK);
}

bool regSaveString(std::string key, std::string in) {
	HKEY mainKey;

	std::string subKey(MAIN_REG_KEY);
	subKey.append("\\");
	subKey.append(APP_NAME);
	LONG result = RegCreateKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, 0, 0, KEY_ALL_ACCESS, 0, &mainKey, 0);

	if (result != ERROR_SUCCESS) {
		return false;
	}

	result = RegSetValueEx(mainKey, key.c_str(), 0, REG_SZ, (BYTE*)in.c_str(), in.length());

	if (result != ERROR_SUCCESS) {
		return false;
	}

	RegCloseKey(mainKey);

	return true;
}

bool regFetchString(std::string key, std::string* out) {
	LONG result = ERROR_MORE_DATA;
	DWORD size = 1;

	std::string value;
	std::string subKey(MAIN_REG_KEY);
	subKey.append("\\");
	subKey.append(APP_NAME);

	while (result == ERROR_MORE_DATA) {
		DWORD type;
		char* buf = new char[size];

		// NOTE: Let RegGetValue determine the size of the buffer. By specifying a small buffer the RegGetValue returns
		// the needed size. Then use that new size to create our buffer.
		result = RegGetValue(HKEY_CURRENT_USER, subKey.c_str(), key.c_str(), RRF_RT_REG_SZ, &type, (PVOID)buf, &size);

		if (result == ERROR_SUCCESS) {
			value.append(buf);
		}

		delete[] buf;
	}

	if (result != ERROR_SUCCESS) {
		return false;
	}

	*out = value;
	return true;
}

bool regDeleteKey(const std::string key) {
	HKEY hKey;
	std::string subKey(MAIN_REG_KEY);
	subKey.append("\\");
	subKey.append(APP_NAME);

	LONG result = RegOpenKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, DELETE | KEY_ALL_ACCESS, &hKey);

	if (result != ERROR_SUCCESS) {
		return false;
	}

	result = RegDeleteValue(hKey, key.c_str());

	RegCloseKey(hKey);
	return result == ERROR_SUCCESS;
}


bool protect(const std::string plain, std::string* enc) {
	bool result = false;
	DATA_BLOB dataIn;
	DATA_BLOB dataOut;

	BYTE* data = (BYTE*)plain.c_str();
	DWORD dataSize = plain.size() + 1;

	dataIn.pbData = data;
	dataIn.cbData = dataSize;

	if (CryptProtectData(&dataIn, NULL, NULL, NULL, NULL, 0, &dataOut)) {
		*enc = base64_encode(dataOut.pbData, dataOut.cbData);
		LocalFree(dataOut.pbData);
		result = true;
	}

	return result;
}

bool unprotect(const std::string enc, std::string* plain) {
	bool result = false;
	DATA_BLOB dataIn;
	DATA_BLOB dataOut;

	std::string dec = base64_decode(enc);
	dataIn.pbData = (BYTE*)dec.c_str();
	dataIn.cbData = dec.size();

	if (CryptUnprotectData(&dataIn, NULL, NULL, NULL, NULL, 0, &dataOut)) {
		*plain = std::string((char*)dataOut.pbData);
		LocalFree(dataOut.pbData);
		result = true;
	}

	return result;
}

void doProtect(HWND hwnd) {
	int keyLen = GetWindowTextLength(hKeyEdit);
	int passphraseLen = GetWindowTextLength(hValueEdit);

	if (keyLen > 0 && keyLen <= MAX_EDIT_SIZE && passphraseLen > 0) {
		keyLen += 1;
		passphraseLen += 1;

		char* key = new char[keyLen];
		char* passphrase = new char[passphraseLen];
		GetWindowText(hKeyEdit, key, keyLen);
		GetWindowText(hValueEdit, passphrase, passphraseLen);

		// Check if this key already exists.
		std::string existingPassphrase;
		bool proceed = true;

		if (regFetchString(std::string(key), &existingPassphrase)) {
			int msgBoxResult = MessageBox(hwnd, "Override existing key?", APP_NAME, MB_ICONQUESTION | MB_YESNO);

			if (msgBoxResult == IDNO) {
				proceed = false;
			}
		}

		if (proceed) {
			std::string enc;

			if (protect(std::string(passphrase), &enc)) {
				SetWindowText(hProtectedEdit, enc.c_str());
				SetWindowText(hValueEdit, "");
				regSaveString(std::string(key), enc);
			}
			else {
				MessageBox(hwnd, "Failed to protect the data.", APP_NAME, MB_ICONERROR | MB_OK);
			}
		}

		delete[] key;
		delete[] passphrase;

	} else if (keyLen > MAX_EDIT_SIZE) {
		MessageBox(hwnd, "Max key length is 255.", APP_NAME, MB_ICONERROR | MB_OK);

	} else {
		MessageBox(hwnd, "Key or passphrase is not specified.", APP_NAME, MB_ICONERROR | MB_OK);
	}
}

void doUnprotect(HWND hwnd) {
	int keyLen = GetWindowTextLength(hKeyEdit);

	if (keyLen > 0) {
		keyLen += 1;

		char* key = new char[keyLen];
		GetWindowText(hKeyEdit, key, keyLen);

		std::string passphrase;

		if (regFetchString(std::string(key), &passphrase)) {
			std::string dec;

			SetWindowText(hProtectedEdit, passphrase.c_str());

			if (unprotect(std::string(passphrase), &dec)) {
				SetWindowText(hValueEdit, dec.c_str());
			}
			else {
				MessageBox(hwnd, "Failed to unprotect the message.", APP_NAME, MB_ICONERROR | MB_OK);
			}
		}
		else {
			MessageBox(hwnd, "Failed to obtain the passphrase from the specified key.", APP_NAME, MB_ICONERROR | MB_OK);
		}

		delete[] key;
	}
	else {
		MessageBox(hwnd, "Key is not specified.", APP_NAME, MB_ICONERROR | MB_OK);
	}
}

void doDelete(HWND hwnd) {
	int keyLen = GetWindowTextLength(hKeyEdit);

	if (keyLen > 0) {
		keyLen += 1;

		char* key = new char[keyLen];
		GetWindowText(hKeyEdit, key, keyLen);

		// Check if this key already exists.
		std::string existingPassphrase;
		bool proceed = true;

		if (regFetchString(std::string(key), &existingPassphrase)) {
			int msgBoxResult = MessageBox(hwnd, "Are you sure you want to delete this key?", APP_NAME, MB_ICONQUESTION | MB_YESNO);

			if (msgBoxResult == IDNO) {
				proceed = false;
			}
		}

		if (proceed) {
			if (regDeleteKey(key)) {
				SetWindowText(hKeyEdit, "");
				SetWindowText(hValueEdit, "");
				SetWindowText(hProtectedEdit, "");

				std::string msg = std::string("Key: '");
				msg.append(key);
				msg.append("' was successfully deleted.");

				MessageBox(hwnd, msg.c_str(), APP_NAME, MB_ICONINFORMATION | MB_OK);
			}
			else {
				MessageBox(hwnd, "Failed to delete the specified key.", APP_NAME, MB_ICONERROR | MB_OK);
			}
		}

		delete[] key;
	}
	else {
		MessageBox(hwnd, "Key or passphrase is not specified.", APP_NAME, MB_ICONERROR | MB_OK);
	}
}

void togglePassword() {
	if (passwHidden) {
		SendMessage(hValueEdit, EM_SETPASSWORDCHAR, (WPARAM)0, MAKELPARAM(FALSE, 0));
		SetWindowText(hUnhideButton, HIDE_BUTTON_TEXT);
		passwHidden = false;
	}
	else {
		SendMessage(hValueEdit, EM_SETPASSWORDCHAR, PASSW_MASK_CHAR, MAKELPARAM(FALSE, 0));
		SetWindowText(hUnhideButton, SHOW_BUTTON_TEXT);
		passwHidden = true;
	}

	SetFocus(hValueEdit);
}

void handleCommandMessage(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	// Handle button clicks.
	switch (LOWORD(wParam)) {
	case ID_PROTECT_BUTTON:
		doProtect(hwnd);
		break;
	case ID_RETRIEVE_BUTTON:
		doUnprotect(hwnd);
		break;

	case ID_DELETE_BUTTON:
		doDelete(hwnd);
		break;

	case ID_INFO_BUTTON:
		doShowInfo(hwnd);
		break;

	case ID_EXIT_BUTTON:
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		break;

	case ID_UNHIDE_BUTTON:
		togglePassword();
		break;
	}
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;

	switch (msg) {
	case WM_CREATE:
		initUi(hwnd);
		init(hwnd);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Handle controls.
	case WM_COMMAND:
		handleCommandMessage(hwnd, wParam, lParam);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return result;
}

void centerWindow(HWND hwnd, int width, int height, RECT* rect) {
	GetClientRect(hwnd, rect);

	rect->left = (rect->right / 2) - (width / 2);
	rect->top = (rect->bottom / 2) - (height / 2);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL_ICON));

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", APP_NAME, MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	RECT rect;
	centerWindow(GetDesktopWindow(), windowWidth, windowHeight, &rect);

	hwnd = CreateWindowEx(NULL, className, APP_NAME,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		rect.left, rect.top, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", APP_NAME, MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		// IsDialogMessage is needed for tabbing to next control.
		// See: https://stackoverflow.com/questions/3299193/how-to-make-tabs-work-with-win32-im-not-using-a-dialog
		if (!IsDialogMessage(hwnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}