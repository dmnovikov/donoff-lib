
#define BLYNK_HEARTBEAT 300
#define DCONFIG 0 // disable WiFiManager config

#define DS1820_OUT 0
//#define DDISPLAY 0 //oled shield 

#define CREDS_FILE

#ifdef CREDS_FILE
  #include "credentials_blynk.h"
#else
  char auth[] = "xxxxxx"; // blynk token
  char dev_id[]="l240_blynk";
  char ssid[] = "xxxxxx";       // wifi shield
  char pass[] = "xxxxxxxxx";    // wifi passwd
#endif


/*

   V12 время включенного состояния
   V11 врнутренняя температура
   V8  внешний датчик
   V6  виртуальный индикатор
   V18 - хронометр
   V19 - аптайм
   V20 - текущее время
   V21 - текущая дата
   V2  - кнопка переключения
   V5 - кнопка включить
   V6 - кнопка выключить
*/


#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include <Bounce2.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <Ticker.h>
//#include <SimpleTimer.h>
#include <donoffsettings.h>
#include <donoffpublisher_blynk.h>
#include <donoffcommands.h>
#include <donoffdisplay.h>
#include <donoffrelay.h>
#include <donoffsupply.h>
#include <PubSubClient.h>


static bool BLYNK_ENABLED = true;

WMSettings settings;

WidgetRTC rtc;

Queue<pub_events> que_wanted= Queue<pub_events>(MAX_QUEUE_WANTED);

DPublisherBlynk pubblynk(&settings);

DSupply supply(&settings);

DNotifyerEmailMQTTStandalone notifyer(&settings);

Ticker ticker;

//toggle - button
BLYNK_WRITE(2) {
  int a = param.asInt();
  Serial.println("Blynk Button pressed:" + String(a));
  //Serial.println(a);
  if (a == 1) {
    if (supply.r1_is_on()) {
      supply.r1_off();
    }
    else {
      supply.r1_on();
    }
  }
}

BLYNK_WRITE(30) {
  int a = param.asInt();
  if (a == 1) {
   pubblynk.publish_tlevels_to_info();
  }
}


BLYNK_WRITE(32) {
  int a = param.asInt();
  if (a == 1) {
    Serial.println("Blynk Button pressed:" + String(a));
    //clear_blynk_info_bar();
    pubblynk.clear_info_channel();
  }
}

BLYNK_WRITE(5) {
  int a = param.asInt();
  Serial.println("Press V5:" + String(a));
  if (a == 1) {
    supply.r1_on();
    
  }
}

BLYNK_WRITE(7) {
  int a = param.asInt();
  Serial.println("Press V7:" + String(a));
  if (a == 1) {
    supply.r1_off();
    
  }
}

BLYNK_WRITE(22) {
  String inS = param.asString();
  //Serial.println("TERMINAL:" + StrMsg);
  pubblynk.recognize_incoming_str(inS);
}

BLYNK_CONNECTED() {
  //debug("BLYNK", "rtc begin");
  rtc.begin();

  /*
  if (isFirstConnect) {
    // Request Blynk server to re-send latest values for all pins
    //Console.println(F("Sync"));
    Blynk.syncVirtual(V10);
    Blynk.syncVirtual(V15);
    Blynk.syncVirtual(V14);
    isFirstConnect = false;
  }
  */
}

void tick(){
  supply.tick();
}

// ********************************** SETUP ***************************************************

void setup()
{
  Serial.begin(9600);

  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.end();

#ifdef PINS_SET_V1
  Wire.begin(D7, D6);
#endif 

  strcpy(settings.dev_id, dev_id);
  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  supply.init(&notifyer, &pubblynk, &que_wanted);

  pinMode(SONOFF_LED, OUTPUT);
  
  ticker.attach(0.25,tick);
  
  
  supply.set_blink(BL_CONNECTING);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  
  Serial.println("Connecting Wifi:" + String(ssid));
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  pubblynk.init(&que_wanted);
  notifyer.init(&pubblynk);
 
}


/************************* LOOOOOOOOOOOOOOOOOOOOOOOOOOP *******************************************/
//int bl = 1;
int reconfigured = 0;
unsigned long mytimer = millis();
int  mycounter = 0;

void loop()
{

  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.run();
  }
  
  supply.supply_loop();

}
