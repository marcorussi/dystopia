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




/* -------------- inclusion files ------------ */

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




/* ---------------- local constants -------------------- */

#define GET_DWEETS_URL_STRING 			"https://dweet.io:443/get/dweets/for/"
#define CMD_DWEET_URL_STRING				"https://dweet.io:443/dweet/for/"
#define DATA_DWEET_URL_STRING				"https://dweet.io:443/get/latest/dweet/for/"




/* ---------------- local functions prototypes -------------------- */

static bool checkDweetSucceded	( string );
static bool parseJsonGetData		( string, Unit * );
static bool parseJsonGetUnits		( string, string, vector<Unit> * );




/* ---------------- exported functions -------------- */

/* set and publish a unit command to dweet.io */
bool DWEET_setAndPublishUnitCmd( Json *json_obj, Unit *unit_info )
{
	string jsontext;
	string url = CMD_DWEET_URL_STRING;
	stringstream result;

	/* complete the url with the unit UID */
	url.append(unit_info->getCmdUID());

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


/* get latest unit data value from dweet.io */
bool DWEET_getLatestUnitData( Unit *unit_info )
{
	string url = DATA_DWEET_URL_STRING;
	stringstream result;

	/* complete the url with the unit UID */
	url.append(unit_info->getDataUID());

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

	/* parse Json string and get data */
	return parseJsonGetData( result.str(), unit_info );
}


/* discover online units */
bool DWEET_discOnlineUnits( string unit_thing_string, vector<Unit> *unit_list_ptr )
{
	string url = GET_DWEETS_URL_STRING;
	stringstream result;

 	url.append(unit_thing_string);

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
	return parseJsonGetUnits(result.str(), unit_thing_string, unit_list_ptr);
}




/* ---------------- local functions ---------------------- */

/* check success of a POST method to dweet.io */
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


/* parse received JSON and get data value */
static bool parseJsonGetData( string json_to_parse, Unit *unit_info_ref )
{
	bool ret_bool = false;
	Json obj1, obj2, obj3, obj4, obj5; 

	obj1 = Json::parse(json_to_parse);

	if(Json::OBJECT == obj1.type())
	{
		if(true == obj1.has("this"))
		{
			string tmp = (obj1["this"]);
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
							if(unit_info_ref->getDataUID() == thing_str)
							{
								if((true == obj3.has("content"))
								&& (true == obj3.has("created")))
								{
									string date_str = obj3["created"];
									/* copy date */
									unit_info_ref->setDataUpdate(date_str);

									obj4 = obj3["content"];
									if(Json::OBJECT == obj4.type())
									{
										if((true == obj4.has("uid"))
										&& (true == obj4.has("name"))
										&& (true == obj4.has("status"))
										&& (true == obj4.has("data")))
										{
											/* ATTENTION: other keys can be checked for security */
											obj5 = obj4["data"];
											if(Json::OBJECT == obj5.type())
											{
												int i;
												/* clear pevious data */
												unit_info_ref->clearDatas();
												/* get data keys */
												vector<string> keys = obj5.keys();
												/* get data values */
												for(i=0; i<keys.size(); i++)
												{
													unit_info_ref->setDataObj(keys[i], obj5[keys[i]]);
												}

												/* data found */
												ret_bool = true;
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
	}

	return ret_bool;
}


/* parse received JSON and get units info */
static bool parseJsonGetUnits( string json_to_parse, string thing_to_find, vector<Unit> *unit_list_ptr_ref )
{
	int i, j;
	bool ret_bool;
	Json obj1, obj2, obj3, obj4, obj5; 
	Unit temp_unit;

	ret_bool = false;

	obj1 = Json::parse(json_to_parse);

	if(Json::OBJECT == obj1.type())
	{
		if(true == obj1.has("this"))
		{
			string tmp = (obj1["this"]);
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
								if(thing_to_find == thing_str)
								{
									if((true == obj3.has("content"))
									&& (true == obj3.has("created")))
									{
										/* store creation date */
										temp_unit.setCreationDate(obj3["created"]);

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
												/* clear cmd and data arrays */
												temp_unit.clearCmds();
												temp_unit.clearDatas();

												/* store unit info */
												temp_unit.setIndex(i);
												temp_unit.setOnlineFoundFlag(true);
												/* ATTENTION: UID must be a string else a bad cast error will occur */
												temp_unit.setUID(obj4["uid"]);
												temp_unit.setCmdUID(obj4["cmd_uid"]);
												temp_unit.setDataUID(obj4["data_uid"]);
												temp_unit.setName(obj4["name"]);
												temp_unit.setStatus(obj4["status"]);
												/* store MQTT info */
												temp_unit.setMQTTHost(obj4["mqtt_host"]);
												temp_unit.setMQTTPort(obj4["mqtt_port"]);
												temp_unit.setMQTTRxTopic(obj4["mqtt_rxtopic"]);
												temp_unit.setMQTTTxTopic(obj4["mqtt_txtopic"]);

												obj5 = obj4["cmd"];
												if(Json::ARRAY == obj5.type())
												{
													for(j=0; j<obj5.size(); j++)
													{
														temp_unit.setCmdObj(obj5[j], " - ");
													}
												}
												else if(Json::STRING == obj5.type())
												{
													temp_unit.setCmdObj(obj5, " - ");
												}
												else
												{
													/* do nothing at the moment */
												}
		
												obj5 = obj4["data"];
												if(Json::ARRAY == obj5.type())
												{
													for(j=0; j<obj5.size(); j++)
													{
														temp_unit.setDataObj(obj5[j], " - ");
													}
												}
												else if(Json::STRING == obj5.type())
												{
													temp_unit.setDataObj(obj5, " - ");
												}
												else
												{
													/* do nothing at the moment */
												}

												/* push found unit into the units list */
												unit_list_ptr_ref->push_back(temp_unit);

												/* 1 found unit at least */
												ret_bool = true;
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
	}

	if(true == ret_bool)
	{
		cout << " - Found " << i << " valid unit(s) of type: " << thing_to_find << endl;
	}
	else
	{
		cout << " - NO valid units found :(" << endl;
	}

	return ret_bool;
}




/* End of file */








