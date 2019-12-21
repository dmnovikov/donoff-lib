#ifndef dpublisherblynk_h
#define dpublisherblynk_h

#include <BlynkSimpleEsp8266.h>
#include <donoffcommands.h>
#include <donoffbase.h>
#include <publishers/donoffpublisher.h>



#define BLYNK_TERMINAL_PIN 0


class DPublisherBlynk: public DPublisher {
  protected:
    int init_ok = 0;
    const int reconnect_period = 30;
    ulong last_connect_attempt;
    WidgetTerminal terminal;
    
   
  public:
    DPublisherBlynk(WMSettings * __s): DPublisher(__s), terminal(BLYNK_TERMINAL_PIN) {

    };


    int init( Queue<pub_events>* _q) {
      DPublisher::init(_q);
      Blynk.config(auth, "blynk-cloud.com", 80);
      Blynk.connect();
      debug("PUBLISHER", "INIT !");
      last_connect_attempt = millis();
      if (Blynk.connected() ) {
        //if you get here you have connected to the WiFi
        debug("PUBLISHER", "connected !");
        once_connected=1;

      }
      init_ok = 1;
    };

    int virtual is_connected() {
      if (Blynk.connected()){
        once_connected=1;
        return 1;
      }
      return 0;
    };

    int virtual is_time_synced(){
      if(once_connected) return 1;
      return 0;
    };

    int virtual reconnect() {
      if (!Blynk.connected() && millis() - last_connect_attempt > reconnect_period * 1000) {

        debug("RECONNECT", "*** Try to connect again");
        Blynk.connect();
        if (is_connected()) debug("CONNECT","OK");
        last_connect_attempt = millis();
      }
    };

    int virtual publish_sh_to_info_topic(String shStr, String _valStr) {
      if (!is_connected()) return 0;
      String outS = shStr + "=" + _valStr;
      Blynk.virtualWrite(String(INFO_CHANNEL).toInt(), outS);
      terminal.println(outS);
      terminal.flush();
    };

    int virtual publish_to_info_topic(String _valStr) {
      if (!is_connected()) return 0;
      Blynk.virtualWrite(String(INFO_CHANNEL).toInt(), _valStr);
      terminal.println(_valStr);
      terminal.flush();
    };

     int virtual publish_to_log_topic(String _valStr) {
      if (!is_connected()) return 0;
      Blynk.virtualWrite(String(LOG_CHANNEL).toInt(), _valStr);
      terminal.println(_valStr);
      terminal.flush();
    };

    int virtual publish_sensor(DSensor * _sensor) {
      Blynk.virtualWrite(_sensor->get_channelStr().toInt(), _sensor->get_val_Str());
    };

    int virtual publish_relay(DRelay * _r) {
      if (_r->is_on()) Blynk.virtualWrite(_r->get_onoff_channel_str().toInt(), 220);
      if (_r->is_off()) Blynk.virtualWrite(_r->get_onoff_channel_str().toInt(), 0);
    };

    int virtual publish_relay_on(DRelay * _r, String reason = "") {
      if (!is_connected()) return 0;
      Blynk.virtualWrite(_r->get_onoff_channel_str().toInt(), 220);
      //if (reason == "")  publish_to_info_topic("L:"+_r->get_nameStr() + ":ON");
      //else publish_to_info_topic("L:"+_r->get_nameStr() + ":ON, " + reason);
    };


    // int virtual log_relay_on(DRelay * _r, String reason = "") {
    //   if (!is_connected()) return 0;
    //   //debug(_r->get_nameStr(), "on"+reason);
    //   if (reason == "")  publish_to_log_topic("L:"+_r->get_nameStr() + ":ON");
    //   else publish_to_log_topic("L:"+_r->get_nameStr() + ":ON, " + reason);
    // };

    int virtual publish_relay_off(DRelay * _r, String reason = "") {
      if (!is_connected()) return 0;
      //debug(_r->get_nameStr(), "off"+reason);
      Blynk.virtualWrite(_r->get_onoff_channel_str().toInt(), 0);
      //if (reason == "")  publish_to_info_topic("L:"+_r->get_nameStr() + ":OFF");
      //else publish_to_info_topic("L:"+_r->get_nameStr() + ":OFF, " + reason);
    };

    //  int virtual log_relay_off(DRelay * _r, String reason = "") {
    //   if (!is_connected()) return 0;
    //   if (reason == "")  publish_to_log_topic("L:"+_r->get_nameStr() + ":OFF");
    //   else publish_to_log_topic("L:"+_r->get_nameStr() + ":OFF, " + reason);
    // };

    int virtual publish_uptime(DRelay * _r) {
      if (!is_connected()) return 0;
      Blynk.virtualWrite(String(UPTIME_CHANNEL).toInt(), _r->get_uptime_str());
    };

    int virtual publish_ontime(DRelay * _r) {
      if (!is_connected()) return 0;
      Blynk.virtualWrite(_r->get_ontime_channel_str().toInt(), _r->get_ontime_str());
      //debug("PUBLISH", String(_r->get_ontime_str()));
    };

    int virtual publish_downtime(DRelay * _r) {
      if (!is_connected()) return 0;
      Blynk.virtualWrite(_r->get_downtime_channel_str().toInt(), _r->get_downtime_str());
    };




};
#endif