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

/* file: main.cpp */

/* TODO: 
1 - when a data value is updated, the timestamp is valid only for it but other values previously updated could be out of date
2 - implement more than one unit 
*/


/* ---------------- inclusion files ------------- */

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

#include "unit.h"




/* --------------- local defines --------------- */

/* max number of cli commands */
#define MAX_NUM_OF_CLI_COMMANDS				8

/* Commands auto check period in ms */
#define CMD_AUTO_CHECK_PERIOD_MS				30000	/* 30 s */




using namespace std;




/* ----------------- classes definition ------------------ */

/* periodic callback class */
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


/* delayed call class. ATTENTION: not used at the moment */
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




/* ------------- local variables -------------- */

/* availale cli commands */
static const string cmd_list[MAX_NUM_OF_CLI_COMMANDS] =
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


/* Indicate if auto command check is enabled */
static bool auto_cmd_running = false;


/* Indicate when a command auto check request is ready to go
	ATTENTION: Start as false. A first call will be executed anyway when the callack is enabled */
static volatile bool auto_cmd_go = false;


/* Store periodic callback info */
static CallBackTimer auto_cmd_cb;	




/* -------------- local functions prototypes ---------------- */

static void	auto_cmd_cb_func			( void );
static int	convertStrNum				( string );
static int	getCliCmdIndex				( string );
static bool	getCliCmdAndExecuteIt	( void );




/* ---------------- local functions --------------- */

/* periodic callback to check unit commands online */
static void auto_cmd_cb_func(void)
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


static int convertStrNum(string num_str)
{
	int num;
	stringstream conv_stream;

	conv_stream << num_str;
	conv_stream >> num;
	conv_stream.str("");
	conv_stream.clear();

	return num;
}


/* get cli command index */
static int getCliCmdIndex(string command)
{
	int n = 0;
	while((n < MAX_NUM_OF_CLI_COMMANDS)
	&&		(command.compare(cmd_list[n])))
	{
		n++;
	}

	return n;
}


/* Function to get cli command input and execute it */
static bool getCliCmdAndExecuteIt( void )
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

	cmd_index = getCliCmdIndex(cmd_str);

	switch(cmd_index)
	{
		case 0:	//"pu"
		{
			if(false == UNIT_publishUnitOnline())
			{
				cout << "Online error!" << endl;
				error = 1;
			}
		}
		break;
		case 1:	//"si"
		{
			UNIT_showUnitInfo();
		}
		break;
		case 2:	//"sc"
		{
			UNIT_showLatestCmds();
		}
		break;
		case 3:	//"sd"
		{
			UNIT_showDataValues();
		}
		break;
		case 4:	//"cg"
		{
			/* get and manage new commands */
			UNIT_getAndManageNewCmds();
		}
		break;
		case 5:	//"ds"
		{
			int d;
			string v;
			d = convertStrNum(input.substr(3, 1));	/* ATTENTION: erroneous letter values are converted anyway */
			v = input.substr(5);	
			cout << "Inserted d: " << d << " - Inserted v: " << v << endl;
			if(false == UNIT_setAndPublishDataValue(d, v))
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
			/* invalid command: the getCmdIndex() function has returned MAX_NUM_OF_CLI_COMMANDS value */
			cerr << "VERTEX: error -> invalid command" << endl;
			error = 3;
		}
	}

	input.clear();
	cmd_str.clear();
	
	(0 == error) ? cout << "_____Command executed_____" << endl << endl << endl : cout << "_____Wrong command_____ ERROR: " << error << endl << endl << endl;

	return end;
}




/* --------------- main loop ----------- */
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

	while(end_prog != true)
	{
		/* if command auto check is enabled and ready to go */
		if(true == auto_cmd_running)
		{
			if(true == auto_cmd_go)
			{
				/* clear stored commands before checking new ones */
				/* ATTENTION: previous commands are cleared in the dweet module before getting the new ones */
				UNIT_clearCmds();

				/* get and manage new commands */
				UNIT_getAndManageNewCmds();
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




/* End of file */





