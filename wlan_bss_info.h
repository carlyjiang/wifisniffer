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

class LocalizationNode
{
private:
	std::string mac_id;
	std::string ssid;
	std::vector<int> rssi_recoder;

public:
	LocalizationNode(const std::string &id, const std::string &ssid, int new_rssi)
		:mac_id(id), ssid(ssid)
	{
		this->rssi_recoder.push_back(new_rssi);
	}

	LocalizationNode(const LocalizationNode &rhs)
		:mac_id(rhs.mac_id), ssid(rhs.ssid), rssi_recoder(rhs.rssi_recoder)
	{}

	LocalizationNode & operator= (const LocalizationNode &rhs)
	{
		if (&rhs != this)
		{
			this->mac_id = rhs.mac_id;
			this->rssi_recoder = rhs.rssi_recoder;
			this->ssid = rhs.ssid;
		}
		else
		{
			return *this;
		}
	}

	VECTOR_INT_ITER find_the_first_unidentical_position(std::vector<int> &rhs)
	{
		// returns the rhs iterator which points to the position of object that is unidentical to lhs one
		VECTOR_INT_ITER lhs_iter = this->rssi_recoder.begin();
		VECTOR_INT_ITER rhs_iter = rhs.begin();
		for (; lhs_iter != this->rssi_recoder.end() && rhs_iter != rhs.end();
			lhs_iter++, rhs_iter++)
		{
			if (*lhs_iter != *rhs_iter)
				return rhs_iter;
		}
		return rhs_iter;
	}

	int add_recoder(int rssi)
	{
		rssi_recoder.push_back(rssi);
		return true;
	}

	std::vector<int> & get_all_recoders()
	{
		return rssi_recoder;
	}

	const std::string &get_key() const
	{
		return mac_id;
	}

	const std::string &get_ssid() const
	{
		return ssid;
	}

	void print(std::ostream &out) const
	{
		out << "mac_id: " << this->mac_id
			<< "\tssid: " << this->ssid << "\nrssi recoder: ";
		VECTOR_INT_CONST_ITER iter = rssi_recoder.begin();
		for (; iter != rssi_recoder.end(); iter++)
		{
			out << *iter << " ";
		}
		out << std::endl;
	}

	~LocalizationNode()
	{
		rssi_recoder.clear();
	}
};


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

