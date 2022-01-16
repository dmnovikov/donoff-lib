#ifndef donoffsensorads11x5_h
#define donoffsensorads11x5_h

#include <sensors/donoff-multisensor.h>
#include <Adafruit_ADS1015.h>

#ifdef D_MQTT
    const char ADS_OUT_CHANNEL0[]="/out/sensors/ads0";
    const char ADS_OUT_CHANNEL1[]="/out/sensors/ads1";
    const char ADS_OUT_CHANNEL2[]="/out/sensors/ads2";
    const char ADS_OUT_CHANNEL3[]="/out/sensors/ads3";
   
#endif

#define PIN_SDA D7
#define PIN_SCL D2

#define CH1 1
#define CH2 1
#define CH3 1

class ADSSensorCH: public DSensor{
  public:
    
  Adafruit_ADS1115* pads; 
  uint channel;

  public:
    ADSSensorCH(WMSettings* __s, Adafruit_ADS1115* __pads, uint __channel) : DSensor(__s) {
      pads=__pads;
      channel=__channel;
    };

    int virtual init(String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json) {
      
      
      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 5000;
      NEED_ASKING = 0;
      MULTIPLIER = 1;
      TYPE=_type;
      need_json=_need_json;
      debug(nameStr, "ADS"+String(channel)+": init ok");
    };

    long virtual get_long_from_sensor(){
      
      long raw=pads->readADC_SingleEnded(channel);
      
      if (raw>=65535){
       debug("ADS", String(NO_SENSOR_VAL));
       return NO_SENSOR_VAL;
      } 
     
      debug("ADS"+String(channel), String(raw));
      return long (MULTIPLIER*raw);
    };
         
  };


class ADS1115Sensor: public DMultiSensor {
  protected:
    
  public:
    Adafruit_ADS1115 ads; 
    ADSSensorCH* adschannels[4]={NULL,NULL,NULL,NULL};
   
  public:
    ADS1115Sensor(WMSettings* __s): DMultiSensor(__s) {
      //debug("DHTCONSTRUCTOR", "pin="+String(_pin1)+", type="+String(DHT_TYPE));     
    };

    int virtual init(String _name, String _chname, int _type, Queue<sensor_state> *_que_sensor_states) {

      DMultiSensor::init(_name, _chname, _type, _que_sensor_states);

      Wire.begin(PIN_SDA, PIN_SCL);

      ads.begin(); //start phy sensor
      debug("ADS", "ADS _BEGIN - DONE");

      adschannels[0] = new ADSSensorCH(_s,&ads, 0);
      adschannels[0]->init("ADSCHANNEL0", ADS_OUT_CHANNEL0, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_ADSCGANNEL, JSON_CHANNEL_NO);

      if(CH1){
        adschannels[1] = new ADSSensorCH(_s,&ads, 1);
        adschannels[1]->init("ADSCHANNEL1", ADS_OUT_CHANNEL1, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_ADSCGANNEL, JSON_CHANNEL_NO);
      }

      if(CH2){
        adschannels[2] = new ADSSensorCH(_s,&ads, 2);
        adschannels[2]->init("ADSCHANNEL2", ADS_OUT_CHANNEL2, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_ADSCGANNEL, JSON_CHANNEL_NO);
      }

      if(CH3){
        adschannels[3] = new ADSSensorCH(_s,&ads, 3);
        adschannels[3]->init("ADSCHANNEL3", ADS_OUT_CHANNEL3, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_ADSCGANNEL, JSON_CHANNEL_NO);
      }

     /*  adschannels[1] = new ADSSensorCHX(_s, &ads, 1);
      adschannels[1]->init("ADSCHANNEL1", ADS_OUT_CHANNEL1, 0, _que_sensor_states, DONOFF_SENSOR_TYPE_ADSCGANNEL, JSON_CHANNEL_NO); */
        /***************/
      debug(nameStr, "MULTI ADS init ok");

    };

    virtual String multi_json_Str(){
      DynamicJsonBuffer jsonBuffer;
      JsonObject &root = jsonBuffer.createObject();
      String json_str;
      root["timestamp"] = s_get_timestamp();
      root["ch0"] = adschannels[0]->get_val_Str();
      if(CH1) root["ch1"] = adschannels[1]->get_val_Str();
      if(CH1) root["ch2"] = adschannels[2]->get_val_Str();
      if(CH1) root["ch3"] = adschannels[3]->get_val_Str();
      root.printTo(json_str);
      return json_str;
      
    };


};




#endif