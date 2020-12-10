#ifndef donoffbase_h
#define donoffbase_h

class DBase {
  protected:
    int debug_level = 1;
    WMSettings * _s;
  public:
    DBase(WMSettings * __s) {
      _s = __s;
    };

    void debug(String sourceStr, String debugStr) {
      String debug_outStr = "DEBUG:" + sourceStr + ":" + debugStr;
      if (debug_level > 0) Serial.println(debug_outStr);
    };

 String get_time_str(unsigned long val) {
      //debug("BASETIMESTR", String(val));
      String outStr;
      ulong wt = val / 1000;
      uint h = wt / 3600;
      wt = wt % 3600;
      uint m = wt / 60;
      uint s = wt % 60;
      if(h<10) outStr="0"+String(h); else outStr=String(h);
      outStr+=":";
      if(m<10) outStr+="0"+String(m); else outStr+=String(m);
      outStr+=":";
      if(s<10) outStr+="0"+String(s); else outStr+=String(s);
      
      return outStr;
    };

 String get_uptime_str(){
        return get_time_str(millis());
    };

int virtual save(){
     EEPROM.begin(MEM_SIZE);
     EEPROM.put(0, *_s);
     EEPROM.end();     
};
int load(){
    EEPROM.begin(MEM_SIZE);
    EEPROM.get(0, *_s);
    EEPROM.end();
}

String s_get_timestamp(){
    String s_timestamp;
    s_timestamp=String(year());
    s_timestamp+="-";
    s_timestamp+= month()<10? "0"+String(month()): String(month());
    s_timestamp+="-";
    s_timestamp+= day()<10? "0"+String(day()): String(day());
    s_timestamp+="T";
    s_timestamp+= hour()<10? "0"+String(hour()): String(hour());
    s_timestamp+=":";
    s_timestamp+= minute()<10? "0"+String(minute()): String(minute());
    s_timestamp+=":";
    s_timestamp+= second()<10? "0"+String(second()): String(second());
    s_timestamp+="Z";
    return s_timestamp;
};

int virtual reset(){
  ESP.reset();
}


};

#endif
