/* ********** Config supply *******************/
#define DS1820_OUT 1
//#define DDISPLAY 0 //oled shield 
/*********************************************/

#include <ESP8266WiFi.h>
#include <TimeLib.h>

#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>

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
  
  supply.load();
  // EEPROM.begin(512);
  // EEPROM.get(0, settings);
  // EEPROM.end();

#ifdef PINS_SET_V1
  Wire.begin(13, 12); //d6 d7
#endif 
 
  delay(2000);

  Serial.println ("SALT="+String(settings.salt));

  if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  pinMode(SONOFF_LED, OUTPUT);
  
  ticker.attach(0.25,tick);
  
  
  supply.set_blink(BL_CONNECTING);

  /* try to connect to wifi*/
  Serial.println("Connecting Wifi...");

  if (WiFi.SSID()) {
    Serial.printf("We have saved SSID: %s\n", WiFi.SSID().c_str());
    //trying to fix connection in progress hanging
    WiFi.begin();
    int c = 0;
    while (WiFi.status() != WL_CONNECTED && c < 20) {
      delay(500);
      Serial.print(".");
      c++;
    }
  } else {
    Serial.println("No saved SSID, GO OFFLINE");
  }
  
 if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI_CONNECTED");
  }else{
    Serial.println("WIFI_NOT_CONNECTED");
  }

  client.setCallback(callback);
  pubmqtt.init(&que_wanted);
  notifyer.init(&pubmqtt);

  supply.init(&notifyer, &pubmqtt, &que_wanted);

}


/************************* LOOOOOOOOOOOOOOOOOOOOOOOOOOP *******************************************/

void loop()
{

if (client.connected()) client.loop();
supply.supply_loop();


}
