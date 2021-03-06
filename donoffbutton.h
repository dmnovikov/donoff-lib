#ifndef donoffbutton_h
#define donoffbutton_h

#include <Bounce2.h>

//#include <donoffrelay.h>
//#include <donoffdisplay.h>
//#include <donoffpublisher.h>

#define BUTTON_MIN_DELAY 25
//#define BUTTON_DELAY 50
#define SHORT_PRESS 1
#define CONFIG_PRESS 2
#define FAIL_PRESS 0
#define START_PRESS 3

class DButton: public DBase {
  private:
    // int pin;
    Bounce debouncer;
    //Button myBtn;   
    int init_ok = 0;
    ulong start_bt = 0;
    const int SHORT_PRESS_DELAY = 1000;
    const int CONFIG_PRESS_DELAY = 7000;
     const int RESET_PRESS_DELAY = 8000;
    // bool b_state=HIGH;
    bool b_state;


  public:
    DButton(WMSettings * __s): DBase(__s), debouncer() {};

    int init() {
      pinMode(BUTTON_PIN, INPUT);
      debouncer.attach(BUTTON_PIN, INPUT_PULLUP);
      debouncer.interval(5);
      init_ok = 1;
    };

    int button_loop() {
      
      
      if (!init_ok) return 0;
      
      // debug("DBUTTON_LOOP", "LOOP");
      b_state=debouncer.update();
      //if ( debouncer.fell()) {

     
      if ( debouncer.rose()) {
       #ifdef BUTTON_REVERSE
        return button_pushed();    
       #else
         return button_freed();
       #endif
       }

      //if ( debouncer.rose()  ) {
      if ( debouncer.fell()  ) {
      #ifdef BUTTON_REVERSE
        return button_freed();
      #else
        return button_pushed();    
      #endif
      }


    };

    int  button_pushed(){
      debug("BUTTON", "DETECT:START!");
      start_bt = millis();
      return START_PRESS;
    };

    int button_freed(){
        long duration = millis() - start_bt;
        debug("BUTTON", "DETECT:FREED, FREE:" + String(millis())+", PUSHED="+String(start_bt)+", DURATION="+String(duration/1000));
        if(start_bt==0) return 0;
        start_bt=0;
        if (duration < BUTTON_MIN_DELAY) {
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
        }

    };
    


};


#endif