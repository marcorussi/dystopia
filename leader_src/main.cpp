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
1 - fix crash in case of missing cli command parameter (ex. su)
2 - consider to add a video comment saying that shown data are the received ones while shown cmds are the inserted ones
*/




/* -------------- inclusion file ------------- */

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

#include "unit_mng.h"




/* -------------- local constants ------------- */

#define MAX_NUM_OF_CLI_COMMANDS						6




/* local namespace */
using namespace std;




/* -------------- local variables ------------- */

/* list of cli commands */
static const string cmd_list[MAX_NUM_OF_CLI_COMMANDS] =
{
	"du",
	"su",
	"sa",
	"cs",
	"dg",
	"en"
};




/* --------------- Local functions prototypes ----------------- */

static int	getCmdIndex		( string );
static int 	convertStrNum	( string );




/* --------------- Local functions ----------------- */

/* get cli command index */
static int getCmdIndex(string command)
{
	int n = 0;
	while((n < MAX_NUM_OF_CLI_COMMANDS)
	&&		(command.compare(cmd_list[n])))
	{
		n++;
	}

	return n;
}


/* convert string to number */
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




/* -------------- main lopp --------------- */

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
		<< "dg [u]: get unit data [u=unit index]" << endl;

		cout << "Insert command:";
		getline(cin, input);
		cmd_str = input.substr(0, 2);

		/* wait for cli commands */
		cmd_index = getCmdIndex(cmd_str);

		/* execute cli command */
		switch(cmd_index)
		{
			case 0: //"du"
			{
				if(false == UNIT_discOnlineUnits())
				{
					cout << "Fail to find online units!" << endl;
					error = 2;
				}
				else
				{
					/* show all units after discovery */
					UNIT_showAllUnitsInfo();
				}
				break;
			}
			
			case 1: //"su"
			{
				int u;
				u = convertStrNum(input.substr(3, 1));
				cout << "Inserted u: " << u << endl;
				if(false == UNIT_showUnitInfo(u))
				{
					cout << "Parameter error!" << endl;
					error = 1;
				}
				break;
			}
			
			case 2: //"sa"
			{
				/* show all units */
				UNIT_showAllUnitsInfo();
				
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
				if(false == UNIT_setUnitCmd(u, c, v))
				{
					cout << "Parameters error!" << endl;
					error = 1;
				}
				break;
			}
			
			case 4: //"dg"
			{
				int i, u;
				u = convertStrNum(input.substr(3, 1));
				cout << "Inserted u: " << u << endl;
				if(false == UNIT_getUnitData(u))
				{
					cout << "Parameters error!" << endl;
					error = 1;
				}
				else
				{
					/* show unit info (including new received data) */
					UNIT_showUnitInfo(u);
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
				cerr << "Invalid command" << endl;
				error = 3;
			}
		}

		input.clear();
		cmd_str.clear();
		
		(0 == error) ? cout << "DONE " << endl << endl << endl : cout << "ERR: " << error << endl << endl << endl;
	}

	return EXIT_SUCCESS;
}




/* End of file */







