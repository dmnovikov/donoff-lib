#ifndef donoffsensorsmera_h
#define donoffsensorsmera_h

#include <sensors/donoffsensor.h>
#include <SoftwareSerial.h>



class SCT013Sensor: public DSensor {
  public:
    EnergyMonitor emon1;
    float Irms = 0;
    int SENSOR_PIN;

  public:
    SCT013Sensor(WMSettings* __s, int _pin): DSensor(__s) {
      SENSOR_PIN=_pin;
    };

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      START_DELAY = 30000;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 100;
      NEED_ASK_WHILE_WATING=1;
      TYPE=_type;
      need_json=_need_json;
      
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