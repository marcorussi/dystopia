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


#define PUB_FIRST_MSG							"Welcome..."

#define PUB_FIRST_MSG_LEN						strlen(PUB_FIRST_MSG)


static char mqttTopic_str[MQTT_FIELDS_STRING_MAX_LEN+1];	


mqtt_mng::mqtt_mng(const char *id, const char *host, int port, const char *mqttTopic) : mosquittopp(id)
{
	int keepalive = 60;

	memcpy(mqttTopic_str, mqttTopic, strlen(mqttTopic));


	/* Connect immediately. This could also be done by calling
	 * mqtt_tempconv->connect(). */
	connect(host, port, keepalive);
};

mqtt_mng::~mqtt_mng()
{
}

void mqtt_mng::on_connect(int rc)
{
	printf("Connected with code %d.\n", rc);
	if(rc == 0)
	{
		/* Only attempt to subscribe on a successful connect. */
		subscribe(NULL, mqttTopic_str);
	}
}

void mqtt_mng::on_message(const struct mosquitto_message *message)
{
	char buf[51];

	if(!strcmp(message->topic, mqttTopic_str)){
		memset(buf, 0, 51*sizeof(char));
		/* Copy N-1 bytes to ensure always 0 terminated. */
		memcpy(buf, message->payload, 50*sizeof(char));

		/* ATTENTION: send message on screen output at the moment */
		printf("MQTT MSG: %s.\n", buf);
	}
}

void mqtt_mng::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("Subscription succeeded.\n");

	/* Send first (welcome) message first */
	publish(NULL, mqttTopic_str, PUB_FIRST_MSG_LEN, PUB_FIRST_MSG);
}




/* End of file */





