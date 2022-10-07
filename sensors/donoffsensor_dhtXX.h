#ifndef donoffsensordhtxx_h
#define donoffsensordhtxx_h

#include <sensors/donoff-multisensor.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#ifdef D_MQTT
    const char DHT_HUMIDITY_OUT_CHANNEL[]="/out/sensors/dht_humidity";
    const char DHT_TEMP_OUT_CHANNEL[]="/out/sensors/dht_temp";
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
      debug(nameStr, "HUMIDITY init ok");
    };

    long virtual get_long_from_sensor(){
      sensors_event_t event;
      pdht->humidity().getEvent(&event);
      if (isnan(event.relative_humidity)){
       debug("DHT_HUM", String(NO_SENSOR_VAL));
       return NO_SENSOR_VAL;
      }
      float raw=event.relative_humidity;
      debug("DHT_HUM", String(raw));
      return long (MULTIPLIER*raw);
    };
         
  };


class DHTSensorTemp: public DSensor{
  public:
    
  DHT_Unified* pdht;

  public:
    DHTSensorTemp(WMSettings* __s, DHT_Unified* __pdht) : DSensor(__s) {
      pdht=__pdht;
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
      debug(nameStr, "TEMP init ok");
    };

    long virtual get_long_from_sensor(){
      sensors_event_t event;
      pdht->temperature().getEvent(&event);
      if (isnan(event.relative_humidity)){
       debug("DHT_TEMP",String(NO_SENSOR_VAL));
       return NO_SENSOR_VAL;
      }
      float raw=event.relative_humidity;
      debug("DHT_TEMP", String(raw));
      return long (MULTIPLIER*raw);
    };
         
  };



class DHTSensor: public DMultiSensor {
  protected:
    
  public:
    DHT_Unified m_dht;
    DHTSensorHumidity* dht_humidity;
    DHTSensorTemp* dht_temp;
    
  public:
    DHTSensor(WMSettings* __s, int _pin1): DMultiSensor(__s), m_dht(_pin1, DHT_TYPE) {
      //debug("DHTCONSTRUCTOR", "pin="+String(_pin1)+", type="+String(DHT_TYPE));
    };

    void virtual init(String _name, String _chname, int _type, Queue<sensor_state> *_que_sensor_states) {

      DMultiSensor::init(_name, _chname, _type, _que_sensor_states);

      m_dht.begin(); //start phy sensor

      dht_humidity= new DHTSensorHumidity(_s, &m_dht);
      dht_humidity->init("DHT_HUMIDITY", DHT_HUMIDITY_OUT_CHANNEL, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_HUMIDITY, JSON_CHANNEL_NO);

      dht_temp= new DHTSensorTemp(_s, &m_dht);
      dht_temp->init("DHT_TEMP", DHT_TEMP_OUT_CHANNEL, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_TEMP, JSON_CHANNEL_NO);

        /***************/
      debug(nameStr, "MULTI DHT init ok");

    };

    virtual String multi_json_Str(){
      DynamicJsonBuffer jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      String json_str;
      root["timestamp"] = s_get_timestamp();
      root["humidity"] = dht_humidity->get_val_Str();
      root["temp"] = dht_temp->get_val_Str();
      root.printTo(json_str);
      return json_str;
      
    };


};




#endif