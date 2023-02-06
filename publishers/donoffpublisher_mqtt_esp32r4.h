#ifndef dpublishermqttesp32r4_h
#define dpublishermqttesp32r4_h

//#include <>
//#include <TZ.h>
#include <time.h>
#include <sys/time.h> 
#include <donoffcommands.h>
#include <donoffbase.h>
#include <publishers/donoffpublisher_mqtt.h>
#include <sensors/donoff-multisensor.h>
#include <PubSubClient.h>

const char* D_R3_1="r31";
const char* D_R3_0="r30";
const char* D_R4_1="r41";
const char* D_R4_0="r40";

const char* C_LSCHM3="lschm3"; //_s->custom_leve11
const char* C_LSCHM4="lschm4"; //_s->custom_level2

const char* C_AOFS3="aofs3";  //_s->custom_leve13
const char* C_AOFS4="aofs4";  //_s->custom_leve14

class DPublisherMQTTESP32R4 : public DPublisherMQTT
{
protected:
  int init_ok = 0;

public:
  DPublisherMQTTESP32R4(WMSettings *__s, PubSubClient *__c) : DPublisherMQTT(__s, __c){
        _c=__c;
  };

  void init(Queue<pub_events> *_q){
      DPublisherMQTT::init(_q);
      init_ok = 1;
    };

  int virtual cmd_loop(String inS){
      DPublisherMQTT::cmd_loop(inS);

      if (inS == D_R3_1) {
              debug("QUEUE_WANTED PUSH EVENT", "R3_1");
              que_wanted->push(PUBLISHER_WANT_R3_ON);
              return 1;
              
      }

      if (inS == D_R3_0) {
              debug("QUEUE_WANTED PUSH EVENT", "R3_0");
              que_wanted->push(PUBLISHER_WANT_R3_OFF);
              return 1;
      }


      if (inS == D_R4_1) {
              debug("QUEUE_WANTED PUSH EVENT", "R4_1");
              que_wanted->push(PUBLISHER_WANT_R4_ON);
              return 1;
      }

      if (inS == D_R4_0) {
              debug("QUEUE_WANTED PUSH EVENT", "R3_0");
              que_wanted->push(PUBLISHER_WANT_R4_OFF);
              return 1;
      }
    return 0;
  };

  int show_parameters_loop() {

        DPublisherMQTT::show_parameters_loop();

        if (shStr == C_LSCHM3) {
          publish_sh_to_info_topic( shStr, String(_s->custom_level1));
          return 1;
        }

        if (shStr == C_LSCHM4) {
          publish_sh_to_info_topic( shStr, String(_s->custom_level2));
          return 1;
        }

        if (shStr == C_AOFS3) {
          publish_sh_to_info_topic( shStr, String(_s->custom_level1)+":"+String(_s->custom_level3));
          return 1;
        }

        if (shStr == C_AOFS4) {
          publish_sh_to_info_topic( shStr, String(_s->custom_level2)+":"+String(_s->custom_level4));
          return 1;
        }

         return 1;
  };

   int set_parameters_loop() {

      DPublisherMQTT::set_parameters_loop();

      if (cmdStr == C_TEST_B) {
        bool a;
        set_settings_val_bool(cmdStr, valStr, &a);
        return 1;

      }
      if (cmdStr == C_LSCHM3) {
        set_settings_val_int(cmdStr, valStr, (int*) &_s->custom_level1, 0, MAX_LSCHM_NUM);
        if(_s->custom_level1==0) que_wanted->push(PUBLISHER_WANT_R3_OFF);
        return 1;
      }

      if (cmdStr == C_LSCHM4) {
         set_settings_val_int(cmdStr, valStr, (int*) &_s->custom_level2, 0, MAX_LSCHM_NUM);
         if(_s->custom_level2==0) que_wanted->push(PUBLISHER_WANT_R4_OFF);
        return 1;
      }

      if (cmdStr == C_AOFS3) {
        if (set_settings_val_int(cmdStr, valStr, (int*) &_s->custom_level3, MIN_AUTOSTOP_SEC, MAX_AUTOSTOP_SEC)) {
          return 2;
        }
        return 1;
      }

      if (cmdStr == C_AOFS4) {
        if (set_settings_val_int(cmdStr, valStr, (int*) &_s->custom_level4, MIN_AUTOSTOP_SEC, MAX_AUTOSTOP_SEC)) {
          return 2;
        }
        return 1;
      }

      return 1;
   };

  };


   



#endif