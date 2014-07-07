#ifndef UNICODE
#define UNICODE
#endif

#ifndef __HTTP_TRANSPORT_CPP__
#define __HTTP_TRANSPORT_CPP__

#include <Windows.h>
#include <Winhttp.h>
#include <stdio.h>
#include "wlan_bss_info.h"

#pragma comment(lib, "winhttp.lib")

int send_data(LPSTR data)
{
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	DWORD dwBytesWritten = 0;
	LPSTR pszOutBuffer;
	LPSTR pszData = data;
	LPCWSTR addtional_headers = L"Content-type:application/json";
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL;
	HINTERNET  hConnect = NULL;
	HINTERNET  hRequest = NULL;
	int input = 0;

	hSession = WinHttpOpen(L"RSSI Sensor/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	if (hSession)
		hConnect = WinHttpConnect(hSession,
		L"localhost",
		8000, 0);

	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect,
		L"POST", L"geolbs/get_token/", L"1.1",
		WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		WINHTTP_FLAG_REFRESH);

	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,
		addtional_headers, -1L,
		WINHTTP_NO_REQUEST_DATA, 0,
		(DWORD)strlen(pszData), 0);

	if (bResults)
		bResults = WinHttpWriteData(hRequest,
		pszData, (DWORD)strlen(pszData),
		&dwBytesWritten
		);

	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	if (bResults)
	{
		do
		{
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u in WinHttpQueryDataAvailable.\n",
				GetLastError());
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				dwSize = 0;
			}
			else
			{
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				else
					printf("%s", pszOutBuffer);
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}
	if (!bResults)
		printf("Error %d has occurred.\n", GetLastError());
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
	return 0;
}

int main()
{
	FILE *f = NULL;
	freopen_s(&f, "data_html.txt", "w", stdout);
	LPSTR str = "abcdefg";
	send_data(str);
	


	fflush(f);
	fclose(f);
	return 0;
}

#endif

