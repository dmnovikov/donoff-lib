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
              
      }

      if (inS == D_R3_0) {
              debug("QUEUE_WANTED PUSH EVENT", "R3_0");
              que_wanted->push(PUBLISHER_WANT_R3_OFF);
              
      }


      if (inS == D_R4_1) {
              debug("QUEUE_WANTED PUSH EVENT", "R4_1");
              que_wanted->push(PUBLISHER_WANT_R4_ON);
              
      }

      if (inS == D_R4_0) {
              debug("QUEUE_WANTED PUSH EVENT", "R3_0");
              que_wanted->push(PUBLISHER_WANT_R4_OFF);
              
      }
    
  };

  };


   



#endif