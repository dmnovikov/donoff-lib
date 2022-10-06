#ifndef dnotifyeremail_h
#define dnotifyeremail_h


#include <notifyers/donoffnotifyer.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>


#define GS_SERIAL_LOG_2
//#define CREDS_EMAIL_NOTIFYER_FILE

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

#endif