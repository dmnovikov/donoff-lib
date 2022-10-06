#ifndef donoffsupplydht_h
#define donoffsupplydht_h

#include <supplies/donoffsupply-donoff-universal.h>
#include <sensors/donoffsensor_ads11x5.h>


#ifdef D_MQTT
    const char ADS_OUT_CHANNEL[]="/out/sensors/ads_json";
#endif

class DSupplyADS: public DSupplyDonoffUni{

  protected:
    ADS1115Sensor* multi_ads;

  public:
    DSupplyADS(WMSettings * __s): DSupplyDonoffUni(__s) {};

    void init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
      
        DSupplyDonoff::init(_notifyer, _pub, _q);
        init_ok = 0;
      
        debug("SUPPLY_INIT", "DHT INIT");
        
        multi_ads = new ADS1115Sensor(_s);
        multi_ads->init("ADS", ADS_OUT_CHANNEL, DONOFF_SENSOR_TYPE_ADS, que_sensor_states);
        
        init_ok=1;
     
    };

   //first sensor loop
   void sensors_loop(int sensor_num){
       DSupplyDonoff::sensors_loop(sensor_num);

       if(sensor_num==3){
          multi_ads->adschannels[0]->sensor_loop();
          if(CH1) multi_ads->adschannels[1]->sensor_loop();
          if(CH2) multi_ads->adschannels[2]->sensor_loop();
          if(CH3) multi_ads->adschannels[3]->sensor_loop();
       }

   };


   //add public sensor and relay_1 status
   void virtual service_loop() {   

       DSupplyDonoff::service_loop();

      if (pub->is_connected()) pub->publish_sensor(multi_ads->adschannels[0]);

      if (pub->is_connected() && pub->is_time_synced()) pub->publish_multi_sensor(multi_ads);
    
   };


};





#endif