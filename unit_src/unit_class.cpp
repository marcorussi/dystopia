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

/* file: unit_class.cpp */


/* -------------- inclusion files ------------ */

#include "unit_class.h"

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







Unit::Unit()
{
	int j;
	unit_index = 0;
	unit_name = "no_name";
	uid_str = "";
	cmd_uid_str = "";
	data_uid_str = "";
	status_str = "";
	creation_date = "";
	data_update = "";
	online_found = false;
	available_cmds.clear();
	cmd_key.clear();
	cmd_value.clear();
	data_key.clear();
	data_value.clear();
} ;


Unit::~Unit()
{
	/* do nothing at the moment */
} ;















