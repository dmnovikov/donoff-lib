#ifndef donoffbutton_h
#define donoffbutton_h

#include <Bounce2.h>
#include <donoffbase.h>
#include <donoffsettings.h>

//#include <donoffrelay.h>
//#include <donoffdisplay.h>
//#include <donoffpublisher.h>

#define BUTTON_MIN_DELAY 25
//#define BUTTON_DELAY 50
#define SHORT_PRESS 1
#define CONFIG_PRESS 2
#define FAIL_PRESS 4
#define START_PRESS 3

class DButton: public DBase {
  private:
    // int pin;
    Bounce debouncer;
    ulong b_duration=0;
    //Button myBtn;   
    int init_ok = 0;
    ulong start_bt = 0;
    const int SHORT_PRESS_DELAY = 2000;
    const int CONFIG_PRESS_DELAY = 6000;
    const int RESET_PRESS_DELAY = 8000;
    // bool b_state=HIGH;
    bool b_state;


  public:
    DButton(WMSettings * __s): DBase(__s), debouncer() {};

    void init() {
      pinMode(BUTTON_PIN, INPUT);
      debouncer.attach(BUTTON_PIN, INPUT_PULLUP);
      debouncer.interval(5);
      init_ok = 1;
      debug("BUTTON", "Init ok");
    };

    int button_loop() {  
      if (!init_ok) return 0;
      debouncer.update();
      if (debouncer.read()== LOW) {
        b_duration=debouncer.currentDuration();
    
      }
  
      if(debouncer.rose()){
        debug("BUTTON", "Duration->"+String(b_duration));
        return button_state(b_duration);
      }
      return 0;
    };

   

    int button_state(uint duration){
        if (duration > 0 && duration < BUTTON_MIN_DELAY) {
          debug("BUTTON", "FAIL PRESS, DO NOTHING");
          return FAIL_PRESS;
        } else if (duration >= BUTTON_MIN_DELAY && duration < SHORT_PRESS_DELAY) {
          debug("BUTTON", "--->short press - toggle relay");
          return SHORT_PRESS;

        } else if (duration >= SHORT_PRESS_DELAY && duration < CONFIG_PRESS_DELAY) {
           debug("BUTTON", "--->long press - config");
           return CONFIG_PRESS;

        }else if (duration >=  RESET_PRESS_DELAY) {
           debug("BUTTON", "--->long press - reset");
           reset();
           return -1;
        }

        return 0;

    };


};


#endif