#ifndef donoffsupplydonoff6temp_h
#define donoffsupplydonoff6temp_h


#include <supplies/donoffsupply-ds1820-2.h>
#include <sensors/donoffsensor_ds1820.h>


#ifdef D_MQTT
    const char DS1_CHANNEL[] = "/out/sensors/temp1";
    const char DS2_CHANNEL[] = "/out/sensors/temp2";
    const char DS3_CHANNEL[] = "/out/sensors/temp3";
    const char DS4_CHANNEL[] = "/out/sensors/temp4";
    const char DS5_CHANNEL[] = "/out/sensors/temp5";
    const char DS6_CHANNEL[] = "/out/sensors/temp6";
#endif

#define PIN1  D7
#define PIN2  D6
#define PIN3  D5
#define PIN4  D1
#define PIN5  D2
#define PIN6  D6


class DSupplyDonoff6Temp: public DSupplyBase {

  protected:

    DigitalDS1820Sensor* ds[5];

  public:
    DSupplyDonoff6Temp(WMSettings * __s): DSupplyBase(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {

       DSupplyBase::init(_notifyer, _pub, _q);
       init_ok = 0;

          
      debug("SUPPLY_INIT", "DS1 INIT");
      ds[0] = new DigitalDS1820Sensor(_s, PIN1);
      ds[0]->init("ds1", DS1_CHANNEL, DS1820_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_YES);

      debug("SUPPLY_INIT", "DS2 INIT");
      ds[1] = new DigitalDS1820Sensor(_s, PIN2);
      ds[1]->init("ds2", DS2_CHANNEL, DS1820_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_YES);

      debug("SUPPLY_INIT", "DS3 INIT");
      ds[2] = new DigitalDS1820Sensor(_s, PIN3);
      ds[2]->init("ds3", DS3_CHANNEL, DS1820_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_YES);

      debug("SUPPLY_INIT", "DS3 INIT");
      ds[3] = new DigitalDS1820Sensor(_s, PIN4);
      ds[3]->init("ds4", DS4_CHANNEL, DS1820_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_YES);

      debug("SUPPLY_INIT", "DS4 INIT");
      ds[4] = new DigitalDS1820Sensor(_s, PIN5);
      ds[4]->init("ds5", DS5_CHANNEL, DS1820_FILTERED,que_sensor_states,DONOFF_SENSOR_TYPE_TEMPERATURE,JSON_CHANNEL_YES);





      init_ok=1;
    
    
    };

   //second sensor loop
   int sensors_loop(int sensor_num){ 
      
       if(sensor_num==1){
          ds[0]->sensor_loop();
          debug("DS1", ds[0]->get_val_Str());
       }

       if(sensor_num==2){
          ds[1]->sensor_loop();
          debug("DS2", ds[1]->get_val_Str());
       }

       if(sensor_num==3){
          ds[2]->sensor_loop();
          debug("DS3", ds[2]->get_val_Str());
       }

        if(sensor_num==4){
          ds[3]->sensor_loop();
          debug("DS4", ds[3]->get_val_Str());
       }

        if(sensor_num==5){
          ds[4]->sensor_loop();
          debug("DS5", ds[4]->get_val_Str());
       }
   };
   
   //add public sensor and relay_1 status
   int virtual service_loop() {
       DSupplyBase::service_loop();
       if(pub->is_connected()){
            pub->publish_sensor(ds[0]);
            pub->publish_sensor(ds[1]);
            pub->publish_sensor(ds[2]);
            pub->publish_sensor(ds[3]);
            pub->publish_sensor(ds[4]);
       }
   };

};

 





#endif