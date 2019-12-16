#ifndef donoffconfigmqtt_h
#define donoffconfigmqtt_h

#include <WiFiManager.h> 
#include <configs/donoffconfig.h>
#include <donoffbase.h>
#include <donoffsettings.h>

class DConfigMQTT: public DConfig{

    public:
       
        WiFiManagerParameter* p1; 
        WiFiManagerParameter* p2;
        WiFiManagerParameter* p3;
        WiFiManagerParameter* p4;
        WiFiManagerParameter* p5;
        WiFiManagerParameter* p6;

        DConfigMQTT(WMSettings * __s): DConfig(__s){};

    private:

      String newssid;
      String newpass;
      struct station_config stationConf;
      int new_creds=0;

    public:
        

    int init() {

        DConfig::init();

        debug("CONFIG_MQTT_INIT", "CONFIG INIT");
        init_ok=0;

        p1= new WiFiManagerParameter("mqtt-server", "mqtt server", _s->mqttServer, 22);
        p2= new WiFiManagerParameter("mqtt-user", "mqtt user", _s->mqttUser, 12);
        p3= new WiFiManagerParameter("mqtt-pass", "mqtt passw", _s->mqttPass, 22);
        p4= new WiFiManagerParameter("mqtt-port", "mqtt port", _s->mqttPort, 6);
        p5= new WiFiManagerParameter("dev_id", "device id", _s->dev_id, 10);
        p6= new WiFiManagerParameter("email", "email to notify", _s->email_notify, 50);

        wm.addParameter(p1);
        wm.addParameter(p2);
        wm.addParameter(p3);
        wm.addParameter(p4);
        wm.addParameter(p5);
        wm.addParameter(p6);

        //wm.setTimeout(120);
        
        init_ok = 1;
    };

    int connect_new_creds(){
        new_creds=0;
        char c_ssid[32];
        char c_password[64];

        wifi_station_get_config (&stationConf);

        debug("CONFIG", "old ssid="+String((char*)stationConf.ssid)+", old pass=" + String((char*) stationConf.password));

        if (newssid != "" && newpass != "")
        {
            
            int  e1=strcmp((char*) stationConf.ssid,  newssid.c_str());
            int  e2=strcmp((char*) stationConf.password,   newpass.c_str());

            // if somthing is changed, write config
            if ((e1!=0) ||(e2!=0))
            {
                // ets_strcpy((char*) stationConf.ssid,newssid.c_str());
                // ets_strcpy((char*) stationConf.password,newpass.c_str());
                strcpy(c_ssid, newssid.c_str());
                strcpy(c_password, newpass.c_str());

                os_memcpy(&stationConf.ssid, c_ssid, 32);
                os_memcpy(&stationConf.password, c_password, 64);
   
                //debug("CONFIG", "B:SET CONFIG");
                debug("CONFIG", "new ssid:"+String((char*)stationConf.ssid)+", new pass=" + String((char*) stationConf.password));
                //wifi_station_set_config_current(&stationConf);
                //wifi_station_set_config(&stationConf);
               
                //debug("CONFIG", "E:SET CONFIG");
                new_creds=1;
                debug("CONFIG", "newcreds="+String(new_creds)+", new ssid:"+String((char*)stationConf.ssid)+", new pass=" + String((char*) stationConf.password));
            }

        }

        //WiFi.disconnect(); 
        //WiFi.mode(WIFI_STA);

        // if(new_creds){
            //debug("CONFIG", "start Wifi Client with ssid\pass");            
        debug("CONFIG", "Starting wifi with ssid="+String((char*)stationConf.ssid)+", pass=" + String((char*) stationConf.password));
        if(String((char*)stationConf.ssid)!="" && String((char*) stationConf.password)!=""){
            debug("CONFIG", "Start with new ssid\pass");
            //WiFi.begin((char*)stationConf.ssid, (char*) stationConf.password);
            WiFi.begin(newssid.c_str(), newpass.c_str());
        }else{
            debug("CONFIG", "Start with old ssid\pass");
            WiFi.begin();
        }
        // } 
        // else{
        //     debug("CONFIG", "start Wifi Client with saved ssid\pass");
        //     WiFi.begin();  
        // } 

        int c = 0;
        while (WiFi.status() != WL_CONNECTED && c < 20)
        {
            delay(500);
            Serial.print(".");
            c++;
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("WIFI_CONNECTED");
        }
        else
        {
            Serial.println("WIFI_NOT_CONNECTED");
        }
        debug("CONFIG", "FINISH CONFIG");

       

    };

    int virtual connect_with_new_creds(){

    }

 
    int virtual config(){
      
    //   if (!wm.startConfigPortal("OnDemandAP", NULL, &newssid, &newpass )) {
    //         Serial.println("failed to connect and hit timeout");
    //         delay(3000);
    //         //reset and try again, or maybe put it to deep sleep

    //         ESP.reset();
    //         delay(5000);
    //     }

        wm.startConfigPortal(WIFI_CONFIG_AP_NAME, NULL, false);

        //wm.startConfigPortal(WIFI_CONFIG_AP_NAME, NULL, &newssid, &newpass );
        //debug("CONFIG", "GOT FROM WIFIMANAGER - newssid="+newssid+", newpass="+newpass);

        //connect_new_creds();
        
        // WifiCreds w;
        // w.SALT=WIFI_EEPROM_SALT;
        // strcpy(w.ssid,newssid.c_str());
        // strcpy(w.pass,newpass.c_str());
        // Serial.println(String(w.ssid)+"||"+ String(w.pass));

        // EEPROM.begin(100);
        // EEPROM.put(700, w);
        // EEPROM.end();   

        // debug("CONFIG", "Saved !");

 

        if(String(p1->getValue()) != ""){
            strcpy(_s->mqttServer,p1->getValue());
            debug("CONFIG", "NEWVALUE=" + String(p1->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        // delete p1;

        if(String(p2->getValue()) != ""){
            strcpy(_s->mqttUser,p2->getValue());
            debug("CONFIG", "NEWVALUE=" + String(p2->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        // delete p2;


        if(String(p3->getValue()) != ""){
            strcpy(_s->mqttPass,p3->getValue());
            debug("CONFIG", "NEWVALUE=" + String(p3->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        // delete p3;

        if(String(p4->getValue()) != ""){
            strcpy(_s->mqttPort,p4->getValue());
            debug("CONFIG", "NEWVALUE=" + String(p4->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        // delete p4;

        if(String(p5->getValue()) != "" || p5->getValue() != "empty_dev"){
            strcpy(_s->dev_id,p5->getValue());
            debug("CONFIG", "NEWVALUE=" + String(p5->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        // delete p5;

        if(String(p6->getValue()) != ""){
            strcpy(_s->email_notify,p6->getValue());
            debug("CONFIG", "NEWVALUE=" + String(p6->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        // delete p6;

        save();
        
        //wifi_station_set_config(&stationConf);

    };





};

#endif