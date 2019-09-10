#ifndef donoffbutton_h
#define donoffbutton_h

//#include <donoffrelay.h>
//#include <donoffdisplay.h>
//#include <donoffpublisher.h>

#define BUTTON_MIN_DELAY 50
#define BUTTON_DELAY 50
#define SHORT_PRESS 1

class DButton: public DBase {
  private:
    int pin;
    Bounce debouncer;
    int init_ok = 0;
    ulong start_bt = 0;
    const int short_press_delay = 2000;
    const int long_press_delay = 4000;


  public:
    DButton(WMSettings * __s): DBase(__s), debouncer() {};

    int init() {
      debouncer.attach(BUTTON_PIN);
      debouncer.interval(5);
      init_ok = 1;
    };

    int button_loop() {
      debouncer.update();
      if ( debouncer.fell()  ) {
        debug("BUTTON", "down");
        start_bt = millis();
      }

      if ( debouncer.rose()  ) {
        long duration = millis() - start_bt;
        start_bt = 0;
        debug("BUTTON", "up" + String(duration));

        if (duration < BUTTON_MIN_DELAY) {
          debug("BUTTON", "FAIL PRESS, DO NOTHING");
          return 0;
        } else if (duration >= BUTTON_DELAY && duration < short_press_delay) {
          debug("BUTTON", "--->short press - toggle relay");
          return SHORT_PRESS;

        } else if (duration >= long_press_delay) {
          //config here
        }

      }
    }




};


#endif