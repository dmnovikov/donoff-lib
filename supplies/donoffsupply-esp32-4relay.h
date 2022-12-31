#ifndef donoffsupplyesp324relays_h
#define donoffsupplyesp324relays_h

#include <supplies/donoffsupply-base.h>
//#include <sensors/donoffsensor_ds1820.h>

#ifdef D_MQTT
    const char RELAY1_ONOFF_CHANNEL[] = "/out/relays/r1";
    const char RELAY1_ONTIME_CHANNEL[] = "/out/time_comm";
    const char RELAY1_DOWNTIME_CHANNEL[] = "/out/time_down";
    const char DS_IN_CHANNEL[] = "/out/sensors/temp_in";
#endif

#define TOGGLE_ON_BUNNON_PRESS  0

class DSupplyESP324R: public DSupplyBase {

  protected:

    DRelay *r[4]={NULL, NULL, NULL, NULL};
    //DigitalDS1820Sensor* ds_in;

    String outStr;

  public:
    DSupplyESP324R(WMSettings * __s): DSupplyBase(__s) {};

    void init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
        DSupplyBase::init(_notifyer, _pub, _q);
        init_ok = 0;

        numrelays++;
        debug("SUPPLY_INIT", "R1 INIT, PIN:"+String(RELAY1_PIN));
        r[0] = new DRelay(_s);
        r[0]->init(RELAY1_PIN, "r1", 1, String(RELAY1_ONOFF_CHANNEL), String(RELAY1_ONTIME_CHANNEL), String(RELAY1_DOWNTIME_CHANNEL));
        //debug("SUPPLYINIT", "r1name="+r1->get_nameStr());

        if(_s->start_on && _s->custom_scheme1==0){
          relay_on(r[0],"on, start on");
        }
        
        // debug("SUPPLY_INIT", "DS_IN INIT");
        // ds_in = new DigitalDS1820Sensor(_s, IN_WIRE_BUS);
        // ds_in->init("ds_in", DS_IN_CHANNEL, DS1820_NOT_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_NO);

        init_ok=1;
    
    };

   //first sensor loop
   void sensors_loop(int sensor_num){
      //  if(sensor_num==1){
      //     ds_in->sensor_loop();
      //     debug("DS_IN", ds_in->get_val_Str());
      //  }
   };
   
   //add public sensor and relay_1 status
   void virtual service_loop() {
       DSupplyBase::service_loop();
       if(pub->is_connected()){
            //pub->publish_sensor(ds_in);

            pub->publish_ontime(r[0]);
            pub->publish_downtime(r[0]);
            pub->publish_relay_state(r[0]);
       }
   };

   int virtual notifyer_loop(){
      return 1;
   };

   void virtual notify_relay_hours_loop(){
   };

   //add short_press event to toggle relay_1
   void virtual button_loop(){

       DSupplyBase::button_loop();

        if (button_result == SHORT_PRESS && TOGGLE_ON_BUNNON_PRESS){
           debug("SUPPLY_DBUTTON", "Toggle relay");
           if(_s->hotter>=1) _s->hotter=0;
           if(_s->cooler>=1) _s->cooler=0;
           if(_s->lscheme_num>0) _s->lscheme_num=0;
           if(_s->autostop_sec>0) _s->autostop_sec=0;
           relay_toggle(r[0], "hardware"); //if SHORT_PRESS, TOGGLE
        }

    };

     void slow_loop(int mycounter){

         DSupplyBase::slow_loop(mycounter);

         if(mycounter==5){
             //debug("SUPPLY", "LSCHM_LOOP");
             lschm_loop();
         }

         if(mycounter==6){
             aofh_loop();
         }
     };

    void virtual do_want_event(){
      DSupplyBase::do_want_event();

      if(what_to_want==PUBLISHER_WANT_R1_ON) {
          if(r[0]!=NULL) relay_on(r[0],"from publisher");
          else pub->publish_to_info_topic("I:R1=DISABLED");
      }

      if(what_to_want==PUBLISHER_WANT_R1_OFF) {
         if(r[0]!=NULL) relay_off(r[0],"from publisher");
         else pub->publish_to_info_topic("I:R1=DISABLED");
      }

      if(what_to_want==PUBLISHER_WANT_R1_OFF_LSCHM0) {
        if(r[0]!=NULL) relay_off(r[0],"lschm or hotter=0");
        else pub->publish_to_info_topic("I:R1=DISABLED");
      }

      if(what_to_want==PUBLISHER_WANT_SH_R1) {
        if(r[0]!=NULL){
            if(r[0]->is_on()) pub->publish_to_info_topic("I:R1=ON");
            else pub->publish_to_info_topic("I:R1=OFF");
        }else{
           pub->publish_to_info_topic("I:R1=DISABLED");
        
        }
      }

      if(what_to_want==PUBLISHER_WANT_RESET_HOUR_R1_M) {
        if(r[0]!=NULL){
          r[0]->reset_lschm_hour();
          pub->publish_to_info_topic("I: R1 lschm last hour is reseted");
        }else pub->publish_to_info_topic("I:R1=DISABLED");
      }

      if(what_to_want==PUBLISHER_WANT_RESET_HOUR_R2_M) {
        if(r[1]!=NULL){
          r[1]->reset_lschm_hour();
          pub->publish_to_info_topic("I: R2 lschm last hour is reseted");
        }else pub->publish_to_info_topic("I:R2=DISABLED");
      }


    };

     void virtual fast_loop() {
        aofs_loop();
     };


     void virtual aofs_loop() {
    
      if (_s->autostop_sec > 0 && _s->hotter==0 && _s->cooler==0) {
        //debug("AOFS", String(_s->autostop_sec));
        aofs_off(r[0], _s->autostop_sec);
      }

    };
    
    //autooff hours loop for relay_1
    void virtual aofh_loop() {
      //300 ms loop
      if (_s->autooff_hours > 0 && _s->lscheme_num == 0 &&  _s->hotter==0 && _s->cooler==0 ) {
        aofh_off(r[0], _s->autooff_hours);
      }
    };

    //light scheme loop for relay_1
    void virtual lschm_loop() {
      if (_s->lscheme_num > 0 &&  _s->hotter==0 && _s->cooler==0) {
        lschm_on_off(r[0], _s->lscheme_num);
      }
    };

    void virtual display_loop() {
   
    };

  void virtual reset(){
    //turn off relay before reset
      relay_off(r[0], "off, before reset");
      DBase::reset();
  };

  void virtual cooler_loop(){};
  void virtual hotter_loop(){};

  void virtual hotter_cooler_loop() {
  };


};





#endif