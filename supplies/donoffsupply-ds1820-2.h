#ifndef donoffsupplydonoffds2_h
#define donoffsupplydonoffds2_h

#define DS1820_OUT2 1 //for SUPPLY_DS2
#define DS2_PIN D1

#include <supplies/donoffsupply-donoff-universal.h>

#ifdef D_MQTT
    const char DS_OUT_CHANNEL2[] = "/out/sensors/temp_out2";
#endif

class DSupplyDonoffDS2: public DSupplyDonoffUni {

  protected:

    DigitalDS1820Sensor* ds_out2;

  public:
    DSupplyDonoffDS2(WMSettings * __s): DSupplyDonoffUni(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

        DSupplyDonoffUni::init(_notifyer, _pub, _q);
        init_ok = 0;

      if (DS1820_OUT2) {
        debug("SUPPLY_INIT", "DS_OUT2 INIT");
        ds_out2 = new DigitalDS1820Sensor(_s, DS2_PIN);
        ds_out2->init("ds_out2", DS_OUT_CHANNEL2, DS1820_FILTERED,que_sensor_states, DONOFF_SENSOR_TYPE_TEMPERATURE, JSON_CHANNEL_NO);
      }
        init_ok=1;
    
    };

   //second sensor loop
   int sensors_loop(int sensor_num){ 
      
       DSupplyDonoffUni::sensors_loop(sensor_num);

       if(sensor_num==3){
         if(DS1820_OUT2){
              ds_out2->sensor_loop();
              debug("DS_OUT2", ds_out2->get_val_Str());
         }
       }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyDonoffUni::service_loop();

       if(pub->is_connected() && DS1820_OUT2){
            pub->publish_sensor(ds_out2);
       }
   };

     int slow_loop(int mycounter){

         DSupplyDonoffUni::slow_loop(mycounter);

     };

    int virtual hotter_cooler_loop() {

      DSupplyDonoffUni::hotter_cooler_loop();

    };


};

 





#endif