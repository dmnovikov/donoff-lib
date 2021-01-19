#ifndef donoffsensordhtxx_h
#define donoffsensordhtxx_h

#include <sensors/donoff-multisensor.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#ifdef D_MQTT
    const char DHT_HUMIDITY_OUT_CHANNEL[]="/out/sensors/dht_humidity";
#endif

//#define DHT_TYPE    DHT11     // DHT 11
#define DHT_TYPE    DHT22     // DHT 22 (AM2302)
//#define DHT_TYPE    DHT21     // DHT 21 (AM2301)

class DHTSensorHumidity: public DSensor{
  public:
    
  DHT_Unified* pdht;

  public:
    DHTSensorHumidity(WMSettings* __s, DHT_Unified* __pdht) : DSensor(__s) {
      pdht=__pdht;
    };

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
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

    long virtual get_long_from_sensor(){
      sensors_event_t event;
      pdht->humidity().getEvent(&event);
      if (isnan(event.relative_humidity)){
       debug("DHT","Error reading humidity!");
       return NO_SENSOR_VAL;
      }
      float raw=event.relative_humidity;
      debug("DHT", String(raw));
      return long (MULTIPLIER*raw);
    };
         
  };


class DHTSensor: public DMultiSensor {
  protected:
    
  public:
    DHT_Unified m_dht;
    DHTSensorHumidity* dht_humidity;
    
  public:
    DHTSensor(WMSettings* __s, int _pin1): DMultiSensor(__s), m_dht(_pin1, DHT_TYPE) {};

    int virtual init(String _name, String _chname, int _type, Queue<sensor_state> *_que_sensor_states) {

      DMultiSensor::init(_name, _chname, _type, _que_sensor_states);

      dht_humidity= new DHTSensorHumidity(_s, &m_dht);
      dht_humidity->init("DHT_HUMIDITY", DHT_HUMIDITY_OUT_CHANNEL, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_HUMIDITY, JSON_CHANNEL_NO);

      debug("INIT_DHT", "type="+String(_type));

        /***************/
      debug(nameStr, "init ok");

    };

    virtual String multi_json_Str(){
      DynamicJsonBuffer jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      String json_str;
      root["timestamp"] = s_get_timestamp();
      root["humidity"] = dht_humidity->get_val_Str();
      root.printTo(json_str);
      return json_str;
      
    };


};




#endif