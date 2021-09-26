#ifndef donoffsupplydonoffmax44003
#define donoffsupplydonoffmax44003


#include <supplies/donoffsupply-donoff-universal.h>
#include <sensors/donoffsensor_max44003.h>

#define MAX44003 1 //for SUPPLY_UNI_SCT013 

#ifdef D_MQTT
    const char  LUX_OUT_CHANNEL[] = "/out/sensors/lux";
#endif

class DSupplyDonoffUniMAX44003: public DSupplyDonoffUni {

  protected:

  MAX44003Sensor *lux=NULL;

  public:
    DSupplyDonoffUniMAX44003(WMSettings * __s): DSupplyDonoffUni(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

        DSupplyDonoffUni::init(_notifyer, _pub, _q);
        init_ok = 0;

        if(MAX44003){
          debug("SUPPLY_INIT", "MAX440033 INIT");
          lux = new MAX44003Sensor(_s);
          lux->init("LUX", LUX_OUT_CHANNEL, SENSOR_NOT_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_LUX,JSON_CHANNEL_YES);
        }
        init_ok=1;
    
    };

   //second sensor loop
   int sensors_loop(int sensor_num){ 
      
       DSupplyDonoffUni::sensors_loop(sensor_num);

       if(MAX44003){ 
        if(sensor_num==3){
          lux->sensor_loop();
          debug("LUX_OUT", lux->get_val_Str());
        }
       }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyDonoffUni::service_loop();

       if(MAX44003){
          if(pub->is_connected()){
                pub->publish_sensor(lux);
          }
       }
   };
   


   
};

 





#endif