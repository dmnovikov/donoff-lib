#ifndef dpublisher_h
#define dpublisher_h

#include <BlynkSimpleEsp8266.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <donoffcommands.h>
#include <donoffbase.h>
#include <sensors/donoffsensor.h>
#include <donoffrelay.h>
#include <Queue.h>


class DPublisher: public DBase {
  protected:

    //String shStr;
    //WMSettings * _s;

    String incomingStr;
    int is_val_present = 0;
    int is_sh_present = 0;

    String shStr = "";
    String valStr = "";
    String cmdStr = "";

    String channel_uptime_str="0";

    int once_connected=0;

public:
    /* 
    int wanted_reset_hour_r1=0;
    int wanted_reset_hour_r1_m=0;
    int wanted_reset_hour_r2=0;
    int wanted_r1_on=0;
    int wanted_r1_off=0;
    int wanted_r2_on=0;
    int wanted_r2_off=0;
    int wanted_save=0;
    int wanted_reset=0;
    int wanted_r1_off_lschm0=0;
    */
    Queue<pub_events>* que_wanted;
   


  public:
    DPublisher(WMSettings * __s): DBase(__s) {};

     int init( Queue<pub_events>* _q) {
      que_wanted= _q;
     }


    //int virtual publish_sensor(String channelStr, String dataStr){};
    
    int virtual publish_sensor(DSensor* _sensor){};

    int virtual publish_relay_state(DRelay * _r)=0;    

    int virtual publish_relay_on(DRelay * _r,String reason=""){};
    int virtual publish_relay_off(DRelay * _r,String reason=""){};

    int virtual publish_uptime()=0;
    int virtual publish_ontime(DRelay * _r)=0;
    int virtual publish_downtime(DRelay * _r)=0;

    int virtual publish_to_log_topic(String _valStr)=0;

    

    int virtual is_connected()=0;
    int virtual is_time_synced()=0;

    int virtual reconnect()=0;


    int virtual publish_sh_err(){
      if (!is_connected())
        return 0;    
      publish_to_info_topic("E: sh param not recognized");
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

    int virtual log_relay_off(DRelay * _r, String reason = "")
    {
      if (!is_connected()) return 0;
      if (reason == "")
        publish_to_log_topic("L:" + _r->get_nameStr() + ":OFF");
      else
        publish_to_log_topic("L:" + _r->get_nameStr() + ":OFF, " + reason);
    };

    int recognize_incoming_str(String _incomingStr) {
      bool autosave=0;
      int index_c=_incomingStr.indexOf(";");
      if (index_c != -1) {
        autosave=1;
        _incomingStr=_incomingStr.substring(0,_incomingStr.length()-1);
      }

      debug("PUBLISH_RECOGNIZE", "IncomingString"+incomingStr+", autosave:" + String(autosave));
      int  index_val = _incomingStr.indexOf("=");
      if (index_val == -1) is_val_present = 0; else is_val_present = 1;
      is_sh_present = _incomingStr.startsWith("sh ");

      //debug("RECOGNIZE", "is_val=" + String(is_val_present) + " is_sh=" + String(is_sh_present));
      if (is_val_present) {
        cmdStr = _incomingStr.substring(0, index_val);
        valStr = _incomingStr.substring(index_val + 1, _incomingStr.length());
        set_parameters_loop();
        //debug("RECOGNIZE", "command=" + cmdStr + "; value=" + valStr);
        if(autosave) que_wanted->push(PUBLISHER_WANT_SAVE);
      }
      else if (is_sh_present) {
        shStr = _incomingStr.substring(3, _incomingStr.length());
        show_parameters_loop();
        //debug("RECOGNIZE", "sh val=" + shStr);
        
      }else {
        cmd_loop(_incomingStr);
      }
    };


    int virtual cmd_loop(String inS){
        
          if (inS== D_SAVE) {
              que_wanted->push(PUBLISHER_WANT_SAVE);
            }
            
            if (inS == D_R1_1) {
              debug("QUEUE_WANTED PUSH EVENT", "R1_1");
              que_wanted->push(PUBLISHER_WANT_R1_ON);
              
            }
            
            if (inS == D_R1_0) {
              debug("QUEUE_WANTED PUSH EVENT", "R1_0");
              que_wanted->push(PUBLISHER_WANT_R1_OFF);
            }

             
            if (inS == D_R2_1) {
              debug("QUEUE_WANTED PUSH EVENT", "R2_1");
              que_wanted->push(PUBLISHER_WANT_R2_ON);
              
            }
            
            if (inS == D_R2_0) {
              debug("QUEUE_WANTED PUSH EVENT", "R2_0");
              que_wanted->push(PUBLISHER_WANT_R2_OFF);
            }
          
            if (inS==D_RESET) {
              que_wanted->push(PUBLISHER_WANT_RESET);
            }

            if (inS==D_RESET_HOUR1) {
              que_wanted->push(PUBLISHER_WANT_RESET_HOUR_R1_M);
            }
            if (inS==D_RESET_HOUR2) {
              que_wanted->push(PUBLISHER_WANT_RESET_HOUR_R2_M);
            }
            
    };


    int show_parameters_loop() {

      if (is_sh_present == 0) {
        debug("SHOWPARAMSLOOP", "no sh recognized");
        return 0;
      }

      debug("SHOWPARAMS", shStr);

      if (shStr == C_NOTIFYER) {
        publish_sh_to_info_topic(shStr, String(_s->notifyer));
        return 1;
      }

      if (shStr == C_HOTTER) {
        publish_sh_to_info_topic(shStr, String(_s->hotter));
        return 1;
      }
      if (shStr == C_COOLER) {
        publish_sh_to_info_topic(shStr, String(_s->cooler));
        return 1;
      }

      if (shStr == C_BASELOG) {
        publish_sh_to_info_topic(shStr, String(_s->baselog));
        return 1;
      }

      if (shStr == C_AUTO_STOP_SEC) {
        publish_sh_to_info_topic( shStr, String(_s->autostop_sec));
        return 1;
      }
      if (shStr == C_AUTO_STOP_SEC2 || shStr == C_AUTO_STOP_SEC2T) {
        publish_sh_to_info_topic( shStr, String(_s->autostop_sec2));
        return 1;
      }
      if (shStr == C_LIGHT_SCHEME_NUM) {
        publish_sh_to_info_topic( shStr, String(_s->lscheme_num));
        return 1;
      }
      if (shStr == C_LIGHT_SCHEME_NUM2 || shStr == C_LIGHT_SCHEME_NUM2T) {
        publish_sh_to_info_topic( shStr, String(_s->lscheme_num2));
        return 1;
      }
      if (shStr == C_ON_START) {
        publish_sh_to_info_topic( shStr, String(_s->start_on));
        return 1;
      }
      if (shStr == C_AUTO_STOP_HOURS) {
        publish_sh_to_info_topic( shStr, String(_s->autooff_hours));
        return 1;
      }
      if (shStr == C_ONOFF_SCHEME1) {
        publish_sh_to_info_topic( shStr, String(_s->schm_onoff_num1));
        return 1;
      }

      if (shStr == C_ONOFF_SCHEME2) {
        publish_sh_to_info_topic( shStr, String(_s->schm_onoff_num2));
        return 1;
      }

      if (shStr == C_ANALOG_LEVEL1) {
        publish_sh_to_info_topic( shStr, String(_s->analog_level1));
        return 1;
      }

      if (shStr == C_ANALOG_LEVEL2) {
        publish_sh_to_info_topic( shStr, String(_s->analog_level2));
        return 1;
      }

      if (shStr == C_CUSTOM_LEVEL1) {
        publish_sh_to_info_topic( shStr, String(_s->custom_level1));
        return 1;
      }

      if (shStr == C_CUSTOM_LEVEL2) {
        publish_sh_to_info_topic( shStr, String(_s->custom_level2));
        return 1;
      }

      if (shStr == C_NOTIFY_ONOFF) {
        publish_sh_to_info_topic( shStr, String(_s->notifyer_onoff));
        return 1;
      }

      if (shStr == C_NOTIFY_HOURS_ONOFF) {
        publish_sh_to_info_topic( shStr, String(_s-> hours_on_notify));
        return 1;
      }

      if (shStr == C_LOW_NOTIFY_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s-> temp_low_level_notify));
        return 1;
      }

      if (shStr == C_HIGH_NOTIFY_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s-> temp_high_level_notify));
        return 1;
      }

      if (shStr == C_ANALOG1_NOTIFY_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s-> analog_level_notify1));
        return 1;
      }

      if (shStr == C_ANALOG2_NOTIFY_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s-> analog_level_notify2));
        return 1;
      }

      if (shStr == C_CUSTOM1_NOTIFY_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s->custom_level_notify1));
        return 1;
      }

      if (shStr == C_CUSTOM2_NOTIFY_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s->custom_level_notify2));
        return 1;
      }

      if (shStr == C_DTEMP_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s->day_temp_level));
        return 1;
      }

      if (shStr == C_NTEMP_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s->night_temp_level));
        return 1;
      }

      if (shStr == C_CTEMP_LEVEL) {
        publish_sh_to_info_topic( shStr, String(_s->critical_temp_level));
        return 1;
      }

      if (shStr == C_LDELTA) {
        publish_sh_to_info_topic( shStr, String(_s->level_delta));
        return 1;
      }

      if (shStr == C_TIME_ZONE) {
        publish_sh_to_info_topic( shStr, String(_s->time_zone));
        return 1;
      }


      if (shStr == C_CSHM1 || shStr== C_CSHM2) {
        String sbits="";
        for(int i=0; i<=23; i++){
          if (i == 4 || i == 8 || i == 12 || i == 16 || i == 20) sbits += " ";
          shStr==C_CSHM1? sbits+=bitRead(_s->cb_schm1,23-i): sbits+=bitRead(_s->cb_schm2,23-i);
        }
        //publish_sh_to_info_topic( shStr, sbits);
        publish_to_info_topic(sbits);
        return 1;
      }

      if (shStr.startsWith(C_TEMP_MATRIX)) {
        if(shStr==C_TEMP_MATRIX){
          String outS="";
          for(int i=0; i<=23; i++){
            outS+=_s->temp_matix[i];
            outS+=":";
          }
          debug("CTEMP_MATRIX", outS);
          publish_to_info_topic(outS);
          return 1;
        }
        //debug("PUBLISHER",shStr);
        int point = shStr.indexOf(":");
        String testVal = shStr.substring(point + 1, shStr.length());
        int hour = testVal.toInt();
        if(hour<0 || hour>23) {
          publish_sh_err();
          return -1;
        }
        
        publish_sh_to_info_topic(shStr, String(_s->temp_matix[hour]));
        return 1;
      }




      if (shStr == C_EMAIL) {
        Serial.println("***** SH EMAIL");
        publish_sh_to_info_topic( shStr, String(_s->email_notify));
        return 1;
      }

      if (shStr == I_NET) {
        int rssdb = WiFi.RSSI();
        String ssid = WiFi.SSID();
        String ipstring = WiFi.localIP().toString();
        //Serial.println("rssdb="+String(rssdb)+", SSID="+String(ssid)+" ,ip="+ipstring);
        String outS = "SSID=" + ssid + ",rss=" + String(rssdb) + ",ip=" + String(ipstring);
        publish_to_info_topic(outS);
        return 1;
      }


      if (shStr.startsWith (I_BSCHM)) {
        String outS = "";
        //debug("PUBLISHER",shStr);
        int point = shStr.indexOf(":");
        String testVal = shStr.substring(point + 1, shStr.length());
        int numschm = testVal.toInt();
        //debug("PUBLISHER",String(numschm));
        if (numschm != 0 && numschm != 99 && numschm != 100) {
          for (int h = 0; h < 24; h++) {
            if (h == 4 || h == 8 || h == 12 || h == 16 || h == 20) outS += " ";
            uint l_p1 = lamp_modes[numschm - 1] & (1 << ((24 - h - 1) & 0xFFF));
            if (l_p1 == 0) outS += "0"; else outS += "1";
          }
          publish_to_info_topic(outS);
          return 1;
        }
        if (numschm == 99 || numschm == 100) {
          for (int i = 0; i <= 23; i++) {
            if (i == 4 || i == 8 || i == 12 || i == 16 || i == 20) outS += " ";
            if (numschm == 99) {
              if (_s->custom_scheme1[i]) outS += "1";
              else outS += "0";
            } else {
              if (_s->custom_scheme2[i]) outS += "1";
              else outS += "0";
            }
          }
          debug("PUBLISHER", outS);
          publish_to_info_topic(outS);
          return 1;
        }
      }

      if (shStr == I_TEMP_LEVELS || shStr == I_TEMP_LEVELS_ALIAS) {
        String outS = "C=" + String(_s->critical_temp_level) + ",D=" + String(_s->day_temp_level) + ",N=" + String(_s->night_temp_level) + " dT=" + String(_s->level_delta);
        publish_to_info_topic(outS);
        return 1;
      }

      if (shStr == I_RELAY1) {
         que_wanted->push(PUBLISHER_WANT_SH_R1);
         return 1;
      }

      if (shStr == I_RELAY2) {
         que_wanted->push(PUBLISHER_WANT_SH_R2);
         return 1;
      }

      if (shStr == I_TIME) {
         publish_to_info_topic("I:S="+String(is_time_synced())+",T="+String(hour())+":"+String(minute())+":"+String(year()));
         return 1;
      }

      if (shStr == I_SALT) {
         publish_sh_to_info_topic( shStr, String(_s->salt));
         return 1;
      }
      
      publish_sh_err();
      return 0;


    };

    int set_parameters_loop() {

      if (is_val_present == 0) {
        debug("SETPARAMSLOOP", "no cmd=val recognized");
        return 0;
      }

      //String _cStr=cmdStr;
      //String _vStr=valStr;

      if (cmdStr == C_TEST_B) {
        bool a;
        set_settings_val_bool(cmdStr, valStr, &a);
        return 1;

      }
      if (cmdStr == C_TEST_I) {
        int i;
        set_settings_val_int( cmdStr, valStr, &i, -30, 10);
        return 1;
      }

      if (cmdStr == C_AUTO_STOP_SEC) {
        if (set_settings_val_int(cmdStr, valStr, (int*) &_s->autostop_sec, MIN_AUTOSTOP_SEC, MAX_AUTOSTOP_SEC)) {
          return 2;
        }
        return 1;
      }

      if (cmdStr == C_AUTO_STOP_SEC2 ||  cmdStr == C_AUTO_STOP_SEC2T ) {

        if (set_settings_val_int(cmdStr, valStr, (int*) &_s->autostop_sec2, MIN_AUTOSTOP_SEC, MAX_AUTOSTOP_SEC)) {
          return 2;
        }
        return 1;
      }

      if (cmdStr == C_HOTTER) {
        bool b;
        if (set_settings_val_bool( cmdStr, valStr, &_s->hotter)) {
          if (_s->hotter) {
            //debug("SETHOTTER:", "set hotter to 1");
            _s->lscheme_num = 0;
            _s->autooff_hours = 0;
            _s->autostop_sec = 0;
            _s->cooler = 0;
          } else {
            // debug("SETHOTTER:", "set hotter to 0");
            /*supply.r1.reset_lschm_hour();
              supply.relay1_off("hotter=0");
            */
            que_wanted->push(PUBLISHER_WANT_R1_OFF);
          }
          return 2;
        }
        return 1;
      }

      if (cmdStr == C_COOLER) {
        if (set_settings_val_bool( cmdStr, valStr, &_s->cooler)) {
          if (_s->cooler) {

            _s->lscheme_num = 0;
            _s->autooff_hours = 0;
            _s->autostop_sec = 0;
            //debug("SETCOOLER","set hotter to 0");
            _s->hotter = 0;
            //debug("SETCOOLER", "hotter="+String(_s->hotter));
          } else {
            /*
              supply.r1.reset_lschm_hour();
              supply.relay1_off("cooler=0");
            */
            que_wanted->push(PUBLISHER_WANT_R1_OFF);
          }
          return 2;
        }
        return 1;
      }

      if (cmdStr == C_ON_START) {
        set_settings_val_bool( cmdStr, valStr, &_s->start_on);
        return 1;
      }

      if (cmdStr == C_BASELOG) {
        set_settings_val_bool( cmdStr, valStr, &_s->baselog);
        return 1;
      }

      if (cmdStr == C_NOTIFY_ONOFF) {
        set_settings_val_bool( cmdStr, valStr, &_s->notifyer_onoff);
        return 1;
      }

      if (cmdStr == C_TIME_ZONE) {
       if (set_settings_val_int(cmdStr, valStr, (int*) &_s->time_zone, -15, 15)) {
          return 2;
        }
        return 1;
      }


      if (cmdStr == C_AUTO_STOP_HOURS) {
        if (set_settings_val_int( cmdStr, valStr, (int*) &_s->autooff_hours, 0, MAX_AUTOOFF_HOURS)) {
          if (_s->autooff_hours > 0) {
            _s->lscheme_num = 0;
            _s->hotter = 0; // not together !
            _s->cooler = 0;
          }
        }
        return 1;
      }

      if (cmdStr == C_LIGHT_SCHEME_NUM) {
        if (set_settings_val_int( cmdStr, valStr, (int*) &_s->lscheme_num, 0, MAX_LSCHM_NUM)) {
          if (_s->lscheme_num > 0) {
            _s->hotter = 0; // not together !
            _s->cooler = 0;
            _s->autooff_hours = 0;
            //wanted_reset_hour_r1=1;
            //que_wanted->push(PUBLISHER_WANT_RESET_HOUR_R1);
          } else {
             //wanted_r1_off_lschm0=1;
             que_wanted->push(PUBLISHER_WANT_R1_OFF_LSCHM0);
            //supply.relay1_off("clight config");
          }
          //supply.r1.reset_lschm_hour();

        }
        return 1;
      }

      if (cmdStr == C_LIGHT_SCHEME_NUM2 || cmdStr == C_LIGHT_SCHEME_NUM2T  ) {
        set_settings_val_int(cmdStr, valStr, (int*) &_s->lscheme_num2, 0, MAX_LSCHM_NUM);
        if(_s->lscheme_num2==0) que_wanted->push(PUBLISHER_WANT_R2_OFF_LSCHM0);
        //supply.r2.reset_lschm_hour();
        return 1;
      }

      if (cmdStr == C_ANALOG1_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->analog_level_notify1, -128, MAX_ANALOG_NOTIFY);
        return 1;
      }

      if (cmdStr == C_ANALOG2_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->analog_level_notify2, -128, MAX_ANALOG_NOTIFY);
        return 1;
      }

      if (cmdStr == C_NOTIFY_HOURS_ONOFF) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->hours_on_notify, 1, MAX_HOURS_NOTIFY);
        return 1;
      }

      if (cmdStr == C_TARIFF) {
        if (set_settings_val_int( cmdStr, valStr, (int*) &_s->tariff, 0, MAX_PWR_TARIFF)) {
          //pwr_amount = 0;
        }
        return 1;
      }

      if (cmdStr == C_LDELTA) {
        if (set_settings_val_int( cmdStr, valStr, (int*) &_s->level_delta, 0, MAX_LEVEL_DELTA)) {}
        return 1;
      }

      if (cmdStr == C_NOTIFYER) {
        set_settings_val_bool( cmdStr, valStr, &_s->notifyer);
        return 1;
      }

      if (cmdStr == C_DTEMP_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->day_temp_level, MIN_TEMP_LEVEL, MAX_TEMP_LEVEL);
        return 1;
      }

      if (cmdStr == C_NTEMP_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->night_temp_level, MIN_TEMP_LEVEL, MAX_TEMP_LEVEL);
        return 1;
      }

      if (cmdStr == C_CTEMP_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->critical_temp_level, MIN_TEMP_LEVEL, MAX_TEMP_LEVEL);
        return 1;
      }

      if (cmdStr == C_TIME_ZONE) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->time_zone, -12, 14);
        return 1;
      }


      if (cmdStr == C_ANALOG_LEVEL1) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->analog_level1, -128, MAX_ANALOG_LEVEL);
        return 1;
      }

      if (cmdStr == C_ANALOG_LEVEL2) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->analog_level2, -128, MAX_ANALOG_LEVEL);
        return 1;
      }

      if (cmdStr == C_LOW_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->temp_low_level_notify, -128, MAX_TEMP_LEVEL);
        return 1;
      }

      if (cmdStr == C_HIGH_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->temp_high_level_notify, -128, MAX_TEMP_LEVEL);
        return 1;
      }

      if (cmdStr == C_CUSTOM1_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->custom_level_notify1, -128, MAX_NOTIFY_CUSTOM);
        return 1;
      }

      if (cmdStr == C_CUSTOM2_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->custom_level_notify2, -128, MAX_NOTIFY_CUSTOM);
        return 1;
      }

      if (cmdStr == C_CUSTOM3_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->custom_level_notify3, -128, MAX_NOTIFY_CUSTOM);
        return 1;
      }

      if (cmdStr == C_CUSTOM4_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->custom_level_notify4, -128, MAX_NOTIFY_CUSTOM);
        return 1;
      }

      if (cmdStr == C_ONOFF_SCHEME1) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->schm_onoff_num1, 0, MAX_LSCHM_NUM);
        return 1;
      }

      if (cmdStr == C_ONOFF_SCHEME2) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->schm_onoff_num2, 0, MAX_LSCHM_NUM);
        return 1;
      }

      if (cmdStr == C_ONOFF1_VAL || cmdStr == C_ONOFF2_VAL) {
        int err=0;
        int point=valStr.indexOf(":");
        int len=valStr.length();
        String  hStr=valStr.substring(0,point);
        String  vStr=valStr.substring(point+1,len);
        int _h=hStr.toInt();
        int  _v=vStr.toInt();
        
        if(hStr!="0" && _h==0) err=1;
        if(vStr!="0" && _v==0) err=1;
        if(_h<0 || _h>23) err=1;
        if(_v<0 || _v>1) err=1;
        
        debug("ONOFFV", "hStr="+hStr+"; "+"vStr="+vStr+"; err="+String(err));
        if(err==0){    
            if(cmdStr==C_ONOFF1_VAL) _s->custom_scheme1[_h]=_v;
            if(cmdStr==C_ONOFF2_VAL) _s->custom_scheme2[_h]=_v;
            publish_to_info_topic("N:V["+String(_h)+"]"+"="+String(_v));
            return 1;
        }

        publish_to_info_topic("E:params error");
        return 0;
      }

      if (cmdStr == C_CSHM1 || cmdStr == C_CSHM2) {
        int err=0;
        int point=valStr.indexOf(":");
        int len=valStr.length();
        String  hStr=valStr.substring(0,point);
        String  vStr=valStr.substring(point+1,len);
        int _h=hStr.toInt();
        int  _v=vStr.toInt();
        
        if(hStr!="0" && _h==0) err=1;
        if(vStr!="0" && _v==0) err=1;
        if(_h<0 || _h>23) err=1;
        if(_v<0 || _v>1) err=1;
        
        debug("C_CSCHM", "hStr="+hStr+"; "+"vStr="+vStr+"; err="+String(err));
        if(err==0){    
            if(cmdStr==C_CSHM1) bitWrite(_s->cb_schm1,23-_h,_v);
            if(cmdStr==C_CSHM2) bitWrite(_s->cb_schm2,23-_h,_v);
            publish_to_info_topic("N:V["+String(_h)+"]"+"="+String(_v));
            return 1;
        }

        publish_to_info_topic("E:params error");
        return 0;
      }

    };
/*
    int commands_loop(){
      
      if (incomingStr == C_CUSTOM4_NOTIFY_LEVEL) {
        set_settings_val_int( cmdStr, valStr, (int*) &_s->custom_level_notify4, -128, MAX_NOTIFY_CUSTOM);
        return 1;
      }
    };
*/
    int set_settings_val_bool(String _command, String _valStr, bool* _setting_val) {
      bool recognize = 0;
      if (_valStr.startsWith("0") && _valStr.length() == 1) {
        *_setting_val = 0;
        recognize = 1;
      }
      if (_valStr.startsWith("1") && _valStr.length() == 1) {
        *_setting_val = 1;
        recognize = 1;
      }
      if (recognize) {
        publish_to_info_topic(String("N:" + _command + "=" + String(*_setting_val)).c_str());
        return 1;
      }
      publish_to_info_topic("E:not set, 01 wait");
      return 0;
    };

    int set_settings_val_int(String _command, String _valStr, int* _setting_val,  int _min, int _max) {
      int test_val;
      bool recognize = 0;
      if (_valStr.startsWith("0") && _valStr.length() == 1) {
        test_val = 0;
        recognize = 1;
      }
      test_val = _valStr.toInt();
      if (recognize == 0 && test_val != 0) recognize = 1;
      if (recognize == 0) {
        publish_to_info_topic("E:not set, INT wait");
        return 0;
      }

      if (test_val >= _min && test_val <= _max) {
        *_setting_val = test_val;
        publish_to_info_topic(String("N:" + _command + "=" + String(test_val)).c_str());
        return 1;
      }

      publish_to_info_topic(String("E:expected " + String(_min) + "<>" + String(_max)).c_str());
      return 0;

    };

    int virtual publish_sh_to_info_topic(String shStr, String _valStr) =0;


    int virtual publish_to_info_topic(String _valStr) =0;

    int virtual clear_info_channel(){publish_to_info_topic("                          ");};

    int virtual publish_tlevels_to_info(){
            String infostr = "";
            infostr += "h=" + String(_s->hotter) + ";";
            infostr += "ct=" + String(_s->critical_temp_level) + "; ";
            infostr += "dt=" + String(_s->day_temp_level) + "; ";
            infostr += "nt=" + String(_s->night_temp_level) + "; ";
            infostr += "tdelta=" + String(_s->level_delta) + "; ";
            infostr += "schm=" + String(_s->schm_onoff_num1) + ";";
            publish_to_info_topic(infostr);
    };

};

#endif

