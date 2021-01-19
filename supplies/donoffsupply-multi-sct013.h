#ifndef donoffsupplymultisct013collector_h
#define donoffsupplymultisct013collector_h

#include <supplies/donoffsupply-base.h>
#include <sensors/donoff-multisensor-sct013.h>

#ifdef D_MQTT
    const char SCT013_X3_OUT_CHANNEL[]="/out/sensors/sct013x3";
#endif

#define MULTISENSOR_PIN1 12
#define MULTISENSOR_PIN2 13


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
        multi_sct013->init(MULTISENSOR_PIN1, MULTISENSOR_PIN2, "SCT013x3", SCT013_X3_OUT_CHANNEL, 0,que_sensor_states, DONOFF_SENSOR_TYPE_CURRENT, JSON_CHANNEL_YES);
        

        init_ok=1;
    
    };

   //first sensor loop
   int virtual sensors_loop(int sensor_num){

       if(sensor_num==1){
          multi_sct013->multi_sensor_loop(sensor_num);
          debug("SCT013_OUT_1", multi_sct013->sensors[0]->get_val_Str());
       }

       if(sensor_num==2){
          multi_sct013->multi_sensor_loop(sensor_num);
          debug("SCT013_OUT_2", multi_sct013->sensors[1]->get_val_Str());
       }

      if(sensor_num==3){
          multi_sct013->multi_sensor_loop(sensor_num);
          debug("SCT013_OUT_3", multi_sct013->sensors[2]->get_val_Str());
       }

       if(sensor_num==4){
          multi_sct013->multi_sensor_loop(sensor_num);
          debug("SCT013_ALL",multi_sct013->get_val_Str());
       }



   };


   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyBase::service_loop();

       if (pub->is_connected()) {
            //debug("SERVICE_LOOP", "PUBLISH SCT-XX");

             for(int i=0; i<=2; i++){
                  pub->publish_sensor(multi_sct013->sensors[i]);
                  //debug("SERVICE_LOOP", "PUBLISH SCT-XX:"+String(i));
             }

             pub->publish_sensor(multi_sct013);

       }

   };





};





#endif