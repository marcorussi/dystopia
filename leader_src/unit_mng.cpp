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

/* file: unit.cpp */

/* TODO: 
1 - in case of multiple dweets of same units the leader discover them as different units. SOLVE THIS.
2 - consider to store commands and then publish all together with a dedicated user command
*/




/* ----------------- inclusion file -------------- */

#include "unit_mng.h"

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

#include "unit_class.h"
#include "mqtt_mng.h"
#include "dweet.h"




/* ----------------- local constants -------------- */

#define UNIT1_THING_DWEET_CODE 			"unit1"
#define UNIT2_THING_DWEET_CODE 			"unit2"
#define UNIT3_THING_DWEET_CODE 			"unit3"
#define UNIT4_THING_DWEET_CODE 			"unit4"
#define UNIT5_THING_DWEET_CODE 			"unit5"
#define UNIT6_THING_DWEET_CODE 			"unit6"
#define UNIT7_THING_DWEET_CODE 			"unit7"
#define UNIT8_THING_DWEET_CODE 			"unit8"
#define UNIT9_THING_DWEET_CODE 			"unit9"

#define UNIT_THING_DWEET_CODE				UNIT1_THING_DWEET_CODE

#define SENDER_UID_STRING					"2017TEST001A"
#define SENDER_NAME_STRING					"DYSTOPIA"

#define SENDER_FIXED_CMD_PRIORITY		"medium"	/* Fixed at the moment */




/* local namespace */
using namespace std;




/* ----------------- local variables -------------- */

/* list of units */
static vector<Unit> unit_list;


/* Indicate that MQTT is running (true) */
static bool bMQTTIsRunning = false;


/* Store MQTT instance */
/* ATTENTION: one MQTT instance only */
static class mqtt_mng *mqtt_instance;




/* ------------- Local functions prototypes ----------------- */

static void startMQTT	( void );
static void stopMQTT		( void );




/* ------------- Exported functions ----------------- */

/* show info of all units */
void UNIT_showAllUnitsInfo( void )
{
	int i;
	for(i=0; i<unit_list.size(); i++)
	{
		UNIT_showUnitInfo(i);
	}
}


/* show unit info */
bool UNIT_showUnitInfo(int unit_index)
{
	bool bool_result;
	int j, num;

	cout << "Unit index: " << unit_list[unit_index].getIndex() << endl;
	cout << "- Uid: " << unit_list[unit_index].getUID() << endl;
	cout << "- Cmd uid: " << unit_list[unit_index].getCmdUID() << endl;
	cout << "- Data uid: " << unit_list[unit_index].getDataUID() << endl;
	cout << "- Name: " << unit_list[unit_index].getName() << endl;
	cout << "- Status: " << unit_list[unit_index].getStatus() << endl;

	/* show MQTT info */
	cout << "- MQTT Host: " << unit_list[unit_index].getMQTTHost() << endl;
	cout << "- MQTT Port: " << unit_list[unit_index].getMQTTPort() << endl;
	cout << "- MQTT Rx Topic: " << unit_list[unit_index].getMQTTRxTopic() << endl;
	cout << "- MQTT Tx Topic: " << unit_list[unit_index].getMQTTTxTopic() << endl;

	cout << "- Cmd: ";
	num = unit_list[unit_index].getNumOfCmds();
	for(j=0; j<num; j++)
	{
		cout << "[" << unit_list[unit_index].getCmdKey(j) << " : " << unit_list[unit_index].getCmdValue(j) << "]";
		(j<(num-1)) ? cout << ", " : cout << "." << endl;
	}

	cout << "- Data: ";
	num = unit_list[unit_index].getNumOfDatas();
	for(j=0; j<num; j++)
	{
		cout << "[" << unit_list[unit_index].getDataKey(j) << " : " << unit_list[unit_index].getDataValue(j) << "]";
		(j<(num-1)) ? cout << ", " : cout << "." << endl;
	}
	cout << ".............." << endl;

	bool_result = true;

	return bool_result;
}


/* discover online units */
bool UNIT_discOnlineUnits( void )
{
	return DWEET_discOnlineUnits(UNIT_THING_DWEET_CODE, &unit_list);
}


/* send unit command online */
/* ATTENTION: only one command at a time can be sent */
bool UNIT_setUnitCmd(int unit_index, int cmd_index, string cmd_str)
{
	JSON_OBJ obj1, obj2;
	string cmd_key;

	if(unit_index >= unit_list.size())
	{
		return false;
	}

	if(cmd_index < unit_list[unit_index].getNumOfCmds())
	{
		cmd_key = unit_list[unit_index].getCmdKey(cmd_index);
		unit_list[unit_index].setCmdValue(cmd_index, cmd_str);
	}
	else
	{
		return false;
	}

	/* fill all necessary keys-values fields in */
	obj1.set("sender_name", SENDER_NAME_STRING);
	obj1.set("sender_uid", SENDER_UID_STRING);
	obj1.set("priority", SENDER_FIXED_CMD_PRIORITY);
	/* set cmd key-value fields */
	obj2.set(cmd_key, cmd_str);	/* ATTENTION: only one command in the "cmd" ojbect is allowed at the moment */
	obj1.set("cmd", obj2);

	/* call dweet module function */
	return DWEET_setAndPublishUnitCmd(&obj1, &unit_list[unit_index]);
}


/* get unit data from online */
bool UNIT_getUnitData(int unit_index)
{
	if(unit_index >= unit_list.size())
	{
		return false;
	}

	/* get latest unit data */
	return DWEET_getLatestUnitData( &unit_list[unit_index] );
}


/* Return true if MQTT is running */
bool UNIT_bIsMQTTRunning( void )
{
	return bMQTTIsRunning;
}


/* Function to manage MQTT loop */
void UNIT_manageMQTTLoop( void )
{
	int rc;

	rc = mqtt_instance->loop();
	if(rc)
	{
		mqtt_instance->reconnect();
	}
}	


/* start MQTT */
bool UNIT_startMQTT( int unit_index )
{
	int port, i, num;
	string field_to_conv;
	char id_str[MQTT_FIELDS_STRING_MAX_LEN+1];	
	char host_str[MQTT_FIELDS_STRING_MAX_LEN+1];	
	char rxtopic_str[MQTT_FIELDS_STRING_MAX_LEN+1];
	char txtopic_str[MQTT_FIELDS_STRING_MAX_LEN+1];

	memset(id_str, 0, MQTT_FIELDS_STRING_MAX_LEN+1);
	memset(host_str, 0, MQTT_FIELDS_STRING_MAX_LEN+1);
	memset(rxtopic_str, 0, MQTT_FIELDS_STRING_MAX_LEN+1);
	memset(txtopic_str, 0, MQTT_FIELDS_STRING_MAX_LEN+1);

	if(unit_index >= unit_list.size())
		return false;

	/* MQTT UID */
	field_to_conv = SENDER_UID_STRING;
	memcpy(id_str, field_to_conv.c_str(), field_to_conv.size());
	field_to_conv.clear();

	/* MQTT host */
	field_to_conv = unit_list[unit_index].getMQTTHost();
	memcpy(host_str, field_to_conv.c_str(), field_to_conv.size());
	field_to_conv.clear();

	/* MQTT RX topic */
	field_to_conv = unit_list[unit_index].getMQTTRxTopic();
	memcpy(rxtopic_str, field_to_conv.c_str(), field_to_conv.size());
	field_to_conv.clear();

	/* MQTT TX topic */
	field_to_conv = unit_list[unit_index].getMQTTTxTopic();
	memcpy(txtopic_str, field_to_conv.c_str(), field_to_conv.size());
	field_to_conv.clear();

	/* MQTT port */
	port = unit_list[unit_index].getMQTTPort();

	mosqpp::lib_init();

	/* ATTENTION: one MQTT instance */
	mqtt_instance = new mqtt_mng(	(const char *)id_str, 
											(const char *)host_str, 
											port, 
											(const char *)rxtopic_str,
											(const char *)txtopic_str);

	/* ATTENTION: this flag should be set after successfull connection and subscription! */
	bMQTTIsRunning = true;

	return true;
}


/* stop MQTT */
void UNIT_stopMQTT( void )
{
	mosqpp::lib_cleanup();

	bMQTTIsRunning = false;
}


/* send MQTT message */
void UNIT_sendMQTTMessage( const char *message )
{
	if(true == bMQTTIsRunning)
	{
		mqtt_instance->send_msg(message);
	}
	else
	{
		/* dicard the request */
	}
}




/* End of line */












