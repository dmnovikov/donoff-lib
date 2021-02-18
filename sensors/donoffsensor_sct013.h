#ifndef donoffsensorsct013_h
#define donoffsensorsct013_h

#include <sensors/donoffsensor.h>
#include <EmonLib.h>



class SCT013Sensor: public DSensor {
  public:
    EnergyMonitor emon1;
    float Irms = 0;
    int SENSOR_PIN;
    int analog_check=0;
    int analog_check_pre=0;
    ulong start_delay_const=0;

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
      start_delay_const=START_DELAY;
     
      
      debug("INITDCT013", "type="+String(_type));

      emon1.current(SENSOR_PIN, 31.24);

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      analog_check_pre=analog_check;
      analog_check=analogRead(SENSOR_PIN); //is sensor present. if n sensor val could be 1024 (not in all versions of shield) 
      debug("SCT013", "ANALOG="+String(analog_check));
      //if ADC_MAX_VAL or near (-1), it means that sensor is absent, hard checker present
      if(analog_check>=MAX_ADC_VAL-1){  
         debug("SCT013", "No Sensor A0="+String(analog_check));
         return NO_SENSOR_VAL;
      }
      if(analog_check_pre>=MAX_ADC_VAL-1){ //olala sensor just appeared, lets wait before get vlues (START PAUSE)
        START_DELAY=start_delay_const+millis();
        return NOT_READY_VAL;
        analog_check_pre=0;
      }
      float raw = emon1.calcIrms(1480);
      //debug("SCT013", "RAW="+String(raw));
      return long (raw * MULTIPLIER);
    };


};

#endif