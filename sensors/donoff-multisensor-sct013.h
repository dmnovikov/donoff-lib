#ifndef donoffmultisensorsct013_h
#define donoffmultisensorsct013_h

#include <Queue.h>
#include <donoffbase.h>
#include <donoffsettings.h>
#include <sensors/donoffsensor_sct013.h>

#ifdef D_MQTT
    const char SCT013_1_OUT_CHANNEL[]="/out/sensors/sct013_1";
    const char SCT013_2_OUT_CHANNEL[]="/out/sensors/sct013_2";
    const char SCT013_3_OUT_CHANNEL[]="/out/sensors/sct013_3";
#endif


class DMultiSensorSCT013: public DSensor {

  public:

  DSensor* sensors[4]={NULL,NULL,NULL,NULL};

  protected:

    int num_of_sensors=0;
    long val;
    
    int MULTI_PIN1;
    int MULTI_PIN2;

  public:
    DMultiSensorSCT013(WMSettings * __s): DSensor(__s) {
        
    };

    void virtual init(int _pin1, int _pin2, String _name, String _chname, int _filtered, Queue<sensor_state> *_que_sensor_states, int _type, int _need_json){
   
      MULTI_PIN1=_pin1;
      MULTI_PIN2=_pin2;

      pinMode(MULTI_PIN1,OUTPUT);
      pinMode(MULTI_PIN2,OUTPUT);

      DSensor::init(_name,_chname, _filtered, _que_sensor_states, _type, _need_json);

      NO_SENSOR_VAL = -12700;
      NOT_READY_VAL = -12800;
      START_DELAY = 30000;
      NEED_ASKING = 0;
      NEED_FILTERED = _filtered;
      // REQUEST_CIRCLE = 750;
      MULTIPLIER = 100;
      NEED_ASK_WHILE_WATING=1;
      TYPE=_type;

      DSensor::init(_name, _chname, _filtered, _que_sensor_states, _type, _need_json);

      sensors[0]= new SCT013Sensor(_s, A0);
      sensors[0]->init("SCT013_1",SCT013_1_OUT_CHANNEL, 0, _que_sensor_states, _type, _need_json);

      sensors[1]= new SCT013Sensor(_s, A0);
      sensors[1]->init("SCT013_2",SCT013_2_OUT_CHANNEL, 0, _que_sensor_states, _type, _need_json);

      sensors[2]= new SCT013Sensor(_s, A0);
      sensors[2]->init("SCT013_3",SCT013_3_OUT_CHANNEL, 0, _que_sensor_states, _type, _need_json);

      num_of_sensors=3;

      init_ok = 1;


    };

    int virtual multi_sensor_loop(int _current_number) {
      //debug(nameStr, "Sensor_LOOP, ms="+String(millis()));
      if (init_ok == 0) {
        debug(nameStr, "No init, no sensor loop");
        return 0;
      }

      if(_current_number==1){
          digitalWrite(MULTI_PIN1, LOW);
          digitalWrite(MULTI_PIN2, LOW);
          sensors[0]->sensor_loop();
      }

       if(_current_number==2){
          digitalWrite(MULTI_PIN1, HIGH);
          digitalWrite(MULTI_PIN2, LOW);
          sensors[1]->sensor_loop();
      }

       if(_current_number==3){
          digitalWrite(MULTI_PIN1, LOW);
          digitalWrite(MULTI_PIN2, HIGH);
          sensors[2]->sensor_loop();

      }
      
      if(_current_number==4){
            int ready_c=0;
            
            //debug("MULTI_SENSOR", "Counter=4");

            // for(DSensor* _ps: sensors){
            //     if(_ps->is_ready() && _ps->is_started()) ready_c++;
            // }

            
            for(int i=0; i<=2; i++){
              if( sensors[i]->is_ready() && sensors[i]->is_started()) ready_c++;
            }

            //debug("MULTI_SENSOR", "Counter="+String(ready_c));

            
            if(ready_c==3) {
                started_ok =1;
                calculate_value();
                
            }else{
                sens.val=NOT_READY_VAL;
            }

         

        }
        
      
      
    };

    void virtual calculate_value(){
              sens.val=0;
              for(int i=0; i<=2; i++){
                sens.val+=sensors[i]->get_val();
              }
              debug("MULTI_SENSOR", "VAL="+String(sens.val));
              sensor_ready=1;
    };

};

#endif
