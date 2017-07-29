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
1 - when a data value is updated, the timestamp is valid only for it but other values previously updated could be out of date
2 - implement more than one unit 
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




using namespace std;




/* --------------- UnitSim class ----------------- */

class UnitSim
{
	private:
		int unit_index;
		string unit_name;
		string uid_str;
		string cmd_uid_str;
		string data_uid_str;
		string status_str;
		bool online_pub;
		vector<string> cmd;
		vector<string> data;
		vector<string> data_values;
	public:
		UnitSim();
		~UnitSim();
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
		void setOnlinePubFlag(bool b) { online_pub = b; }
		bool getOnlinePubFlag() { return online_pub; }
		void setCmd(string n) { cmd.push_back(n); }
		string getCmd(int i) { return cmd.at(i); }
		int getNumOfCmds() { return cmd.size(); }
		void setData(string n) { data.push_back(n); }
		string getData(int i) { return data.at(i); }
		int getNumOfDatas() { return data.size(); }
		void setDataValue(int i, string n) { data_values[i] = (n); }
		string getDataValue(int i) { return data_values.at(i); }
} ;




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


/* latest commands for the implemented unit */
static vector<cmd_st> latest_cmds;


/* ATTENTION: this istance must be after cmd and data vectors instances */
static UnitSim units_sim;




/* --------------- class functions ----------------- */

/* ATTENTION: only one unit used at the moment */
UnitSim::UnitSim()
{
	int j;
	
	/* Default init info */
	unit_index = 0;
	unit_name = "SIM_UNIT";
	uid_str = "2017SIMUNIT001A";
	cmd_uid_str = "CMDSIMUNIT01";
	data_uid_str = "DATASIMUNIT01";
	status_str = "ACTIVE";
	online_pub = false;
	/* ATTENTION: be sure that cmd and data vectors instances are declared before UnitSim instance */
	cmd = unit_cmd_list;
	data = unit_data_list;
	for(j=0; j<unit_data_list.size(); j++)
		data_values.push_back(" - ");
} ;


UnitSim::~UnitSim()
{
	/* do nothing at the moment */
} ;




/* --------------- local functions prototypes ----------------- */
static void	executeCommand	( cmd_st * );




/* ------------------- exported functions --------------- */

/* clear stored commands */
void UNIT_clearCmds( void )
{
	latest_cmds.clear();
}


/* show the unit info */
void UNIT_showUnitInfo( void )
{
	bool bool_result;
	int j, num;

	cout << "Unit index: " << units_sim.getIndex() << endl;
	cout << "- Uid: " << units_sim.getUID() << endl;
	cout << "- Cmd uid: " << units_sim.getCmdUID() << endl;
	cout << "- Data uid: " << units_sim.getDataUID() << endl;
	cout << "- Name: " << units_sim.getName() << endl;
	cout << "- Status: " << units_sim.getStatus() << endl;

	cout << "- Cmd: ";
	num = units_sim.getNumOfCmds();
	for(j=0; j<num; j++)
	{
		cout << units_sim.getCmd(j);
		(j<(num-1)) ? cout << ", " : cout << "." << endl;
	}

	cout << "- Data: ";
	num = units_sim.getNumOfDatas();
	for(j=0; j<num; j++)
	{
		cout << units_sim.getData(j);
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

	/* fill all necessary keys-values fields in */
	str = units_sim.getUID();
	obj.set("uid", str);
	str = units_sim.getCmdUID();
	obj.set("cmd_uid", str);
	str = units_sim.getDataUID();
	obj.set("data_uid", str);
	str = units_sim.getName();
	obj.set("name", str);
	str = units_sim.getStatus();
	obj.set("status", str);

	/* set cmd key-value fields */
	for(i=0; i<units_sim.getNumOfCmds(); i++)
	{
		str = units_sim.getCmd(i);
		cmd_obj << str;
	}
	obj.set("cmd", cmd_obj);

	/* set data key-value fields */
	for(i=0; i<units_sim.getNumOfDatas(); i++)
	{
		str = units_sim.getData(i);
		data_obj << str;
	}
	obj.set("data", data_obj);

	/* call dweet module function */
	return DWEET_publishUnitOnline( &obj, UNIT_THING_DWEET_CODE );
}


/* function to get and manage new commands */
void UNIT_getAndManageNewCmds( void )
{
	/* get new commands */
	if(true == UNIT_getLatestCmds())
	{
		/* ATTENTION: execution means show commands and it is done immediately */
		for(int i=0; i<latest_cmds.size(); i++)
		{
			executeCommand(&latest_cmds[i]);
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
	stringstream result;

 	return DWEET_getLatestCommands( units_sim.getCmdUID(), &latest_cmds );
}


/* show latest received data commands */
void UNIT_showLatestCmds( void )
{
	int k, m;

	for(k=0; k<latest_cmds.size(); k++)
	{
		cout << "COMMAND " << k << ":" << endl;
		cout << "- sender uid: " << latest_cmds[k].sender_uid << endl;
		cout << "- sender name: " << latest_cmds[k].sender_name << endl;
		cout << "- priority: " << latest_cmds[k].priority << endl;
		cout << "- cmd: " << endl;

		for(m=0; m<latest_cmds[k].cmd_keys.size(); m++)
		{
			cout << "      - " << latest_cmds[k].cmd_keys[m] << " -> " << latest_cmds[k].cmd_strings[m] << endl;
		}

		cout << "________________________" << endl;
	}
}


/* show stored unit data values */
void UNIT_showDataValues( void )
{
	int m;

	cout << "Unit 0: " << endl;
	cout << "- uid: " << units_sim.getUID() << endl;
	cout << "- data uid: " << units_sim.getDataUID() << endl;
	cout << "- name: " << units_sim.getName() << endl;
	cout << "- data: " << endl;

	for(m=0; m<units_sim.getNumOfDatas(); m++)
	{
		cout << "      - " << units_sim.getData(m) << " -> " << units_sim.getDataValue(m) << endl;
	}

	cout << "________________________" << endl;
}


/* store and publish online a data value */
bool UNIT_setAndPublishDataValue( int data_index, string data_value_str )
{
	int i;
	JSON_OBJ obj, data_obj;	
	string str;

	if(data_index >= units_sim.getNumOfDatas())
	{
		return false;
	}

	/* store data locally */
	units_sim.setDataValue(data_index, data_value_str);

	/* fill all necessary keys-values fields in */
	str = units_sim.getUID();
	obj.set("uid", str);
	str = units_sim.getName();
	obj.set("name", str);
	str = units_sim.getStatus();
	obj.set("status", str);

	/* store data */
	for(i=0; i<units_sim.getNumOfDatas(); i++)
	{
		str = units_sim.getDataValue(i);
		data_obj.set(units_sim.getData(i), str);
	}
	obj.set("data", data_obj);

	/* call dweet module function */
	return DWEET_publishDataValue( &obj, units_sim.getDataUID() );
}




/* ------------- local functions ------------- */

/* execute received unit commands. ATTENTION: just show commands at the moment */
static void executeCommand(cmd_st *command)
{
	int i;

	/* print command info */
	cout << endl << "---------------------------------" << endl;
	cout << "COMMAND INFO: " << endl;
	cout << "Sender ID: " << command->sender_uid << endl;
	cout << "Sender Name: " << command->sender_name << endl;
	cout << "Priority: " << command->priority << endl;

	/* print commands and their values */
	for(i = 0; i < command->cmd_keys.size(); i++)
	{
		cout << " - " << command->cmd_keys[i] << " : " << command->cmd_strings[i] << endl;
	}
}




/* End of file */











