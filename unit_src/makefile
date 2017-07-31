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


BIN_PATH = bin

OBJ_PATH = obj

#CURLPP_INC_PATH := /usr/local/include/curlpp
#MOSQUITTO_INC_PATH := /usr/local/include

LIB_PATH := /usr/local/lib

LIB_PATHS = -L$(abspath $(LIB_PATH))

BIN = $(BIN_PATH)/unit_demo.exe

OBJS = $(patsubst %.o, $(OBJ_PATH)/%.o ,json11.o unit_class.o dweet.o mqtt_mng.o unit.o main.o)

CXX_FLAGS = -std=c++11
#-fno-rtti -fno-exceptions

LD_FLAGS = -lcurlpp -lcurl -lmosquittopp -lmosquitto

REBUILDABLES = $(OBJS) $(BIN)


all : $(BIN)
	echo done

clean :
	rm -f $(REBUILDABLES)
	echo clean

$(BIN) : $(OBJS)
	g++ -o $@ $(OBJS) $(LIB_PATHS) $(LD_FLAGS) 

obj/%.o : %.cpp
	g++ -c -o $@ $(CXX_FLAGS) $< 




# end of file


