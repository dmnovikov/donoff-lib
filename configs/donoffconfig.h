#ifndef donoffconfig_h
#define donoffconfig_h

#include <WiFiManager.h> 
#include <donoffbase.h>

class DConfig: public DBase{

    public:
        int init_ok=0;
        WiFiManager wm;
       
        DConfig(WMSettings * __s): DBase(__s){};

    private:

    public:
        

    void init() {
        init_ok = 1;
    };

    void virtual config(){
    //    String newssid;
    //    String newpass;
    //    if (!wm.startConfigPortal("OnDemandAP", NULL, &newssid, &newpass)) {
    //         Serial.println("failed to connect and hit timeout");
    //         delay(3000);
    //         //reset and try again, or maybe put it to deep sleep

    //         ESP.reset();
    //         delay(5000);
    //     }
    };

   
};

#endif