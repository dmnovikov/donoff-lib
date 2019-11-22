#ifndef dnotifyermqttpointer_h
#define dnotifyermqttpointer_h

#include <notifyers/donoffnotifyer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class DNotifyerMQTT : public DNotifyer
{
private:
    PubSubClient * _pub;

public:
  DNotifyerMQTT(WMSettings *__s, PubSubClient* __pub) : DNotifyer(__s){
      _pub=__pub;
  };

  int virtual is_connected()
  {
    if (_pub->connected())
      return 1;
    return 0;
  };

  int virtual send_notify(String _notifySubj, String _notifyBody)
  {

    if (!_pub->connected())
    {
      debug("NOTIFYER", "No connect to mqtt");
      return 0;
    }
    debug("NOTIFYER", "connected");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    String json_str;
    root["dev"] = _s->dev_id;
    root["email"] = _s->email_notify;
    root["subj"] = _notifySubj;
    root["body"] = _notifyBody;
    root.printTo(json_str);
    int len = json_str.length();
    _pub->publish(TOPIC_SENDMAIL, json_str.c_str());
    //client.publish(TOPIC, "hh");
    debug("NOTIFYER", "PUBLISHED:" + json_str + ";" + String(len) + ";" + String(MQTT_MAX_PACKET_SIZE));
  };
  
};


#endif