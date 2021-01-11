#ifndef donoffsupply_h
#define donoffsupply_h

#include <donoffrelay.h>
#include <donoffdisplay.h>
#include <configs/donoffconfig-mqtt.h>
#include <donoffbutton.h>
#include <Queue.h>


// #define D_MQTT

#ifdef D_BLYNK
  #include <publishers/donoffpublisher_blynk.h>
#endif

#ifdef D_MQTT
  #include <publishers/donoffpublisher_mqtt.h>
  #include <notifyers/donoffnotifyer-mqtt-pointer.h>
#endif

#if !defined(DDISPLAY)
    #define DDISPLAY 0
#endif

#if !defined(DDISPLAY_SSD1306)
    #define DDISPLAY_SSD1306 0
#endif

#if !defined(DOFFLINE)
    #define DOFFLINE 0
#endif


#define MAX_LOOPS 10
#define MAX_SENSORS 10
#define MAX_LOOP_COUNTER 30
#define MS_LOOP_TIMING 200


class DSupply: public DBase {
  private:
  
    
 protected:
    String reasonStr = "";
    uint8_t  blink_loop = 0;
    uint current_blink_type = 0;


    int numrelays = 0;
   
    uint mycounter = 0;
    uint mycounter2=0;
    ulong mytimer = 0;
    int init_ok = 0;
    int m_just_synced=0;
    pub_events what_to_want;

    Queue<pub_events>* que_wanted;
    Queue<sensor_state> * que_sensor_states;

    DPublisher* pub;
    DNotifyer* notifyer;
    DDisplay* D=NULL;



  public:
    DSupply(WMSettings * __s): DBase(__s) {};

    // int add_relay(){

    // };

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

      pub = _pub;
      notifyer = _notifyer;
      que_wanted=_q;
      que_sensor_states = new Queue<sensor_state>(10);
      mytimer = millis();

      if (DDISPLAY_SSD1306) {
        D = new DDisplay_SSD1306(_s);
        D->init();
      }

      init_ok = 1;

    };

    int supply_loop() {

      if ((millis() - mytimer) >= MS_LOOP_TIMING ) {
        //6 sec loop
        if(mycounter >=0 && mycounter<=MAX_LOOPS){
           //debug("SUPPLY_LOOP", "SLOW LOOP");
           slow_loop(mycounter);
           
        }

        if(mycounter >MAX_LOOPS && mycounter <=MAX_LOOPS+MAX_SENSORS){
          //debug("SUPPLY_LOOP", "SENSORS LOOP");
          sensors_loop(mycounter-MAX_LOOPS);
        }

        if(mycounter==MAX_LOOP_COUNTER) {
          debug("VERY_SLOWLOOP", "counter="+String(mycounter2));
          very_slow_loop(mycounter2);
          mycounter2++;
        }

        mycounter++;
        //300 ms loop
       

        fast_loop();

        pub_wanted_loop();
       
        //

        
        if (mycounter > MAX_LOOP_COUNTER) {
          mycounter = 0;
          if(m_just_synced==1) m_just_synced=0;
        }
        mytimer = millis();

      }

      if(mycounter2>MAX_SLOW_LOOP_COUNTER){
        mycounter2=0;
      }

      native_loop();

    };

    int virtual one_minute_loop(){};
    int virtual ten_minutes_loop(){};
    int virtual thirty_minutes_loop(){};
    int virtual one_hour_loop(){};

    int virtual very_slow_loop(int counter){
          
          if(counter % 10 == 0 && counter!=0){ 
              debug("SLOWLOOP", "1 MINUTE, counter="+String(counter));
              one_minute_loop();
          }


          if(counter % 100 == 1 && counter!=0){   
              debug("SLOWLOOP", " 10 MINUTES, counter="+String(counter));
              ten_minutes_loop();
          }

          if((counter == 302 || counter==598) && counter!=0){           
              debug("SLOWLOOP", "30 MINUTES, counter="+String(counter));
              thirty_minutes_loop();
          }

          if(counter==599 && counter!=0){           
              debug("SLOWLOOP", " one_hour, counter="+String(counter));
              one_hour_loop();
          }

    };

    int virtual slow_loop(int mycounter){
         if (mycounter == 0) {
          //debug("SUPPLY_LOOP", "Reconnect loop");
          if(!DOFFLINE) reconnect_loop();
        }

         if (mycounter == 1) {
          //debug("SUPPLY_LOOP", "Sync loop");
          sync_blink_mode();
        }

        if (mycounter == 2) {
          service_loop();
        }

        if(mycounter==3){
          notifyer_loop();
        }

        if(mycounter==4){
          if(D!=NULL) display_loop();
        }


    };

    int virtual notifyer_loop(){
      if(!_s->notifyer) return 0;
    };

    int virtual display_loop(){};

    int virtual pub_wanted_loop(){
      //debug("SUPPLY_QUEUE", "Loop queue wanted");

      if (que_wanted->count()==0) return 0;

      debug("SUPPLY_QUEUE", "WANTED EVENT DETECTED:"+String(what_to_want));
      what_to_want=que_wanted->pop();

      if(what_to_want==PUBLISHER_WANT_SAY_JUST_SYNCED){
        m_just_synced=1;
      }

      do_want_event();
      
    };

    int virtual do_want_event(){
        if(what_to_want==PUBLISHER_WANT_SAVE){
        save();
        pub->publish_to_info_topic("N: saved");
      }
      
      if(what_to_want==PUBLISHER_WANT_RESET) {
        pub->publish_to_info_topic("I: OK, LET'S RESET");
        reset();
      }
    };
    
    int virtual sensors_loop(int mycounter){};
    int virtual fast_loop() {};
    int virtual native_loop() {};


    int reconnect_loop() {
      if (!pub->is_connected() || !pub->is_time_synced()) {
        pub->reconnect();
      }
    };

    int virtual service_loop() {

      debug("SHEDULER", "**Service loop->Time=" + String(hour()) + ":" + String(minute()) + ", y=" + String(year()) + " ,t_sync=" + String(pub->is_time_synced())+ 
           ", user="+ String(_s->mqttUser)+", dev_id=" + String(_s->dev_id)+" ,online="+String(pub->is_connected())+
           ", size_s="+String(sizeof(*_s))
       );

      if(pub->is_connected()) pub->publish_uptime();
      sync_blink_mode();

    };

    void set_blink(uint _current_blink_type ) {
      current_blink_type = _current_blink_type;
    };

    void tick() {
      if (  modes[current_blink_type] & 1 << (blink_loop & 0x07) ) digitalWrite(SONOFF_LED, HIGH);
      else  digitalWrite(SONOFF_LED, LOW);
      blink_loop++;
    };

    void virtual sync_blink_mode() {
      
        if(pub->is_connected()) {
          set_blink(BL_CONNECTED_OFF);
          return;
        } 
        else {
          set_blink(BL_OFFLINE_OFF);
          return;
        } 
    };

      int relay_on(DRelay* _r, String reasonStr) {
      if(_r->is_off()){
          _r->turn_on();
            debug("SUPPLY_RELAY_ON", _r->get_nameStr());
            pub->publish_relay_on(_r, reasonStr);
            pub->log_relay_on(_r,reasonStr);
            pub->publish_ontime(_r);
            notifyer->notify_on(_r, reasonStr);
            sync_blink_mode();

      } 
    };

    int relay_off(DRelay* _r, String reasonStr) {
      if(_r->is_on()){
        _r->turn_off();
        debug("SUPPLY_RELAY_OFF", _r->get_nameStr());
        pub->publish_relay_off(_r, reasonStr);
        pub->log_relay_off(_r,reasonStr);
        pub->publish_ontime(_r);
        notifyer->notify_off(_r, reasonStr);
        sync_blink_mode();
      } 
    };

    int relay_toggle(DRelay* _r, String reasonStr) {
      if (_r->is_on()) {
        relay_off(_r, reasonStr);
        return 1;
      }
      if (_r->is_off()) {
        relay_on(_r, reasonStr);
        return 1;
      }
    };


    uint get_lschm_mode_bit(uint curr_hour, uint mode) {
      uint l_loop = 24 - curr_hour;
      uint l_p0 = (l_loop - 1) & 0xFFF;
      uint l_p1 = 1 << l_p0;
      uint l_p2;

      if (mode < MAX_LSCHEME_MODES) {
        l_p2 = lamp_modes[mode - 1] & l_p1;
      }
      else {
        // l_p2=WetManage.custom_scheme & l_p1;
      }
      if (l_p2 == 0) return 0;
      else return 1;
    };


    int is_day2(int _schm_num) {
      if (!pub->is_time_synced()) return -1;
      if (_schm_num < 0 || _schm_num > 100) return -1;
      int h = hour();
      debug("IS_DAY","lschm="+String(_schm_num)+", hour="+String(h));
      if (_schm_num == 99){
          debug("IS_DAY","on_bit="+String(_s->custom_scheme1[h]));
          return _s->custom_scheme1[h];
      } 
      if (_schm_num == 100) return _s->custom_scheme2[h];
      return get_lschm_mode_bit(h, _schm_num);
    }

    int is_day(int _schm_num) {

      int h = hour();

      if(!pub->is_time_synced() && _schm_num==2) {  //always on sheme (2), can be without time sync
        return -2;
      }
      if(!pub->is_time_synced() && _schm_num==1) return -3; //always off sheme (1), can be without time sync
      if (!pub->is_time_synced()) return -1;
      
      if (_schm_num < 0 || _schm_num > 100) return -1;
      
      debug("IS_DAY","lschm="+String(_schm_num)+", hour="+String(h));
      if (_schm_num == 99){
          //_s->cb_schm1=0B000000000011111111111110;
          bool br=bitRead(_s->cb_schm1,23-h); //read bit of hour in custom scheme
          debug("IS_DAY","hour="+String(h)+", on_bit="+String(br));
          return br;
      } 
      if (_schm_num == 100){
            return bitRead(_s->cb_schm2,23-h);
      } 
      return get_lschm_mode_bit(h, _schm_num);
    }

   

    int aofs_off(DRelay* _r, int _aofs) {
      if (_aofs < 0) _aofs = 0; //wrong aofs ??
      if (_aofs == 0) return -1; //aofs=0 - nothing to do;

      if (_r->is_off()) return 0; //nothing to do

      if ((millis() - _r->get_start_ms()) > _aofs * 1000) {
        debug(_r->get_nameStr() + ":AOFS", "aofs is out, turn off");
        relay_off(_r, "aofs,off");
        return 1;
      } else {
        //debug("AOFS","aofs is going now, nothing to do");
        return 0;
      }
      return 0;
    };

    int aofh_off(DRelay* _r, int _aofh) {
      if (_r->is_off()) return 0; //nothing to do

      if (_aofh < 0) _aofh = 0; //wrong aofs ??
      if (_aofh == 0) return -1; //aofs=0 - nothing to do;

      String debugSrcStr = "AOFH<" + String(_r->get_num()) + ">";
      if (_r->get_hours_working() >= _aofh) {
        debug(_r->get_nameStr() + ":AOFH", "auto off hours is out, turn off");
        relay_off(_r, "aofh,off");
        return 1;
      }
      debug(_r->get_nameStr() + ":AOFH", "auto off hours, nothing to do, h=" + String(_r->get_hours_working()));
      return 0;
    };

    int lschm_on_off(DRelay* _r, int _schm_num) {
      if (_schm_num == 0) return -1; //error parameter

      int on_bit = is_day(_schm_num);
      uint hour_curr = hour();

       String debugSrcStr = "LSCHM<" + String(_r->get_num()) + ">";


      //debug(debugSrcStr, "just_synced="+String(m_just_synced));

      if(m_just_synced==1 && _schm_num==2){
        
        _r->set_hour_lschm_on(hour_curr);
         debug(debugSrcStr, "JUST SYNCED, set lschm_hour="+String(hour_curr));
      }

     
      debug(debugSrcStr, "on_bit=" + String(on_bit) + "; schm_num=" + String(_schm_num));

      if (on_bit == -1 ) { //not synced
        if (_r->is_on()) relay_off(_r, "lschm, no time sync"); //shm_num2 is always on 
        return -1;
      }

      //********  must be on  ************************
      if (on_bit == 1 || on_bit ==-2 ) {
        if (_r->is_on()) {
          if (_r->get_last_hour_when_on() != hour_curr) {
            _r->set_hour_lschm_on(hour_curr);
            debug(debugSrcStr, "set turn_on hour");
          }
          debug(debugSrcStr, "should be on and is on, nothing to do, h=" + String(hour_curr) + "; relay_h=" + String(_r->get_last_hour_when_on()));
          return 0;
        }
        //relay is off, can we turn on ?
        if (_r->is_off() && _r->get_last_hour_when_on() != hour_curr) {
          debug(debugSrcStr, "turn on, h=" + String(hour_curr) + "; relay_h=" + String(_r->get_last_hour_when_on()));
          _r->set_hour_lschm_on(hour_curr);
          relay_on(_r, "lschm,on");
          return 2;
        }
        else {
          debug(debugSrcStr, "already was on in this hour, h=" + String(hour_curr) + "; relay_h=" + String(_r->get_last_hour_when_on()));
          return 0;
        }

       
      }

      //********  must be off  ************************
      if (on_bit == 0 || on_bit == -3) { //must off
        if (relay_off(_r, "lschm,off")) {
          debug(debugSrcStr, "turn off !");
          _r->reset_lschm_hour();
          return 1;
        } else {
          debug(debugSrcStr, "shuould be off and is off, nothing to do");
          _r->reset_lschm_hour();
          return 0;
        }

      }

    };

    int get_temp_settings_ver1(int _schm_num) {
      if (is_day(_schm_num) == 0)  return _s->night_temp_level;
      if (is_day(_schm_num) == 1)  return _s->day_temp_level;
      return _s->default_temp_level;
    };

    int get_temp_settings_ver2(int _schm_num) {
      if(!pub->is_time_synced()) return _s->default_temp_level;
      
      uint h=hour();
      return _s->temp_matrix[h];
    };

     int hotter1(DSensor* _sensor, DRelay* _r){
       int hotter_settings_temp;
       long sensor_temp;
       long delta;

      if(_s->hotter!=1 && _s->hotter!=2) return -1;
       
      if(!_sensor->is_started_and_ready()){
        debug("HOTTER", "sensor not ready");
        if(_r->is_on()) relay_off(_r,"hotter, sensor not ready");
        return -1;
      }
      
     
      sensor_temp=_sensor->get_val();
      if(_s->hotter==1) hotter_settings_temp=get_temp_settings_ver1(_s->schm_onoff_num1);
      if(_s->hotter==2) hotter_settings_temp=get_temp_settings_ver2(_s->schm_onoff_num1);
      delta=_s->level_delta;

      if (delta<50) delta=delta*10; //old value in eeprom

      if(hotter_settings_temp <100) hotter_settings_temp=hotter_settings_temp*100; //old value in eeprom (in C)

      debug("HOTTER","sensor_t="+String(sensor_temp)+"; settings_temp="+String(hotter_settings_temp)+"; delta="+String(delta));

       //temp bad 
       
       if(sensor_temp<hotter_settings_temp){
         if(_r->is_off()){ 
           relay_on(_r,"hotter");
           //publish_relay_status(_r);
           debug ("HOTTER", "turn on");
           return 2;
         }
         debug("HOTTER","already turned on, nothing to do");     
         return 0;      
       }

    
     //temp ok;
     debug("HOTTER","temp_ok");

     if(_r->is_off()){
      debug("HOTTER","nothing to do");
      return 0;
     }

     //relay is on

     if(sensor_temp>=hotter_settings_temp+delta){
      if(_r->is_on()) relay_off(_r,"hotter");
      //publish_relay_status(_r);
      debug("HOTTER","temp+delta ok, turn off");
      return 1;
     }
      debug("HOTTER","delta is small, still hot");
      return 0;
};

int hotter2(DSensor* _sensor, DRelay* _r){
   if(!_s->hotter==2) return -1;

};

int cooler(DSensor* _sensor, DRelay* _r){

      if(!_s->cooler) return -1;
       
      if(!_sensor->is_started_and_ready()){
        debug("COOLER","sensor not ready");
        if(_r->is_on()) relay_off(_r,"cooler, sensor err");
        return -1;
      }
      
     
      long sensor_temp=_sensor->get_val();
      int hotter_settings_temp=get_temp_settings_ver1(_s->schm_onoff_num1);
      int delta=_s->level_delta;

      if (delta<50) delta=delta*10; //old value in eeprom

      if(hotter_settings_temp <100) hotter_settings_temp=hotter_settings_temp*100; //old value in eeprom (in C)

      debug("COOLER","sensor_t="+String(sensor_temp)+"; settings_temp="+String(hotter_settings_temp)+"; delta="+String(delta));

       //temp bad 
       
       if(sensor_temp>hotter_settings_temp){
         if(_r->is_off()){ 
           relay_on(_r,"cooler");
           //publish_relay_status(_r);
           debug ("COOLER", "turn on");
           return 2;
         }
         debug("COOLER","already turned on, nothing to do");     
         return 0;      
       }

    
     //temp ok;
     debug("COOLER", "temp_ok");

     if(_r->is_off()){
      debug("COOLER","nothing to do");
      return 0;
     }

     //relay is on

     if(sensor_temp<=hotter_settings_temp-delta){
      if(_r->is_on()) relay_off(_r,"cooler");
      //publish_relay_status(_r);
      debug("COOLER","temp-delta ok, turn off");
      return 1;
     }
      debug("COOLER","delta is small, still cooling");
      return 0;
};



  


};





#endif