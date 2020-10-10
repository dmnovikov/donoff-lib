#ifndef donoffsupplypzem004_h
#define donoffsupplypzem004_h

#include <supplies/donoffsupply-base.h>
#include <sensors/donoffsensor_PZEM.h>



#ifdef D_MQTT
    const char PZEM_004_OUT_CHANNEL[]="/out/sensors/pzem004";
#endif

#define PIN1 D1
#define PIN2 D2


class DSupplyPZEM004: public DSupplyBase {

  protected:
    PZEM004Tv30Sensor* pzem004;

  public:
    DSupplyPZEM004(WMSettings * __s): DSupplyBase(__s) {};

    int init(DNotifyer * _notifyer, DPublisher* _pub, Queue<pub_events>* _q) {
        DSupplyBase::init(_notifyer, _pub, _q);
        init_ok = 0;
      
        debug("SUPPLY_INIT", "PZEM004 INIT");
        
        pzem004 = new PZEM004Tv30Sensor(_s, PIN1, PIN2);
        pzem004->init("PZEM004", PZEM_004_OUT_CHANNEL, que_sensor_states, 10, 0);
        
        init_ok=1;
    
    };

   //first sensor loop
   int sensors_loop(int sensor_num){
       if(sensor_num==1){
          pzem004->pzem_current->sensor_loop();
          debug("PZEM004C", pzem004->pzem_current->get_val_Str());

          pzem004->pzem_voltage->sensor_loop();
          debug("PZEM004V", pzem004->pzem_voltage->get_val_Str());

          pzem004->pzem_power->sensor_loop();
          debug("PZEM004P", pzem004->pzem_power->get_val_Str());

          pzem004->pzem_energy->sensor_loop();
          debug("PZEM004E", pzem004->pzem_energy->get_val_Str());
       }
   };


   //add public sensor and relay_1 status
   int virtual service_loop() {

       DSupplyBase::service_loop();

       if (pub->is_connected()) pub->publish_sensor(pzem004->pzem_current);
       if (pub->is_connected()) pub->publish_sensor(pzem004->pzem_voltage);
       if (pub->is_connected()) pub->publish_sensor(pzem004->pzem_power);
       if (pub->is_connected()) pub->publish_sensor(pzem004->pzem_energy);

   };





};





#endif