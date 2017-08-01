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

/* file: unit.h */

#ifndef _UNIT_H
#define _UNIT_H


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

#include "unit_class.h"




/* namespace needed here. TODO: I don't like here... think about it */
using namespace std;




/* -------------- exported functions prototypes ------------- */

extern void UNIT_showAllUnitsInfo	( void );
extern bool UNIT_showUnitInfo			( int );
extern bool UNIT_discOnlineUnits		( void );
extern bool UNIT_setUnitCmd			( int, int, string);
extern bool UNIT_getUnitData			( int );
extern bool UNIT_bIsMQTTRunning		( void );
extern void UNIT_manageMQTTLoop		( void );
extern bool UNIT_startMQTT				( int );
extern void UNIT_stopMQTT				( void );
extern void UNIT_sendMQTTMessage		( const char *);




#endif


/* End of file */


