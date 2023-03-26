
#ifndef drelay_h
#define drelay_h

#include "Arduino.h"
#include <donoffsettings.h>
#include <donoffcommands.h>
#include <donoffbase.h>

/*******************************   RELAY  ***************************************/
class DRelay: public DBase {

  protected:
    int num = 1;
    uint8_t pin = 0;
    unsigned long start_ms = 0;
    unsigned long stop_ms = 0;
    String nameStr="";
    String channel_onoff_str="";
    String channel_ontime_str="";
    String channel_downtime_str="";
    int init_ok = 0;
    int notifyed_h=0;

  private:
    uint lschm_hour_on = 24; //24 means not set

  public:
    DRelay(WMSettings * __s): DBase(__s) {};

    void init(int _pin, String _nStr, int _num, String _ch_onoffStr,  String _ch_ontimeStr="", String _ch_down_timeStr="") {
      pin = _pin;
      stop_ms = millis();
      start_ms = 0;
      pinMode(pin, OUTPUT);
      nameStr=_nStr;
      channel_onoff_str=_ch_onoffStr;
      channel_ontime_str=_ch_ontimeStr;
      channel_downtime_str=_ch_down_timeStr;
      num=_num;
      init_ok=1;
      //debug("INITR1","name="+nameStr);
       debug("RELAY:"+nameStr, "PIN:"+String(pin));
    };

    int is_notifyed_h(){
      if(notifyed_h==0) return 0;
      return notifyed_h;
    };

    int set_notifyed_h(int v){
      return notifyed_h;
    };

    String get_onoff_channel_str(){return channel_onoff_str;};

    String get_ontime_channel_str(){return channel_ontime_str;};

    String get_downtime_channel_str(){return channel_downtime_str;};

    String get_ontime_str(){
        if(start_ms!=0) return get_time_str(millis()-start_ms);
        return "OFF";
    };

    String get_downtime_str(){
        
        if(start_ms==0) return get_time_str(millis()-stop_ms);
        return "00:00:00";
    };

    bool is_on() {
      if (digitalRead(pin)) {
        if (start_ms == 0) {
          debug("RELAY", "ERROR-RELAY: strt_ms error !!!");
          turn_off();
          return 0;
        }
        return 1;
      }
      return 0;
    };

    bool is_off() {
      bool state = is_on();
      return !state;
    };

    unsigned long turn_on() {
      debug("RELAY", "Let's on, pin="+String(pin));
      digitalWrite(pin, HIGH);
      if (digitalRead(pin) == HIGH) {
        start_ms = millis();
        stop_ms = 0;
      }else{
        debug("RELAY", "Error, Relay didnt start");
      }
      return start_ms;
    };

    unsigned long t_turn_on() {
      if (is_off()) return turn_on();
      else return 0;
    };

    unsigned long turn_off() {
      // debug("Let's off, pin="+String(pin));
      digitalWrite(pin, LOW);
      if (digitalRead(pin) == LOW) {
        start_ms = 0;
        stop_ms = millis();;
      }
      return stop_ms;
    };

    unsigned long t_turn_off() {
      if (is_on()) return turn_off();
      else return 0;
    };

    unsigned long get_start_ms() {
      return start_ms;
    };

    unsigned long get_stop_ms() {
      return stop_ms;
    };
    int get_num() {
      return num;
    };
    void set_num(int _num) {
      num = _num;
    };
    /*
      void debug(String debugStr) {
      Serial.println(debugStr);
      };
    */
    uint8_t get_last_hour_when_on() {
      return lschm_hour_on ;
    };
    void set_hour_lschm_on(int _h) {
      lschm_hour_on = _h ;
    };
    void reset_lschm_hour() {
      debug("RELAY<"+String(num)+">", "reset lschm_hour");
      lschm_hour_on = 24 ;

    };

    unsigned long turn_on_if_didnt_on_this_hour(uint _h) {
      debug("LASTHOUR", String(_h) + "; curr_hour=" + String(lschm_hour_on));
      if (lschm_hour_on  == _h) {
        return 0; //we dont turn on in this our
      }
      else {
        //debug("LSCHMHOUR: change lschm_hour");
        lschm_hour_on = _h;
        return turn_on();
      }

    };

    int get_hours_working() {
      if (start_ms == 0) return 0;
      unsigned long uptime = millis() - start_ms;
      long wt = uptime / 1000;
      long h = wt / 3600;
      return h;
    };

    String get_nameStr(){
        return nameStr;
    };

    String get_name(){
        return nameStr;
    };




};


#endif
