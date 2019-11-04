

#define DS1820_OUT 0
//#define DDISPLAY 0 //oled shield 

//#define CREDS_FILE

#ifdef CREDS_FILE
  #include "credentials_mqtt.h"
#else
  char mqtt_user[] = "lab240"; // blynk token
  char mqtt_server[]="mqtt.lab240.ru";
  char dev_id[]="mqt1";
  char ssid[] = "dmn3";       // wifi shield
  char pass[] = "6788766788";    // wifi passwd
  char mqtt_pass[] = "240240";    // mqtt passwd
#endif

#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <Bounce2.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <Ticker.h>
//#include <SimpleTimer.h>
#include <donoffsettings.h>
#include <donoffpublisher_mqtt.h>
#include <donoffcommands.h>
#include <donoffdisplay.h>
#include <donoffrelay.h>
#include <donoffsupply.h>
#include <PubSubClient.h>



WMSettings settings;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);

Queue<pub_events> que_wanted= Queue<pub_events>(MAX_QUEUE_WANTED);

DPublisherMQTT pubmqtt(&settings, &client);


DSupply supply(&settings);


DNotifyerEmailMQTT notifyer(&settings, &client);


Ticker ticker;

void tick(){
  supply.tick();
}

// ********************************** SETUP ***************************************************

void callback(char* topic, byte* payload, unsigned int length){
  // Serial.println("nature callback");
  pubmqtt.callback(topic,payload,length);
}

void setup()
{
  Serial.begin(9600);

  EEPROM.begin(512);
  EEPROM.get(0, settings);
  EEPROM.end();

#ifdef PINS_SET_V1
  Wire.begin(D7, D6);
#endif 
 
  delay(2000);

  Serial.println ("SALT="+String(settings.salt));

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  strcpy(settings.dev_id, dev_id);
  strcpy(settings.mqttUser, mqtt_user);
  strcpy(settings.mqttPass, mqtt_pass);
  strcpy(settings.mqttServer, mqtt_server);


  
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
   if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI_CONNECTED");
  }else{
    Serial.println("WIFI_NOT_CONNECTED");
  }


  client.setCallback(callback);
  
  Serial.println("mqtt init");
  pubmqtt.init(&que_wanted);
  Serial.println("notufyer init");
  notifyer.init(&pubmqtt);

  Serial.println("supply init");
  supply.init(&notifyer, &pubmqtt, &que_wanted);

  
}


/************************* LOOOOOOOOOOOOOOOOOOOOOOOOOOP *******************************************/
//int bl = 1;
int reconfigured = 0;
unsigned long mytimer = millis();
int  mycounter = 0;

void loop()
{

if (client.connected()) client.loop();

/*
  if (WiFi.status() == WL_CONNECTED && Blynk.connected()) {
    Blynk.run();
  }

*/

  supply.supply_loop();


}
