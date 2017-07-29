#	The MIT License (MIT)
#
#	Copyright (c) 2017 Marco Russi
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy
#	of this software and associated documentation files (the "Software"), to deal
#	in the Software without restriction, including without limitation the rights
#	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#	copies of the Software, and to permit persons to whom the Software is
#	furnished to do so, subject to the following conditions:
#
#	The above copyright notice and this permission notice shall be included in all
#	copies or substantial portions of the Software.
#
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#	SOFTWARE.


LINK_TARGET = unit.exe

#CURLPP_PATH := /home/marco/Documents/projects/http_curlpp/curlpp-0.8.1/

LIB_PATH := /usr/local/lib

OBJS = json11.o dweet.o unit.o main.o 

CXX_FLAGS = -std=c++11
#-fno-rtti -fno-exceptions

LD_FLAGS = -lcurlpp -lcurl

#CANARY_ARGS = -DJSON11_ENABLE_DR1467_CANARY=0

REBUILDABLES = $(OBJS) $(LINK_TARGET)

#INC_PATHS = -I$(abspath /home/marco/Documents/projects/http_curlpp/json11-master)

LIB_PATHS = -L$(abspath $(LIB_PATH))

all : $(LINK_TARGET)
	echo done

clean :
	rm -f $(REBUILDABLES)
	echo clean

$(LINK_TARGET) : $(OBJS)
	g++ -o $@ $^ $(LIB_PATHS) $(LD_FLAGS) 

%.o : %.cpp
#	g++ -c -o $@ $< $(CANARY_ARGS) $(CXX_FLAGS) $(INC_PATHS)
	g++ -c -o $@ $< $(CXX_FLAGS)

json11.o :
#	g++ -c -o json11.o ../json11-master/json11.cpp -std=c++11
	g++ -c -o json11.o json11.cpp -std=c++11




# end of file


