 
#ifndef donoffsettings_h
#define donoffsettings_h

#include "Arduino.h"

#define MAX_QUEUE_WANTED 12

#define WIFI_CONFIG_AP_NAME "donoff-ap"

#define D_MQTT

#ifdef D_MQTT
const char INFO_CHANNEL[] = "/out/info";
const char LOG_CHANNEL[] = "/out/log";
const char UPTIME_CHANNEL[] = "/out/time_up";

const char PARAMS_CHANNEL[]="/in/params";

const char TOPIC_SENDMAIL[] = "/sys/sendmail";

const char TOPIC_SENSOR_BASELOG[] = "/sys/sensor_baselog";

#endif



#ifdef D_BLYNK

const char INFO_CHANNEL[] = "29";
const char LOG_CHANNEL[] = "34";
const char UPTIME_CHANNEL[] = "19";


const char RELAY1_ONOFF_CHANNEL[] = "6";
const char RELAY1_ONTIME_CHANNEL[] = "12";
const char RELAY1_DOWNTIME_CHANNEL[] = "13";
const char DS_IN_CHANNEL[] = "11";
const char DS_OUT_CHANNEL[] = "8";

const char RELAY2_ONOFF_CHANNEL[] = "36";

#endif

#define NTP_SERVER_1 "0.ru.pool.ntp.org"
#define NTP_SERVER_2 "pool.ntp.org"
#define NTP_SERVER_3 "time.nist.gov"


enum pub_events { 
 PUBLISHER_WANT_SAVE,
 PUBLISHER_WANT_R1_ON,
 PUBLISHER_WANT_R1_OFF,
 PUBLISHER_WANT_R2_ON,
 PUBLISHER_WANT_R2_OFF,
 PUBLISHER_WANT_RESET,
 PUBLISHER_WANT_RESET_HOUR_R1,
 PUBLISHER_WANT_RESET_HOUR_R2,
 PUBLISHER_WANT_R1_OFF_LSCHM0,
 PUBLISHER_WANT_R2_OFF_LSCHM0,
 PUBLISHER_WANT_RESET_HOUR_R1_M,
 PUBLISHER_WANT_RESET_HOUR_R2_M,
 PUBLISHER_WANT_SH_R1,
 PUBLISHER_WANT_SH_R2,
 PUBLISHER_WANT_SAY_JUST_SYNCED
};

#define DS1820_NOT_FILTERED 0
#define DS1820_FILTERED 1


enum sensor_states {
 UNDEF_STATE,
 LOW_STATE,
 HIGH_STATE,
 NORM_STATE,
 NORM_FROMLOW,
 NORM_FROMHIGH,
 ERR_STATE
};


/* sensors types */

#define SENSOR_TYPE_TEMPERATURE 1
#define SENSOR_TYPE_CURRENT 2
#define SENSOR_TYPE_POWER 3
#define SENSOR_TYPE_VOLTAGE 4
#define SENSOR_TYPE_ENERGY 5


/* need to write to database*/

#define  BASELOG_NO 0
#define  BASELOG_YES 1



/* 
#define MAX_QUEUE_WANTED 12
#define PUBLISHER_WANT_SAVE 1
#define PUBLISHER_WANT_R1_ON 2
#define PUBLISHER_WANT_R1_OFF 3
#define PUBLISHER_WANT_R2_ON  4
#define PUBLISHER_WANT_R2_OFF 5
#define PUBLISHER_WANT_RESET 6
#define PUBLISHER_WANT_RESET_HOUR_R1 7
#define PUBLISHER_WANT_RESET_HOUR_R2 8
#define PUBLISHER_WANT_R1_OFF_LSCHM0 9
#define PUBLISHER_WANT_R2_OFF_LSCHM0 10
#define PUBLISHER_WANT_RESET_HOUR_R1_M 7
#define PUBLISHER_WANT_RESET_HOUR_R2_M 8
*/


/******** config defines *******/

#define WIFI_POINT "donoff-mq"
#define RECONNECT_PERIOD 30
#define IGNORE_FIRST_MSGS
#define CONFIG_B_DELAY 3
#define RESET_DEFAULT_DELAY 10
#define ANALOG_PWR_PERIOD 600
#define ALARM_TEMP1_MAX 80
#define  BUTTON_MIN_DELAY 5
#define PR_DIFF 317
#define MAX_LSCHEME_MODES 20

#define DALLAS_RES_IN 9
#define DALLAS_RES_OUT 10
#define DALLAS_RES_OUT2 9

#define FLASH_TEMPLATE1

//************************** PINS **********************************************


#ifdef PINS_SET_V1
    #define BUTTON_PIN   0      //d3
    #define RELAY1_PIN    5   //d1  
    #define SONOFF_LED      2  //d4
    #define IN_WIRE_BUS    14  //d5
    #define OUT_WIRE_BUS 4 //D2
    #define ANALOG_A0_PIN A0
    #define RELAY2_PIN 12 //new d6 //old d8
#endif

#ifdef PINS_SET_V2
    #define BUTTON_PIN   0      //d3
    #define RELAY1_PIN    12   //d6  
    #define SONOFF_LED      2  //d4
    #define IN_WIRE_BUS    14  //d5
    #define OUT_WIRE_BUS 13 //D7
    #define ANALOG_A0_PIN A0
    #define RELAY2_PIN 5 //new d6 //old d8
#endif


//************** DEBUG  ***************************

#define DEBUG_CONN 0
#define DEBUG_ADC 1
#define DEBUG_ADCPOWER 0
#define DEBUG_LSCHM 1
#define DEBUG_CURRENT 1
#define DEBUG_TEMP 1
#define DEBUG_PRESSURE 1
//#define DEBUG_MYPUBLISH

#define FAKE_NOTIFY 0


//************* OTHER

#define MIN_AUTOSTOP_SEC 0
#define MAX_AUTOSTOP_SEC 1200
#define MAX_AUTOOFF_HOURS 24
#define MAX_LSCHM_NUM 100
#define MAX_ANALOG_NOTIFY 50000
#define MAX_HOURS_NOTIFY 1000
#define MAX_PWR_TARIFF 10000
#define MAX_LEVEL_DELTA 50000
#define MIN_TEMP_LEVEL -5000
#define MAX_TEMP_LEVEL 10000
#define MAX_ANALOG_LEVEL 16000
#define MAX_NOTIFY_CUSTOM 5000

#define MIN_CUSTOM_LEVEL -32000
#define MAX_CUSTOM_LEVEL 32000
#define DONOFF_DEBUG_PUBLISH

/*
typedef struct  {
  float preprev=-128;
  float prev=-128;
  float curr=-128;
  float val=-128;
}tempsensor;
*/

typedef struct{
    long no_sensor_val = -12700;   
    long not_ready_val = -12800;
    long  start_delay = 0;
    int need_asking = 0;
    uint need_filtered = 0;
    uint request_circle = 750;
    uint multiplier = 1;
    uint priority=1;
    ulong ask_circle_ms=300;
} sensor_settings;

typedef struct{
    uint priority=1;
    ulong ask_circle_ms=300;
    enum sensor_type {SENSOR,RELAY,PUBLISHER,NOTIFYER};
   // DSensor * sensor;
} dtask;

typedef struct {
  uint what_to_notify;
  String reason;
} notify_task;


typedef struct {
  long v[3];
  long val;
 
  
} sensor_data;


#define EEPROM_SALT 1206
//#define EEPROM_SALT 1205
//#define EEPROM_SALT 1206

#define MEM_SIZE 512

typedef struct {
  
  // ** from web config
  int   salt = EEPROM_SALT;
  char  mqttServer[22]  = "mqtt.lab240.ru";
  char  mqttUser[12] = "";
  char  mqttPass[22]    = "";
  char  mqttPort[6]="1883";
  char  autooff[2] = "";
  char  dev_id[10]="empty_dev";

  // *** params config
  uint      autostop_sec=0;
  uint      autostop_sec2=0;
  uint      autooff_hours=0;
  bool      reverse=0;
  bool      notifyer_onoff=0;
  bool      current_check=1;
  uint      lscheme_num=0;
  uint      lscheme_num2=0;
  uint      time_zone=3; //3 for Moscow //2 for Riga
  bool      start_on=0;
//                             0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  bool     custom_scheme1[24]={0,0,0,0,0,0,0,0,0,0,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0};
  bool     custom_scheme2[24]={1,1,1,1,1,1,1,1,1,1,1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //uint      custom_scheme2=0B101010101010101010101010;
  int      custom_level_notify1=-128;
  bool     baselog=1;
  
// ** hotter \ cooler 
  bool      hotter=0;
  bool      cooler=0;
  uint      critical_temp_level=20;
  uint      day_temp_level=18;
  uint      night_temp_level=24;
  uint      schm_onoff_num1=99;
  uint      schm_onoff_num2=0;
  uint      level_delta=10;  //delta in Celsius*10

  bool      notifyer=0;
  char      email_notify[50]="dj.novikov@gmail.com";
  uint      hours_on_notify=6; //notify period in hours
  int      temp_low_level_notify=-128;
  int      analog_level_notify1=-128;
  int      analog_level_notify2=-128;
  int      temp_high_level_notify=-128;
  uint      autooff_hours2=0;
// ** 
  int      custom_level_notify2=-128;
  int      custom_level_notify3=-128;
  int      custom_level_notify4=-128;
  byte      urgent_off=75;
  uint      tariff=500;      // копееек \ центов за 1квт\ч
  unsigned long      pwr_amount=0;  //счетчик потребления в рублях\долл\евр
  uint weekschm[7]={99,99,99,99,99,99,99};
  //*****
  int     analog_level1=-128;
  int     analog_level2=-128;
  int     custom_level1=-128;
  int     custom_level2=-128;
  //uint    hotter2=0;
  //int     custom_hotter2[24]={22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22};
  uint cb_schm1=0B000000000011111111111100;
  uint cb_schm2=0B111111111111100000000000;
  int     temp_matix[24]={22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21};
} WMSettings;

#define WIFI_EEPROM_SALT 111

typedef struct{
  int  SALT = WIFI_EEPROM_SALT;
  char ssid[20];
  char pass[50];
} WifiCreds;


/********** blink ****************/

#define BL_CONNECTED_ON 0
#define BL_CONNECTED_OFF 1
#define BL_CONNECTING 2
#define BL_CONFIG 2
#define BL_OFFLINE_ON 5
#define BL_OFFLINE_OFF 4



const uint8_t modes[] = {
  0B00000000, //Светодиод выключен
  0B11111111, //Горит постоянно
  0B00001111, //Мигание по 0.5 сек
  0B00000001, //Короткая вспышка раз в секунду
  0B00000101, //Две короткие вспышки раз в секунду
  0B00010101, //Три короткие вспышки раз в секунду
  0B01010101,  //Частые короткие вспышки (4 раза в секунду)
  0B11111110
};


/****** light **********/
const uint lamp_modes[] = {
   0B000000000000000000000000, //1
   0B111111111111111111111111, //2   every hour
   0B000000000000000000001111, //3   20-24
   0B111111000000000000111111, //4   0-6 \ 18-23
   0B000000011111111111111110, //5   7-22 
   0B000001111111111111111110, //6   5-22
   0B000000111100000000111110, //7   6-9 \ 18-22
   0B000011111000000000111110, //8   4-8 \ 18-22
   0B010101010101010101010101, //9   
   0B101010101010101010101010, //10   10-22, every 2 ours
   0B111111111111000000001111,  //11   0-11 \ 20-23 
   0B001001001001001001001001,  //12  every 3 hours
   0B000100010001000100010001,  //13  every 4 hours
   0B000010000100001000010001,  //14  every 5 hours
   0B000000010000000100000001,  //15  3 times
   0B000000000010000000000001,  //16  2 times
   0B000000000000000000000010,   //17  2 times
   0B111111111110000000000011,   //18  2 times
   0B001111111111111000000000  //19 2 times

}; 




#endif
