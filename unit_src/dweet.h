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

/* file: dweet.h */

#ifndef _DWEET_H
#define _DWEET_H


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

#include "json11.h"




/* namespace needed here. TODO: I don't like here... think about it */
using namespace std;




/* -------------- exported macros ------------- */

/* shared JSON object */
#define JSON_OBJ						Json




/* -------------- exported typedefs ------------- */

/* command structure */
typedef struct 
{
	string sender_uid;
	string sender_name;
	string priority;
	string date;
	vector<string> cmd_keys;
	vector<string> cmd_strings;
} cmd_st;




/* -------------- exported functions prototypes ------------- */

extern bool DWEET_publishUnitOnline	( Json *, string );
extern bool DWEET_getLatestCommands	( string, vector<cmd_st> * );
extern bool DWEET_publishDataValue	( Json *, string );




#endif


/* End of file */


