# dystopia
Leader and unit sides for managing connected devices over dweet.io JSON API. Work in progress...

A known and fixed *thing* field value (UID) is used by many units to publish their data over *dweet.io*. A request on this value will result in a list of all available units. Indeed, this value is used by a leader as a collector for obtaining info of all the units. 

A unit publish its info on *dweet.io* on a fixed UID (such as "unit7") with a specific JSON structure. Among this info fields there are two special values (*cmd_uid* and *data_uid*) that can be used for sending commands and obtaining data info with other two dedicated JSON structures. These two values are used as *thing* field in the request URL.
 
The leader side can discover for any available online units by a request on the known UID (of course both leader and units must share this value). The the leader side uses the specific cmd_uid and the data_uid of each unit to respectively send commands and read data. Available commands and data are listed in the info structure of each unit.

Example of request URL and related JSON structure:

### `https://dweet.io:443/get/dweets/for/unit1`

```
{
  "this": "succeeded",
  "by": "getting",
  "the": "dweets",
  "with": [
    {
      "thing": "unit1",
      "created": "2017-08-05T18:27:56.982Z",
      "content": {
        "uid": "2017SIMUNIT001A",
        "cmd_uid": "CMDSIMUNIT01",
        "data_uid": "DATASIMUNIT01",
        "name": "SIM_UNIT",
        "status": "ACTIVE",
        "mqtt_host": "test.mosquitto.org",
        "mqtt_port": 1883,
        "mqtt_rxtopic": "unit1/rx",
        "mqtt_txtopic": "unit1/tx",
        "cmd": [
          "valve",
          "led",
          "ctrl",
          "mqtt"
        ],
        "data": [
          "temp",
          "light",
          "switch",
          "batt"
        ]
      }
    },
    {
      "thing": "unit1",
      "created": "2017-08-05T18:25:42.556Z",
      "content": {
        "uid": "2017SIMUNIT002A",
        "cmd_uid": "CMDSIMUNIT02",
        "data_uid": "DATASIMUNIT02",
        "name": "SIM_UNIT",
        "status": "ACTIVE",
        "mqtt_host": "test.mosquitto.org",
        "mqtt_port": 1883,
        "mqtt_rxtopic": "unit2/rx",
        "mqtt_txtopic": "unit2/tx",
        "cmd": [
          "valve",
          "led",
          "ctrl",
          "mqtt"
        ],
        "data": [
          "temp",
          "light",
          "switch",
          "batt"
        ]
      }
    }
  ]
}
```

### `https://dweet.io:443/get/latest/dweet/for/CMDSIMUNIT01`

```
{
  "this": "succeeded",
  "by": "getting",
  "the": "dweets",
  "with": [
    {
      "thing": "CMDSIMUNIT01",
      "created": "2017-08-05T17:14:34.933Z",
      "content": {
        "sender_name": "DYSTOPIA",
        "cmd": {
          "mqtt": "on"
        },
        "sender_uid": "2017TEST001A",
        "priority": "medium"
      }
    }
  ]
}
```


**Dependencies**
curlpp and mosquitto client libraries needed.

**TODO**
Check source files for related TODO lists.

**Known Issues**
Leader: In case of same unit published several time the leader will discover it as several different units.

Unit: After MQTT cleanup call the SSL library is freed/cleaned so any consecutive HTTPS request will fail with a crash (segmentation fault). This is evident with auto-check command function enabled and MQTT is turned on and the off (by a leader).

**WORK IN PROGRESS...**


