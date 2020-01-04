#ifndef donoffsensor_pzem_h
#define donoffsensor_pzem_h

#include <sensors/donoffsensor.h>
#include <PZEM004Tv30.h>



#ifdef D_MQTT
    const char PZEM_CURRENT_OUT_CHANNEL[]="/out/current";
    const char PZEM_POWER_OUT_CHANNEL[]="/out/power";
    const char PZEM_ENERGY_OUT_CHANNEL[]="/out/energy";
    const char PZEM_VOLTAGE_OUT_CHANNEL[]="/out/voltage";
#endif


class PZEM004Tv30SensorCurrent: public DSensor{
  public:
    PZEM004Tv30* pzemsensor;
    float Irms = 0;


  public:
    PZEM004Tv30SensorCurrent(WMSettings* __s, PZEM004Tv30* __pzemsensor) : DSensor(__s) {
      pzemsensor=__pzemsensor;
    };

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type=11, int _need_baselog=0) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_baselog);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 10000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      // REQUEST_CIRCLE = 750;
      MULTIPLIER = 100;

      debug(nameStr, "type="+String(_type));

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw =pzemsensor->current();

      if( !isnan(raw) ){
          return long (raw * MULTIPLIER);
      }

      return NO_SENSOR_VAL;

      //debug("SCT013", "RAW="+String(raw));
      
    };


};


class PZEM004Tv30Sensor: public DBase {
  protected:
    String nameStr="";
    String channelName="";

  public:
    PZEM004Tv30 pzem;
    PZEM004Tv30SensorCurrent* pzem_current;
    // PZEM004Tv30SensorPower* pzem_power;
    // PZEM004Tv30SensorEnergy* pzem_energy;
    // PZEM004Tv30SensorVoltage* pzem_voltage;


  public:
    PZEM004Tv30Sensor(WMSettings* __s, int _pin1, int _pin2): DBase(__s), pzem(_pin1, _pin2) {
    };

    int virtual init(String _name, String _chname, Queue<sensor_state> *_que_sensor_states, int _type=2, int _need_baselog=0) {

      nameStr=_name;
      channelName=_chname;

      pzem_current= new PZEM004Tv30SensorCurrent(_s, &pzem);
      pzem_current->init("PZEM04_Current", PZEM_CURRENT_OUT_CHANNEL, 0, _que_sensor_states, 11,1);

        
      debug("INITPZEM004", "type="+String(_type));

        /***************/
      debug(nameStr, "init ok");

    };


};



#endif