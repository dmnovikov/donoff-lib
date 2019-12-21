#ifndef donoffsupplyconfigbutton_h
#define donoffsupplyconfigbutton_h


#include <configs/donoffconfig-mqtt.h>
#include <supplies/donoffsupply.h>
#include <configs/donoffconfig-mqtt.h>

class DSupplyBase: public DSupply {

  protected:

    DButton* b1;
    DConfigMQTT* conf;
    int button_result=0;

  public:
    DSupplyBase(WMSettings * __s): DSupply(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
        DSupply::init(_notifyer, _pub, _q);
        init_ok = 0;

        debug("SUPPLY_INIT", "DBUTTON INIT");
        b1 = new DButton(_s);
        b1->init();

        debug("SUPPLY_INIT", "CONFIG INIT");
        conf = new DConfigMQTT(_s);
        conf->init();

        init_ok=1;
    
    };


    int virtual native_loop() {
        button_loop();
    };

    int virtual button_loop(){

         // debug("SUPPLY_LOOP", "Button loop");
        button_result = b1->button_loop(); //return button status

        if (button_result == CONFIG_PRESS){
          debug("SUPPLY_DBUTTON", "Enter Config Mode...");
          set_blink(BL_CONNECTING);
          conf->config();

        }

    };



};





#endif