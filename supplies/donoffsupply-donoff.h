#ifndef donoffsupplydonoff_h
#define donoffsupplydonoff_h

#include <supplies/donoffsupply-base.h>
#include <sensors/donoffsensor_ds1820.h>

#ifdef D_MQTT
    const char RELAY1_ONOFF_CHANNEL[] = "/out/relays/r1";
    const char RELAY1_ONTIME_CHANNEL[] = "/out/time_comm";
    const char RELAY1_DOWNTIME_CHANNEL[] = "/out/time_down";
    const char DS_IN_CHANNEL[] = "/out/sensors/temp_in";
#endif

class DSupplyDonoff: public DSupplyBase {

  protected:

    DRelay *r[4]={NULL, NULL, NULL, NULL};
    DigitalDS1820Sensor* ds_in;

    String outStr;

  public:
    DSupplyDonoff(WMSettings * __s): DSupplyBase(__s) {};

    int virtual notify_sesnors_loop(){   
      if (que_sensor_states->count()==0) return 0;
      
      debug("QUE_SENS", String(que_sensor_states->count()) );
      while(que_sensor_states->count()!=0){
         sensor_state state=que_sensor_states->pop();
         debug("QUE_SENS", state.ps->get_nameStr()+":"+String(state.val)+":"+String(state.curr_state)+":"+String(state.prev_state));
         notifyer->notify_sensor_state(&state);
      }
      
    };

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
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
        
       
        debug("SUPPLY_INIT", "DS_IN INIT");
        ds_in = new DigitalDS1820Sensor(_s, IN_WIRE_BUS);
        ds_in->init("ds_in", DS_IN_CHANNEL, DS1820_NOT_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_NO);

        init_ok=1;
    
    };

   //first sensor loop
   int sensors_loop(int sensor_num){
       if(sensor_num==1){
          ds_in->sensor_loop();
          debug("DS_IN", ds_in->get_val_Str());
       }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {
       DSupplyBase::service_loop();
       if(pub->is_connected()){
            pub->publish_sensor(ds_in);

            pub->publish_ontime(r[0]);
            pub->publish_downtime(r[0]);
            pub->publish_relay_state(r[0]);
       }
   };

   int virtual notifyer_loop(){
      if(!_s->notifyer || _s->hours_on_notify==0 ) return 0;
      
      if(_s->hours_on_notify!=0) notify_relay_hours_loop();
      notify_sesnors_loop();
   };

   int virtual notify_relay_hours_loop(){
      if(r[0]->is_on()){
        if(r[0]->get_hours_working()>_s->hours_on_notify && r[0]->is_notifyed_h()==0){
          if(notifyer->notify_working_hours(r[0])) r[0]->set_notifyed_h(1) ;
        }     
      }
   };

   //add short_press event to toggle relay_1
   int virtual button_loop(){

       DSupplyBase::button_loop();

        if (button_result == SHORT_PRESS){
           debug("SUPPLY_DBUTTON", "Toggle relay");
           if(_s->hotter>=1) _s->hotter=0;
           if(_s->cooler>=1) _s->cooler=0;
           if(_s->lscheme_num>0) _s->lscheme_num=0;
           if(_s->autostop_sec>0) _s->autostop_sec=0;
           relay_toggle(r[0], "hardware"); //if SHORT_PRESS, TOGGLE
        }

    };

     int slow_loop(int mycounter){

         DSupplyBase::slow_loop(mycounter);

         if(mycounter==5){
             //debug("SUPPLY", "LSCHM_LOOP");
             lschm_loop();
         }

         if(mycounter==6){
             aofh_loop();
         }
     };

    int virtual do_want_event(){
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

     int virtual fast_loop() {
        aofs_loop();
     };


     int virtual aofs_loop() {
    
      if (_s->autostop_sec > 0 && _s->hotter==0 && _s->cooler==0) {
        //debug("AOFS", String(_s->autostop_sec));
        aofs_off(r[0], _s->autostop_sec);
      }

    };
    
    //autooff hours loop for relay_1
    int virtual aofh_loop() {
      //300 ms loop
      if (_s->autooff_hours > 0 && _s->lscheme_num == 0 &&  _s->hotter==0 && _s->cooler==0 ) {
        aofh_off(r[0], _s->autooff_hours);
      }
    };

    //light scheme loop for relay_1
    int virtual lschm_loop() {
      if (_s->lscheme_num > 0 &&  _s->hotter==0 && _s->cooler==0) {
        lschm_on_off(r[0], _s->lscheme_num);
      }
    };

    int virtual display_loop() {
      String outStr="";
      if (r[0]->is_on()) outStr += "r1:on"; else outStr += "r1:off";
      D->show_str(1, outStr, "", D_FIRST_STRING, !D_LAST_STRING);

        // D.show_sensor(1, "t:",  ds1820_get_filtered_val(), 100,0,0);
      D->show_sensor(1, "t1", ds_in,  !D_FIRST_STRING, !D_LAST_STRING);

    };

  int virtual reset(){
    //turn off relay before reset
      relay_off(r[0], "off, before reset");
      DBase::reset();
  };

  int virtual cooler_loop()=0;
  int virtual hotter_loop()=0;

  int virtual hotter_cooler_loop() {
      if (_s->hotter>=1) {
        if(_s->lscheme_num>0 || _s->autooff_hours >0){
            pub->publish_to_info_topic("E:ignore hotter, lschm,aofs");
            _s->hotter=0;
        }else{
            hotter_loop();
        }
      } else if (_s->cooler){
         if(_s->lscheme_num>0 || _s->autooff_hours >0){
            pub->publish_to_info_topic("E:ignore cooler, lschm,aofs");
            _s->cooler=0;
        }else{
           cooler_loop();
        }
      }
    };


};





#endif