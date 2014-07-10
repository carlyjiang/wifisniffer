#ifndef UNICODE
#define UNICODE
#endif

#ifndef __HTTP_TRANSPORT_CPP__
#define __HTTP_TRANSPORT_CPP__

#include <Windows.h>
#include <Winhttp.h>
#include <string.h>
#include <iostream>
#include "wlan_bss_info.h"

using namespace std;

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

int http_transport_data()
{
	FILE *f = NULL;
	freopen_s(&f, "data_html.txt", "w", stdout);
	RADIOMAP radiomap;
	std::string json_data;
	int ret = 0;
	int len = 0;
	int input = 0;
	int index = -1;
	int repeat = 10;
	char *buf = NULL;
	char num_to_str[20];
	LocalizationNode *node = NULL;

	while (repeat--)
	{
		ret = get_ap_rssi_data(radiomap);
		if (ret != 0)
		{
			printf("ERROR: get_ap_rssi_data an error occurs\n");
			exit(-1);
		}
	}

	json_data += "[";
	RADIOMAP_ITER iter = radiomap.begin();
	for (; iter != radiomap.end(); iter++)
	{
		if (find_at_radiomap(std::string("C8-3A-35-46-2D-28"), radiomap, node) == 0)
		{
			cout << "ERROR AP: " << node->get_key() << endl
				<< "SSID: ###" << node->get_ssid() << "###" << endl;
		}
		index = iter->second.get_ssid().find(' ');
		if (index >= 0 && index < 1000)
			continue;

		json_data += "{\"mac_id\":\"";
		json_data += iter->first;
		json_data += "\",";
		json_data += "\"ssid\":\"";
		json_data += iter->second.get_ssid();
		json_data += "\",";
		json_data += "\"rssi\":[";

		for (VECTOR_INT_ITER vector_iter = iter->second.get_all_recoders().begin();
			vector_iter != iter->second.get_all_recoders().end(); vector_iter++)
		{
			_itoa_s(*vector_iter, num_to_str, 20, 10);
			json_data += num_to_str;
			json_data.push_back(',');
		}
		json_data += "]},\n";
	}
	json_data += "]";
	while ((index = json_data.rfind(",]")) < 100000 && index >=0)
		json_data.replace(index, 2, "]");
	while ((index = json_data.rfind(",\n]")) < 100000 && index >= 0)
		json_data.replace(index, 3, "]");

	const char *src = json_data.c_str();
	len = strlen(src) + 1;
	buf = new char[len];
	strcpy_s(buf, len, src);

	send_data(buf);
	fflush(f);
	fclose(f);

	//scanf_s("%d", &input);
	delete[]buf;
	return 0;
}

int main_check_error_ap()
{
	RADIOMAP radiomap;
	LocalizationNode *node;
	int repeat = 30;
	while (repeat--)
	{
		get_ap_rssi_data(radiomap);
		if (find_at_radiomap(std::string("C8-3A-35-46-2D-28"), radiomap, node) == 0)
			cout << node->get_key() << endl;
		Sleep(500);
	}
	cout << "done!" << endl;
	cin.get();
	return 0;
}

#endif


