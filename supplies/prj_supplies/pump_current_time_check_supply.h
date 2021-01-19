#ifndef donoffsupplydonoffuniversalcurrent_h
#define donoffsupplydonoffuniversalcurrent_h

/*
Control current and and time of strong current, turn off after period

commands: 
  clevel1 - current in A*100 (1A : clevel1=100)
  clevel2 - working time (with sensor's current > clevel1) before alarm in seconds (10min : clevel2=600)
   
  if clevel1, clevel2 are undefined (-128 from default eeprom), we use  DEFAULT_CURRENT, DEFAULT_STRONG_CURRENT_SEC
  
*/

#include <supplies/donoffsupply-donoff-universal.h>
#include <sensors/donoffsensor_sct013.h>


#define DEFAULT_CURRENT 100; // 1.00 Amp
#define DEFAULT_STRONG_CURRENT_SEC  3600 //1 hour before alarm off

#ifdef D_MQTT
    const char SCT013_OUT_CHANNEL[]="/out/sensors/current";
    const char CURRENT_STRONG_TIME_CHANNEL[]="/out/sensors/strong_sec";
#endif


class DSupplyDonoffUniCurr: public DSupplyDonoffUni {

  protected:

   SCT013Sensor *sct013=NULL;
   ulong start_strong_current_ms=0;
   ulong strong_current_ms=0;

  public:
    DSupplyDonoffUniCurr(WMSettings * __s): DSupplyDonoffUni(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

        DSupplyDonoffUni::init(_notifyer, _pub, _q);
        init_ok = 0;

        debug("SUPPLY_INIT", "SCT013_1 INIT");
        sct013= new SCT013Sensor(_s, A0);
        sct013->init("SCT013_1", SCT013_OUT_CHANNEL, 0,que_sensor_states,DONOFF_SENSOR_TYPE_CURRENT,JSON_CHANNEL_NO);

        if(_s->custom_level1<0){
          _s->custom_level1=DEFAULT_CURRENT; //if not defined (-128), set default
          debug("SUPPLY", "Set CURRENT LEVEL TO DEFAULT");
        } 
        if(_s->custom_level2<0){
          _s->custom_level2=DEFAULT_STRONG_CURRENT_SEC;
          debug("SUPPLY", "Set STRONG TIME TO DEFAULT");
        }

        init_ok=1;
    
    };

    int sensors_loop(int sensor_num){

       DSupplyDonoffUni::sensors_loop(sensor_num);

       if(sensor_num==3 && _s->custom_level1>0){
          sct013->sensor_loop();
          debug("SCT013_OUT", sct013->get_val_Str());
          ulong current=sct013->get_val();
          //current=160; // test

          if(sct013->is_started_and_ready()){
            if(current <= _s->custom_level1) {
              debug("STRONG_CURR", "No strong time, current is small:" + String(current)+"; level="+ String(_s->custom_level1) );
              start_strong_current_ms=0;
              strong_current_ms=0;
            }
            else{
              if(strong_current_ms==0) {
                start_strong_current_ms=millis();
                strong_current_ms=1;
                debug("STRONG_CURR", "Start strong time" );
              }
              else{
                strong_current_ms=millis()-start_strong_current_ms;
                //debug("STRONG_CURR", "STRONG_TIME:"+String(strong_current_ms/1000));
              }
            }
          }else{
            debug("SUPPLY", "SCT013 is not ready or started, ignore calculations");
          }
       }
   };


   
   //add public sensor and relay_1 status
    int virtual service_loop() {

       DSupplyDonoffUni::service_loop();

       if (pub->is_connected()){
          pub->publish_sensor(sct013);
          pub->publish_to_topic(CURRENT_STRONG_TIME_CHANNEL,String((int) (strong_current_ms/1000)));
       }

   };


     int slow_loop(int mycounter){

         DSupplyDonoffUni::slow_loop(mycounter);      

         if(mycounter==8 && _s->custom_level1>0){  //id custom_level1==0 ignore this sensor

            if(strong_current_ms>0)
               debug("STRONG_CURR", "CURRENT MS="+String(strong_current_ms)+"; level="+String(_s->custom_level2*1000));

             if(strong_current_ms >_s->custom_level2*1000){
                
                debug("STRONG_CURR", "SET lschm=1, always off");
                _s->lscheme_num=1; //set lschm=1 (always off)
                pub->publish_to_info_topic("strong_curr: off, current strong time");
             } 

             if(r[0]->is_off() && strong_current_ms>0){
               strong_current_ms=0;
               start_strong_current_ms=0;
               debug("STRONG_CURR", "Err: relay off but curent is big");
               // pub->publish_to_info_topic("E: relay is off, but curr");
             }
         }
     };
  
//  int virtual notifyer_loop(){

//    debug("NOTIFYER_LOOP", "sensor notify");
//  };

};

 





#endif