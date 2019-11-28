#ifndef donoffsupplymultisct013collector_h
#define donoffsupplymultisct013collector_h

#include <donoffrelay.h>
#include <donoffdisplay.h>
#include <configs/donoffconfig-mqtt.h>
#include <donoffbutton.h>

#include <supplies/donoffsupply-base.h>
#include <sensors/donoff-multisensor-sct013.h>

#ifdef D_MQTT
    const char SCT013_X3_OUT_CHANNEL[]="/out/sct013x3";
#endif


class DSupplyMultiSCT013Collector: public DSupplyBase {

  protected:
    DMultiSensorSCT013 *multi_sct013;
  

  public:
    DSupplyMultiSCT013Collector(WMSettings * __s): DSupplyBase(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
        DSupplyBase::init(_notifyer, _pub, _q);
        init_ok = 0;
      
        debug("SUPPLY_INIT", "SCT013_1 INIT");
        multi_sct013 = new DMultiSensorSCT013 (_s);
        multi_sct013->init(12, 13, "SCT013x3", SCT013_X3_OUT_CHANNEL, 0,que_sensor_states);
        

        init_ok=1;
    
    };

   //first sensor loop
   int sensors_loop(int sensor_num){
       if(sensor_num==1){
          sct013[0]->sensor_loop();
          debug("SCT013_OUT", sct013[0]->get_val_Str());
       }
   };


   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyBase::service_loop();

       if (pub->is_connected()) pub->publish_sensor(sct013[0]);

   };





};





#endif