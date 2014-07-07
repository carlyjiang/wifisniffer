#ifndef UNICODE
#define UNICODE
#endif

#ifndef __WLAN_BSS_INFO_H__
#define __WLAN_BSS_INFO_H__

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>

class LocalizationNode;

typedef unsigned int uint;
typedef std::vector<int>::iterator VECTOR_INT_ITER;
typedef std::vector<int>::const_iterator VECTOR_INT_CONST_ITER;
typedef std::map<std::string, LocalizationNode>::iterator RADIOMAP_ITER;
typedef std::map<std::string, LocalizationNode>::const_iterator RADIOMAP_CONST_ITER;
typedef std::map<std::string, LocalizationNode> RADIOMAP;

int add_to_radiomap(LocalizationNode &value, std::map<std::string,
	LocalizationNode> &container);

int check_set_padding(RADIOMAP &_in);

int find_at_radiomap(std::string &key,
	RADIOMAP &container, LocalizationNode *&_out);

int get_ap_rssi_data(RADIOMAP &result_map);

int get_mac_id(DOT11_MAC_ADDRESS &_in, std::string &_out);

int get_ssid(DOT11_SSID &ssid, std::string &_out);

std::ostream &operator<<(std::ostream &out, const LocalizationNode &data);

int wlan_bss_info();

#endif

