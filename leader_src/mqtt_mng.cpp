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


/* Inclusion files */
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

#include "mqtt_mng.h"
#include <mosquittopp.h>


/* local namespace */
using namespace std;




/* Welcome message defines */
#define WELCOME_MSG						"Hello, welcome..."
#define WELCOME_MSG_LEN					strlen(WELCOME_MSG)




/* RX and TX topics strings */
static char mqttRxTopic_str[MQTT_FIELDS_STRING_MAX_LEN+1] = { 0 };
static char mqttTxTopic_str[MQTT_FIELDS_STRING_MAX_LEN+1] = { 0 };	




/* class initialiser */
mqtt_mng::mqtt_mng(const char *id, const char *host, int port, const char *mqttRxTopic, const char *mqttTxTopic) : mosquittopp(id)
{
	int keepalive = 60;

	/* copy RX and TX topics */
	memcpy(mqttRxTopic_str, mqttRxTopic, strlen(mqttRxTopic));
	memcpy(mqttTxTopic_str, mqttTxTopic, strlen(mqttTxTopic));

	cout << "ID: " << id << endl;
	cout << "HOST: " << host << endl;
	cout << "PORT: " << port << endl;
	cout << "Rx topic: " << mqttRxTopic << endl;
	cout << "Tx topic: " << mqttTxTopic << endl;

	/* Connect immediately. This could also be done by calling
	 * mqtt_mng->connect(). */
	connect(host, port, keepalive);
};


/* class de-initialiser */
mqtt_mng::~mqtt_mng()
{
}


/* on connection event */
void mqtt_mng::on_connect(int rc)
{
	cout << "Connected with code: " << rc << endl;
	if(rc == 0)
	{
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, mqttTxTopic_str);
	}
}


/* on message event */
void mqtt_mng::on_message(const struct mosquitto_message *message)
{
	char buf[51];

	/* if received topic is the TX unit one */
	if(!strcmp(message->topic, mqttTxTopic_str))
	{
		/* clear buffer */
		memset(buf, 0, 51*sizeof(char));
		/* Copy N-1 bytes to ensure always 0 terminated. */
		memcpy(buf, message->payload, 50*sizeof(char));

		/* ATTENTION: send message on screen output at the moment */
		cout << "---> " << buf << endl;
	}
	else
	{
		/* do nothing */
	}
}


/* on subscribe event */
void mqtt_mng::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	cout << "Subscription succeeded to: " << mqttTxTopic_str << endl;

	//publish(NULL, mqttRxTopic_str, WELCOME_MSG_LEN, WELCOME_MSG);
}


/* function to send a message to RX unit topic */
void mqtt_mng::send_msg(const char *message)
{
	/* Send message to unit RX topic */
	publish(NULL, mqttRxTopic_str, strlen(message), message);
}




/* End of file */





