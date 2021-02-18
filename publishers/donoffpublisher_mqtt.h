#ifndef dpublishermqtt_h
#define dpublishermqtt_h

//#include <>
#include <TZ.h>
#include <time.h>
#include <sys/time.h> 
#include <donoffcommands.h>
#include <donoffbase.h>
#include <publishers/donoffpublisher.h>
#include <sensors/donoff-multisensor.h>

class DPublisherMQTT : public DPublisher
{
protected:
  int init_ok = 0;
  const int reconnect_period = 30;
  ulong last_connect_attempt;
  PubSubClient* _c;
  int not_configured=0;
  int time_synced=0;
  ulong m_incoming_ms=0;
  uint attempts=0;

public:
  DPublisherMQTT(WMSettings *__s, PubSubClient *__c) : DPublisher(__s){
        _c=__c;
  };

  int init(Queue<pub_events> *_q){
      DPublisher::init(_q);
      configTime(_s->time_zone * 3600,  0, NTP_SERVER_1, NTP_SERVER_2, NTP_SERVER_3);
      sync_time();
      try_connect();
      m_incoming_ms=millis();
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

    int virtual sync_time(int forced=0){
        if(forced==0 && time_synced==1) return 0;

        time_t tnow = time(nullptr);
        struct tm * timeinfo;

        debug("TIMESYNC", "TIME FROM SERVER:"+String(ctime(&tnow)));

        timeinfo=localtime(&tnow);

        //setTime(tnow); //this is set wrong hour, without time zone

        //instead we use manual setTime
        // debug("TIMESYNC", "SET HOUR:"+String(timeinfo->tm_hour));
        // debug("TIMESYNC", "SET MIN:"+String(timeinfo->tm_min));
        // debug("TIMESYNC", "SET DAY:"+String(timeinfo->tm_mday));
        // debug("TIMESYNC", "SET YEAR:"+String(timeinfo->tm_year+1900));

        setTime(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year+1900);

        if(year()==1970){ //default date
          debug("TIMESYNC", "FAIL TO SYNC TIME :(");
          time_synced=0;
        }else{
          debug("TIMESYNC", "TIME SYNCED:"+String(d_hour())+":"+String(minute())+":"+String(year())+":"+String(month())+"; push event");
          que_wanted->push(PUBLISHER_WANT_SAY_JUST_SYNCED);
          time_synced=1;
        }
    };

    int virtual is_time_synced()
    {

      return time_synced;
    };

    int virtual try_connect(){

        debug("RECONNECT", "Try attemps:" + String(attempts));

        if(attempts>MAX_CONNECT_ATTEMPTS_BEFORE_RESET) reset();
      
        if (WiFi.status() != WL_CONNECTED) {
            debug("RECONNECT", "NO WIFI CONNECTION, try to Wifi.begin");
            // WiFi.begin();


                      // We start by connecting to a WiFi network
            // Удаляем предыдущие конфигурации WIFI сети
            // WiFi.disconnect(); // обрываем WIFI соединения
            // WiFi.softAPdisconnect(); // отключаем отчку доступа(если она была
            // WiFi.mode(WIFI_OFF); // отключаем WIFI
            // delay(500);

            // wifi_station_get_config (&stationConf);

            // Serial.println(String("saved ssid:"+String((char*)stationConf.ssid)+", saved pass=" + String((char*) stationConf.password)));

            // // присваиваем статичесий IP адрес
            // WiFi.mode(WIFI_STA); // режим клиента
            // delay(10);
            // WiFi.begin();
            WiFi.reconnect();

            return 0;
        }
        
        

        //Serial.println("\nStarting connection to server...");

        IPAddress result;
        int err = WiFi.hostByName(_s->mqttServer, result) ;

        if(err !=1){
          debug("RECONNECT", "Cant resolve mqtt server");
        }else{
  
          Serial.print("MQTT Ip address: ");
          Serial.println(result);

          //debug("RECONNECT", "Server IP:"+ String(result));
        }

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
          debug("PUBLISHER", "MQTT CONNECTED");
          attempts=0;
          subscribe_all();
        }else{
          debug("PUBLISHER", "MQTT FAILED TO CONNECT");
        }
        
        
        
        
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
        if(millis() - last_connect_attempt > reconnect_period * 1000){
          if (!is_connected()){
            attempts++;
            try_connect();
          }
          if(!is_time_synced()){
            sync_time();
          }

          last_connect_attempt = millis();

        }
      
    };

    void callback(char* topic, byte* payload, unsigned int length){
        //debug("CALLBACK", String(topic)+"::"+String((char*)payload));
        char message[100];
        
        //antispam, check time between commands
        if(millis()-m_incoming_ms<DDOS_MS){
          publish_to_info_topic("DDOS CHECK");
          m_incoming_ms=millis();
          return;
        }
        m_incoming_ms=millis();


        String inS="";
        for (int i = 0; i < length; i++)
        {
          inS = inS + (char)payload[i];
        }
        //inS = String(message);
        //inS=inS.substring(0, length);
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

    int virtual publish_sensor(DSensor * _sensor, uint _debug=0){
       if (!is_connected()) {
         debug("PUB_SENSOR", "mqtt DISCONNECTED");
         return 0;   
       }
       if(_debug) debug("PUB_SENSOR",form_full_topic(_sensor->get_channelStr())+_sensor->get_val_Str());
      _c->publish(form_full_topic(_sensor->get_channelStr()).c_str(), _sensor->get_val_Str().c_str());
      if(_sensor->need_publish_json() && is_time_synced()){
         publish_sensor_json(_sensor);
      }
    };

    int virtual publish_multi_sensor(DMultiSensor * _multi_sensor){
       if (!is_connected()) return 0;   
      _c->publish(form_full_topic(_multi_sensor->get_channelStr()).c_str(), _multi_sensor->multi_json_Str().c_str());
      debug("PUBLISHMULTI", "publish to:"+_multi_sensor->get_channelStr()+" , json:"+_multi_sensor->multi_json_Str() );
    };


    int virtual publish_relay_state(DRelay * _r)
    {
      // debug("PUBLISH_ONOOFF", "Topic="+form_full_topic(_r->get_onoff_channel_str()));
      if (_r->is_on())
        _c->publish(form_full_topic(_r->get_onoff_channel_str()).c_str(), "1");
      if (_r->is_off())
         _c->publish(form_full_topic(_r->get_onoff_channel_str()).c_str(), "0");
    };

    int virtual publish_uptime()
    {
      if (!is_connected()) return 0;
      _c->publish(form_full_topic(UPTIME_CHANNEL).c_str(), get_time_str(millis()).c_str());

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


    int virtual baselog_sensor(DSensor * _sensor){
      if (!is_connected()) return 0;   
      if(!_sensor->is_started_and_ready()){
        debug("BASELOG", _sensor->get_nameStr()+":Skip baselog, sensor is not ready");
        return 0;
      }

      debug("BASELOG", _sensor->get_nameStr()+":Send to database sensor");

      DynamicJsonBuffer jsonBuffer;

      JsonObject &root = jsonBuffer.createObject();
      String json_str;
      root["timestamp"] = s_get_timestamp();
      root["dev"] = _s->dev_id;
      root["user"] = _s->mqttUser;
      root["s_type"] = String(_sensor->get_type());
      root["name"] = String(_sensor->get_nameStr());
      root["val"] = _sensor->get_longval_Str();
      root["mult"] = _sensor->get_multiplier();

      root.printTo(json_str);
      _c->publish(TOPIC_SENSOR_BASELOG, json_str.c_str());

      //debug("BASELOG",json_str);

      // root["body"] = _notifyBody;
      
      // int len = json_str.length();
      // _pub->publish(TOPIC_SENDMAIL, json_str.c_str());

      // _c->publish(form_full_topic(_sensor->get_channelStr()).c_str(), _sensor->get_val_Str().c_str());
    };

    int virtual publish_sensor_json(DSensor * _sensor){
      if (!is_connected()) return 0;   

      //debug("JSONLOG", _sensor->get_nameStr()+":Send to json sensor");

      DynamicJsonBuffer jsonBuffer;

      JsonObject &root = jsonBuffer.createObject();
      String json_str;
      root["timestamp"] = s_get_timestamp();
      root["s_type"] = String(_sensor->get_type());
      root["name"] = String(_sensor->get_nameStr());
      root["val"] = _sensor->get_longval_Str();
      root["mult"] = _sensor->get_multiplier();

      root.printTo(json_str);
      _c->publish(form_full_topic(_sensor->get_channelStr()+"_json").c_str(), json_str.c_str());

     
    };


    int virtual publish_to_topic(String _topic, String _valStr){
        _c->publish(form_full_topic(_topic).c_str(), _valStr.c_str());
    };

  };



#endif