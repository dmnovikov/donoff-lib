#ifndef donoffsupplydht_h
#define donoffsupplydht_h

#include <supplies/donoffsupply-donoff.h>
#include <sensors/donoffsensor_dhtXX.h>



#ifdef D_MQTT
    const char DHT_OUT_CHANNEL[]="/out/sensors/dht_json";
#endif

//#define SENSOR_PIN 13 //d7
#define DHT_PIN 4 //d2

class DSupplyDHT: public DSupplyDonoff{

  protected:
    DHTSensor* multi_dht;

  public:
    DSupplyDHT(WMSettings * __s): DSupplyDonoff(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
      
        DSupplyDonoff::init(_notifyer, _pub, _q);
        init_ok = 0;
      
        debug("SUPPLY_INIT", "DHT INIT");
        
        multi_dht = new DHTSensor(_s, DHT_PIN);
        multi_dht->init("DHT", DHT_OUT_CHANNEL, DONOFF_SENSOR_TYPE_DHT, que_sensor_states);
        
        init_ok=1;
     
    };

   //first sensor loop
   int sensors_loop(int sensor_num){
       DSupplyDonoff::sensors_loop(sensor_num);

       if(sensor_num==2){
          multi_dht->dht_temp->sensor_loop();
          //debug("DHT_TEMP", multi_dht->dht_humidity->get_val_Str());
       }

       if(sensor_num==3){
          multi_dht->dht_humidity->sensor_loop();
          //debug("DHT_HUMIDITY", multi_dht->dht_humidity->get_val_Str());
       }
     
   };


   //add public sensor and relay_1 status
   int virtual service_loop() {   

       DSupplyDonoff::service_loop();

      if (pub->is_connected()) pub->publish_sensor(multi_dht->dht_temp);
      if (pub->is_connected()) pub->publish_sensor(multi_dht->dht_humidity);


      if (pub->is_connected() && pub->is_time_synced()) pub->publish_multi_sensor(multi_dht);
    
   };



     int slow_loop(int mycounter){

         DSupplyDonoff::slow_loop(mycounter);

         if(mycounter==7){
             hotter_cooler_loop();
         }

     };

    int virtual hotter_loop(){
        //debug("HOTTER", "START HOTTER");
        hotter1(multi_dht->dht_temp, r[0]);
    };

    int virtual cooler_loop(){
        cooler(multi_dht->dht_temp,r[0]);
    };





};





#endif