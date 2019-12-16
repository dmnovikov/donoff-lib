#include <ESP8266WiFi.h>       

struct station_config stationConf;
boolean try_to_connect = false;  //by default we dont try to connect, we only 

char* ssid = "flashing_ssid";
char* pass = "flashing_pass";


void setup() {
  Serial.begin(9600);
  
  Serial.println("Read creds from flash ");

  wifi_station_get_config (&stationConf);

  Serial.println(String("saved ssid=" + String((char*)stationConf.ssid) + ", saved pass=" + String((char*) stationConf.password)));

  Serial.println("Write creds without connect");
   
  WiFi.begin(ssid, pass, 0, NULL, false);

  Serial.println("Read new creds from flash ");

  wifi_station_get_config (&stationConf);
  

  Serial.println(String("new saved ssid=" + String((char*)stationConf.ssid) + ", new saved pass=" + String((char*) stationConf.password)));


  if (try_to_connect) {

    WiFi.begin();

    int c = 0;
    while (WiFi.status() != WL_CONNECTED && c < 20) {
      delay(500);
      Serial.print(".");
      c++;
    }


    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WIFI_CONNECTED");
    } else {
      Serial.println("WIFI_NOT_CONNECTED");
    }

    
  }

}


void loop() {
}