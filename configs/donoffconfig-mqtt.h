#ifndef donoffconfigmqtt_h
#define donoffconfigmqtt_h

#include <WiFiManager.h> 
#include <configs/donoffconfig.h>
#include <donoffbase.h>
#include <donoffsettings.h>

class DConfigMQTT: public DConfig{

    public:
       
     WiFiManagerParameter *mqtt_user; 
     WiFiManagerParameter* mqtt_pass;
     WiFiManagerParameter* mqtt_port;
     WiFiManagerParameter* dev_id;
     WiFiManagerParameter* email;
     WiFiManagerParameter* mqtt_server;

    DConfigMQTT(WMSettings * __s): DConfig(__s){};

    void init() {

        DConfig::init();

        debug("CONFIG_MQTT_INIT", "CONFIG INIT");
        init_ok=0;
        wm.setDarkMode(true);
        wm.setTimeout(120);
        wm.setBreakAfterConfig(true);
        wm.setScanDispPerc(true);   
        wm.setConnectTimeout(20);
        
        
        init_ok = 1;
    };


    void virtual config(){
      
    
      //WiFiManagerParameter custom_html("<p style=\"color:pink;font-weight:Bold;\">DonOff config page</p>"); // only custom html
        mqtt_server = new WiFiManagerParameter ("mqtt_server", "mqtt server", _s->mqttServer, 22);
        mqtt_user = new WiFiManagerParameter ("mqtt_user", "mqtt user", _s->mqttUser, 12);
        mqtt_pass = new WiFiManagerParameter ("mqtt_pass", "mqtt passw", _s->mqttPass, 22);
        mqtt_port = new WiFiManagerParameter ("mqtt_port", "mqtt port", _s->mqttPort, 6);
        dev_id= new WiFiManagerParameter     ("dev_id", "device id", _s->dev_id, 10);
        email = new WiFiManagerParameter     ("email", "email to notify", _s->email_notify, 50);

        //wm.addParameter(&custom_html);
        wm.addParameter(mqtt_server);
        wm.addParameter(mqtt_user);
        wm.addParameter(mqtt_pass);
        wm.addParameter(dev_id);
        wm.addParameter(email);

        wm.startConfigPortal(WIFI_CONFIG_AP_NAME);

       debug("CONFIG", "End of WifiManager, return to donoff");
   
        if(String(mqtt_server->getValue()) != ""){
            strcpy(_s->mqttServer,mqtt_server->getValue());
            debug("CONFIG", "NEWVALUE=" + String(mqtt_server->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        
        if(String(mqtt_user->getValue()) != ""){
            strcpy(_s->mqttUser,mqtt_user->getValue());
            debug("CONFIG", "NEWVALUE=" + String(mqtt_user->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
        
        if(String(mqtt_pass->getValue()) != ""){
            strcpy(_s->mqttPass,mqtt_pass->getValue());
            debug("CONFIG", "NEWVALUE=" + String(mqtt_pass->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
       
        if(String(mqtt_port->getValue()) != ""){
            strcpy(_s->mqttPort,mqtt_port->getValue());
            debug("CONFIG", "NEWVALUE=" + String(mqtt_port->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
      
        if(String(dev_id->getValue()) != "" || dev_id->getValue() != "empty_dev"){
            strcpy(_s->dev_id,dev_id->getValue());
            debug("CONFIG", "NEWVALUE=" + String(dev_id->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
       
        if(String(email->getValue()) != ""){
            strcpy(_s->email_notify,email->getValue());
            debug("CONFIG", "NEWVALUE=" + String(email->getValue()));
        }else{
            debug("CONFIG", "EMPTY VAL, OLD VALUE WILL BE SAVED");
        }
       
        debug("CONFIG", "Saving new parameters to EEPROM");

        save();

        debug("CONFIG", "Delete WifiParameters objects");

        delete mqtt_server;
        delete mqtt_pass;
        delete mqtt_user;
        delete mqtt_port;
        delete dev_id;
        delete email;

    };






};


#endif