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

  };



#endif