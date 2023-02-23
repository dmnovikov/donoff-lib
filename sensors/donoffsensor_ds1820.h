#ifndef donoffsensords1820_h
#define donoffsensords1820_h

#include <sensors/donoffsensor.h>

#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>


class DigitalDS1820Sensor: public DSensor {

  private:
    OneWire oneWire;
    DallasTemperature ds_sensor;
    

  public:
    DigitalDS1820Sensor(WMSettings* __s, int _pin): DSensor(__s), oneWire(_pin), ds_sensor(&oneWire) {};

    void virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
    
      

      DSensor::init(_name,_chname, _filtered, _que_sensor_states, _type, _need_json);

      //ds_1820 needs asking before get results 
      NEED_ASKING = 1;
      REQUEST_CIRCLE = 750;
      TYPE=_type;
      need_json=_need_json;
      debug_level=0;

      /* 
      debug("DSINIT", "need json:"+ String(need_json));
      debug("DSINIT", "deebug_level:"+ String(debug_level));
      */


      //25.24C we store as 2524
      MULTIPLIER = 100;

      DeviceAddress tempDeviceAddress;
      ds_sensor.begin();
      ds_sensor.getAddress(tempDeviceAddress, 0);
      //ds_sensor.setResolution(tempDeviceAddress, _res);
      ds_sensor.setWaitForConversion(false);

      debug(nameStr, "init ok");

      /***************/
      
    };

    int  req_sensor() {
      //debug(nameStr, "Req t");
      ds_sensor.requestTemperatures();
      start_request_ms = millis();
      return 1;
    };

    long virtual get_long_from_sensor() {
      float raw = ds_sensor.getTempCByIndex(0);
      //debug(nameStr, "DS1820_RAW:" + String(raw) + "; " + "ms_req=" + String(start_request_ms));
      //debug(nameStr, "DS1820_RAW:" + String(raw));
      return long (raw * MULTIPLIER);
    };

    

    int virtual fake_val(long _val) {
      if (_val == 85) return 1; //this is first "test" val of ds1820, ignore
      return 0;
    };




};

#endif