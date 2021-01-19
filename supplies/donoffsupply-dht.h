#ifndef donoffsupplydht_h
#define donoffsupplydht_h

#include <supplies/donoffsupply.h>
#include <sensors/donoffsensor_dhtXX.h>



#ifdef D_MQTT
    const char DHT_OUT_CHANNEL[]="/out/sensors/dht_json";
#endif

#define SENSOR_PIN 13 //d7

class DSupplyDHT: public DSupply{

  protected:
    DHTSensor* multi_dht;

  public:
    DSupplyDHT(WMSettings * __s): DSupply(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
      
        DSupply::init(_notifyer, _pub, _q);
        init_ok = 0;
      
        debug("SUPPLY_INIT", "PZEM004 INIT");
        
        multi_dht = new DHTSensor(_s, SENSOR_PIN);
        multi_dht->init("DHT", DHT_OUT_CHANNEL, DONOFF_SENSOR_TYPE_DHT, que_sensor_states);
        
        init_ok=1;
     
    };

   //first sensor loop
   int sensors_loop(int sensor_num){
     
       if(sensor_num==3){
          multi_dht->dht_humidity->sensor_loop();
          debug("DHT_HUMIDITY", multi_dht->dht_humidity->get_val_Str());
       }
     
   };


   //add public sensor and relay_1 status
   int virtual service_loop() {
     

       DSupply::service_loop();

      if (pub->is_connected()) pub->publish_sensor(multi_dht->dht_humidity);

      if (pub->is_connected() && pub->is_time_synced()) pub->publish_multi_sensor(multi_dht);
     
      // DynamicJsonBuffer jsonBuffer;
      // JsonObject &root = jsonBuffer.createObject();
      // String json_str;
      // root["timestamp"] = s_get_timestamp();
      // root["current"] =  pzem004->pzem_current->get_val_Str();
      // root["volage"] = pzem004->pzem_voltage->get_val_Str();
      // root["power"] = pzem004->pzem_power->get_val_Str();
      // root["energy"] = pzem004->pzem_energy->get_val_Str();
      // root.printTo(json_str);
      // //int len = json_str.length();
      // debug("PZEM004-json", json_str);

      //if (pub->is_connected() && pub->is_time_synced()) pub->publish_to_topic(PZEM_004_OUT_CHANNEL,json_str);
    
   };





};





#endif