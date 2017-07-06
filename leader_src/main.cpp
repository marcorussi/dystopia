/**
*
* .
*
*/


/* TODO: 
1 - consider to store commands and then publish all together with a dedicated user command
2 - implement a unit count check (MAX_NUM_OF_AVAILABLE_UNITS) 
*/


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

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

#include "json11.h"


#define MAX_NUM_OF_AVAILABLE_UNITS		5
#define MAX_NUM_OF_COMMANDS				6

#define GET_DWEETS_URL_STRING 			"https://dweet.io:443/get/dweets/for/"
#define CMD_DWEET_URL_STRING				"https://dweet.io:443/dweet/for/"
#define DATA_DWEET_URL_STRING				"https://dweet.io:443/get/latest/dweet/for/"

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

#define SENDER_UID_STRING					"2017TEST001A"
#define SENDER_NAME_STRING					"DYSTOPIA"

#define SENDER_FIXED_CMD_PRIORITY		"medium"	/* Fixed at the moment */


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
		bool online_found;
		vector<string> cmd;
		vector<string> data;
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
		void setOnlineFoundFlag(bool b) { online_found = b; }
		bool getOnlineFoundFlag() { return online_found; }
		void setCmd(string n) { cmd.push_back(n); }
		string getCmd(int i) { return cmd.at(i); }
		int getNumOfCmds() { return cmd.size(); }
		void setData(string n) { data.push_back(n); }
		string getData(int i) { return data.at(i); }
		int getNumOfDatas() { return data.size(); }
} ;


Unit::Unit()
{
	int j;
	unit_index = 0;
	unit_name = "no_name";
	uid_str = "";
	cmd_uid_str = "";
	data_uid_str = "";
	status_str = "";
	online_found = false;
	cmd.clear();
	data.clear();
} ;


Unit::~Unit()
{
	/* do nothing at the moment */
} ;


Unit units[MAX_NUM_OF_AVAILABLE_UNITS];
int units_count = 0;


string cmd_list[MAX_NUM_OF_COMMANDS] =
{
	"du",
	"su",
	"sa",
	"cs",
	"dg",
	"en"
};


/* Local functions prototypes */
void 	discOnlineUnits						( void );
bool 	showUnitInfo							( int );
int	getCmdIndex								( string );
int 	parseJsonGetUnits						( string, string );
bool 	setUnitCmd								( int, int, string );
int 	convertStrNum							( string );
bool 	parseJsonCheckDweetSucceded		( string );
bool 	getUnitData								( int, int );
bool 	parseJsonGetDataValue				( string, string, string );


int convertStrNum(string num_str)
{
	int num;
	stringstream conv_stream;

	conv_stream << num_str;
	conv_stream >> num;
	conv_stream.str("");
	conv_stream.clear();

	return num;
}


int main(int argc, char *argv[])
{
/*
	if(argc < 2) 
	{
   	cerr << "VERTEX: error -> insert URL" << endl;
		return EXIT_FAILURE;
	}

	char *url = argv[1];
*/
	int error = 0;
	volatile bool end_prog = false;
	string input, cmd_str;
	int cmd_index;

	while(end_prog != true)
	{
		error = 0;
		cout << "Available commands:" << endl 
		<< "du: check online units and list them" << endl 
		<< "su [u]: show unit data and cmd [u=unit index]" << endl 
		<< "sa: show data and cmd of all units" << endl 
		<< "cs [u] [c] [v]: set unit cmd value [u=unit index] [c=cmd index] [v=value]" << endl
		<< "dg [u] [d]: get unit data value [u=unit index] [d=data index]" << endl;

		cout << "Insert command:";
		getline(cin, input);
		cmd_str = input.substr(0, 2);

		cout << "__full command:" << input << endl;
		cout << "__command:" << cmd_str << endl;

		cmd_index = getCmdIndex(cmd_str);

		switch(cmd_index)
		{
			case 0: //"du"
			{
				discOnlineUnits();
				break;
			}
			
			case 1: //"su"
			{
				int u;
				u = convertStrNum(input.substr(3, 1));
				cout << "Inserted u: " << u << endl;
				if(false == showUnitInfo(u))
				{
					cout << "Parameter error!" << endl;
					error = 1;
				}
				break;
			}
			
			case 2: //"sa"
			{
				/* show all units */
				for(int i=0; i<units_count; i++)
				{
					showUnitInfo(i);
				}
				break;
			}
			
			case 3: //"cs"
			{
				int u, c;
				string v;
				u = convertStrNum(input.substr(3, 1));
				c = convertStrNum(input.substr(5, 1));	/* ATTENTION: erroneous letter values are converted anyway */
				v = input.substr(7);
				cout << "Inserted u: " << u << " - Inserted c: " << c << " - Inserted v: " << v << endl;
				if(false == setUnitCmd(u, c, v))
				{
					cout << "Parameters error!" << endl;
					error = 1;
				}
				break;
			}
			
			case 4: //"dg"
			{
				int u, d;
				u = convertStrNum(input.substr(3, 1));
				d = convertStrNum(input.substr(5, 1));	/* ATTENTION: erroneous letter values are converted anyway */
				cout << "Inserted u: " << u << " - Inserted d: " << d << endl;
				if(false == getUnitData(u, d))
				{
					cout << "Parameters error!" << endl;
					error = 1;
				}
				break;
			}
			
			case 5: //"en"
			{
				end_prog = true;
				break;
			}
			
			default:
			{
				/* invalid command: the getCmdIndex() function has returned MAX_NUM_OF_COMMANDS value */
				cerr << "VERTEX: error -> invalid command" << endl;
				error = 3;
			}
		}

		input.clear();
		cmd_str.clear();
		
		(0 == error) ? cout << "_____Command executed_____" << endl << endl << endl : cout << "_____Wrong command_____" << endl << endl << endl;
	}

	return EXIT_SUCCESS;
}


int getCmdIndex(string command)
{
	int n = 0;
	while((n < MAX_NUM_OF_COMMANDS)
	&&		(command.compare(cmd_list[n])))
	{
		n++;
	}

	return n;
}


bool showUnitInfo(int unit_index)
{
	bool bool_result;
	int j, num;

	if(unit_index <= units_count)
	{
		cout << "Unit index: " << units[unit_index].getIndex() << endl;
		cout << "- Uid: " << units[unit_index].getUID() << endl;
		cout << "- Cmd uid: " << units[unit_index].getCmdUID() << endl;
		cout << "- Data uid: " << units[unit_index].getDataUID() << endl;
		cout << "- Name: " << units[unit_index].getName() << endl;
		cout << "- Status: " << units[unit_index].getStatus() << endl;

		cout << "- Cmd: ";
		num = units[unit_index].getNumOfCmds();
		for(j=0; j<num; j++)
		{
			cout << units[unit_index].getCmd(j);
			(j<(num-1)) ? cout << ", " : cout << "." << endl;
		}

		cout << "- Data: ";
		num = units[unit_index].getNumOfDatas();
		for(j=0; j<num; j++)
		{
			cout << units[unit_index].getData(j);
			(j<(num-1)) ? cout << ", " : cout << "." << endl;
		}
		cout << ".............." << endl;

		bool_result = true;
	}
	else
	{
		bool_result = false;
	}

	return bool_result;
}


void discOnlineUnits( void )
{
	string url = GET_DWEETS_URL_STRING;
	stringstream result;
	int json_parse_code;

 	url.append(UNIT_THING_DWEET_CODE);

	try 
	{
		curlpp::Easy request;

		using namespace curlpp::Options;
		request.setOpt(Verbose(true));
		request.setOpt(Url(url));

		request.setOpt(cURLpp::Options::WriteStream(&result));

		request.perform();

		cout << "Result string: " << result.str() << endl;
	}

	catch ( curlpp::LogicError & e ) {
		cout << e.what() << endl;
	}
	catch ( curlpp::RuntimeError & e ) {
		cout << e.what() << endl;
	}

	/* parse Json string */
	json_parse_code = parseJsonGetUnits(result.str(), UNIT_THING_DWEET_CODE);

	switch(json_parse_code)
	{
		case 0:
		{
			cout << "LOG: Json parsing failed :(" << endl;
		}
		break;
		case 1:
		{
			cout << "LOG: Json parsing done successfully!" << endl;
		}
		break;
		default:
		{
			cout << "LOG: !Intenral uknown error!" << endl;
		}
	}
}


/* ATTENTION: only one command at a time can be sent */
bool setUnitCmd(int unit_index, int cmd_index, string cmd_str)
{
	Json obj1, obj2;
	string jsontext;
	string url = CMD_DWEET_URL_STRING;
	string cmd_key;
	stringstream result;

	if(unit_index >= units_count)
	{
		return false;
	}

	if(cmd_index < units[unit_index].getNumOfCmds())
	{
		cmd_key = units[unit_index].getCmd(cmd_index);
	}
	else
	{
		return false;
	}
	
	/* complete the url with the unit UID */
	url.append(units[unit_index].getCmdUID());

	/* fill all necessary keys-values fields in */
	obj1.set("sender_name", SENDER_NAME_STRING);
	obj1.set("sender_uid", SENDER_UID_STRING);
	obj1.set("priority", SENDER_FIXED_CMD_PRIORITY);
	/* set cmd key-value fields */
	obj2.set(cmd_key, cmd_str);	/* ATTENTION: only one command in the "cmd" ojbect is allowed at the moment */
	obj1.set("cmd", obj2);

	/* transform the Json object into a text string */
	jsontext = obj1.stringify();

	cout << "----- DWEET URL: " << url << endl;
	cout << "----- JSON TEXT: " << jsontext << endl;

	try
	{
		curlpp::Cleanup cleaner;
		curlpp::Easy request;

		request.setOpt(new curlpp::options::Url(url)); 
		request.setOpt(new curlpp::options::Verbose(true)); 

		list<string> header; 
		header.push_back("Content-Type: application/json"); 

		request.setOpt(new curlpp::options::HttpHeader(header)); 

		request.setOpt(new curlpp::options::PostFields(jsontext));
		request.setOpt(new curlpp::options::PostFieldSize(jsontext.size()));

		request.setOpt(cURLpp::Options::WriteStream(&result));

		request.perform(); 
	}

	catch ( curlpp::LogicError & e ) 
	{
		cout << e.what() << endl;
	}

	catch ( curlpp::RuntimeError & e ) 
	{
		cout << e.what() << endl;
	}

	cout << "Result string: " << result.str() << endl;

	return parseJsonCheckDweetSucceded(result.str());
}


bool getUnitData(int unit_index, int data_index)
{
	string url = DATA_DWEET_URL_STRING;
	string opt_str;
	stringstream result;

	if(unit_index >= units_count)
	{
		return false;
	}

	if(data_index >= units[unit_index].getNumOfDatas())
	{
		return false;
	}
	
	/* complete the url with the unit UID */
	url.append(units[unit_index].getDataUID());

	cout << "----- DWEET URL: " << url << endl;

	try 
	{
		curlpp::Easy request;

		using namespace curlpp::Options;
		request.setOpt(Verbose(true));
		request.setOpt(Url(url));

		request.setOpt(cURLpp::Options::WriteStream(&result));

		request.perform();

		cout << "Result string: " << result.str() << endl;
	}

	catch ( curlpp::LogicError & e ) {
		cout << e.what() << endl;
	}
	catch ( curlpp::RuntimeError & e ) {
		cout << e.what() << endl;
	}

	/* parse Json string */
	if(false == parseJsonGetDataValue(result.str(), units[unit_index].getDataUID(), units[unit_index].getData(data_index)))
		return false;

	return true;
}


bool parseJsonCheckDweetSucceded( string json_to_parse )
{
	string tmp;
	bool ret_bool = false;
	Json obj1; 

	obj1 = Json::parse(json_to_parse);

	/* ATTENTION: only "this" field is checked at the moment */
	if(Json::OBJECT == obj1.type())
	{
		if(true == obj1.has("this"))
		{
			tmp.append(obj1["this"]);
			if("succeeded" == tmp)
			{
				ret_bool = true;
			}
		}
	}

	return ret_bool;
}


int parseJsonGetUnits( string json_to_parse, string thing_to_find )
{
	string tmp;
	int i, j, ret_code = 0;	/* ATTENTION: consider to implement more codes */
	Json obj1, obj2, obj3, obj4, obj5; 

	obj1 = Json::parse(json_to_parse);

	if(Json::OBJECT == obj1.type())
	{
		if(true == obj1.has("this"))
		{
			tmp.append(obj1["this"]);
			if("succeeded" == tmp)
			{
				if(true == obj1.has("with"))
				{
					obj2 = obj1["with"];
					if(Json::ARRAY == obj2.type())
					{
						units_count = 0;
						for(i=0; i<obj2.size(); i++)
						{
							cout << "_______________" << endl;
							obj3 = obj2[i];
							if(true == obj3.has("thing"))
							{
								string thing_str = obj3["thing"];
								if(thing_to_find == thing_str)
								{
									if((true == obj3.has("content"))
									&& (true == obj3.has("created")))
									{
										string date_str = obj3["created"];
										cout << "last update: " << date_str << endl;

										obj4 = obj3["content"];
										if(Json::OBJECT == obj4.type())
										{
											if((true == obj4.has("uid"))
											&& (true == obj4.has("cmd_uid"))
											&& (true == obj4.has("data_uid"))
											&& (true == obj4.has("name"))
											&& (true == obj4.has("status"))
											&& (true == obj4.has("cmd"))
											&& (true == obj4.has("data")))
											{
												/* store and show unit info */
												units[units_count].setIndex(units_count);
												units[units_count].setOnlineFoundFlag(true);
												/* ATTENTION: UID must be a string else a bad cast error will occur */
												string str = "uid: "; 
												str.append(obj4["uid"]);
												cout << "_____ -" << str << endl;
												str.clear();
												units[units_count].setUID(obj4["uid"]);

												str = "cmd_uid: "; 
												str.append(obj4["cmd_uid"]);
												cout << "_____ -" << str << endl;
												str.clear();
												units[units_count].setCmdUID(obj4["cmd_uid"]);

												str = "data_uid: "; 
												str.append(obj4["data_uid"]);
												cout << "_____ -" << str << endl;
												str.clear();
												units[units_count].setDataUID(obj4["data_uid"]);

												str = "name: "; 
												str.append(obj4["name"]);
												cout << "_____ -" << str << endl;
												str.clear();
												units[units_count].setName(obj4["name"]);

												str = "status: ";
												str.append(obj4["status"]);
												cout << "_____ -" << str << endl;
												str.clear();
												units[units_count].setStatus(obj4["status"]);

												str = "cmd: ";
												obj5 = obj4["cmd"];
												if(Json::ARRAY == obj5.type())
												{
													for(j=0; j<obj5.size(); j++)
													{
														str.append(obj5[j]);
														(j==(obj5.size()-1)) ? str.append(".") : str.append(", ");
														units[units_count].setCmd(obj5[j]);
													}
												}
												else if(Json::STRING == obj5.type())
												{
													str.append(obj5);
													units[units_count].setCmd(obj5);
												}
												else
												{
													/* do nothing at the moment */
												}
												cout << "_____ -" << str << endl;
												str.clear();

												str = "data: ";
												obj5 = obj4["data"];
												if(Json::ARRAY == obj5.type())
												{
													for(j=0; j<obj5.size(); j++)
													{
														str.append(obj5[j]);
														(j==(obj5.size()-1)) ? str.append(".") : str.append(", ");
														units[units_count].setData(obj5[j]);
													}
												}
												else if(Json::STRING == obj5.type())
												{
													str.append(obj5);
													units[units_count].setData(obj5);
												}
												else
												{
													/* do nothing at the moment */
												}

												cout << "_____ -" << str << endl;
												str.clear();

												/* count number of found units */
												units_count++;
												/* 1 found unit at least */
												ret_code = 1;
											}
										}
									}
								}
							}
						}
						/* end of units array for loop */
					}
				}
			}
		}
	}

	cout << "_______________" << endl;
	if(1 == ret_code)
	{
		cout << "LOG: Found " << units_count << " valid unit(s) of type: " << thing_to_find << endl;
	}
	else
	{
		cout << "LOG: NO valid units found :(" << endl;
	}

	return ret_code;
}


bool parseJsonGetDataValue( string json_to_parse, string thing_to_find, string data_key )
{
	bool ret_bool = false;
	string tmp;
	int i, j;	/* ATTENTION: consider to implement more codes */
	Json obj1, obj2, obj3, obj4, obj5; 

	obj1 = Json::parse(json_to_parse);

	if(Json::OBJECT == obj1.type())
	{
		if(true == obj1.has("this"))
		{
			tmp.append(obj1["this"]);
			if("succeeded" == tmp)
			{
				if(true == obj1.has("with"))
				{
					obj2 = obj1["with"];
					if((Json::ARRAY == obj2.type())
					&& (1 == obj2.size()))				/* only 1 element should be present (latest dweet) */
					{
						obj3 = obj2[0];
						if(true == obj3.has("thing"))
						{
							string thing_str = obj3["thing"];
							if(thing_to_find == thing_str)
							{
								if((true == obj3.has("content"))
								&& (true == obj3.has("created")))
								{
									string date_str = obj3["created"];
									cout << "last update: " << date_str << endl;

									obj4 = obj3["content"];
									if(Json::OBJECT == obj4.type())
									{
										if((true == obj4.has("uid"))
										&& (true == obj4.has("name"))
										&& (true == obj4.has("status"))
										&& (true == obj4.has("data")))
										{
											/* ATTENTION: UID must be a string else a bad cast error will occur */
											string str = "uid: "; 
											str.append(obj4["uid"]);
											cout << "_____ -" << str << endl;
											str.clear();

											str = "name: "; 
											str.append(obj4["name"]);
											cout << "_____ -" << str << endl;
											str.clear();

											str = "status: ";
											str.append(obj4["status"]);
											cout << "_____ -" << str << endl;
											str.clear();

											str = "Requested data: ";
											obj5 = obj4["data"];
											if(Json::OBJECT == obj5.type())
											{
												if(true == obj5.has(data_key))
												{
													str.append(obj5[data_key]);
													str.append(" (found)");

													/* data found */
													ret_bool = true;
												}
												else
												{
													str.append(" - (not found)");
												}
											}
											else
											{
												str.append(" - (No data found)");
											}
											cout << "_____ -" << str << endl;
											str.clear();
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return ret_bool;
}






