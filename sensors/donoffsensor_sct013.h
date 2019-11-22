#ifndef donoffsensorsct013_h
#define donoffsensorsct013_h

#include <sensors/donoffsensor.h>
#include <EmonLib.h>



class SCT013Sensor: public DSensor {
  public:
    EnergyMonitor emon1;
    float Irms = 0;

  public:
    SCT013Sensor(WMSettings* __s): DSensor(__s) {
    };

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states) {
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states);
        
      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 30000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      // REQUEST_CIRCLE = 750;
      MULTIPLIER = 100;
      NEED_ASK_WHILE_WATING=1;

      emon1.current(A0, 31.24);

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw = emon1.calcIrms(1480);
      //debug("SCT013", "RAW="+String(raw));
      return long (raw * MULTIPLIER);
    };

    float get_float_val() {
      return (float)sens.val / (float)MULTIPLIER;
    };

    String virtual  get_val_Str() {
      return String(get_float_val());
    };


};

#endif