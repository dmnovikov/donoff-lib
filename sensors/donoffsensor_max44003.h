#ifndef donoffsensormax44003_h
#define donoffsensormax44003_h

#include <sensors/donoffsensor.h>
#include <Wire.h>
#include <MAX44009.h>

// define sda, scl for this sensor

#define PIN_SDA D2
#define PIN_SCL D7

class MAX44003Sensor: public DSensor {
  public:
   
    MAX44009 light;
    int error=1;
   
   
  public:
    MAX44003Sensor(WMSettings* __s): DSensor(__s) {
    };

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      START_DELAY = 10000;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 1;
      NEED_ASK_WHILE_WATING=0;
      TYPE=_type;
      need_json=_need_json;

      Wire.begin(PIN_SDA, PIN_SCL);
      if(light.begin()){
          debug(nameStr, "Error init MAX44003 Sensor");
      }else error=0;

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      return error==0 ?  (long) light.get_lux() : (long) NO_SENSOR_VAL;
    };

    String virtual  get_val_Str() {
      return String(get_val());
    };


};

#endif