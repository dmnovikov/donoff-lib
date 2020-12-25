#ifndef donoffsupplydonoffusct013_h
#define donoffsupplydonoffusct013_h


#include <supplies/donoffsupply-donoff-universal.h>
#include <sensors/donoffsensor_sct013.h>

#ifdef D_MQTT
    const char  SCT013_OUT_CHANNEL[] = "/out/sensors/sct01";
#endif

class DSupplyDonoffUniSct013: public DSupplyDonoffUni {

  protected:

   SCT013Sensor *sct013[4]={NULL, NULL, NULL, NULL};

  public:
    DSupplyDonoffUniSct013(WMSettings * __s): DSupplyDonoffUni(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

        DSupplyDonoffUni::init(_notifyer, _pub, _q);
        init_ok = 0;

        debug("SUPPLY_INIT", "SCT013_1 INIT");
        sct013[0] = new SCT013Sensor(_s, A0);
        sct013[0]->init("SCT013_1", SCT013_OUT_CHANNEL, 0,que_sensor_states,SENSOR_TYPE_CURRENT,JSON_CHANNEL_YES);

        init_ok=1;
    
    };

   //second sensor loop
   int sensors_loop(int sensor_num){ 
      
       DSupplyDonoffUni::sensors_loop(sensor_num);

        if(sensor_num==3){
          sct013[0]->sensor_loop();
          debug("SCT013_OUT", sct013[0]->get_val_Str());
        }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyDonoffUni::service_loop();

       if(pub->is_connected()){
            pub->publish_sensor(sct013[0]);
       }
   };


   
};

 





#endif