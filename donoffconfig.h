#ifndef donoffconfig_h
#define donoffconfig_h

#include <WiFiManager.h> 
#include <donoffbase.h>

class DConfig: public DBase{

    public:
        int init_ok=0;
        WiFiManager wm;
        WiFiManagerParameter* p1; 
        WiFiManagerParameter* p2;
        WiFiManagerParameter* p3;
        WiFiManagerParameter* p4;
        WiFiManagerParameter* p5;
        WiFiManagerParameter* p6;

        DConfig(WMSettings * __s): DBase(__s){};

        // WiFiManagerParameter custom_mqtt_text("mqtt server <br/> No token to disable.");
        // WiFiManagerParameter custom_mqtt_server("mqtt-server", "mqtt server", settings.mqttServer, 22);
        // WiFiManagerParameter custom_mqtt_user("mqtt-user", "mqtt user", settings.mqttUser, 12);
        // WiFiManagerParameter custom_mqtt_pass("mqtt-pass", "mqtt passw", settings.mqttPass, 22);
        // WiFiManagerParameter custom_mqtt_port("mqtt-port", "mqtt port", settings.mqttPort, 6);
        // WiFiManagerParameter custom_dev_id("dev_id", "device id", settings.dev_id, 10);
        // WiFiManagerParameter custom_email("email", "email to notify", settings.email_notify, 50);

    private:

    public:
        

    int init() {
        p1= new WiFiManagerParameter("mqtt-server", "mqtt server (ip or name)", _s->mqttServer, 22);
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

        wm.setTimeout(120);
        
        init_ok = 1;
    };

    int config(){
      if (!wm.startConfigPortal("OnDemandAP")) {
            Serial.println("failed to connect and hit timeout");
            delay(3000);
            //reset and try again, or maybe put it to deep sleep

            ESP.reset();
            delay(5000);
        }

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

        if(String(p5->getValue()) != ""){
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
        
        

    };





};

#endif