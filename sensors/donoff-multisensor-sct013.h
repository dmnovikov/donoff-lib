#ifndef donoffmultisensor_h
#define donoffmultisensor_h

#include <Queue.h>
#include <donoffbase.h>
#include <donoffsettings.h>
#include <donoffsensor_sct013.h>

#ifdef D_MQTT
    const char SCT013_1_OUT_CHANNEL[]="/out/sct013_1";
    const char SCT013_2_OUT_CHANNEL[]="/out/sct013_2";
    const char SCT013_3_OUT_CHANNEL[]="/out/sct013_3";
#endif


class DMultiSensorSCT013;

class DMultiSensorSCT013: public DBase {
  protected:

    DSensor* sensors[4]={NULL,NULL,NULL,NULL};
    int num_of_sensors=0;
    long val;
    
    String nameStr = "unknown";
    String channelStr="unknown";
    int id;
    int MULTI_PIN1;
    int MULTI_PIN2;
    
    long  NO_SENSOR_VAL = -12700;
    long NOT_READY_VAL = -12800;
    long  START_DELAY = 0;
    int NEED_ASKING = 0;
    int NEED_FILTERED = 0;
    int REQUEST_CIRCLE = 750;
    int MULTIPLIER = 1;
    int NEED_ASK_WHILE_WATING=0;


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
    DMultiSensorSCT013(WMSettings * __s): DBase(__s) {};

    int virtual init(int _pin1, int _pin2, String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states){
   
      que_states=_que_sensor_states;
      nameStr=_name;
      channelStr=_chname;
      val =  NOT_READY_VAL;
      MULTI_PIN1=_pin1;
      MULTI_PIN2=_pin2;

      sensors[0]= new SCT013Sensor(_s, A0);
      sensors[0]->init("SCT013_1",SCT013_1_OUT_CHANNEL, 0, _que_sensor_states);

      sensors[1]= new SCT013Sensor(_s, A0);
      sensors[1]->init("SCT013_2",SCT013_2_OUT_CHANNEL, 0, _que_sensor_states);

      sensors[2]= new SCT013Sensor(_s, A0);
      sensors[2]->init("SCT013_3",SCT013_3_OUT_CHANNEL, 0, _que_sensor_states);

      num_of_sensors=3;

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

    int virtual multi_sensor_loop(int _current_number) {
      //debug(nameStr, "Sensor_LOOP, ms="+String(millis()));
      if (init_ok == 0) {
        debug(nameStr, "No init, no sensor loop");
        return 0;
      }

      if(_current_number==1){
          digitalWrite(MULTI_PIN1, HIGH);
          sensors[0]->calculate_value();
      }

       if(_current_number==2){
          digitalWrite(MULTI_PIN2, HIGH);
          sensors[1]->calculate_value();
      }

       if(_current_number==3){
          digitalWrite(MULTI_PIN1, HIGH);
          digitalWrite(MULTI_PIN2, HIGH);
          sensors[2]->calculate_value();

      }
      
      if(_current_number==4){
            int ready_c=0;
            
            for(DSensor* _ps: sensors){
                if(_ps->is_ready() && _ps->is_started()) ready_c++;
            }
            
            if(ready_c==3) {
                for(DSensor* _ps: sensors){
                val+=_ps->get_val();
                }
            }else{
                val=NOT_READY_VAL;
            }

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

   
    String virtual  get_val_Str() {
      return String(val);
    };

    String get_nameStr() {
      return nameStr;
    };

    long get_val() {
      return val;
    };

};

#endif
