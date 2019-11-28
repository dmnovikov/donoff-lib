#ifndef ddisplay_h
#define ddisplay_h

#include <donoffsettings.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define  D_FIRST_STRING 1 
#define  D_LAST_STRING 1

class DDisplay:public DBase{
 public:
  int virtual init(){};
  DDisplay(WMSettings * __s): DBase(__s) {};
  int virtual show_sensor(int ppos, String name, long data,int mult,  bool isFirst, bool isLast){};
  int virtual show_sensor(int ppos, String short_name, DSensor* _ps,  bool isFirst, bool isLast){};
  int virtual show_str(int ppos, String name, String data,  bool isFirst, bool isLast){};
};

class DDisplay_SSD1306 :public DDisplay{
  protected:
   int SDA_PIN=5; //D1
   int SCL_PIN=4; //D2
   const int OLED_RESET=0;  
   const int NUMFLAKES=10;
   const int XPOS=0;
   const int YPOS=1;
   const int DELTAY=2;
   const int LOGO16_GLCD_HEIGHT=16;
   const int LOGO16_GLCD_WIDTH=16;
   int debug_level = 1;
   int pos=0;
   
   Adafruit_SSD1306 display;
   
  public:
    DDisplay_SSD1306(WMSettings * __s): DDisplay(__s), display(0) {};

    int virtual init(){
      
      display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
      display.display();
      display.clearDisplay();
      // text display tests
      display.setTextSize(0);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("hi, donoff"); 
      display.display();
      debug("DISPLAY", "init ok");
      
    };

    int virtual show_sensor(int ppos, String name, long data,int mult,  bool isFirst, bool isLast){
      String outS;
      if(isFirst){
        display.clearDisplay();
        pos=0;
      }
      display.setCursor(0,pos*10);
      //debug("DISPLAY", "pos="+String(pos)+"; first="+String(isFirst));

      if(mult!=0) outS=name+String(float(data)/float(mult)); else outS=name+String(data);
      //debug("DISPLAY",outS);
      display.println(outS);
      display.display();
      pos++;
    };

     int virtual show_sensor(int ppos, String short_name, DSensor* _ps,  bool isFirst, bool isLast){
      String outS;
      if(isFirst){
        //debug("DISPLAY", "CLEAR DISPLAY");
        display.clearDisplay();
        pos=0;
      }
      display.setCursor(0,pos*10);
      //debug("DISPLAY", "pos="+String(pos)+"; first="+String(isFirst));
      if(_ps->is_ready() && _ps->is_started()  )
         outS=short_name+":"+_ps->get_val_Str();
      else outS=short_name+":nc";
      
      display.println(outS);
      display.display();
      pos++;
    };

     int virtual show_str(int ppos, String name, String data,  bool isFirst, bool isLast){
      String outS;
      if(isFirst){
        //debug("DISPLAY", "CLEAR DISPLAY");
        display.clearDisplay();
        pos=0;
      }
      display.setCursor(0,pos*10);
      //debug("DISPLAY", "pos="+String(pos)+"; first="+String(isFirst));

      outS=name+data;
      //debug("DISPLAY",outS);
      display.println(outS);
      display.display();
      pos++;
      
    };
    
    
    // void debug(String debugStr) {
    //   String debug_outStr = "DEBUG:<UNDEF SOURCE>:" + debugStr;
    //   if (debug_level > 0) Serial.println(debug_outStr);
    // };
    // void debug(String sourceStr, String debugStr) {
    //   String debug_outStr = "DEBUG:" + sourceStr + ":" + debugStr;
    //   if (debug_level > 0) Serial.println(debug_outStr);
    // };

};



#endif
