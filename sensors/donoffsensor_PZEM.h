#ifndef donoffsensor_pzem_h
#define donoffsensor_pzem_h

#include <sensors/donoffsensor.h>
#include <PZEM004Tv30.h>



#ifdef D_MQTT
    const char PZEM_CURRENT_OUT_CHANNEL[]="/out/sensors/pzem_current";
    const char PZEM_POWER_OUT_CHANNEL[]="/out/sensors/pzem_power";
    const char PZEM_ENERGY_OUT_CHANNEL[]="/out/sensors/pzem_energy";
    const char PZEM_VOLTAGE_OUT_CHANNEL[]="/out/sensors/pzem_voltage";
    const char PZEM_JSON_OUT_CHANNEL[]="/out/sensors/pzem_voltage";
#endif


class PZEM004Tv30SensorCurrent: public DSensor{
  public:
    PZEM004Tv30* pzemsensor;

  public:
    PZEM004Tv30SensorCurrent(WMSettings* __s, PZEM004Tv30* __pzemsensor) : DSensor(__s) {
      pzemsensor=__pzemsensor;
    };

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 10000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 100;
      TYPE=_type;
      need_json=_need_json;
      debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw =pzemsensor->current();

      if( !isnan(raw) ){
          return long (raw * MULTIPLIER);
      }

      return NO_SENSOR_VAL;

    };


};

class PZEM004Tv30SensorPower: public DSensor{
  public:
    PZEM004Tv30* pzemsensor;

  public:
    PZEM004Tv30SensorPower(WMSettings* __s, PZEM004Tv30* __pzemsensor) : DSensor(__s) {
      pzemsensor=__pzemsensor;
    };

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 10000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 100;

      debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw =pzemsensor->power();

      if( !isnan(raw) ){
          return long (raw * MULTIPLIER);
      }

      return NO_SENSOR_VAL;

    };


};


class PZEM004Tv30SensorVoltage: public DSensor{
  public:
    PZEM004Tv30* pzemsensor;

  public:
    PZEM004Tv30SensorVoltage(WMSettings* __s, PZEM004Tv30* __pzemsensor) : DSensor(__s) {
      pzemsensor=__pzemsensor;
    };

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 10000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 100;

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw =pzemsensor->voltage();

      if( !isnan(raw) ){
          return long (raw * MULTIPLIER);
      }

      return NO_SENSOR_VAL;
    };


};


class PZEM004Tv30SensorEnergy: public DSensor{
  public:
    PZEM004Tv30* pzemsensor;

  public:
    PZEM004Tv30SensorEnergy(WMSettings* __s, PZEM004Tv30* __pzemsensor) : DSensor(__s) {
      pzemsensor=__pzemsensor;
    };

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 10000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      MULTIPLIER = 100;

      /***************/
       debug(nameStr, "init ok");
    };


    long virtual get_long_from_sensor() {
      float raw =pzemsensor->energy();

      if( !isnan(raw) ){
          return long (raw * MULTIPLIER);
      }

      return NO_SENSOR_VAL;
    };


};


class PZEM004Tv30Sensor: public DBase {
  protected:
    String nameStr="";
    String channelName="";

  public:
    PZEM004Tv30 pzem;
    PZEM004Tv30SensorCurrent* pzem_current;
    PZEM004Tv30SensorPower* pzem_power;
    PZEM004Tv30SensorEnergy* pzem_energy;
    PZEM004Tv30SensorVoltage* pzem_voltage;


  public:
    PZEM004Tv30Sensor(WMSettings* __s, int _pin1, int _pin2): DBase(__s), pzem(_pin1, _pin2) {
    };

    void virtual init(String _name, String _chname, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {

      nameStr=_name;
      channelName=_chname;

      pzem_current= new PZEM004Tv30SensorCurrent(_s, &pzem);
      pzem_current->init("PZEM04_Current", PZEM_CURRENT_OUT_CHANNEL, 0, _que_sensor_states, 11,JSON_CHANNEL_NO);

      pzem_power= new PZEM004Tv30SensorPower(_s, &pzem);
      pzem_power->init("PZEM04_Power", PZEM_POWER_OUT_CHANNEL, 0, _que_sensor_states, 12,JSON_CHANNEL_NO);

      pzem_voltage= new PZEM004Tv30SensorVoltage(_s, &pzem);
      pzem_voltage->init("PZEM04_Voltage", PZEM_VOLTAGE_OUT_CHANNEL, 0, _que_sensor_states, 13,JSON_CHANNEL_NO);

      pzem_energy= new PZEM004Tv30SensorEnergy(_s, &pzem);
      pzem_energy->init("PZEM04_Energy", PZEM_ENERGY_OUT_CHANNEL, 0, _que_sensor_states, 14,JSON_CHANNEL_NO);

        
      debug("INITPZEM004", "type="+String(_type));

        /***************/
      debug(nameStr, "init ok");

    };


};



#endif