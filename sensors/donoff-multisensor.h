#ifndef donoffmultisensor_h
#define donoffmultisensor_h

#include <donoffbase.h>
#include <sensors/donoffsensor.h>
#include <donoffsettings.h>



class DMultiSensor;

class DMultiSensor: public DBase {
  protected:
   String nameStr = "unknown";
   String channelStr="unknown";
   uint type;

  public:
    DMultiSensor(WMSettings * __s): DBase(__s) {};

    void virtual init(String _name, String _chname, int _type, Queue<sensor_state> *_que_sensor_states){
      nameStr=_name;
      channelStr=_chname;
      type=_type;
  
    };


    virtual String multi_json_Str()=0;

    virtual String get_channelStr(){
      return channelStr;
    }


};

    
#endif
