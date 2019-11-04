#ifndef dpublishermqtt_h
#define dpublishermqtt_h

//#include <>
#include <donoffcommands.h>
#include <donoffbase.h>
#include <donoffpublisher.h>

class DPublisherMQTT : public DPublisher
{
protected:
  int init_ok = 0;
  const int reconnect_period = 30;
  ulong last_connect_attempt;
  PubSubClient* _c;
  int not_configured=0;

public:
  DPublisherMQTT(WMSettings *__s, PubSubClient *__c) : DPublisher(__s){
        _c=__c;
  };

  int init(Queue<pub_events> *_q){
    DPublisher::init(_q);
      try_connect();
      init_ok = 1;
    };

    int virtual is_connected()
    {
      //Serial.print(WiFi.status());
      if (not_configured)
      {
        //Serial.println("CONNECT: not_configured dtected, dev_id empty");
        return 0;
      }
      if (_c->connected())
      {
        //debug("PUBLISHER", "MQTT CONNECTED");
      }
      else
      {
        // debug("PUBLISHER", "ERROR MQTT, ERR_NUM:");
        // debug("PUBLISHER", String(_c->state()));
      }

      if (_c->connected() && WiFi.status() == WL_CONNECTED)
        return 1;
      if (!_c->connected() && WiFi.status() == WL_CONNECTED)
        return 0; //later add what to do if mqtt disconnected
      else
        return 0;
    };

    int virtual time_synced()
    {
      return 1;
    };

    int virtual try_connect(){

        int port = atoi(_s->mqttPort);
        _c->setServer(_s->mqttServer, port);

        debug("PUBLISHER", "Attempting MQTT...");
        // Create a random client ID
        String clientId = "donoff-" + String(_s->dev_id);
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        debug("PUBLISHER", "Server=" + String(_s->mqttServer) + "| User:" + String(_s->mqttUser) + "| Pass:" + String(_s->mqttPass) + "| port=" + String(_s->mqttPort));
        //if (client.connect(clientId.c_str(),"xvjlwxhs","_k7d9m2yt0hn")) {
        if (_c->connect(clientId.c_str(), _s->mqttUser, _s->mqttPass)){
          debug("PUBLISHER", "CONNECTED");
        }
        last_connect_attempt = millis();
        subscribe_all();
    };

    String form_full_topic(const char* short_topic){
        return "/"+String(_s->mqttUser)+"/"+String(_s->dev_id)+String(short_topic);
    };


    String form_full_topic(String short_topic){
        return "/"+String(_s->mqttUser)+"/"+String(_s->dev_id)+short_topic;
    };


    int subscribe(const char* short_topic){
        
        String formed_topic;
        formed_topic="/"+String(_s->mqttUser)+"/"+String(_s->dev_id)+String(short_topic);
        _c->subscribe(formed_topic.c_str());
    };

    int subscribe_all(){
        subscribe(PARAMS_CHANNEL);
    };

    int reconnect(){
        if (!is_connected() && millis() - last_connect_attempt > reconnect_period * 1000) 
        {
          try_connect();
        }


    };

    void callback(char* topic, byte* payload, unsigned int length){
        debug("CALLBACK", String(topic)+"::"+String((char*)payload));
        char message[100];
        String inS;
        for (int i = 0; i < length; i++)
        {
          message[i] = (char)payload[i];
        }
        inS = String(message);
        inS=inS.substring(0, length);
        String topic_params_full = form_full_topic(PARAMS_CHANNEL);
        if (String(topic) == topic_params_full) {
          debug("CALLBACK_PARAMS", "Params incoming msg is detected-->"+inS);
          recognize_incoming_str(inS);
        }
    };

    int virtual publish_sh_to_info_topic(String shStr, String _valStr)
    {
      if (!is_connected())
        return 0;
      
      String sht=form_full_topic(INFO_CHANNEL);
      debug("PUBLISH_SH_INFO","TOPIC="+sht);
      _c->publish(form_full_topic(INFO_CHANNEL).c_str(), (shStr+"="+_valStr).c_str());

    };

    int virtual publish_sh_err()
    {
      if (!is_connected())
        return 0;    
      publish_to_info_topic("E: sh param not recognized");
    };

    int virtual publish_to_info_topic(String _valStr)
    {
      if (!is_connected()) return 0;

      // String sht=form_full_topic(INFO_CHANNEL);
      // debug("PUBLISH_CLEAR_INFO","TOPIC="+sht);
     _c->publish(form_full_topic(INFO_CHANNEL).c_str(), _valStr.c_str());
    };

    int virtual publish_to_log_topic(String _valStr)
    {
      debug("PUBLISH_LOG", "Publish to log topic->"+form_full_topic(LOG_CHANNEL)+", msg:"+_valStr);
      if (!is_connected()) return 0;
      _c->publish(form_full_topic(LOG_CHANNEL).c_str(), _valStr.c_str());

    };

    int virtual publish_sensor(DSensor * _sensor)
    {
      
    };

    int virtual publish_relay_state(DRelay * _r)
    {
      // debug("PUBLISH_ONOOFF", "Topic="+form_full_topic(_r->get_onoff_channel_str()));
      if (_r->is_on())
        _c->publish(form_full_topic(_r->get_onoff_channel_str()).c_str(), "1");
      if (_r->is_off())
         _c->publish(form_full_topic(_r->get_onoff_channel_str()).c_str(), "0");
    };

    int virtual publish_relay_on(DRelay * _r, String reason = "")
    {
      if (!is_connected())
        return 0;
      
    };

    int virtual log_relay_on(DRelay * _r, String reason = "")
    {
      if (!is_connected())
        return 0;

      debug("LOG_RELAY", "Publish on");
      //debug(_r->get_nameStr(), "on"+reason);
      if (reason == "")
        publish_to_log_topic("L:" + _r->get_nameStr() + ":ON");
      else
        publish_to_log_topic("L:" + _r->get_nameStr() + ":ON, " + reason);
    };

    int virtual publish_relay_off(DRelay * _r, String reason = "")
    {
      if (!is_connected())
        return 0;
      
    };

    int virtual log_relay_off(DRelay * _r, String reason = "")
    {
      if (!is_connected()) return 0;
      if (reason == "")
        publish_to_log_topic("L:" + _r->get_nameStr() + ":OFF");
      else
        publish_to_log_topic("L:" + _r->get_nameStr() + ":OFF, " + reason);
    };

    int virtual publish_uptime(DRelay * _r)
    {
      if (!is_connected()) return 0;
      _c->publish(form_full_topic(UPTIME_CHANNEL).c_str(), _r->get_uptime_str().c_str());

    };

    int virtual publish_ontime(DRelay * _r)
    {
      if (!is_connected()) return 0;
      _c->publish(form_full_topic(_r->get_ontime_channel_str()).c_str(), _r->get_ontime_str().c_str());
    };

    int virtual publish_downtime(DRelay * _r)
    {
      if (!is_connected()) return 0;
     _c->publish(form_full_topic(_r->get_downtime_channel_str()).c_str(), _r->get_downtime_str().c_str());
    };
  };
#endif