#ifndef donofftempodds_h
#define donofftempodds_h

#include <Queue.h>
#include <donoffbase.h>
#include <donoffsettings.h>
#include <sensors/donoffsensor_ds1820.h>

#ifdef D_MQTT
    const char TEMP_ODDS[]="/out/sensors/temp_odds";
#endif


class DTempOddsSensor: public DSensor {

  public:

  DSensor* sensors[2]={NULL,NULL};

  protected:

    int num_of_sensors=0;
    long val;

  public:
    DTempOddsSensor(WMSettings * __s, DSensor* _sensor1, DSensor* _sensor2): DSensor(__s) {
        sensors[0]= _sensor1;
        sensors[1]= _sensor2;
        num_of_sensors=2;
    };

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json){

      DSensor::init(_name,_chname, _filtered, _que_sensor_states, _type, _need_json);
   
      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 0;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      // REQUEST_CIRCLE = 750;
      MULTIPLIER = 100;
      NEED_ASK_WHILE_WATING=0;
      TYPE=_type;

      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);
      init_ok = 1;


    };

    void virtual calculate_value(){
          sens.val=NO_SENSOR_VAL;
          if(sensors[0]->is_started_and_ready() && sensors[1]->is_started_and_ready()){
              sens.val=sensors[1]->get_val()-sensors[0]->get_val();
              //debug("ODD_SENSOR", "VAL="+String(sens.val));
              sensor_ready=1;
          }
    };

};

#endif
