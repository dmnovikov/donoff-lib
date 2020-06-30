#ifndef donoffsupplydonoffuniversal_h
#define donoffsupplydonoffuniversal_h

#include <supplies/donoffsupply-donoff.h>

#ifdef D_MQTT
    const char DS_OUT_CHANNEL[] = "/out/temp_out";
    const char RELAY2_ONOFF_CHANNEL[] = "/out/b2";
#endif

#if !defined (RELAY2)
    #define RELAY2 0
#endif

#if !defined(DS1820_OUT)
    #define DS1820_OUT 1
#endif


class DSupplyDonoffUni: public DSupplyDonoff {

  protected:

    DigitalDS1820Sensor* ds_out;

  public:
    DSupplyDonoffUni(WMSettings * __s): DSupplyDonoff(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

        DSupplyDonoff::init(_notifyer, _pub, _q);
        init_ok = 0;

        if (RELAY2) {
        numrelays++;
        debug("SUPPLY_INIT", "R2 INIT, PIN:"+String(RELAY2_PIN));
        r[1] = new DRelay(_s);
        r[1]->init(RELAY2_PIN, "r2", 2, String(RELAY2_ONOFF_CHANNEL));
      }

      if (DS1820_OUT) {
        debug("SUPPLY_INIT", "DS_OUT INIT");
        ds_out = new DigitalDS1820Sensor(_s, OUT_WIRE_BUS);
        ds_out->init("ds_out", DS_OUT_CHANNEL, DS1820_FILTERED,que_sensor_states, SENSOR_TYPE_TEMPERATURE, BASELOG_YES);
      }
        init_ok=1;
    
    };

   //second sensor loop
   int sensors_loop(int sensor_num){ 
      
       DSupplyDonoff::sensors_loop(sensor_num);

       if(sensor_num==2){
         if(DS1820_OUT){
              ds_out->sensor_loop();
              debug("DS_OUT", ds_out->get_val_Str());
         }
         if (DS1820_OUT && _s->notifyer ){
          long low_level=_s->temp_low_level_notify;
          long high_level=_s->temp_high_level_notify;
          //old format (celsius), new format celsius*100
          if(abs(_s->temp_low_level_notify)<100) low_level=_s->temp_low_level_notify*100;
          if(abs(_s->temp_high_level_notify)<100) high_level=_s->temp_high_level_notify*100;
          ds_out->sensor_check_state(low_level,high_level);
        }
       }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyDonoff::service_loop();

       if(pub->is_connected() && DS1820_OUT){
            pub->publish_sensor(ds_out);
       }

       if(pub->is_connected() && RELAY2){
          pub->publish_relay_state(r[1]);
       }

   };

     int slow_loop(int mycounter){

         DSupplyDonoff::slow_loop(mycounter);

         if(mycounter==7){
             hotter_cooler_loop();
         }

     };

    int virtual hotter_cooler_loop() {
      if (_s->hotter) {
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

     int virtual hotter_loop(){
      if(DS1820_OUT) hotter(ds_out, r[0]);
    };

    int virtual cooler_loop(){
      if(DS1820_OUT) cooler(ds_out,r[0]);
    };





    int virtual do_want_event(){
      DSupplyDonoff::do_want_event();

      if(RELAY2 && what_to_want==PUBLISHER_WANT_R2_OFF_LSCHM0) {
        relay_off(r[1],"lschm=0");
      }
     
       if(RELAY2 && what_to_want==PUBLISHER_WANT_R2_ON) {
        relay_on(r[1],"from publisher");
      }

      if(RELAY2 && what_to_want==PUBLISHER_WANT_R2_OFF) {
        relay_off(r[1],"from publisher");
      }
      

    };

  
     int virtual aofs_loop() {

      DSupplyDonoff::aofs_loop();

      if(RELAY2){
          if (_s->autostop_sec2 > 0 ) {
            //debug("AOFS", String(_s->autostop_sec2));
            aofs_off(r[1], _s->autostop_sec2);
          }
      }

    };
    

    //light scheme loop for relay_1
    int virtual lschm_loop() {

      DSupplyDonoff::lschm_loop();

      if (RELAY2 && _s->lscheme_num2 > 0) {
        debug("LSCHM_LOOP", "r[1] lschm2="+String(_s->lscheme_num2));
        lschm_on_off(r[1], _s->lscheme_num2);
      }
      
    };

    int virtual display_loop() {

      DSupplyDonoff::display_loop();

      String outStr="";
      
      if(RELAY2){
          if (r[1]->is_on()) outStr += "r2:on"; else outStr += "r2:off";
          D->show_str(1, outStr, "", !D_FIRST_STRING, !D_LAST_STRING);
      }

        // D.show_sensor(1, "t:",  ds1820_get_filtered_val(), 100,0,0);
      if(DS1820_OUT){
          D->show_sensor(1, "t2", ds_out,  !D_FIRST_STRING, !D_LAST_STRING);
      }

    };

     int virtual notify_sesnors_loop(){   
       
      if (que_sensor_states->count()==0) return 0;
      debug("QUE_SENS", String(que_sensor_states->count()) );
      while(que_sensor_states->count()!=0){
         sensor_state state=que_sensor_states->pop();
         debug("QUE_SENS", state.ps->get_nameStr()+":"+String(state.val)+":"+String(state.curr_state)+":"+String(state.prev_state));
         notifyer->notify_sensor_state(&state);
      }
      
    };

//  int virtual notifyer_loop(){

//    debug("NOTIFYER_LOOP", "sensor notify");
//  };

};

 





#endif