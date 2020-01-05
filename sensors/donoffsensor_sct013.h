#ifndef donoffsensorsct013_h
#define donoffsensorsct013_h

#include <sensors/donoffsensor.h>
#include <EmonLib.h>



class SCT013Sensor: public DSensor {
  public:
    EnergyMonitor emon1;
    float Irms = 0;
    int SENSOR_PIN;

  public:
    SCT013Sensor(WMSettings* __s, int _pin): DSensor(__s) {
      SENSOR_PIN=_pin;
    };

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_baselog) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_baselog);

      START_DELAY = 30000;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 100;
      NEED_ASK_WHILE_WATING=1;
      
      debug("INITDCT013", "type="+String(_type));

      emon1.current(SENSOR_PIN, 31.24);

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw = emon1.calcIrms(1480);
      //debug("SCT013", "RAW="+String(raw));
      return long (raw * MULTIPLIER);
    };


};

#endif