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

/* file: dweet.cpp */


/* ---------- Inclusions files --------- */
#include "dweet.h"

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

#include "unit_class.h"
#include "json11.h"




/* ------------ constants -------------- */
#define PUBLISH_DWEET_URL_STRING			"https://dweet.io:443/dweet/for/"
#define GET_CMD_DWEET_URL_STRING			"https://dweet.io:443/get/dweets/for/"




/* ------------ local namespace -------------- */
using namespace std;




/* ------------ local variables -------------- */

/* store last cmds date */
static string last_cmds_date = "";




/* ------------ local functions prototypes -------------- */
static bool checkDweetSucceded	( string );
static bool parseJsonGetCmds		( string, Unit * );




/* ------------ exported functions ------------------ */

/* publish units online; POST method to dweet.io */
bool DWEET_publishUnitOnline( Json *json_obj, string unit_uid_str )
{
	string jsontext;
	string url = PUBLISH_DWEET_URL_STRING;
	stringstream result;
	
	/* complete the url with the unit UID */
	url.append(unit_uid_str);

	/* transform the Json object into a text string */
	jsontext = json_obj->stringify();

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

		//cout << "Result string: " << result.str() << endl;
	}

	catch ( curlpp::LogicError & e ) 
	{
		cout << e.what() << endl;
	}

	catch ( curlpp::RuntimeError & e ) 
	{
		cout << e.what() << endl;
	}

	return checkDweetSucceded(result.str());
}


/* get lastest commands; GET method to dweet.io */
bool DWEET_getLatestCommands( Unit *unit_info )
{
	string url = GET_CMD_DWEET_URL_STRING;
	stringstream result;

 	url.append(unit_info->getCmdUID());

	try 
	{
		curlpp::Easy request;

		using namespace curlpp::Options;
		request.setOpt(Verbose(true));
		request.setOpt(Url(url));

		request.setOpt(cURLpp::Options::WriteStream(&result));

		request.perform();

		//cout << "Result string: " << result.str() << endl;
	}

	catch ( curlpp::LogicError & e ) {
		cout << e.what() << endl;
	}
	catch ( curlpp::RuntimeError & e ) {
		cout << e.what() << endl;
	}

	/* parse Json string */
	return parseJsonGetCmds(result.str(), unit_info);
}


/* publish data value online; POST method to dweet.io */
bool DWEET_publishDataValue( Json *obj, string data_uid_str )
{
	string jsontext;
	string url = PUBLISH_DWEET_URL_STRING;
	stringstream result;

	/* complete the url with the data UID */
	url.append(data_uid_str);

	/* transform the Json object into a text string */
	jsontext = obj->stringify();

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

		//cout << "Result string: " << result.str() << endl; 
	}

	catch ( curlpp::LogicError & e ) 
	{
		cout << e.what() << endl;
	}

	catch ( curlpp::RuntimeError & e ) 
	{
		cout << e.what() << endl;
	}

	return checkDweetSucceded(result.str());
}




/* --------------- local functions ---------------- */

/* check success of POST on dweet.io */
static bool checkDweetSucceded( string json_to_parse )
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


/* parse received JSON from dweet.io and store new commands if any */
static bool parseJsonGetCmds( string json_to_parse, Unit *unit_info_ref )
{
	int i, j;
	bool ret_bool = false;
	Json obj1, obj2, obj3, obj4, obj5; 

	/* if there no previous commands */
	if(0 == unit_info_ref->getNumOfCmds())
	{
		/* last new command date is null */
		cout << "--- dweet: No previous commands" << endl;
		last_cmds_date = "";
	}

	/* start parsing */
	obj1 = Json::parse(json_to_parse);

	if(Json::OBJECT == obj1.type())
	{
		if(true == obj1.has("this"))
		{
			string tmp;
			tmp.append(obj1["this"]);
			if("succeeded" == tmp)
			{
				if(true == obj1.has("with"))
				{
					obj2 = obj1["with"];
					if(Json::ARRAY == obj2.type())
					{
						for(i=0; i<obj2.size(); i++)
						{
							obj3 = obj2[i];
							if(true == obj3.has("thing"))
							{
								string thing_str = obj3["thing"];
								if(unit_info_ref->getCmdUID() == thing_str)
								{
									if((true == obj3.has("content"))
									&& (true == obj3.has("created")))
									{
										string date_str = obj3["created"];
										/* check date */
										if(date_str != last_cmds_date)
										{
											obj4 = obj3["content"];
											if(Json::OBJECT == obj4.type())
											{
												if((true == obj4.has("sender_name"))
												&& (true == obj4.has("sender_uid"))
												&& (true == obj4.has("priority"))
												&& (true == obj4.has("cmd")))
												{
													cout << "--- dweet: New valid commands found!" << endl;

													if(0==i)	/* the first cmd is the latest one. Refer to it for next cmds auto check */
													{
														cout << "--- dweet: Prev. cmd date: " << last_cmds_date << ". New cmd date: " << date_str << endl;
														/* store cmd creation date */
														last_cmds_date = date_str;
													}

													/* ATTENTION: sender fields below are not stored at the moment */
													string str;
													/* ATTENTION: sender UID must be a string else a bad cast error will occur */
													str.append(obj4["sender_uid"]);
													cout << "_____ - sender_uid: " << str << endl;
													str.clear();

													str.append(obj4["sender_name"]);
													cout << "_____ - sender_name: " << str << endl;
													str.clear();

													str.append(obj4["priority"]);
													cout << "_____ - priority: " << str << endl;
													str.clear();

													cout << "_____ - cmd: " << endl;
													obj5 = obj4["cmd"];
													if(Json::OBJECT == obj5.type())	/* ATTENTION: only one command is used at the moment */
													{
														vector<string> keys = obj5.keys();

														if(0==i)	/* the first cmd is the latest one. Cancel previous cmds here */
														{
															/* clear previous commands first */
															unit_info_ref->clearCmds();
														}

														for(j=0; j<keys.size(); j++)
														{
															/* store command */
															unit_info_ref->setCmdObj(keys[j], obj5[keys[j]]);
															str.append(keys[j]);
															str.append(" -> ");
															str.append(obj5[keys[j]]);
															cout << "___________ -" << str << endl;
															str.clear();
														}
													}
													else
													{
														/* do nothing at the moment */
													}

													/* 1 found command at least */
													ret_bool = true;
												}
											}
											else
											{
												/* invalid JSON object. The command has the date only at the moment.
													TODO: manage this situation. */
											}
										}
										else
										{
											/* old command: discard it */
											/* going further next commands are older 
												so we can interrupt searching here. */
											return ret_bool;
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

	return ret_bool;
}




/* End of file */



