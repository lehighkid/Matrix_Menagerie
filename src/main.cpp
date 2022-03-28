#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <PxMatrix.h>
#include <TetrisMatrixDraw.h>
#include <ezTime.h>
#include "config.h"
#include "main.h"

// Define wifi client for use in PubSubClient for MQTT
WiFiClient espClient;

// Define the MQTT object
PubSubClient client(mqtt_server, mqtt_port, espClient);

// Initialize the matrix - 64x32 using pin mappings in config.h
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// Initialize 3 displays for various clock parts
TetrisMatrixDraw tetris(display); // Main clock
TetrisMatrixDraw tetris2(display); // The "M" of AM/PM
TetrisMatrixDraw tetris3(display); // The "P" or "A" of AM/PM

// Set timezone to value set in config.h
Timezone myTZ;

bool setMatrixTime() {
  static String lastDisplayedTime;
  static String lastDisplayedAmPm;

  tetris.scale = 2;

  bool timeChanged = false;

  String timeString;
  String AmPmString;
  if (twelveHourFormat) {
    // Get the time in format "1:15" or 11:15 (12 hour, no leading 0)
    // Check the EZTime Github page for info on
    // time formatting
    timeString = myTZ.dateTime("g:i");

    //If the length is only 4, pad it with
    // a space at the beginning
    if (timeString.length() == 4) {
      timeString = " " + timeString;
    }

    //Get if its "AM" or "PM"
    AmPmString = myTZ.dateTime("A");
    if (lastDisplayedAmPm != AmPmString) {
      DPRINTLN(AmPmString);
      lastDisplayedAmPm = AmPmString;
      // Second character is always "M"
      // so need to parse it out
      tetris2.setText("M", forceRefresh);

      // Parse out first letter of String
      tetris3.setText(AmPmString.substring(0, 1), forceRefresh);
    }
  } else {
    // Get time in format "01:15" or "22:15"(24 hour with leading 0)
    timeString = myTZ.dateTime("H:i");
  }

  // Only update Time if its different
  if (lastDisplayedTime != timeString) {
    DPRINTLN(timeString);
    lastDisplayedTime = timeString;
    tetris.setTime(timeString, forceRefresh);
    timeChanged = true;
  }

  return timeChanged;
}

// declare all available animations
bool animateTwelveHour(bool showColon);
bool animateTwentyFourHour(bool showColon);

// set function pointer to the active animation
auto activeAnimation = twelveHourFormat ? animateTwelveHour : animateTwentyFourHour;


bool animateTwelveHour(bool showColon)
{
  setMatrixTime();

  // Place holders for checking are any of the tetris objects
  // currently still animating.
  bool tetris1Done = false;
  bool tetris2Done = false;
  bool tetris3Done = false;

  tetris1Done = tetris.drawNumbers(-6, 26, showColon);
  tetris2Done = tetris2.drawText(56, 25);

  // Only draw the top letter once the bottom letter is finished.
  if (tetris2Done) {
    tetris3Done = tetris3.drawText(56, 15);
  }

  return tetris1Done && tetris2Done && tetris3Done;
}

bool animateTwentyFourHour(bool showColon)
{
  setMatrixTime();

  return tetris.drawNumbers(2, 26, showColon);
}

static bool showColon()
{
  static unsigned int colonCounter = 0;
  colonCounter++;
  unsigned int colonFraction = colonCounter / 5;
  return !!(colonFraction % 2);
}

void setup() {
  Serial.begin(115200);
  // Wait for Serial Port to Initialize
  while (!Serial) {}

  DPRINTLN("\nControllerId: ");
  DPRINT(controller_id);

  // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // Define WiFiManager
  WiFiManager wifiManager;

  //reset saved settings
  // wifiManager.resetSettings();
  // Connect to wifi
  wifiManager.autoConnect(wifi_ap_name);

  // Intialise display library
  display.begin(16);
  display.clearDisplay();
  display.display(70);

  // Setup EZ Time
  setDebug(INFO);
  waitForSync();

  DPRINTLN();
  DPRINTLN("UTC:             " + UTC.dateTime());

  myTZ.setLocation(F(MYTIMEZONE));
  DPRINTLN(F("Time in your set timezone:         "));
  DPRINTLN(myTZ.dateTime());

}

void loop() {
  static bool animationDone = false;
  static bool colonVisible = true;

  unsigned long now = millis();

  if(0 == now % (unsigned long)100)
  {
    bool colonChanged = false;
    if(colonVisible != showColon())
    {
      colonVisible = !colonVisible;
      colonChanged = true;
    }

    if(!animationDone || colonChanged)
    {
      display.clearDisplay();
      animationDone = activeAnimation(colonVisible);
    }
  }

  if(0 == now % (unsigned long)2)
  {
    display.display(70);
  }
}
