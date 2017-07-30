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
1 - implement a dedicated command structure to store all command info
2 - when a data value is updated, the timestamp is valid only for it but other values previously updated could be out of date
3 - consider to implement more than one unit 
*/




/* --------------- inclusions files ----------------- */

#include "unit.h"

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
#include "dweet.h"




/* --------------- local defines ----------------- */

#define UNIT1_THING_DWEET_CODE 			"unit1"
#define UNIT2_THING_DWEET_CODE 			"unit2"
#define UNIT3_THING_DWEET_CODE 			"unit3"
#define UNIT4_THING_DWEET_CODE 			"unit4"
#define UNIT5_THING_DWEET_CODE 			"unit5"
#define UNIT6_THING_DWEET_CODE 			"unit6"
#define UNIT7_THING_DWEET_CODE 			"unit7"
#define UNIT8_THING_DWEET_CODE 			"unit8"
#define UNIT9_THING_DWEET_CODE 			"unit9"

#define UNIT_THING_DWEET_CODE				UNIT7_THING_DWEET_CODE




/* local namespace */
using namespace std;




/* --------------- local variables ----------------- */

/* list of available unit commands */
static const vector<string> unit_cmd_list =
{
	"valve",
	"led",
	"ctrl"
};


/* list of available unit data */
static const vector<string> unit_data_list =
{
	"temp",
	"light",
	"switch",
	"batt"
};


/* ATTENTION: this istance must be after cmd and data vectors instances */
static Unit unit_sim;




/* --------------- local functions prototypes ----------------- */

static void initUnitInfo			( void );
static void	dummyExecuteCommand	( int, string, string );




/* ------------------- exported functions --------------- */

/* clear stored commands */
void UNIT_clearCmds( void )
{
	unit_sim.clearCmds();
}


/* show the unit info */
void UNIT_showUnitInfo( void )
{
	bool bool_result;
	int j, num;

	cout << "Unit index: " << unit_sim.getIndex() << endl;
	cout << "- Uid: " << unit_sim.getUID() << endl;
	cout << "- Cmd uid: " << unit_sim.getCmdUID() << endl;
	cout << "- Data uid: " << unit_sim.getDataUID() << endl;
	cout << "- Name: " << unit_sim.getName() << endl;
	cout << "- Status: " << unit_sim.getStatus() << endl;

	cout << "- Available Cmds: ";
	num = unit_sim.getNumOfAvailableCmds();
	for(j=0; j<num; j++)
	{
		cout << unit_sim.getAvailableCmd(j);
		(j<(num-1)) ? cout << ", " : cout << "." << endl;
	}

	cout << "- Cmd: ";
	num = unit_sim.getNumOfCmds();
	for(j=0; j<num; j++)
	{
		cout << "[" << unit_sim.getCmdKey(j) << " : " << unit_sim.getCmdValue(j) << "]";
		(j<(num-1)) ? cout << ", " : cout << "." << endl;
	}

	cout << "- Data: ";
	num = unit_sim.getNumOfDatas();
	for(j=0; j<num; j++)
	{
		cout << "[" << unit_sim.getDataKey(j) << " : " << unit_sim.getDataValue(j) << "]";
		(j<(num-1)) ? cout << ", " : cout << "." << endl;
	}
	cout << ".............." << endl;
}


/* publish the unit online */
bool UNIT_publishUnitOnline( void )
{
	int i;
	JSON_OBJ obj, cmd_obj, data_obj;
	string str;

	/* init unit info first! */
	initUnitInfo();

	/* fill all necessary keys-values fields in */
	str = unit_sim.getUID();
	obj.set("uid", str);
	str = unit_sim.getCmdUID();
	obj.set("cmd_uid", str);
	str = unit_sim.getDataUID();
	obj.set("data_uid", str);
	str = unit_sim.getName();
	obj.set("name", str);
	str = unit_sim.getStatus();
	obj.set("status", str);

	/* set available cmds fields */
	for(i=0; i<unit_sim.getNumOfAvailableCmds(); i++)
	{
		str = unit_sim.getAvailableCmd(i);
		cmd_obj << str;
	}
	obj.set("cmd", cmd_obj);

	/* set data key-value fields */
	for(i=0; i<unit_sim.getNumOfDatas(); i++)
	{
		str = unit_sim.getDataKey(i);
		data_obj << str;
	}
	obj.set("data", data_obj);

	/* call dweet module function */
	return DWEET_publishUnitOnline( &obj, UNIT_THING_DWEET_CODE );
}


/* function to get and manage new commands */
void UNIT_getAndManageNewCmds( void )
{
	int idx;

	/* get new commands */
	if(true == UNIT_getLatestCmds())
	{
		/* ATTENTION: execution means show commands and it is done immediately */
		for(idx=0; idx<unit_sim.getNumOfCmds(); idx++)
		{
			dummyExecuteCommand(idx, unit_sim.getCmdKey(idx), unit_sim.getCmdValue(idx));
		}
	}
	else
	{
		cout << "No new commands" << endl;
	}
}


/* get latest commands if any */
bool UNIT_getLatestCmds( void )
{
 	return DWEET_getLatestCommands( &unit_sim );
}


/* show latest received data commands */
void UNIT_showLatestCmds( void )
{
	int k, num;

	/* get num of received cmds */
	num = unit_sim.getNumOfCmds();
	/* if there are cmds */
	if(num > 0)
	{
		/* show cmds */
		for(k=0; k<num; k++)
		{
			cout << endl << "---------------------------------" << endl;
			cout << "COMMAND " << k << " :" << endl;
			cout << "Key: " << unit_sim.getCmdKey(k) << endl;
			cout << "Value: " << unit_sim.getCmdValue(k) << endl;
			cout << "---------------------------------" << endl;
		}
	}
	else
	{
		/* no cmds received */
		cout << "No commands to execute" << endl;
	}
}


/* show stored unit data values */
void UNIT_showDataValues( void )
{
	int m;

	cout << "Unit Info: " << endl;
	cout << "- uid: " << unit_sim.getUID() << endl;
	cout << "- data uid: " << unit_sim.getDataUID() << endl;
	cout << "- name: " << unit_sim.getName() << endl;
	cout << "- data: " << endl;

	for(m=0; m<unit_sim.getNumOfDatas(); m++)
	{
		cout << "      - " << unit_sim.getDataKey(m) << " -> " << unit_sim.getDataValue(m) << endl;
	}

	cout << "________________________" << endl;
}


/* store and publish online a data value */
bool UNIT_setAndPublishDataValue( int data_index, string data_value_str )
{
	int i;
	JSON_OBJ obj, data_obj;	
	string str;

	if(data_index >= unit_sim.getNumOfDatas())
	{
		return false;
	}

	/* store data locally */
	unit_sim.setDataValue(data_index, data_value_str);

	/* fill all necessary keys-values fields in */
	str = unit_sim.getUID();
	obj.set("uid", str);
	str = unit_sim.getName();
	obj.set("name", str);
	str = unit_sim.getStatus();
	obj.set("status", str);

	/* store data */
	for(i=0; i<unit_sim.getNumOfDatas(); i++)
	{
		str = unit_sim.getDataValue(i);
		data_obj.set(unit_sim.getDataKey(i), str);
	}
	obj.set("data", data_obj);

	/* call dweet module function */
	return DWEET_publishDataValue( &obj, unit_sim.getDataUID() );
}




/* ------------- local functions ------------- */

/* Dummy fucntion that just shows command key and value */
static void dummyExecuteCommand( int cmd_index, string cmd_str, string cmd_value )
{
	/* print command info */
	cout << endl << "---------------------------------" << endl;
	cout << "COMMAND " << cmd_index << " :" << endl;
	cout << "Key: " << cmd_str << endl;
	cout << "Value: " << cmd_value << endl;
	cout << "---------------------------------" << endl;
}


/* Function to init unit info */
static void initUnitInfo( void )
{
	int j;
	
	/* Default init info */
	unit_sim.setIndex(0);	/* one unit only at the moment */
	unit_sim.setName("SIM_UNIT");
	unit_sim.setUID("2017SIMUNIT001A");
	unit_sim.setCmdUID("CMDSIMUNIT01");
	unit_sim.setDataUID("DATASIMUNIT01");
	unit_sim.setStatus("ACTIVE");			/* fixed at the moment */
	unit_sim.setOnlineFoundFlag(false);	/* used from leader only */

	for(j=0; j<unit_cmd_list.size(); j++)
	{
		unit_sim.setAvailableCmd(unit_cmd_list[j]);
	}

	for(j=0; j<unit_data_list.size(); j++)
	{
		unit_sim.setDataObj(unit_data_list[j], " - ");
	}
}




/* End of file */











