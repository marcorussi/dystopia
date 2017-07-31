/*
	The MIT License (MIT)

	Copyright (c) 2017 Marco Russi

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

/* file: unit_class.h */


/* TODO:
1 - implement a dedicated command structure to store all command info. 
	 ATTENTION: Cmd and Data keys and values could not be aligned if not stored in the same order. 
*/


#ifndef _UNIT_CLASS_H
#define _UNIT_CLASS_H


/* ------------ inclusion files ------------- */

#include <cstdlib>
#include <cerrno>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>
#include <map>
#include <memory>
#include <initializer_list>


/* namespace needed here. TODO: I don't like here... think about it */
using namespace std;

class Unit
{
	private:
		int unit_index;
		string unit_name;
		string uid_str;
		string cmd_uid_str;
		string data_uid_str;
		string status_str;
		string creation_date;
		string data_update;
		bool online_found;					/* used by leaders only */
		string mqttHost;						/* ATTENTION: check MQTT_FIELDS_STRING_MAX_LEN define in mqtt_mng.h */
		string mqttPort;						/* ATTENTION: check MQTT_FIELDS_STRING_MAX_LEN define in mqtt_mng.h */
		string mqttTopic;						/* ATTENTION: check MQTT_FIELDS_STRING_MAX_LEN define in mqtt_mng.h */
		vector<string> available_cmds;	/* store available commands to publish. UNIT -> LEADER */
		vector<string> cmd_key;				/* any received command key to be executed is stored here. LEADER -> UNIT */
		vector<string> cmd_value;			/* any received command value to be executed is stored here. LEADER -> UNIT */
		vector<string> data_key;			/* any sent data key to be published is stored here. UNIT -> LEADER */
		vector<string> data_value;			/* any sent data value to be published is stored here. UNIT -> LEADER */
	public:
		Unit();
		~Unit();
		void setIndex(int i) { unit_index = i; }
		int getIndex() { return unit_index; }
		void setName(string n) { unit_name = n; }
		string getName() { return unit_name; }
		void setUID(string n) { uid_str = n; }
		string getUID() { return uid_str; }
		void setCmdUID(string n) { cmd_uid_str = n; }
		string getCmdUID() { return cmd_uid_str; }
		void setDataUID(string n) { data_uid_str = n; }
		string getDataUID() { return data_uid_str; }
		void setStatus(string n) { status_str = n; }
		string getStatus() { return status_str; }
		void setCreationDate(string n) { creation_date = n; }
		string getCreationDate() { return creation_date; }
		void setDataUpdate(string n) { data_update = n; }
		string getDataUpdate() { return data_update; }
		void setOnlineFoundFlag(bool b) { online_found = b; }
		bool getOnlineFoundFlag() { return online_found; }
		void setMQTTHost(string h) { mqttHost = h; }
		string getMQTTHost() { return mqttHost; }
		void setMQTTPort(string p) { mqttPort = p; }
		string getMQTTPort() { return mqttPort; }
		void setMQTTTopic(string t) { mqttTopic = t; }
		string getMQTTTopic() { return mqttTopic; }
		void setAvailableCmd(string c) { available_cmds.push_back(c); }
		string getAvailableCmd(int i) { return available_cmds.at(i); }
		int getNumOfAvailableCmds() { return available_cmds.size(); }
		void setCmdObj(string k, string v) { cmd_key.push_back(k); cmd_value.push_back(v); }
		void setCmdValue(int i, string v) { cmd_value[i] = v; }
		string getCmdKey(int i) { return cmd_key.at(i); }
		string getCmdValue(int i) { return cmd_value.at(i); }
		int getNumOfCmds() { return cmd_key.size(); }
		void clearCmds() { cmd_key.clear(); cmd_value.clear(); }
		void setDataObj(string k, string v) { data_key.push_back(k); data_value.push_back(v); }
		void setDataValue(int i, string v) { data_value[i] = v; }
		string getDataKey(int i) { return data_key.at(i); }
		string getDataValue(int i) { return data_value.at(i); }
		int getNumOfDatas() { return data_key.size(); }
		void clearDatas() { data_key.clear(); data_value.clear(); }
} ;







#endif







