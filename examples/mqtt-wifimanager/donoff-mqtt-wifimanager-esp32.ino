/* ********** Config supply *******************/

#define BUTTON_REVERSE

//UNIVERSAL SUPPLY parameters 
#define RELAY2 1
#define DS1820_INT 1
#define DS1820_OUT 0
#define DDISPLAY_SSD1306 0

//#define DTIME_ZONE 3

#define PINS_SET_V1
//#define PINS_SET_V2
//#define PINS_SET_V3



//#define SETDEFNEW 

//when PIN_SET_V1 present. we cant use this display, because pin in this set conflicts with RELAY1

#ifdef PINS_SET_V2
  #undef DDISPLAY_SSD1306
  #define DDISPLAY_SSD1306 0
#endif

/**************** Define supply **************************/

//#define SUPPLY_PUMP
//#define SUPPLY_DS2
//#define SCT013 1 //for SUPPLY_UNI_SCT013 
//#define SUPPLY_TEMP_ODDS
//#define SUPPLY_UNI_SCT013 
//#define SUPPLY_SCT013_SINGLE
//#define SUPPLY_MULTI_SCT013
//#define SUPPLY_DHT
//#define SUPPLY_6TEMP
//#define SUPPLY_UNI_MAX44003
//#define SUPPLY_ADS11x5


#define SUPPLY_UNIVERSAL

/************************************************************/

//#define DDISPLAY 0 //oled shield 
/*********************************************/

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

#ifdef SUPPLY_UNI_SCT013
  #include <supplies/donoffsupply-uni-sct013.h>
#endif

#ifdef SUPPLY_SCT013_SINGLE
  #include <supplies/donoffsupply-sct013.h>
#endif

#ifdef SUPPLY_MULTI_SCT013
  #include <supplies/donoffsupply-multi-sct013.h>
#endif

#ifdef SUPPLY_DHT
  #include <supplies/donoffsupply-dht.h>
#endif

#ifdef SUPPLY_6TEMP
  #include <supplies/prj_supplies/donoffsupply-6temp.h>
#endif

#ifdef SUPPLY_UNI_MAX44003
  #include <supplies/donoffsupply-uni-max44003.h>
#endif

#ifdef SUPPLY_ADS11x5
  #include <supplies/donoffsupply-ads.h>
#endif



#include <PubSubClient.h>

WMSettings settings;

WifiCreds wcreds;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length);

Queue<pub_events> que_wanted= Queue<pub_events>(MAX_QUEUE_WANTED);

DPublisherMQTT pubmqtt(&settings, &client);

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


#ifdef SUPPLY_UNI_SCT013
    DSupplyDonoffUniSct013 supply(&settings);
#endif


#ifdef SUPPLY_SCT013_SINGLE
  DSupplySCT013Collector supply(&settings);
#endif

#ifdef SUPPLY_MULTI_SCT013
  DSupplyMultiSCT013Collector supply(&settings);
#endif

#ifdef SUPPLY_DHT
  DSupplyDHT supply(&settings);
#endif

#ifdef SUPPLY_6TEMP
  DSupplyDonoff6Temp supply(&settings);
#endif

#ifdef SUPPLY_UNI_MAX44003
  DSupplyDonoffUniMAX44003 supply(&settings);
#endif

#ifdef SUPPLY_ADS11x5
  DSupplyADS supply(&settings);
#endif





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

  
  pinMode(SONOFF_LED, OUTPUT);
  
  ticker.attach(0.25,tick);
    
  supply.set_blink(BL_CONNECTING);

  /* try to connect to wifi*/
  Serial.println("Connecting Wifi...");

  

  wifi_station_get_config (&stationConf);

  Serial.println(String("saved ssid:"+String((char*)stationConf.ssid)+", saved pass=" + String((char*) stationConf.password)));
  

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);

  WiFi.begin();

    int c = 0;
    while (WiFi.status() != WL_CONNECTED && c < 20) {
      delay(500);
      Serial.print(".");
      c++;
    }
  
  
 if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WIFI_CONNECTED");
    Serial.print("LOCAL IP:");
    Serial.println(WiFi.localIP());
    Serial.print("GATEWAY:");
    Serial.println(WiFi.gatewayIP());
    Serial.print("DNS:");
    Serial.println(WiFi.dnsIP());

  }else{
    Serial.println("WIFI_NOT_CONNECTED");
  }

  client.setCallback(callback);

  pubmqtt.init(&que_wanted);

  notifyer.init(&pubmqtt);

  #ifdef SUPPLY_UNIVERSAL
    Serial.println("UNIVERSAL SUPPLY");
  #endif

  supply.init(&notifyer, &pubmqtt, &que_wanted);


}


/************************* LOOOOOOOOOOOOOOOOOOOOOOOOOOP *******************************************/

void loop()
{

if (client.connected()) client.loop();


supply.supply_loop();


}
