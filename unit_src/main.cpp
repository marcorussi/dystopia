/**
*
* .
*
*/


/* TODO: 
1 - when a data value is updated, the timestamp is valid only for it but other values previously updated could be out of date
2 - implement more than one unit 
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

#include <functional>
#include <chrono>
#include <future>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

#include "json11.h"

//#define MAX_NUM_OF_AVAILABLE_UNITS		5
#define MAX_NUM_OF_COMMANDS				8

/* Commands auto check period in ms */
#define CMD_AUTO_CHECK_PERIOD_MS			30000

#define PUBLISH_DWEET_URL_STRING			"https://dweet.io:443/dweet/for/"
#define GET_CMD_DWEET_URL_STRING			"https://dweet.io:443/get/dweets/for/"

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


class CallBackTimer
{
public:
    CallBackTimer()
    :_execute(false)
    {}

    ~CallBackTimer() {
        if( _execute.load(std::memory_order_acquire) ) {
            stop();
        };
    }

    void stop()
    {
        _execute.store(false, std::memory_order_release);
        if( _thd.joinable() )
            _thd.join();
    }

    void start(int interval, std::function<void(void)> func)
    {
        if( _execute.load(std::memory_order_acquire) ) {
            stop();
        };
        _execute.store(true, std::memory_order_release);
        _thd = std::thread([this, interval, func]()
        {
            while (_execute.load(std::memory_order_acquire)) {
                func();                   
                std::this_thread::sleep_for(
                std::chrono::milliseconds(interval));
            }
        });
    }

    bool is_running() const noexcept {
        return ( _execute.load(std::memory_order_acquire) && 
                 _thd.joinable() );
    }

private:
    std::atomic<bool> _execute;
    std::thread _thd;
};


class later
{
	public:
		template <class callable, class... arguments>
		later(int after, bool async, callable&& f, arguments&&... args)
		{
			std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

			if (async)
			{
				std::thread([after, task]() {
					std::this_thread::sleep_for(std::chrono::milliseconds(after));
					task();
				}).detach();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(after));
			task();
		}
	}
};


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


typedef struct 
{
	string sender_uid;
	string sender_name;
	string priority;
	string date;
	vector<string> cmd_keys;
	vector<string> cmd_strings;
} cmd_st;


vector<string> unit_cmd_list =
{
	"valve",
	"led",
	"ctrl"
};


vector<string> unit_data_list =
{
	"temp",
	"light",
	"switch",
	"batt"
};


string cmd_list[MAX_NUM_OF_COMMANDS] =
{
	"pu",
	"si",
	"sc",
	"sd",
	"cg",
	"ds",
	"ac",
	"en"
};


/* ATTENTION: latest commands stored for one unit only at the moment */
vector<cmd_st> latest_cmds;


/* store newest command date */
string newest_cmd_date = "";


/* Indicate if auto command check is enabled */
bool auto_cmd_running = false;


/* Indicate when a command auto check request is ready to go
	ATTENTION: Start as false. A first call will be executed anyway when the callack is enabled */
volatile bool auto_cmd_go = false;


/* Store periodic callback info */
CallBackTimer auto_cmd_cb;	


/* ATTENTION: only one unit used at the moment */
//UnitSim units_sim[MAX_NUM_OF_AVAILABLE_UNITS];
/* ATTENTION: this istance must be after cmd and data vectors instances */
UnitSim units_sim;


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


/* Local functions prototypes */
bool 			getCliCmdAndExecuteIt			( void );
void 			executeCommand						( cmd_st * );
int			getCmdIndex							( string );
int 			convertStrNum						( string );
bool 			parseJsonCheckDweetSucceded	( string );
bool 			publishUnitOnline					( void );
void 			showUnitInfo						( void );
void 			getAndManageNewCommands			( void );
bool			getLatestCommands					( void );
bool 			parseJsonGetCmds					( string, string, string );
void 			showLatestCmds						( void );
void 			showDataValues						( void );
bool			setAndPublishDataValue			( int, string );
void 			auto_cmd_cb_func					( void );




void auto_cmd_cb_func(void)
{
	cout << "__AUTO CMD CHECK FUNCTION__" << endl;

	/* if no previous requests are pending */
	if(false == auto_cmd_go)
	{
		/* set flag; command auto check is ready to go */
		auto_cmd_go = true;
	}
	else
	{
		/* do nothing */
	}
}


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


void executeCommand(cmd_st *command)
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
	bool end_prog = false;
	
	//later later_test1(4000, true, &test1);

	while(end_prog != true)
	{
		/* if command auto check is enabled and ready to go */
		if(true == auto_cmd_running)
		{
			if(true == auto_cmd_go)
			{
				/* get and manage new commands */
				getAndManageNewCommands();
				/* clear flag */
				auto_cmd_go = false;
			}
			else
			{
				/* wait... */
			}

			/* nothing else is done at the moment
				but a timeout mechanism can be implemented */
		}
		else
		{
			/* wait until cli command input is inserted and execute it */
			end_prog = getCliCmdAndExecuteIt();
		}
	}

	return EXIT_SUCCESS;
}


/* Function to get cli command input and execute it */
bool getCliCmdAndExecuteIt( void )
{
	int error = 0;
	string input, cmd_str;
	int cmd_index;
	volatile bool end = false;
		
	cout << "Available commands:" << endl 
	<< "pu: publish unit online" << endl 
	<< "si: show unit info" << endl 
	<< "sc: show latest received commands" << endl 
	<< "sd: show unit data" << endl 
	<< "cg: get latest commands" << endl
	<< "ds [d] [v]: set data value [d=data index] [v=data value]" << endl
	<< "ac [b]: enable (on) or disable (off) commands auto check" << endl
	<< "en: end of program" << endl;

	cout << "Insert command:";
	getline(cin, input);
	cmd_str = input.substr(0, 2);

	cout << "__full command:" << input << endl;
	cout << "__command:" << cmd_str << endl;

	cmd_index = getCmdIndex(cmd_str);

	switch(cmd_index)
	{
		case 0:	//"pu"
		{
			if(false == publishUnitOnline())
			{
				cout << "Online error!" << endl;
				error = 1;
			}
		}
		break;
		case 1:	//"si"
		{
			showUnitInfo();
		}
		break;
		case 2:	//"sc"
		{
			showLatestCmds();
		}
		break;
		case 3:	//"sd"
		{
			showDataValues();
		}
		break;
		case 4:	//"cg"
		{
			/* get and manage new commands */
			getAndManageNewCommands();
		}
		break;
		case 5:	//"ds"
		{
			int d;
			string v;
			d = convertStrNum(input.substr(3, 1));	/* ATTENTION: erroneous letter values are converted anyway */
			v = input.substr(5);	
			cout << "Inserted d: " << d << " - Inserted v: " << v << endl;
			if(false == setAndPublishDataValue(d, v))
			{
				cout << "Parameters error!" << endl;
				error = 1;
			}
		}
		break;
		case 6:	//"ac"
		{
			string p;
			/* ATTENTION: in case of missing parameter the program fails */
			p = input.substr(3);	
			if(p == "on")
			{
				/* check if already running */
				if(false == auto_cmd_running)
				{
					cout << "--Start Auto command check" << endl;
					auto_cmd_cb.start(CMD_AUTO_CHECK_PERIOD_MS, &auto_cmd_cb_func);
					auto_cmd_running = true;
				}
				else
				{
					/* do nothing */
				}
			}
			else if(p == "off")
			{
				cout << "--Stop Auto command check" << endl;
				auto_cmd_cb.stop();
				auto_cmd_running = false;
			}
			else
			{
				cout << "Parameter error!" << endl;
				error = 1;
			}					
		}
		break;
		case 7:	//"en"
		{
			end = true;
		}
		break;
		default:
		{
			/* invalid command: the getCmdIndex() function has returned MAX_NUM_OF_COMMANDS value */
			cerr << "VERTEX: error -> invalid command" << endl;
			error = 3;
		}
	}

	input.clear();
	cmd_str.clear();
	
	(0 == error) ? cout << "_____Command executed_____" << endl << endl << endl : cout << "_____Wrong command_____ ERROR: " << error << endl << endl << endl;

	return end;
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


/* ATTENTION: only one unit is used at the moment */
void showUnitInfo( void )
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


/* ATTENTION: only one unit used at the moment */
bool publishUnitOnline( void )
{
	int i;
	Json obj;
	Json cmd_obj{};	/* Json array */
	Json data_obj{};	/* Json array */
	string str, jsontext;
	string url = PUBLISH_DWEET_URL_STRING;
	stringstream result;
	
	/* complete the url with the unit UID */
	url.append(UNIT_THING_DWEET_CODE);

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

	/* transform the Json object into a text string */
	jsontext = obj.stringify();

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


/* function to get and manage new commands */
void getAndManageNewCommands( void )
{
	/* get new commands */
	if(true == getLatestCommands())
	{
		/* ATTENTION: execution means show commands
			and it is done immediately */
		for(int i=0; i<latest_cmds.size(); i++)
		{
			executeCommand(&latest_cmds[i]);
		}

		/* clear stored commands after execution */
		latest_cmds.clear();
	}
	else
	{
		cout << "No new commands" << endl;
	}
}


bool getLatestCommands( void )
{
	string url = GET_CMD_DWEET_URL_STRING;
	stringstream result;
	int json_parse_code;
	vector<int> cmds_list;

 	url.append(units_sim.getCmdUID());

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
	if(false == parseJsonGetCmds(result.str(), units_sim.getCmdUID(), newest_cmd_date))
		return false;

	/* update last command update by storing the first retrieved command (the newest one) */
	newest_cmd_date = latest_cmds[0].date;

	return true;
}


bool parseJsonGetCmds( string json_to_parse, string thing_to_find, string last_cmd_date )
{
	string tmp;
	int i, j;
	bool ret_bool = false;
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
										/* check creation date */
										if(date_str > last_cmd_date)
										{
											cmd_st cmdst;
											cmdst.date = date_str;
											obj4 = obj3["content"];
											if(Json::OBJECT == obj4.type())
											{
												if((true == obj4.has("sender_name"))
												&& (true == obj4.has("sender_uid"))
												&& (true == obj4.has("priority"))
												&& (true == obj4.has("cmd")))
												{
													/* ATTENTION: UID must be a string else a bad cast error will occur */
													string str = obj4["sender_uid"];
													cmdst.sender_uid = str;
													cout << "_____ - sender_uid: " << str << endl;
													str.clear();

													str.append(obj4["sender_name"]);
													cmdst.sender_name = str;
													cout << "_____ - sender_name: " << str << endl;
													str.clear();

													str.append(obj4["priority"]);
													cmdst.priority = str;
													cout << "_____ - priority: " << str << endl;
													str.clear();

													cout << "_____ - cmd: " << endl;
													obj5 = obj4["cmd"];
													if(Json::OBJECT == obj5.type())	/* ATTENTION: only one command is used at the moment */
													{
														cmdst.cmd_keys = obj5.keys();
														for(j=0; j<cmdst.cmd_keys.size(); j++)
														{
															cmdst.cmd_strings.push_back(obj5[cmdst.cmd_keys[j]]);
															str.append(cmdst.cmd_keys[j]);
															str.append(" -> ");
															str.append(obj5[cmdst.cmd_keys[j]]);
															cout << "___________ -" << str << endl;
															str.clear();
														}
													}
													else
													{
														/* do nothing at the moment */
													}

													/* store sender commands */
													latest_cmds.push_back(cmdst);

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


/* ATTENTION: only one unit is used at the moment */
void showLatestCmds( void )
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


/* ATTENTION: only one unit is used at the moment */
void showDataValues( void )
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


/* ATTENTION: only one unit is used at the moment */
bool setAndPublishDataValue( int data_index, string data_value_str )
{
	int i;
	Json obj;
	Json data_obj;	
	string str, jsontext;
	string url = PUBLISH_DWEET_URL_STRING;
	stringstream result;

	if(data_index >= units_sim.getNumOfDatas())
	{
		return false;
	}
	
	/* complete the url with the data UID */
	url.append(units_sim.getDataUID());

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
		//data_obj.set(units_sim.getData(i), units_sim.getDataValue(i));
		str = units_sim.getDataValue(i);
		data_obj.set(units_sim.getData(i), str);
	}
	obj.set("data", data_obj);

	/* transform the Json object into a text string */
	jsontext = obj.stringify();

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













