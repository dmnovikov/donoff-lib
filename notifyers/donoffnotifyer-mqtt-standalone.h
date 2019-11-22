#ifndef dnotifyermqttstandalone_h
#define dnotifyermqttstandalone_h

#include <notifyers/donoffnotifyer.h>
#include <ArduinoJson.h>


#define CREDS_MQTT_STANDALONE_FILE

class DNotifyerEmailMQTTStandalone : public DNotifyer
{
private:

#ifdef CREDS_MQTT_STANDALONE_FILE
  #include "credentials_mqtt.h"
#else
  const char *MQTT_SERVER = "xxxxxx";
  const char *MQTT_PORT = "1883";
  const char *MQTT_USER = "xxxx";
  const char *MQTT_PASS = "xxxxx";
  

#endif

public:
  DNotifyerEmailMQTTStandalone(WMSettings *__s) : DNotifyer(__s){};

  int virtual is_connected()
  {
    if (WiFi.status() != WL_CONNECTED)
      return 0;
    return 1;
  };

  int virtual send_notify(String _notifySubj, String _notifyBody)
  {

    WiFiClient espClient;
    PubSubClient client(espClient);
    int port = atoi(MQTT_PORT);
    client.setServer(MQTT_SERVER, port);

    String clientId = "donoff-" + String(_s->dev_id);
    clientId += String(random(0xffff), HEX);
    client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS);
    if (!client.connected())
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
    client.publish(TOPIC_SENDMAIL, json_str.c_str());
    //client.publish(TOPIC, "hh");
    debug("NOTIFYER", "PUBLISHED:" + json_str + ";" + String(len) + ";" + String(MQTT_MAX_PACKET_SIZE));
  };
};

class DNotifyerEmailMQTT : public DNotifyer
{
private:
 PubSubClient *_c;


public:
  DNotifyerEmailMQTT(WMSettings *__s, PubSubClient* __c) : DNotifyer(__s){
    _c=__c;
  };

  int virtual is_connected()
  {
    if (!_c->connected()) return 0;
    return 1;
  };

  int virtual send_notify(String _notifySubj, String _notifyBody)
  {

    WiFiClient espClient;
    
    if (!_c->connected())
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
    _c->publish(TOPIC_SENDMAIL, json_str.c_str());
    //client.publish(TOPIC, "hh");
    debug("NOTIFYER", "PUBLISHED:" + json_str + ";" + String(len) + ";" + String(MQTT_MAX_PACKET_SIZE));
  };
};


#endif