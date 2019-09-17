#ifndef donoffsensor_h
#define donoffsensor_h

#include <Queue.h>


class DSensor;

typedef struct{
  sensor_states curr_state=UNDEF_STATE;
  sensor_states prev_state=UNDEF_STATE;
  long val;
  long level;
  ulong time;
  bool changed=0;
  bool notifyed=0;
  DSensor * ps;
} sensor_state;

class DSensor: public DBase {
  protected:
    
    sensor_data sens;
    // default sensor params

    String nameStr = "unknown";
    String channelStr="unknown";
    int id;
    
    long  NO_SENSOR_VAL = -12700;
    long NOT_READY_VAL = -12800;
    long  START_DELAY = 0;
    int NEED_ASKING = 0;
    int NEED_FILTERED = 0;
    int REQUEST_CIRCLE = 750;
    int MULTIPLIER = 1;


    ulong low_start_ms = 0;
    ulong high_start_ms = 0;
    ulong alarm_start_ms = 0;
    unsigned long start_request_ms = 0;


    bool init_ok = 0;
    uint gather_counter = 0;

    int wait_asking = 0;

    int sensor_ready = 0;
    int started_ok = 0;

    //int sensor_not_ready_millis=10000; //10 sec

    Queue<sensor_state> *que_states;

    sensor_state state;
    int state_changed=0;

  public:
    DSensor(WMSettings * __s): DBase(__s) {};

    long virtual get_long_from_sensor() {};

    int virtual req_sensor() {};

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states){
      que_states=_que_sensor_states;
      nameStr=_name;
      channelStr=_chname;
      NEED_FILTERED=_filtered;

      sens.v[0] = NOT_READY_VAL;
      sens.v[1] = NOT_READY_VAL;
      sens.v[2] = NOT_READY_VAL;
      sens.val =  NOT_READY_VAL;

      init_ok = 1;
    };

    int virtual fake_val(long _val) {
      return 0;
    };

    int virtual nosensor_val(long val){
      if (val==NO_SENSOR_VAL) return 1;
      return 0;
    };

    String get_channelStr(){
        return channelStr;
    };

    int virtual sensor_loop() {
      if (init_ok == 0) {
        debug(nameStr, "No init, no sensor loop");
        return 0;
      }
      //debug("SENSOR", String(millis()));
      if (millis() < START_DELAY) {
        debug(nameStr, "Waiting to start asking sensor");
        return 0;
      } else {
         started_ok = 1;
      }

      if (NEED_ASKING) {
        //debug("SENSOR", "REQUEST SENSOR");
        if (start_request_ms == 0) {
          req_sensor();
          start_request_ms = millis();
          return 0;
        }

        if (millis() - start_request_ms < REQUEST_CIRCLE ) {
          //wait for sensor will be ready
          debug(nameStr, "Wait for sensor after asking");
          return 0;
        }
      }

      if (NEED_FILTERED) {
        debug(nameStr, "Lets calculate filtered");
        calculate_filtered();
        start_request_ms = 0;
      } else {
        debug(nameStr, "Lets calculate");
        calculate_value();
        start_request_ms = 0;
      }

    };


    int is_started_and_ready() {
      if (started_ok == 0) return 0;
      if (sensor_ready == 0) return 0;
      return 1;
    };

    int is_started() {
      if (started_ok == 0) return 0;
      return 1;
    };

    int is_ready() {
      if (sensor_ready == 0) return 0;
      return 1;
    };

    long virtual calculate_middle() {
      if ((sens.v[0] <= sens.v[1]) && (sens.v[0] <= sens.v[2])) {
        sens.val = (sens.v[1] <= sens.v[2]) ? sens.v[1] : sens.v[2];
      }
      else {
        if ((sens.v[1] <= sens.v[0]) && (sens.v[1] <= sens.v[2])) {
          sens.val = (sens.v[0] <= sens.v[2]) ? sens.v[0] : sens.v[2];
        }
        else {
          sens.val = (sens.v[0] <= sens.v[1]) ? sens.v[0] : sens.v[1];
        }
      }
      return 1;
    };

    int virtual calculate_value() {
      long test_val = get_long_from_sensor();

      if (!nosensor_val(sens.val) && !fake_val(sens.val)) {
        sens.val = test_val;
        sensor_ready = 1;
        return 1;
      } else {
        sens.val = test_val;
        sensor_ready = 0;
        return 0;
      }
    };

    int virtual calculate_filtered() {
      long fl_v = get_long_from_sensor();
      sens.v[gather_counter] = fl_v;
      debug(nameStr, "get values, sensor=" + String(fl_v) + "; counter=" + String(gather_counter) + "; sens=" + String(sens.v[0]) + ":" + String(sens.v[1]) + ":" + String(sens.v[2]));
      gather_counter++;

      if (gather_counter == 3) {
        calculate_middle();
        gather_counter = 0;
      }

      if (!nosensor_val(sens.val)  && sens.val != NOT_READY_VAL && !fake_val(sens.val)) {
        sensor_ready = 1;
        return 1;
      } else {
        sensor_ready = 0;
        return 0;
      }
    };

    int virtual sensor_check_state(int low_level=-128, int high_level=-128){
      int state_flg=0;

      if(!is_started() || sens.val==NOT_READY_VAL || fake_val(sens.val) ){
        debug("NOTIFY-->"+nameStr, "WAIT FOR SENSOR");
        return 0;
      }

      debug("NOTIFY-->"+nameStr, "check low="+String(low_level)+","+"high="+String(high_level)+", sens.val="+String(sens.val));
      debug("NOTIFY-->"+nameStr, "curr state="+String(state.curr_state)+","+"prev_state="+String(state.prev_state));

      if(state.curr_state==LOW_STATE && low_level==-128) state.curr_state=NORM_STATE;
      if(state.curr_state==HIGH_STATE && high_level==-128) state.curr_state=NORM_STATE;

       if(sens.val==NO_SENSOR_VAL){
        state.level=high_level;    
        state.curr_state=ERR_STATE;
        state_flg=1;
      }
      
      
      else if(low_level!=-128 && sens.val<low_level&& state.curr_state!=HIGH_STATE){    
        state.level=low_level;
        state.curr_state=LOW_STATE;  
        state_flg=1;
      }
      
      else if(high_level!=-128 && sens.val>high_level && state.curr_state!=LOW_STATE){    
        state.level=high_level;
        state.curr_state=HIGH_STATE;   
        state_flg=1;

      }else{
        state.level=high_level;
        state.curr_state=NORM_STATE;   
        state_flg=1;
      }

      if(state.curr_state!=NORM_STATE){
         if(state.curr_state!=state.prev_state){  
             state.ps=this;
             state.time=millis();
             state.val=sens.val;
             //debug("NOTIFYSENSOR-->"+nameStr, "Push to sensor que");
             if(que_states->count()<3){
                debug("NOTIFYSENSOR-->"+nameStr, "Push to sensor que");
                que_states->push(state); 
                
              }else{
                debug("NOTIFYER", "TOO MORE PUSHS, SKIP");
              }         
        }
        
      }

      state.prev_state=state.curr_state;
/*
      if(state_flg==1){               
       if(state.curr_state!=state.prev_state){  
             state.ps=this;
             state.time=millis();
             state.val=sens.val;
             //debug("NOTIFYSENSOR-->"+nameStr, "Push to sensor que");
             if(que_states->count()<3){
                debug("NOTIFYSENSOR-->"+nameStr, "Push to sensor que");
                que_states->push(state); 
                
              }else{
                debug("NOTIFYER", "TOO MORE PUSHS, SKIP");
              }         
        }
        state.prev_state=state.curr_state;

     }else{
        state.curr_state=NORM_STATE;
        state.time=millis();
        state.prev_state=state.curr_state;
     }
*/
     return 1;
           
    };

/*
    int publish_loop(DPublisher *_pub) {
      _pub->publish_sensor("8", String(sens.val) );
    };

*/

/*
    int is_low_level(int _level) {
      if (!is_started_and_ready()) {
        debug(nameStr, "sensor not ready");
        return -1;
      }

      if (_level < 100) _level = _level * 100; //old value in eeprom (in C)

      if (get_val() > _level) {
        return 0;
      }

      return 1;
    };

    int is_high_level(int _level) {
      if (!is_started_and_ready()) {
        debug(nameStr, "sensor not ready");
        return -1;
      }

      if (_level < 100) _level = _level * 100; //old value in eeprom (in C)

      if (get_val() < _level) {
        return 0;
      }

      return 1;
    };

    */

    String virtual  get_val_Str() {
      return String(sens.val);
    };

    String get_nameStr() {
      return nameStr;
    };

    void set_name_str(String _str) {
      nameStr = _str;
    };

    bool is_low_state() {
      if (low_start_ms == 0) return 0;
      else return 1;
    };

    void start_low_state() {
      low_start_ms = millis();
    };

    void zero_low_state() {
      low_start_ms = 0;
    };

    bool is_high_state() {
      if (high_start_ms == 0) return 0;
      else return 1;
    };
    void start_high_state() {
      high_start_ms = millis();
    };
    void zero_high_state() {
      high_start_ms = 0;
    };

    long get_val() {
      return sens.val;
    };

};

#endif
