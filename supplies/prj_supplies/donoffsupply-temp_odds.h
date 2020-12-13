#ifndef donoffsupplydonofftempodds_h
#define donoffsupplydonofftempodds_h


#include <supplies/donoffsupply-ds1820-2.h>
#include <sensors/donoff-virtual-temp-odds.h>

#define DS_ODDS 1

#ifdef D_MQTT
    const char DS_OUT_ODD_CHANNEL[] = "/out/sensors/temp_odd";
#endif

class DSupplyDonoffTempOdds: public DSupplyDonoffDS2 {

  protected:

    DTempOddsSensor* ds_odds;

  public:
    DSupplyDonoffTempOdds(WMSettings * __s): DSupplyDonoffDS2(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

      DSupplyDonoffDS2::init(_notifyer, _pub, _q);
      init_ok=0;

      if(DS_ODDS){
        ds_odds= new DTempOddsSensor(_s,ds_out, ds_out2);
        ds_odds->init("DS_ODDS", DS_OUT_ODD_CHANNEL, 0,que_sensor_states, SENSOR_TYPE_TEMPERATURE, JSON_CHANNEL_NO);
      }
      init_ok=1;
    
    };

   //second sensor loop
   int sensors_loop(int sensor_num){ 
      
       DSupplyDonoffDS2::sensors_loop(sensor_num);

       if(sensor_num==3){
         if(DS_ODDS){
              ds_odds->sensor_loop();
              debug("DS_ODDS", ds_odds->get_val_Str());
         }
       }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyDonoffDS2::service_loop();

       if(pub->is_connected() && DS_ODDS){
            pub->publish_sensor(ds_odds);
       }
   };

  
     int virtual cooler_loop(){
      if(DS1820_OUT && DS1820_OUT2 && DS_ODDS) cooler(ds_odds,r[0]);
    };
};

 





#endif