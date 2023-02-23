#ifndef dnotifyer_h
#define dnotifyer_h

#include <donoffcommands.h>
#include <donoffbase.h>
#include <sensors/donoffsensor.h>
#include <donoffrelay.h>
#include <Queue.h>

//#define CREDS_MQTT_STANDALONE_FILE
#define GS_SERIAL_LOG_2
//#define CREDS_EMAIL_NOTIFYER_FILE



class DNotifyer : public DBase
{

protected:
protected:
  DPublisher *pub;
  int init_ok = 0;
  int fake_notify = 0;
  Queue<int> *que_not;

public:
  DNotifyer(WMSettings *__s) : DBase(__s){};

  int virtual send_notify(String _notifySubj, String _notifyBody){};

  int virtual is_connected(){return 0;};

  void virtual init(DPublisher *_pub)
  {
    pub = _pub;
    init_ok = 1;
  };


  int notify_on(DRelay *_r, String _reasonStr)
  {
    if (_s->notifyer && _s->notifyer_onoff && is_connected())
    {
      send_notify(_r->get_name() + " on", "turn on, reason=" + _reasonStr);
      return 1;
    }
    return 0;
  };

  int notify_off(DRelay *_r, String _reasonStr)
  {
    if (_s->notifyer && _s->notifyer_onoff && is_connected())
    {
      send_notify(_r->get_name() + " off", "turn on, reason=" + _reasonStr);
      return 1;
    }
    return 0;
  };


  int notify_sensor_state(sensor_state *state)
  {
    String SubjStr;
    String BodyStr;
    String stateStr;
    SubjStr = state->ps->get_nameStr();
    SubjStr += ":";
    if (state->curr_state == HIGH_STATE)
      stateStr += "HIGH_STATE";
    if (state->curr_state == LOW_STATE)
      stateStr += "LOW_STATE";
    if (state->curr_state == ERR_STATE)
      stateStr += "ERR_STATE";
    SubjStr += stateStr;
    BodyStr = "sensor value=" + String(state->val) + ", " + "sensor level=" + String(state->level);
    debug("NOTIFYER", SubjStr + ":::" + BodyStr);
    send_notify(SubjStr, BodyStr);
  };

  int notify_working_hours(DRelay *r){

  };
};

/*

class DNotifyerEmail : public DNotifyer
{
private:
  class Gsender
  {
  public:
    Gsender(){};

  private:
#ifdef CREDS_EMAIL_NOTIFYER_FILE
    #include "credentials_email.h"
#else
    const int SMTP_PORT = 465;
    const char *SMTP_SERVER = "smtp.gmail.com";
    //const char* SMTP_SERVER = "173.194.222.109";
    const char *EMAILBASE64_LOGIN = "xxxxxxxxx";
    const char *EMAILBASE64_PASSWORD = "xxxxxxxxxxxxxxxxxxxxx";
    const char *FROM = "xxxxxxxxxxxxxxxxxxxxxx";
#endif
    
    const char *_error = nullptr;
    String _subject = "empty";
    String _serverResponce;

    bool AwaitSMTPResponse(WiFiClientSecure &client, const String &resp = "", uint16_t timeOut = 10000)
    {
      uint32_t ts = millis();
      while (!client.available())
      {
        if (millis() > (ts + timeOut))
        {
          _error = "SMTP Response TIMEOUT!";
          return false;
        }
      }
      _serverResponce = client.readStringUntil('\n');
#if defined(GS_SERIAL_LOG_1) || defined(GS_SERIAL_LOG_2)
      Serial.println(_serverResponce);
#endif
      if (resp && _serverResponce.indexOf(resp) == -1)
        return false;
      return true;
    };

  public:
    bool Send(const String &subj, const String &to, const String &message)
    {
      WiFiClientSecure client;
      client.setInsecure();
#if defined(GS_SERIAL_LOG_2)
      Serial.print("Connecting to :");
      Serial.println(SMTP_SERVER);
      Serial.println(SMTP_PORT);
#endif
      if (!client.connect(SMTP_SERVER, SMTP_PORT))
      {
        _error = "Could not connect to mail server";
        return false;
      }
      if (!AwaitSMTPResponse(client, "220"))
      {
        _error = "Connection Error";
        return false;
      }

#if defined(GS_SERIAL_LOG_2)
      Serial.println("HELO friend:");
#endif
      client.println("HELO friend");
      if (!AwaitSMTPResponse(client, "250"))
      {
        _error = "identification error";
        return false;
      }

#if defined(GS_SERIAL_LOG_2)
      Serial.println("AUTH LOGIN:");
#endif
      client.println("AUTH LOGIN");
      AwaitSMTPResponse(client);

#if defined(GS_SERIAL_LOG_2)
      Serial.println("EMAILBASE64_LOGIN:");
#endif
      client.println(EMAILBASE64_LOGIN);
      AwaitSMTPResponse(client);

#if defined(GS_SERIAL_LOG_2)
      Serial.println("EMAILBASE64_PASSWORD:");
#endif
      client.println(EMAILBASE64_PASSWORD);
      if (!AwaitSMTPResponse(client, "235"))
      {
        _error = "SMTP AUTH error";
        return false;
      }

      String mailFrom = "MAIL FROM: <" + String(FROM) + '>';
#if defined(GS_SERIAL_LOG_2)
      Serial.println(mailFrom);
#endif
      client.println(mailFrom);
      AwaitSMTPResponse(client);

      String rcpt = "RCPT TO: <" + to + '>';
#if defined(GS_SERIAL_LOG_2)
      Serial.println(rcpt);
#endif
      client.println(rcpt);
      AwaitSMTPResponse(client);

#if defined(GS_SERIAL_LOG_2)
      Serial.println("DATA:");
#endif
      client.println("DATA");
      if (!AwaitSMTPResponse(client, "354"))
      {
        _error = "SMTP DATA error";
        return false;
      }

      client.println("From: <" + String(FROM) + '>');
      client.println("To: <" + to + '>');

      client.print("Subject: ");
      client.println(subj);

      client.println("Mime-Version: 1.0");
      client.println("Content-Type: text/html; charset=\"UTF-8\"");
      client.println("Content-Transfer-Encoding: 7bit");
      client.println();
      String body = "<!DOCTYPE html><html lang=\"en\">" + message + "</html>";
      client.println(body);
      client.println(".");
      if (!AwaitSMTPResponse(client, "250"))
      {
        _error = "Sending message error";
        return false;
      }
      client.println("QUIT");
      if (!AwaitSMTPResponse(client, "221"))
      {
        _error = "SMTP QUIT error";
        return false;
      }
      return true;
    };

    String getLastResponce()
    {
      return _serverResponce;
    };

    const char *getError()
    {
      return _error;
    };
  };

protected:
public:
  DNotifyerEmail(WMSettings *__s) : DNotifyer(__s){};

  int virtual is_connected()
  {
    if (WiFi.status() != WL_CONNECTED)
      return 0;
    return 1;
  };

  int virtual send_notify(String _notifySubj, String _notifyBody)
  {

    Gsender *gsender = new Gsender(); // Getting pointer to class instance
    int res = gsender->Send(_notifySubj, String(_s->email_notify), _notifyBody);
    if (res == 1)
    {
      Serial.println("Message send.");
      return 1;
    }
    else
    {
      Serial.print("Error sending message: ");
      Serial.println(gsender->getError());
      return 0;
    }
    delete gsender;
  };
};




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

    StaticJsonDocument <1024> root;

    String json_str;
    root["dev"] = _s->dev_id;
    root["email"] = _s->email_notify;
    root["subj"] = _notifySubj;
    root["body"] = _notifyBody;
    
    serializeJson(root, json_str);

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

    StaticJsonDocument <1024> root;
   
    String json_str;
    root["dev"] = _s->dev_id;
    root["email"] = _s->email_notify;
    root["subj"] = _notifySubj;
    root["body"] = _notifyBody;

    serializeJson(root, json_str);

    int len = json_str.length();
    _c->publish(TOPIC_SENDMAIL, json_str.c_str());
    //client.publish(TOPIC, "hh");
    debug("NOTIFYER", "PUBLISHED:" + json_str + ";" + String(len) + ";" + String(MQTT_MAX_PACKET_SIZE));
  };
};

*/

#endif