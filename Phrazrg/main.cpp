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

#include <windows.h>
#include <iostream>
#include <string>
#include "base64.h"
#include "resource.h"

#define APP_NAME "Phrazr Get"
#define APP_VER "0.1.6"
#define APP_FULL_NAME "Phrazr Get"
#define APP_COPYRIGHT "Copyright(c) 2017 - 2021 Nino Camdzic"

#define MAIN_KEY "Software"

bool regFetchString(const std::string valKey, std::string* out) {
	LONG result = ERROR_MORE_DATA;
	DWORD size = 1;

	std::string value;
	std::string subKey(MAIN_KEY);
	subKey.append("\\");
	subKey.append("Phrazr");

	while (result == ERROR_MORE_DATA) {
		DWORD type;
		char* buf = new char[size];

		// NOTE: Let RegGetValue determine the size of the buffer. By specifying a small buffer the RegGetValue returns
		// the needed size. Use that new size to create our buffer.
		result = RegGetValue(HKEY_CURRENT_USER, subKey.c_str(), valKey.c_str(), RRF_RT_REG_SZ, &type, (PVOID)buf, &size);

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

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::string key = argv[0];
		size_t keyBeginIndex = key.rfind(".exe");
		
		if (keyBeginIndex > 0) {
			key = key.substr(0, keyBeginIndex);
			std::string enc;
			
			if (regFetchString(key, &enc)) {
				std::string plain;

				if (unprotect(enc, &plain)) {
					std::cout << plain;
				}
			}
		} else {
			std::cout << "Invalid key specified.";
		}
	}
	
	return 0;
}
