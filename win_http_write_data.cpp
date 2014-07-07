#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <fstream>
using namespace std;

int get_content()
{
	char *pFilename = "c:\\temp\\testfile.bmp";
	int input = 0;

	char *type = NULL;
	if (strstr(pFilename, ".bmp"))
		type = "Content-Type: image/bmp\r\n";
	else if (strstr(pFilename, ".gif"))
		type = "Content-Type: image/gif\r\n";
	else if (strstr(pFilename, ".jpg"))
		type = "Content-Type: image/jpeg\r\n";

	if (type == NULL) {
		printf("Unknown image type: %s", pFilename);
		return 1;
	}

	BOOL  bResults = FALSE;
	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"Windows WinHttp Sample-- Karim Sharif",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (!hSession) {
		printf("WinHttpOpen:Error %u has occurred.", GetLastError());
		return 1;
	}

	// Specify an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, L"localhost", 8000, 0);

	if (!hConnect) {
		printf("WinHttpConnect: Error %u has occurred.", GetLastError());
		return 1;
	}

	// Create an HTTP Request handle.
	char uri[1024];
	memset((void *)uri, NULL, sizeof(uri));
	strcpy_s(uri, "");
	int len = strlen(uri) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText, 0, len);
	::MultiByteToWideChar(CP_ACP, NULL, uri, -1, wText, len);

	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"POST",
		wText,
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		0);

	if (!hRequest) {
		printf("WinHttpOpenRequest:Error %u has occurred.", GetLastError());
		return 1;
	}

	// Send a Request.
	DWORD type_len = strlen(type);
	wchar_t *wData = new wchar_t[type_len];
	memset(wData, 0, len);
	::MultiByteToWideChar(CP_ACP, NULL, type, -1, wData, len);

	// Open file in binary/raw to send.
	ifstream imagefile(pFilename, std::ios::binary | std::ios::in);
	if (imagefile.is_open())
	{
		imagefile.seekg(0, ios::end);        //move to the end of the file
		int fl_sz = imagefile.tellg();        //tell me where you are, partner( pointer ) !
		imagefile.seekg(0, ios::beg);        //move to the beg of the file        
		imagefile.seekg(0, ios::beg);

		if (hRequest)
			bResults = WinHttpSendRequest(hRequest,
			wData, type_len, WINHTTP_NO_REQUEST_DATA, 0, fl_sz, 0);


		// Report errors.
		if (!bResults) {
			printf("WinHttpSendRequest:Error %u has occurred.", GetLastError());
			return 1;
		}


		int remaining = fl_sz;
		char data[100];
		while (remaining > 0)
		{
			int blk = sizeof(data);
			if (remaining < blk)
				blk = remaining;
			imagefile.read(data, blk);
			int read = imagefile.gcount();
			DWORD dwBytesWritten = 0;
			bResults = WinHttpWriteData(hRequest, data, (DWORD)read, &dwBytesWritten);
			// Report errors.
			if (!bResults) {
				printf("WinHttpWriteData: Error %u has occurred.", GetLastError());
				return 1;
			}
			remaining -= read;
		}
		imagefile.close();
		// End the request.
		if (bResults)
			bResults = WinHttpReceiveResponse(hRequest, NULL);
		// Report errors.
		if (!bResults) {
			printf("WinHttpReceiveResponse:Error %u has occurred.", GetLastError());
			return 1;
		}
	}
	else {
		printf("can not open file:%s", pFilename);
		return 1;
	}

	// Close open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
	
	return 0;
}

int test_main()
{
	DWORD dwSize = 0;
	int input = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL,
		hConnect = NULL,
		hRequest = NULL;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"WinHTTP Example/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, L"localhost",
		8000, 0);

	// Create an HTTP request handle.
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", NULL,
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_REFRESH);

	// Send a request.
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,
		WINHTTP_NO_ADDITIONAL_HEADERS, 0,
		WINHTTP_NO_REQUEST_DATA, 0,
		0, 0);

	// End the request.
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	// Keep checking for data until there is nothing left.
	if (bResults)
	{
		do
		{
			// Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u in WinHttpQueryDataAvailable.\n",
				GetLastError());

			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				dwSize = 0;
			}
			else
			{
				// Read the data.
				ZeroMemory(pszOutBuffer, dwSize + 1);

				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
					dwSize, &dwDownloaded))
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				else
					printf("%s", pszOutBuffer);

				// Free the memory allocated to the buffer.
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}


	// Report any errors.
	if (!bResults)
		printf("Error %d has occurred.\n", GetLastError());

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	scanf_s("%d", &input);
	return 0;
}

int win_http_write_data_main_test()
{
	int input = 0;
	get_content();
	scanf_s("%d", &input);
	return 0;
}
