#ifndef _MQTT_MNG_H
#define _MQTT_MNG_H

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

#include <mosquittopp.h>


#define MQTT_FIELDS_STRING_MAX_LEN			30		/* ATTENTION: maximum of 30 chars */


class mqtt_mng : public mosqpp::mosquittopp
{
	public:
		mqtt_mng(const char *id, const char *host, int port, const char *mqttTopic);
		~mqtt_mng();

		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
};

#endif
