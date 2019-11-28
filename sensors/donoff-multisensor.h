#ifndef donoffmultisensor_h
#define donoffmultisensor_h

#include <Queue.h>
#include <donoffbase.h>
#include <donoffsensor.h>
#include <donoffsettings.h>



class DMultiSensor;

class DMultiSensor: public DBase {
  protected:

    DSensor* sensors[4]={NULL,NULL,NULL,NULL};
    int num_of_sensors=0;
    long val;
    
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
    DMultiSensor(WMSettings * __s): DBase(__s) {};

    long virtual get_long_from_sensor(){};

    int virtual req_sensor() {};

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states){
   
      que_states=_que_sensor_states;
      nameStr=_name;
      channelStr=_chname;
      val =  NOT_READY_VAL;



      init_ok = 1;


    };

    int virtual init_sensors(){
        sensors[0]= new DSenso
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

    int virtual multi_sensor_loop() {
      //debug(nameStr, "Sensor_LOOP, ms="+String(millis()));
      if (init_ok == 0) {
        debug(nameStr, "No init, no sensor loop");
        return 0;
      }

      sensors_calculate_values();

      calculate_value();
      
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
  
    int virtual sensors_calculate_values() {

        for(int i=0; i<=num_of_sensors; i++){
            sensors[i]->calculate_value();
        }
     
    };

    int virtual calculate_value() {

        val=NO_SENSOR_VAL;
        for(int i=0; i<=num_of_sensors; i++){
            val+=sensors[i]->calculate_value();
        }
     
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
