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

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type=1, int _need_baselog=0) {
      
      

      DSensor::init(_name,_chname, _filtered, _que_sensor_states, _type);
        
      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 10000;
      NEED_ASKING = 1;
      NEED_FILTERED = _filtered;
      REQUEST_CIRCLE = 750;
      MULTIPLIER = 100;
      NEED_BASELOG=_need_baselog;

     

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