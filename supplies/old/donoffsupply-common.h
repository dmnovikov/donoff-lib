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
#endif

#include <Queue.h>
#include <donoffbutton.h>
#include <donoffnotifyer.h>
#include <sensors/donoffsensor_ds1820.h>
#include <sensors/donoffsensor_sct013.h>

#if !defined (RELAY1)
    #define RELAY1 1
#endif

#if !defined (RELAY2)
    #define RELAY2 0
#endif

#if !defined (DS1820_INT)
    #define DS1820_INT 1
#endif

#if !defined(DS1820_OUT)
    #define DS1820_OUT 1
#endif

#if !defined(DDISPLAY)
    #define DDISPLAY 1
#endif

#if !defined(DBUTTON)
    #define DBUTTON 1
#endif

#if !defined(NOTIFYER)
    #define NOTIFYER 0
#endif

#if !defined(DCONFIG)
    #define DCONFIG 1
#endif

#if !defined(SCT013_1)
    #define SCT013_1 0
#endif


#define DS1820_NOT_FILTERED 0
#define DS1820_FILTERED 1


class DSupply: public DBase {
  private:
    String reasonStr = "";
    uint8_t  blink_loop = 0;
    uint current_blink_type = 0;

    int numrelays = 0;
    int init_ok = 0;
    int mycounter = 0;
    ulong mytimer = 0;
    
 protected:
    Queue<pub_events>* que_wanted;
    Queue<sensor_state> * que_sensor_states;

    DRelay *r1;
    DRelay* r2;
    DigitalDS1820Sensor* ds_in;
    DigitalDS1820Sensor* ds_out;
    SCT013Sensor* sct013_1;
    DDisplay* D;
    DPublisher* pub;
    DButton *b1;
    DNotifyer *notifyer;
    DConfigMQTT *conf;


  public:
    DSupply(WMSettings * __s): DBase(__s) {};

    // int add_relay(){

    // };

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

      pub = _pub;
      notifyer = _notifyer;
      que_wanted=_q;
      que_sensor_states = new Queue<sensor_state>(10);
      debug("SUPPLY_INIT", "** Start SUPPLY INIT");

      if (DDISPLAY) {
        D = new DDisplay();
        D->init();
      }
      if (RELAY1) {
        numrelays++;
        debug("SUPPLY_INIT", "R1 INIT");
        r1 = new DRelay(_s);
        r1->init(RELAY1_PIN, "r1", String(RELAY1_ONOFF_CHANNEL), String(RELAY1_ONTIME_CHANNEL), String(RELAY1_DOWNTIME_CHANNEL));
        //debug("SUPPLYINIT", "r1name="+r1->get_nameStr());
      }
      if (RELAY2) {
        numrelays++;
        debug("SUPPLY_INIT", "R2 INIT");
        r2 = new DRelay(_s);
        r2->init(RELAY2_PIN, "r2", String(RELAY2_ONOFF_CHANNEL));
      }

      if (DS1820_INT) {
        debug("SUPPLY_INIT", "DS_IN INIT");
        ds_in = new DigitalDS1820Sensor(_s, IN_WIRE_BUS);
        ds_in->init("DS_IN", DS_IN_CHANNEL, DS1820_NOT_FILTERED,que_sensor_states);
      }

      if (DS1820_OUT) {
        debug("SUPPLY_INIT", "DS_OUT INIT");
        ds_out = new DigitalDS1820Sensor(_s, OUT_WIRE_BUS);
        ds_out->init("DS_OUT", DS_OUT_CHANNEL, DS1820_FILTERED,que_sensor_states);
      }

      if (SCT013_1) {
        debug("SUPPLY_INIT", "SCT013_1 INIT");
        sct013_1 = new SCT013Sensor(_s);
        sct013_1->init("SCT013_1", SCT013_OUT_CHANNEL, 0,que_sensor_states);
      }

      if (DBUTTON) {
        debug("SUPPLY_INIT", "DBUTTOM INIT");
        b1 = new DButton(_s);
        b1->init();
      }

      if(DCONFIG){
        debug("SUPPLY_INIT", "CONFIG INIT");
        conf = new DConfigMQTT(_s);
        conf->init();
      }

      debug("SUPPLY_INIT", "** Finish SUPPLY INIT");
      mytimer = millis();
      init_ok = 1;

    };

    int supply_loop() {

      if ((millis() - mytimer) >= 300 ) {
        //6 sec loop
        if (mycounter == 0) {
          //debug("SUPPLY_LOOP", "Reconnect loop");
          reconnect_loop();
        }

        if (mycounter == 1) {
          //debug("SUPPLY_LOOP", "Display loop");
          display_loop();
        }
        if (mycounter == 2) {
          //debug("SUPPLY_LOOP", "Sync loop");
          sync_blink_mode();
        }


        if (mycounter == 5)  {
          //debug("SUPPLY_LOOP", "Sensor loop #1");
          sensors_loop(1);
        }
        if (mycounter == 6) {
          //debug("SUPPLY_LOOP", "Sensor loop #2");
          sensors_loop(2);
        } 
        if (mycounter == 7){
           //debug("SUPPLY_LOOP", "Sensor loop #3");
           sensors_loop(3);
        } 
        
        if (mycounter == 8)  sensors_loop(4);
        if (mycounter == 9)  sensors_loop(5);
        if (mycounter == 10) sensors_loop(6);


        if (mycounter == 15) service_loop();
        if (mycounter == 16) hotter_cooler_loop();
        if (mycounter == 17) lschm_loop();
        if (mycounter == 18) aofh_loop();

        if (mycounter==20){
          if(_s->notifyer){
            //debug("SUPPLY_LOOP", "Notify loop");
            notify_sesnors_loop();
            notify_relay_hours_loop();
          } 
        }
        mycounter++;
        //300 ms loop
        aofs_loop();
        pub_wanted_loop();
       
        //
        if (mycounter > 25) mycounter = 0;
        mytimer = millis();

      }


      if (DBUTTON) {
        // debug("SUPPLY_LOOP", "Button loop");
        int result = b1->button_loop(); //return button status
        if (result == SHORT_PRESS){
           debug("SUPPLY_DBUTTON", "Toggle relay");
           if(_s->hotter==1) _s->hotter=0;
           if(_s->cooler==1) _s->cooler=0;
           if(_s->lscheme_num>0) _s->lscheme_num=0;
           if(_s->autostop_sec>0) _s->autostop_sec=0;
           if(RELAY1) relay_toggle(r1, "hardware"); //if SHORT_PRESS, TOGGLE
        }

        if (result == CONFIG_PRESS){
           debug("SUPPLY_DBUTTON", "Enter Config Mode...");
          //  WiFiManager wifiManager;
          //  if (!wifiManager.startConfigPortal("OnDemandAP")) {
          //       Serial.println("failed to connect and hit timeout");
          //       delay(3000);
          //       //reset and try again, or maybe put it to deep sleep
          //       ESP.reset();
          //       delay(5000);
          //   }
          set_blink(BL_CONNECTING);
          conf->config();

        }
      }

    };

    int notify_sesnors_loop(){
      
      if (que_sensor_states->count()==0) return 0;
      //debug("QUE_SENS", String(que_sensor_states->count()) );
      while(que_sensor_states->count()!=0){
         sensor_state state=que_sensor_states->pop();
         //debug("QUE_SENS", state.ps->get_nameStr()+":"+String(state.val)+":"+String(state.curr_state)+":"+String(state.prev_state));
         notifyer->notify_sensor_state(&state);
      }
      
    };

     int notify_relay_hours_loop(){
      if(RELAY1 && r1->is_on()){
        if(r1->get_hours_working()>_s->hours_on_notify && r1->is_notifyed_h()==0){
          if(notifyer->notify_working_hours(r1)) r1->set_notifyed_h(1) ;
        }     
      }
      
    };

    int pub_wanted_loop(){
      //debug("SUPPLY_QUEUE", "Loop queue wanted");

      if (que_wanted->count()==0) return 0;

      debug("SUPPLY_QUEUE", "WANTED EVENT DETECTED");
      pub_events what_to_want=que_wanted->pop();

      if(what_to_want==PUBLISHER_WANT_SAVE){
        save();
        pub->publish_to_info_topic("N: saved");
      }
       
      if(what_to_want==PUBLISHER_WANT_R1_ON) {
        if(RELAY1) relay_on(r1,"from publisher");
        else pub->publish_to_info_topic("I:NO RELAY1");
      }

      if(what_to_want==PUBLISHER_WANT_R1_OFF) {
        if(RELAY1) relay_off(r1,"from publisher");
        else pub->publish_to_info_topic("I:NO RELAY1");
      }

      if(RELAY1 && what_to_want==PUBLISHER_WANT_R1_OFF_LSCHM0) {
        relay_off(r1,"lschm=0");
      }

      if(RELAY2 && what_to_want==PUBLISHER_WANT_R2_OFF_LSCHM0) {
        relay_off(r2,"lschm=0");
      }
     
       if(what_to_want==PUBLISHER_WANT_R2_ON) {
        if(RELAY2) relay_on(r2,"from publisher");
        else pub->publish_to_info_topic("I:NO RELAY2");
      }

      if(what_to_want==PUBLISHER_WANT_R2_OFF) {
        if(RELAY2) relay_off(r2,"from publisher");
        else pub->publish_to_info_topic("I:NO RELAY2");
      }
      
      if(what_to_want==PUBLISHER_WANT_RESET) {
        pub->publish_to_info_topic("I: OK, LET'S RESET");
        ESP.reset();
      }

      if(RELAY1 && what_to_want==PUBLISHER_WANT_RESET_HOUR_R1) {
        r1->reset_lschm_hour();
      }

      if(RELAY2 && what_to_want==PUBLISHER_WANT_RESET_HOUR_R2) {
        r2->reset_lschm_hour();
      }

      
       if(RELAY1 && what_to_want==PUBLISHER_WANT_RESET_HOUR_R1_M) {
        r1->reset_lschm_hour();
        pub->publish_to_info_topic("I: lschm last hour is reseted");
      }
      if(RELAY2 && what_to_want==PUBLISHER_WANT_RESET_HOUR_R2_M) {
        r2->reset_lschm_hour();
        pub->publish_to_info_topic("I: lschm last hour is reseted");
      }
      
      if(what_to_want==PUBLISHER_WANT_SH_R1) {
        if(RELAY1){
            if(r1->is_on()) pub->publish_to_info_topic("I:R1=ON");
            else pub->publish_to_info_topic("I:R1=OFF");
        }else{
           pub->publish_to_info_topic("I:R1=DISABLED");
        
        }
      }

      if(what_to_want==PUBLISHER_WANT_SH_R2) {

        if(RELAY2){
            if(r2->is_on()) pub->publish_to_info_topic("I:R2=ON");
            else pub->publish_to_info_topic("I:R2=OFF");
        }else{
           pub->publish_to_info_topic("I:R2=DISABLED");
        }
        
      }
      
    };

    int virtual aofs_loop() {
    
      if (RELAY1 && _s->autostop_sec > 0 && _s->hotter==0 && _s->cooler==0) {
        //debug("AOFS", String(_s->autostop_sec));
        aofs_off(r1, _s->autostop_sec);
      }

      if (RELAY2 && _s->autostop_sec2 > 0) {
        aofs_off(r2, _s->autostop_sec2);
      }

    };

    int virtual aofh_loop() {
      //300 ms loop
      if (RELAY1 && _s->autooff_hours > 0 && _s->lscheme_num == 0 &&  _s->hotter==0 && _s->cooler==0 ) {
        aofh_off(r1, _s->autooff_hours);
      }
      if (RELAY2 && _s->autooff_hours2 > 0 && _s->lscheme_num2 == 0) {
        aofh_off(r2, _s->autooff_hours2);
      }
    };

    int virtual lschm_loop() {
      if (RELAY1 && _s->lscheme_num > 0 &&  _s->hotter==0 && _s->cooler==0) {
        lschm_on_off(r1, _s->lscheme_num);
      }
      if (RELAY2 && _s->lscheme_num2>0) {
        lschm_on_off(r2, _s->lscheme_num2);

      }
    };

    int virtual hotter_cooler_loop() {
      if (_s->hotter) {
        hotter_loop();
      } else if (_s->cooler){
        cooler_loop();
      }
    };

    int virtual hotter_loop(){
      if(RELAY1 && DS1820_OUT) hotter(ds_out, r1);
    };

    int virtual cooler_loop(){
      if(RELAY1 && DS1820_OUT) cooler(ds_out,r1);
    };


    int reconnect_loop() {
      if (!pub->is_connected() || !pub->is_time_synced()) {
        pub->reconnect();
      }
    };

    int virtual display_loop() {
      String outStr;

      if(DDISPLAY && RELAY1){
         if (r1->is_on()) outStr += "r1:on"; else outStr += "r1:off";
        D->show_str(1, outStr, "", 1, 0);
      }

      if (DDISPLAY && DS1820_INT) {
        // D.show_sensor(1, "t:",  ds1820_get_filtered_val(), 100,0,0);
        D->show_sensor(1, "t1:",  ds_in->get_val(), 100, 0, 0);
      }

       if (DDISPLAY && DS1820_OUT) {
        // D.show_sensor(1, "t:",  ds1820_get_filtered_val(), 100,0,0);
        D->show_sensor(1, "t2:",  ds_out->get_val(), 100, 0, 0);
      }
    };

// ************************************************* SENSORS _LOOP *************************************
    int virtual sensors_loop(int sens_num) {
      //debug("SENSOR_LOOP", "sensors loop DSUPPLY->"+String(sens_num));

      if (sens_num == 1) {
        if (DS1820_INT){
          ds_in->sensor_loop();
          debug("DS_IN", ds_in->get_val_Str());
        } 
        if (DS1820_INT && _s->notifyer && NOTIFYER) ds_in->sensor_check_state(-128, ALARM_TEMP1_MAX*100);
        return 1;

      }

      if (sens_num == 2) {
        if (DS1820_OUT){
           ds_out->sensor_loop();
           debug("DS_OUT", ds_out->get_val_Str());
        }
        
        if (DS1820_OUT && _s->notifyer && NOTIFYER){
          long low_level=_s->temp_low_level_notify;
          long high_level=_s->temp_high_level_notify;
          //old format (celsius), new format celsius*100
          if(abs(_s->temp_low_level_notify)<100) low_level=_s->temp_low_level_notify*100;
          if(abs(_s->temp_high_level_notify)<100) high_level=_s->temp_high_level_notify*100;
          ds_out->sensor_check_state(low_level,high_level);
        }
        return 1;
      }

      if (sens_num == 3) {
        if (sct013_1){
           sct013_1->sensor_loop();
           debug("SCT013_OUT", sct013_1->get_val_Str());
        } 
        return 1;
      }
    };

    int virtual service_loop() {

      debug("SHEDULER", "**Service loop->Time=" + String(hour()) + ":" + String(minute()) + " ,t_sync=" + String(pub->is_time_synced())+ 
           ", user="+ String(_s->mqttUser)+", dev_id=" + String(_s->dev_id)+" ,online="+String(pub->is_connected()) 
       );

      if (RELAY1) sync_blink_mode();

      // publish temp sensors
      if (DS1820_INT && pub->is_connected()) pub->publish_sensor(ds_in);
      if (DS1820_OUT && pub->is_connected()) pub->publish_sensor(ds_out);
      if (SCT013_1 && pub->is_connected()) pub->publish_sensor(sct013_1);

      pub->publish_uptime();

      if (RELAY1 && pub->is_connected()) {
        //debug("SERVICE_LOOP", "Public Rrelay1 info");
       
        pub->publish_ontime(r1);
        pub->publish_downtime(r1);
        pub->publish_relay_state(r1);
      }

    };

    int r1_on(String reasonStr = "") {
      relay_on(r1, reasonStr);
      //r1->turn_on();
      //debug("RELAY_ON", r1->get_nameStr());
    };

    int r1_off(String reasonStr = "") {
      relay_off(r1, reasonStr);
      //r1->turn_off();
    };

    int r2_on(String reasonStr = "") {
      relay_on(r2, reasonStr);
    };

    int r2_off(String reasonStr = "") {
      relay_off(r2, reasonStr);
    };

    int is_relay_on(DRelay* _r) {
      return _r->is_on();
    };

    int r1_is_on() {
      if (!RELAY1) return -1;
      return is_relay_on(r1);
    };

    int r2_is_on() {
      if (!RELAY2) return -1;
      return is_relay_on(r2);
    };

    int r1_toggle(String reasonStr = "") {
      if (!RELAY1) return -1;
      relay_toggle(r1, reasonStr);
    };

    int relay_on(DRelay* _r, String reasonStr) {
      if(_r->is_off()){
          _r->turn_on();
            debug("SUPPLY_RELAY_ON", r1->get_nameStr());
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


    void set_blink(uint _current_blink_type ) {
      current_blink_type = _current_blink_type;
    };

    void tick() {
      if (  modes[current_blink_type] & 1 << (blink_loop & 0x07) ) digitalWrite(SONOFF_LED, HIGH);
      else  digitalWrite(SONOFF_LED, LOW);
      blink_loop++;
    };

    void sync_blink_mode() {
      if(!RELAY1){
        if(pub->is_connected()) {
          set_blink(BL_CONNECTED_OFF);
          return;
        } 
        else {
          set_blink(BL_OFFLINE_OFF);
          return;
        } 

      }

      if (pub->is_connected() && r1->is_off() && RELAY1) {
        //debug("--->connected,off");
        set_blink(BL_CONNECTED_OFF);
        return;
      }
      if (pub->is_connected() && r1->is_on() && RELAY1) {
        set_blink(BL_CONNECTED_ON);
        return;
      }
      if (!pub->is_connected() && r1->is_off() && RELAY1) {
        //debug("--->not connected,off");
        set_blink(BL_OFFLINE_OFF);
        return;
      }
      if (!pub->is_connected() && r1->is_on() && RELAY1) {
        set_blink(BL_OFFLINE_ON);
        return;
      }
    };


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

    int hotter(DSensor* _sensor, DRelay* _r){

      if(!_s->hotter) return -1;
       
      if(!_sensor->is_started_and_ready()){
        debug("HOTTER", "sensor not ready");
        if(_r->is_on()) relay_off(_r,"hotter, sensor not ready");
        return -1;
      }
      
     
      long sensor_temp=_sensor->get_val();
      int hotter_settings_temp=get_temp_settings(_s->schm_onoff_num1);
      int delta=_s->level_delta;

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

int cooler(DSensor* _sensor, DRelay* _r){

      if(!_s->cooler) return -1;
       
      if(!_sensor->is_started_and_ready()){
        debug("COOLER","sensor not ready");
        if(_r->is_on()) relay_off(_r,"cooler, sensor err");
        return -1;
      }
      
     
      long sensor_temp=_sensor->get_val();
      int hotter_settings_temp=get_temp_settings(_s->schm_onoff_num1);
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