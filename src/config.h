#define DEBUG
#include "debug.h"
#include <WString.h>

// wiring pin mapping
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
#define P_D 12
#define P_E 0

// Set a timezone using the following list
// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
#define MYTIMEZONE "America/New_York"

// Sets whether the clock should be 12 hour format or not.
bool twelveHourFormat = false;

// If this is set to false, the number will only change if the value behind it changes
// e.g. the digit representing the least significant minute will be replaced every minute,
// but the most significant number will only be replaced every 10 minutes.
// When true, all digits will be replaced every minute.
bool forceRefresh = true;

// MQTT connection details
int                   mqtt_port                     = 1883;
const char*           mqtt_server                   = "10.0.1.11";
String                mqtt_client_id                = "ESP8266Matrix-" + String(random(0xffff), HEX);
const uint32_t        controller_id                 = ESP.getChipId();
const long            mqtt_recon_time               = 5000;
const long            mqtt_resub_time               = 1200000;
const long            mqtt_loop_time                = 100;
const char*           mqtt_username                 = "matrix1";
const char*           mqtt_password                 = "matrix1";
const char*           mqtt_endpoint                 = "matrixs";
char                  mqtt_endpoint_register[50];
char                  mqtt_endpoint_subscribe[50];
char                  mqtt_endpoint_message[100];

// Wifi credentials
const char*           wifi_ap_name                  = mqtt_client_id.c_str();
const char*           wifi_ssid                     = "";
const char*           wifi_password                 = "";
const long            wifi_recon_time               = 500;
