#ifndef __WLAN_BSS_INFO_CPP__
#define __WLAN_BSS_INFO_CPP__

#include "wlan_bss_info.h"

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

std::ostream &operator<<(std::ostream &out, const LocalizationNode &data)
{
	data.print(out);
	return out;
}

char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

int get_mac_id(DOT11_MAC_ADDRESS &_in, std::string &_out)
{
	if (_in == NULL)
	{
		return false;
	}
	_out.clear();
	for (uint k = 0; k < 6; k++)
	{
		char lowbit = digits[(int)_in[k] & 0xf];
		char highbit = digits[(int)((_in[k] & 0xf0) >> 4)];
		_out.push_back(highbit);
		_out.push_back(lowbit);
		if (k != 5)
			_out.push_back('-');
	}
	return true;
}

int get_ssid(DOT11_SSID &ssid, std::string &_out)
{
	_out.clear();
	if (ssid.uSSIDLength == 0)
	{
		_out.clear();
		return 1;
	}
	else
	{
		for (uint k = 0; k < ssid.uSSIDLength; k++)
			_out.push_back(ssid.ucSSID[k]);
	}
	return 0;
}

//if not found, out is set to NULL
int find_at_radiomap(std::string &key, 
	RADIOMAP &container, LocalizationNode *&_out)
{
	RADIOMAP_ITER iter = container.find(key);
	if (iter == container.end())
	{
		_out = NULL;
		return 1;
	}
	else
	{
		_out = &iter->second;
		return 0;
	}
}

int add_to_radiomap(LocalizationNode &value, std::map<std::string, 
	LocalizationNode> &container)
{
	RADIOMAP_ITER iter = container.find(value.get_key());
	VECTOR_INT_ITER rhs_vector_iter;
	if (iter == container.end())
	{
		container.insert(std::make_pair(value.get_key(), value));
	}
	else
	{
		if ((rhs_vector_iter = iter->second.find_the_first_unidentical_position(value.get_all_recoders()))
			!= value.get_all_recoders().end())
		{
			for (; rhs_vector_iter != value.get_all_recoders().end(); rhs_vector_iter++)
			{
				iter->second.get_all_recoders().push_back(*rhs_vector_iter);
			}
		}
	}
	return 0;
}

int check_set_padding(RADIOMAP &_in)
{
	RADIOMAP_ITER iter = _in.begin();
	int max_size = -1, rssi_record_size, padding = 99999;
	for (; iter != _in.end(); iter++)
	{
		rssi_record_size = iter->second.get_all_recoders().size();
		if (rssi_record_size != max_size)
		{
			max_size = rssi_record_size;
			break;
		}
	}
	for (iter = _in.begin(); iter != _in.end(); iter++)
	{
		rssi_record_size = iter->second.get_all_recoders().size();
		if (rssi_record_size != max_size)
			iter->second.add_recoder(padding);
	}
	return true;
}

int get_ap_rssi_data(RADIOMAP &result_map)
{
	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	DWORD dwRetVal = 0;
	WCHAR GuidString[39] = { 0 };
	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	PWLAN_BSS_ENTRY pBssEntry = NULL;
	PWLAN_BSS_LIST pBssList = NULL;
	LocalizationNode *pLocalizationNode = NULL;
	std::string mac_id;
	std::string ret_ssid;
	int iRet = 0;
	int ret;
	uint i;

	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS)
	{
		wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
		return 1;
	}
	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS)
	{
		wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		return 1;
	}
	else
	{
		wprintf(L"Nunber Entries: %lu\n", pIfList->dwNumberOfItems);
		wprintf(L"Current Index: %lu\n", pIfList->dwIndex);
		for (i = 0; i < (int)pIfList->dwNumberOfItems; i++)
		{
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			wprintf(L" Interface Index[%u]:\t %lu\n", i, i);
			iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString,
				sizeof(GuidString) / sizeof(*GuidString));

			if (iRet == 0)
				wprintf(L"StringFromGUID2 Failed\n");
			else
				wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString);

			wprintf(L"  Interface Description[%d]: %ws", i,
				pIfInfo->strInterfaceDescription);
			wprintf(L"\n");
			wprintf(L"  Interface State[%d]:\t ", i);
			switch (pIfInfo->isState) {
			case wlan_interface_state_not_ready:
				wprintf(L"Not ready\n");
				break;
			case wlan_interface_state_connected:
				wprintf(L"Connected\n");
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				wprintf(L"First node in a ad hoc network\n");
				break;
			case wlan_interface_state_disconnecting:
				wprintf(L"Disconnecting\n");
				break;
			case wlan_interface_state_disconnected:
				wprintf(L"Not connected\n");
				break;
			case wlan_interface_state_associating:
				wprintf(L"Attempting to associate with a network\n");
				break;
			case wlan_interface_state_discovering:
				wprintf(L"Auto configuration is discovering settings for the network\n");
				break;
			case wlan_interface_state_authenticating:
				wprintf(L"In process of authenticating\n");
				break;
			default:
				wprintf(L"Unknown state %ld\n", pIfInfo->isState);
				break;
			}
			wprintf(L"\n");

			const GUID * pGUID = &pIfInfo->InterfaceGuid;
			dwResult = WlanGetNetworkBssList(hClient, pGUID, NULL, dot11_BSS_type_any, 0, NULL, &pBssList);
			if (dwResult != ERROR_SUCCESS)
			{
				wprintf(L"Wlan get network bss list error: %lu\n", dwResult);
				return 1;
			}
			else
			{
				for (uint i = 0; i < pBssList->dwNumberOfItems; i++)
				{
					pBssEntry = &pBssList->wlanBssEntries[i];
					get_mac_id(pBssEntry->dot11Bssid, mac_id);
					if (find_at_radiomap(mac_id, result_map, pLocalizationNode) == 0)
					{
						pLocalizationNode->add_recoder(pBssEntry->lRssi);
					}
					else if (get_ssid(pBssEntry->dot11Ssid, ret_ssid) == 0)
					{
						LocalizationNode new_ap(mac_id, ret_ssid, pBssEntry->lRssi);
						ret = add_to_radiomap(new_ap, result_map);
						if (ret != 0)
						{
							printf("ERROR: add_to_radiomap error");
							exit(1);
						}
					}
				}
			}
		}
	}
	if (pBssList != NULL)
	{
		WlanFreeMemory(pBssList);
		pBssList = NULL;
	}
	if (pIfList != NULL)
	{
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}
	WlanCloseHandle(hClient, 0);
	return 0;
}

int wlan_bss_info()
{
	FILE *stream = NULL;
	RADIOMAP radiomap;
	errno_t err = 0;
	int ret = 0;
	int repeat = 20;
	int input = 0;
	
	// Reassign "stderr" to "freopen.out": 
	err = freopen_s(&stream, "freopen.txt", "w", stdout);
	if (err != 0)
	{
		fprintf(stdout, "error on freopen\n");
	}
	else
	{
		fprintf(stdout, "successfully reassigned\n"); fflush(stdout);
	}
	while (repeat--)
	{
		ret = get_ap_rssi_data(radiomap);
		if (ret != 0)
		{
			printf("ERROR: get_ap_rssi_data encounter error");
			exit(1);
		}
		check_set_padding(radiomap);
		Sleep(1000);
	}
	//diagnose the rssi data of aps
	RADIOMAP_ITER iter = radiomap.begin();
	for (; iter != radiomap.end(); iter++)
	{
		std::cout << iter->second << std::endl;
	}
	//printf("Digits 10 equal:\n\tHex: %i  Octal: %i  Decimal: %i\n", 0x10, 010, 10);
	
	//scanf_s("%d", &input);
	fflush(stream);
	fclose(stream);
	return 0;
}

#endif
