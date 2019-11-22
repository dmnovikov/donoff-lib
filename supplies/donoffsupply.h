#ifndef donoffsupply_h
#define donoffsupply_h

#include <donoffrelay.h>
#include <donoffdisplay.h>
#include <configs/donoffconfig-mqtt.h>
#include <donoffbutton.h>


// #define D_MQTT

#ifdef D_BLYNK
  #include <donoffpublisher_blynk.h>
#endif

#ifdef D_MQTT
  #include <donoffpublisher_mqtt.h>
  #include <notifyers/donoffnotifyer-mqtt-pointer.h>
#endif


#if !defined(DDISPLAY)
    #define DDISPLAY 1
#endif

#include <Queue.h>
#include <donoffbutton.h>

class DSupply: public DBase {
  private:
  
    
 protected:
    String reasonStr = "";
    uint8_t  blink_loop = 0;
    uint current_blink_type = 0;


    int numrelays = 0;
   
    int mycounter = 0;
    ulong mytimer = 0;
    int init_ok = 0;
    pub_events what_to_want;

    Queue<pub_events>* que_wanted;
    Queue<sensor_state> * que_sensor_states;

    DPublisher* pub;
    DNotifyer* notifyer;
    DDisplay* D;



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

      if (DDISPLAY) {
        D = new DDisplay();
        D->init();
      }

      init_ok = 1;

    };

    #define MAX_LOOPS 10
    #define MAX_SENSORS 10

    int supply_loop() {

      if ((millis() - mytimer) >= 300 ) {
        //6 sec loop
        if(mycounter >=0 && mycounter<=MAX_LOOPS){
           //debug("SUPPLY_LOOP", "SLOW LOOP");
           slow_loop(mycounter);
        }

        if(mycounter >MAX_LOOPS && mycounter <=MAX_LOOPS+MAX_SENSORS){
          //debug("SUPPLY_LOOP", "SENSORS LOOP");
          sensors_loop(mycounter-MAX_LOOPS);
        }

        mycounter++;
        //300 ms loop

        fast_loop();
        pub_wanted_loop();
       
        //
        if (mycounter > 25) mycounter = 0;
        mytimer = millis();

      }

      native_loop();

    };

    int virtual slow_loop(int mycounter){
         if (mycounter == 0) {
          //debug("SUPPLY_LOOP", "Reconnect loop");
          reconnect_loop();
        }

         if (mycounter == 2) {
          //debug("SUPPLY_LOOP", "Sync loop");
          sync_blink_mode();
        }

        if (mycounter == 3) {
          service_loop();
        }

        if(mycounter==4){
          notifyer_loop();
        }

        if(mycounter==5){
          display_loop();
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

      do_want_event();
      
    };

    int virtual do_want_event(){
        if(what_to_want==PUBLISHER_WANT_SAVE){
        save();
        pub->publish_to_info_topic("N: saved");
      }
      
      if(what_to_want==PUBLISHER_WANT_RESET) {
        pub->publish_to_info_topic("I: OK, LET'S RESET");
        ESP.reset();
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

      debug("SHEDULER", "**Service loop->Time=" + String(hour()) + ":" + String(minute()) + " ,t_sync=" + String(pub->is_time_synced())+ 
           ", user="+ String(_s->mqttUser)+", dev_id=" + String(_s->dev_id)+" ,online="+String(pub->is_connected()) 
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


    int is_day(int _schm_num) {
      if (!pub->is_time_synced()) return -1;
      if (_schm_num < 0 || _schm_num > 100) return -1;
      int h = hour();
      if (_schm_num == 99) return _s->custom_scheme1[h];
      if (_schm_num == 100) return _s->custom_scheme2[h];
      return get_lschm_mode_bit(h, _schm_num);
    }

    int get_temp_settings(int _schm_num) {
      if (is_day(_schm_num) == 0)  return _s->night_temp_level;
      if (is_day(_schm_num) == 1)  return _s->day_temp_level;
      return _s->critical_temp_level;
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
      debug(debugSrcStr, "on_bit=" + String(on_bit) + "; schm_num=" + String(_schm_num));

      if (on_bit == -1 ) { //not synced
        if (_r->is_on()) relay_off(_r, "lschm, no time sync");
        return -1;
      }

      //********  must be on  ************************
      if (on_bit == 1) {
        if (_r->is_on()) {
          if (_r->get_last_hour_when_on() != hour_curr) {
            _r->set_hour_lschm_on(hour_curr);
            debug(debugSrcStr, "set turn_on hour");
          }
          debug(debugSrcStr, "should be on and is on, nothing to do, h=" + String(hour_curr) + "; relay_h=" + String(_r->get_last_hour_when_on()));
          return 0;
        }
        //relay is off, can we turn on ?
        if (_r->get_last_hour_when_on() != hour_curr) {
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
      if (on_bit == 0) { //must off
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

  


};





#endif