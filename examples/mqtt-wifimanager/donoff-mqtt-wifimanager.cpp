/* ********** Config supply *******************/
#define RELAY2 0
#define DS1820_INT 1
#define DS1820_OUT 1
#define DDISPLAY_SSD1306 0
//#define DTIME_ZONE 3

#define PINS_SET_V2

//#define SETDEFNEW

//when PIN_SET_V1 present. we cant use this display, because pin in this set conflicts with RELAY1


#ifdef PINS_SET_V1
  #undef DDISPLAY_SSD1306
  #define DDISPLAY_SSD1306 0
#endif


//Define here any specific supply 

// #define SUPPLY_PUMP
//#define SUPPLY_DS2

//#define SUPPLY_TEMP_ODDS

//default we compile SUPPLY_UNIVERSAL supply
#ifndef SUPPLY_PUMP 
 #ifndef SUPPLY_DS2
  #ifndef SUPPLY_TEMP_ODDS
      #define SUPPLY_UNIVERSAL
  #endif
 #endif
#endif

//#define DDISPLAY 0 //oled shield 
/*********************************************/

#include <ESP8266WiFi.h>
#include <TimeLib.h>

//#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>

#include <Ticker.h>
//#include <SimpleTimer.h>
#include <donoffsettings.h>
#include <publishers/donoffpublisher_mqtt.h>
#include <donoffcommands.h>
#include <donoffdisplay.h>
#include <donoffrelay.h>
// #include <supplies/donoffsupply-common.h>
// #include <supplies/donoffsupply-base.h>


#ifdef SUPPLY_UNIVERSAL
  #include <supplies/donoffsupply-donoff-universal.h>
#endif

#ifdef SUPPLY_PUMP
  #include <supplies/prj_supplies/pump_current_time_check_supply.h>
#endif

#ifdef SUPPLY_DS2
  #include <supplies/donoffsupply-ds1820-2.h>
#endif

#ifdef SUPPLY_TEMP_ODDS
  #include <supplies/prj_supplies/donoffsupply-temp_odds.h>
#endif

//

//#include <supplies/donoffsupply-donoff.h>

//#include <supplies/donoffsupply-sct013.h>
#include <PubSubClient.h>



WMSettings settings;

WifiCreds wcreds;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);

Queue<pub_events> que_wanted= Queue<pub_events>(MAX_QUEUE_WANTED);

DPublisherMQTT pubmqtt(&settings, &client);


// DSupply supply(&settings);

// DSupplyBase supply(&settings);


//


#ifdef SUPPLY_UNIVERSAL
    DSupplyDonoffUni supply(&settings);
#endif

#ifdef SUPPLY_PUMP
    DSupplyDonoffUniCurr supply(&settings);
#endif

#ifdef SUPPLY_DS2
    DSupplyDonoffDS2 supply(&settings);
#endif

#ifdef SUPPLY_TEMP_ODDS
    DSupplyDonoffTempOdds supply(&settings);
#endif



//DSupplyDonoff supply(&settings);

//DSupplySCT013Collector supply(&settings);


DNotifyerEmailMQTT notifyer(&settings, &client);

struct station_config stationConf;


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

  
  delay(2000);

  Serial.println ("SALT="+String(settings.salt));

  //if(settings.salt==1025) settings.salt=1205;

  if(settings.salt==1204){
    Serial.println("VERY OLD SALT detected(1204), implement new default options from settings");
    Serial.println("set new salt to 1205");
    settings.salt=1205;
    settings.cb_schm1=0B000000000011111111111100;
    settings.cb_schm2=0B111111111111100000000000;
  }

  else if (settings.salt==1205){
    Serial.println("OLD SALT detected (1205), implement new default options from settings");
    Serial.println("set new salt to 1206");
    settings.salt=1206;
    WMSettings defaults;
    for(int i=0; i<=23; i++) settings.temp_matrix[i]=defaults.temp_matrix[i];
  }

  else if (settings.salt != EEPROM_SALT) {
    Serial.println("Invalid settings in EEPROM, trying with defaults");
    WMSettings defaults;
    settings = defaults;
  }

  // #ifdef SETDEFNEW //newdef vualues
  //    settings.cb_schm1=0B000000000011111111111100;
  //    settings.cb_schm2=0B111111111111100000000000;
  // #endif

  pinMode(SONOFF_LED, OUTPUT);
  
  ticker.attach(0.25,tick);
  
  
  supply.set_blink(BL_CONNECTING);

  /* try to connect to wifi*/
  Serial.println("Connecting Wifi...");

  

  wifi_station_get_config (&stationConf);

  Serial.println(String("saved ssid:"+String((char*)stationConf.ssid)+", saved pass=" + String((char*) stationConf.password)));
  

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  WiFi.begin();

    int c = 0;
    while (WiFi.status() != WL_CONNECTED && c < 20) {
      delay(500);
      Serial.print(".");
      c++;
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
