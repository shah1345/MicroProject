#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP32WebServer.h>
#include <ArduinoJson.h>
#include <Preferences.h>




#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <time.h>

#include <SPI.h>
#include <MFRC522.h>

#include "Button2.h"

#include "BluetoothSerial.h"


Preferences preferences;


String hostname = "SHAH_X_Coffee";
const char* ssid = "Jamuna Tech Dhaka";
const char* password = "jAmuna2022";
unsigned long now = millis();
unsigned long WpreviousMillis = 0;
unsigned long Winterval = 10000;

String FirmwareVer = {
  "1.2"
};

String D_ID = {
  "1"
};


int Step = 1;
int Sugger=0;
int Type =0;

int Tvel=95;

int temppp= random(40, 45);
 

 int sensorStatus=0;
String serverFailed = "http://binimoy.shahxauto.com/Petrol/postJson.php?DivID=" + D_ID;
String serverName = "https://pump.shahxauto.com/Coffee/Api.php?DivID=" + D_ID + "&FirVer=" + FirmwareVer;
String CardID ="";

void connect_wifi();

ESP32WebServer server(80);
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=18*/ 14, /* data=23*/ 27, /* CS=5*/ 26, /* reset=*/ 25); // ESP32



void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println();
  Serial.println("Connected to Router successfully!");
  
  buzzer(300);
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 

     u8g2.firstPage();
     do{

Wificon(WiFi.localIP().toString());

} while (u8g2.nextPage());


  
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  buzzer(150);
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");


     u8g2.firstPage();
     do{

WifiError(info.disconnected.reason);

} while (u8g2.nextPage());



  WiFi.begin(ssid, password);
}















#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

int temp=0;
#ifdef U8X8_HAVE_HW_SPI
//#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//RF 5 18 19 22
//Button
#define Buzzer  32  //done
#define B_High  33 //done 
#define B_Mid  34 //done
#define B_Low  39  //done
#define B_Start  35      //16 done



//Sensor
#define Temprature  17
#define IRSensor  4


#define ADC_VREF_mV    3300.0 // in millivolt
#define ADC_RESOLUTION 4096.0

//Pump And AC
#define Pump1  15//12//39 //39 ////done
#define Pump2  2 //big pump
//#define Pump3  12
#define Relay  13


Button2 buttonH, buttonM, buttonL, buttonS;



float tempC; // temperature in Celsius


String Id[]={"211111030","19510466146","24120420428"};

#define SS_PIN  5 //5 SCK-GPIO 18, MOSI-GPIO 19, MISO-GPIO 23, Vcc-3.3v, GND -GND,
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance.
MFRC522::MIFARE_Key key; 
// Init array that will store new NUID 
byte nuidPICC[4];
String OldCardID = "";
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

void  DisplayHandeler(int Type);

void handler(Button2& btn);

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
};

Button button_boot = {
  0,
  0,
  false
};


char temperature [5];
char humidity [5];
const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

// 25 33 26 22
//U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=18*/ 27, /* data=23*/ 26, /* CS=5*/ 12, /* reset=*/ 14); // ESP32



void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void u8g2_box_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawBox");
  u8g2.drawBox(5,10,20,10);
  u8g2.drawBox(10+a,15,30,7);
  u8g2.drawStr( 0, 30, "drawFrame");
  u8g2.drawFrame(5,10+30,20,10);
  u8g2.drawFrame(10+a,15+30,30,7);
}

void u8g2_disc_circle(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawDisc");
  u8g2.drawDisc(10,18,9);
  u8g2.drawDisc(24+a,16,7);
  u8g2.drawStr( 0, 30, "drawCircle");
  u8g2.drawCircle(10,18+30,9);
  u8g2.drawCircle(24+a,16+30,7);
}

void u8g2_r_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawRFrame/Box");
  u8g2.drawRFrame(5, 10,40,30, a+1);
  u8g2.drawRBox(50, 10,25,40, a+1);
}

void u8g2_string(uint8_t a) {
  u8g2.setFontDirection(0);
  u8g2.drawStr(30+a,31, " 0");
  u8g2.setFontDirection(1);
  u8g2.drawStr(30,31+a, " 90");
  u8g2.setFontDirection(2);
  u8g2.drawStr(30-a,31, " 180");
  u8g2.setFontDirection(3);
  u8g2.drawStr(30,31-a, " 270");
}

void u8g2_line(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawLine");
  u8g2.drawLine(7+a, 10, 40, 55);
  u8g2.drawLine(7+a*2, 10, 60, 55);
  u8g2.drawLine(7+a*3, 10, 80, 55);
  u8g2.drawLine(7+a*4, 10, 100, 55);
}

void u8g2_triangle(uint8_t a) {
  uint16_t offset = a;
  u8g2.drawStr( 0, 0, "drawTriangle");
  u8g2.drawTriangle(14,7, 45,30, 10,40);
  u8g2.drawTriangle(14+offset,7-offset, 45+offset,30-offset, 57+offset,10-offset);
  u8g2.drawTriangle(57+offset*2,10, 45+offset*2,30, 86+offset*2,53);
  u8g2.drawTriangle(10+offset,40+offset, 45+offset,30+offset, 86+offset,53+offset);
}

void u8g2_ascii_1() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 1");
  for( y = 0; y < 6; y++ ) {
    for( x = 0; x < 16; x++ ) {
      s[0] = y*16 + x + 32;
      u8g2.drawStr(x*7, y*10+10, s);
    }
  }
}

void u8g2_ascii_2() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 2");
  for( y = 0; y < 6; y++ ) {
    for( x = 0; x < 16; x++ ) {
      s[0] = y*16 + x + 160;
      u8g2.drawStr(x*7, y*10+10, s);
    }
  }
}

void u8g2_extra_page(uint8_t a)
{
  u8g2.drawStr( 0, 0, "Unicode");
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.setFontPosTop();
  u8g2.drawUTF8(0, 24, "☀ ☁");
  switch(a) {
    case 0:
    case 1:
    case 2:
    case 3:
      u8g2.drawUTF8(a*3, 36, "☂");
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      u8g2.drawUTF8(a*3, 36, "☔");
      break;
  }
}

#define cross_width 24
#define cross_height 24
static const unsigned char cross_bits[] U8X8_PROGMEM  = {
  0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 
  0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80, 
  0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03, 
  0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00, };

#define cross_fill_width 24
#define cross_fill_height 24
static const unsigned char cross_fill_bits[] U8X8_PROGMEM  = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26, 
  0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08, 
  0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01, 
  0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04, 
  0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21, 
  0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define cross_block_width 14
#define cross_block_height 14
static const unsigned char cross_block_bits[] U8X8_PROGMEM  = {
  0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 
  0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 
  0x01, 0x20, 0xFF, 0x3F, };

void u8g2_bitmap_overlay(uint8_t a) {
  uint8_t frame_size = 28;

  u8g2.drawStr(0, 0, "Bitmap overlay");

  u8g2.drawStr(0, frame_size + 12, "Solid / transparent");
  u8g2.setBitmapMode(false /* solid */);
  u8g2.drawFrame(0, 10, frame_size, frame_size);
  u8g2.drawXBMP(2, 12, cross_width, cross_height, cross_bits);
  if(a & 4)
    u8g2.drawXBMP(7, 17, cross_block_width, cross_block_height, cross_block_bits);

  u8g2.setBitmapMode(true /* transparent*/);
  u8g2.drawFrame(frame_size + 5, 10, frame_size, frame_size);
  u8g2.drawXBMP(frame_size + 7, 12, cross_width, cross_height, cross_bits);
  if(a & 4)
    u8g2.drawXBMP(frame_size + 12, 17, cross_block_width, cross_block_height, cross_block_bits);
}

void u8g2_bitmap_modes(uint8_t transparent) {
  const uint8_t frame_size = 24;

  u8g2.drawBox(0, frame_size * 0.5, frame_size * 5, frame_size);
  u8g2.drawStr(frame_size * 0.5, 50, "Black");
  u8g2.drawStr(frame_size * 2, 50, "White");
  u8g2.drawStr(frame_size * 3.5, 50, "XOR");
  
  if(!transparent) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawStr(0, 0, "Solid bitmap");
  } else {
    u8g2.setBitmapMode(true /* transparent*/);
    u8g2.drawStr(0, 0, "Transparent bitmap");
  }
  u8g2.setDrawColor(0);// Black
  u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(1); // White
  u8g2.drawXBMP(frame_size * 2, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(2); // XOR
  u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}


uint8_t draw_state = 0;

// void draw(void) {
//   u8g2_prepare();
//   switch(draw_state >> 3) {
//     case 0: u8g2_box_frame(draw_state&7); break;
//     case 1: u8g2_disc_circle(draw_state&7); break;
//     case 2: u8g2_r_frame(draw_state&7); break;
//     case 3: u8g2_string(draw_state&7); break;
//     case 4: u8g2_line(draw_state&7); break;
//     case 5: u8g2_triangle(draw_state&7); break;
//     case 6: u8g2_ascii_1(); break;
//     case 7: u8g2_ascii_2(); break;
//     case 8: u8g2_extra_page(draw_state&7); break;
//     case 9: u8g2_bitmap_modes(0); break;
//     case 10: u8g2_bitmap_modes(1); break;
//     case 11: u8g2_bitmap_overlay(draw_state&7); break;
//   }
// }


void setup(void) {

    Serial.begin(115200); 
    //SerialBT.begin("Smart Coffee Maker"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
 Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);





 pinMode(B_High, INPUT);
  pinMode(B_Mid, INPUT);
   pinMode(B_Low, INPUT);
    pinMode(B_Start, INPUT);

   pinMode(Buzzer, OUTPUT);
   pinMode(Pump1, OUTPUT);
   pinMode(Pump2, OUTPUT);
    pinMode(Relay, OUTPUT);
 pinMode(IRSensor, INPUT);
  //pinMode(Temprature, INPUT);
    

//digitalWrite(Pump1, HIGH);
//digitalWrite(Pump2, HIGH);

  buttonH.begin(B_High);
  buttonH.setClickHandler(click);

  buttonM.begin(B_Mid);
  buttonM.setClickHandler(click);

  buttonL.begin(B_Low);
  buttonL.setClickHandler(click);

  // buttonS.begin(B_Start);
  // buttonS.setClickHandler(click);

  buttonS.begin(B_Start);
  buttonS.setClickHandler(handler);
  buttonS.setLongClickHandler(handler);
  buttonS.setDoubleClickHandler(handler);
  buttonS.setTripleClickHandler(handler);






 while (!Serial);      // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();          // Init SPI bus
  rfid.PCD_Init();   // Init MFRC522 module
    bool result = rfid.PCD_PerformSelfTest(); // perform the test
  Serial.println(F("-----------------------------"));
  Serial.print(F("Result: "));
  if (result)
    Serial.println(F("OK"));
  else
    Serial.println(F("DEFECT or UNKNOWN"));
  Serial.println();


  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);



  u8g2.begin();
    u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf); 
  u8g2.setColorIndex(1); 


  u8g2.firstPage();

do{

  Starting();

} while (u8g2.nextPage());






    WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str());  //define hostname
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  connect_wifi();


}

// void loop(void) {
//   // picture loop  
//   u8g2.clearBuffer();
//   draw();
//   u8g2.sendBuffer();
  
//   // increase the state
//   draw_state++;
//   if ( draw_state >= 12*8 )
//     draw_state = 0;

//   // deley between each page
//   delay(100);

// }


void draw(String Temp, String level){

  readTemperature();
  readHumidity();
   u8g2.setFont(u8g2_font_helvB10_tf); 
  u8g2.drawFrame(0,0,128,31);         
  u8g2.drawFrame(0,33,128,31);           
  
  u8g2.drawStr( 15, 13, "Temperature");   
  u8g2.drawStr( 50, 28, Temp.c_str());   //temperature 
  u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "C");

  u8g2.drawStr(25,46, "Coffee Level");         
  u8g2.drawStr( 50, 61, level.c_str()); 
  u8g2.drawStr(75,61, "%");  




}




void readTemperature()
{
  float t = 45.4;
  dtostrf(t, 3, 1, temperature);
}

void readHumidity()
{
  float h = 97.65;
  dtostrf(h, 3, 1, humidity);
}

void CoffeeType(){

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 5, 13, "A coffee a day keeps the");   //Good Evening
u8g2.drawStr( 30, 25, "Stress Away");
u8g2.drawStr( 10, 35, "Select Your Coffee Type");
u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
u8g2.drawButtonUTF8(80, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Black" );

}

void Insufficient(){
u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 5, 13, "Insufficient Balance");   //Good Evening
u8g2.drawStr( 30, 25, "Please Pay Cash");
u8g2.drawStr( 10, 35, "BKash : 01943955579");
//u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
u8g2.drawButtonUTF8(80, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "OK" );

}

void CoffeeType(int type){

   Type =type;

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 5, 13, "A coffee a day keeps the");   //Good Evening
u8g2.drawStr( 30, 25, "Stress Away");
u8g2.drawStr( 10, 35, "Select Your Coffee Type");


  if(type==1){

u8g2.drawButtonUTF8(25, 55, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
u8g2.drawButtonUTF8(95, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Black" );

} else if(type==2){
u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
u8g2.drawButtonUTF8(102, 55, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0,  2,  2, "Black" );
}
}








void SugerType(){
u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 15, 13, "Cold Days HoT Drinks");   //Good Evening
u8g2.drawStr( 10, 28, "Select Your Suger Level");
u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "LOW" );
u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MAX" );

} 

void SugerType(int sype){
   Sugger=sype;

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 15, 13, "Cold Days HoT Drinks");   //Good Evening
u8g2.drawStr( 10, 28, "Select Your Suger Level");

if(sype==1){

u8g2.drawButtonUTF8(22, 55, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0,  2,  2, "LOW" );
u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MAX" );
} else if(sype==2){
u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "LOW" );
u8g2.drawButtonUTF8(60, 55, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MAX" );
}  else if(sype==3){
  u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "LOW" );
u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(102, 55, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0,  2,  2, "MAX" );
}


}


void Starting(){
  u8g2.drawFrame(0,0,128,64);    
   //u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr( 15, 13, "Please Wait Try");   //Good Evening
  u8g2.drawStr( 25, 28, "To Connect With");   
  u8g2.drawStr(22,46, ssid);         
  u8g2.drawStr( 15, 61, "SHAH X CONNECT");

}


void Welcome(String Name){
  u8g2.drawFrame(0,0,128,64);    
   //u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr( 15, 13, "Good Morning");   //Good Evening
  u8g2.drawStr( 25, 28, Name.c_str());   
  u8g2.drawStr(25,46, "Let's Have");         
  u8g2.drawStr( 15, 61, "a Coffee Break");


}

void WelcomeReg(){
  u8g2.drawFrame(0,0,128,64);    
   //u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr( 20, 13, "No Account");   //Good Evening
  u8g2.drawStr( 10, 28, "SCAN QR Code");   
  u8g2.drawStr(10,46, "Create Account");         
  u8g2.drawStr( 2, 61, "Enjoy Our Service");


}



void Wificon(String ipAds){

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 5, 13, "WiFi connected");   //Good Evening
u8g2.drawStr( 30, 25, "IP address: ");
u8g2.drawStr( 10, 35, ipAds.c_str());
//u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
//u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
//u8g2.drawButtonUTF8(80, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "OK" ); //bold
u8g2.drawButtonUTF8(100, 55, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 0,  2,  2, "OK" ); //blank


//delay(2400);

//ServerUpdatedata("100","RFID");


  u8g2.firstPage();

do{
int Tvel=95;


  
  draw(String(temppp),String(Tvel));

} while (u8g2.nextPage());




 

}


void WifiError(char res){

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 5, 13, "Disconnected from WiFi: ");   //Good Evening
u8g2.drawStr( 30, 25, ssid);
u8g2.drawStr( 10, 35, "Trying to Reconnect");
//u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
//u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(80, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "OK" );



}

void ThansHave(){

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 25, 13, "Thank You Sir!" );   //Good Evening
u8g2.drawStr( 25, 25, "Have A Nice Day" );
u8g2.drawStr( 10, 38, "SHAH X CONNECT");
//u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
//u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
//u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Start" );


  
}

void Final(){

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 25, 13, "Please Put Your" );   //Good Evening
u8g2.drawStr( 25, 25, "Cup in Our Door" );
u8g2.drawStr( 10, 38, "Then Press Start Button");
//u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
//u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Start" );


}

void DisplayHandeler(){

//u8g2_prepare();
//   
//   draw();
//  



  //switch (Type) {

  //  case 1:
  u8g2.drawFrame(0,0,128,64);    
   u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.drawStr( 15, 13, "Good Morning");   //Good Evening
  u8g2.drawStr( 25, 28, "Imam Hossain");   
  u8g2.drawStr(25,46, "Let's Have");         
  u8g2.drawStr( 15, 61, "a Coffee Break");
  Serial.println("1");

//     break;

//     case 2:
// u8g2.drawFrame(0,0,128,64);    
// u8g2.setFont(u8g2_font_helvR08_tr);
// u8g2.drawStr( 15, 13, "Cold Days HoT Drinks");   //Good Evening
// u8g2.drawStr( 10, 28, "Select Your Suger Level");
// u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "LOW" );
// u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
// u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MAX" );
// Serial.println("2");
//     break;
//     case 3:
// u8g2.drawFrame(0,0,128,64);    
// u8g2.setFont(u8g2_font_helvR08_tr);
// u8g2.drawStr( 5, 13, "A coffee a day keeps the");   //Good Evening
// u8g2.drawStr( 30, 25, "Stress Away");
// u8g2.drawStr( 10, 35, "Select Your Suger Level");
// u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
// //u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
// u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Black" );
// Serial.println("3");
//     break;
//     case 4:
// u8g2.drawFrame(0,0,128,64);    
// u8g2.setFont(u8g2_font_helvR08_tr);
// u8g2.drawStr( 25, 13, "Please Put Your");   //Good Evening
// u8g2.drawStr( 25, 25, "Cup in Our Door");
// u8g2.drawStr( 10, 38, "Then Press Start Button");
// //u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
// //u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
// u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Start" );

// Serial.println("4");
//     break;

 
  //}



/*
  u8g2.drawFrame(0,0,128,64);    
  u8g2.drawStr( 15, 13, "Good Morning");   //Good Evening
  u8g2.drawStr( 25, 28, "Imam Hossain");   
  u8g2.drawStr(25,46, "Let's Have");         
  u8g2.drawStr( 15, 61, "a Coffee Break"); 
 */

/*
u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 15, 13, "Cold Days HoT Drinks");   //Good Evening
u8g2.drawStr( 10, 28, "Select Your Suger Level");
u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "LOW" );
u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MAX" );
*/


/*
u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 5, 13, "A coffee a day keeps the");   //Good Evening
u8g2.drawStr( 30, 25, "Stress Away");
u8g2.drawStr( 10, 35, "Select Your Suger Level");
u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
//u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Black" );
*/

u8g2.drawFrame(0,0,128,64);    
u8g2.setFont(u8g2_font_helvR08_tr);
u8g2.drawStr( 25, 13, "Please Put Your");   //Good Evening
u8g2.drawStr( 25, 25, "Cup in Our Door");
u8g2.drawStr( 10, 38, "Then Press Start Button");
//u8g2.drawButtonUTF8(10, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Normal" );
//u8g2.drawButtonUTF8(50, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "MID" );
u8g2.drawButtonUTF8(90, 55, U8G2_BTN_INV|U8G2_BTN_BW2, 0,  2,  2, "Start" );



}







void loop() { 

  //   int sensorStatus = digitalRead(IRSensor); // Set the GPIO as Input
  // if (sensorStatus == 0) 
  // {
   
  //   Serial.println("Motion Detected!"); 
  // }
  // else  {
    
  //   Serial.println("Motion Ended!");
  // }


//   int adcVal = analogRead(Temprature);
//   // convert the ADC value to voltage in millivolt
//   float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);
//   // convert the voltage to the temperature in °C
//    tempC = milliVolt / 10;

// Serial.println(adcVal);

  now = millis();
temppp= random(45, 55);

  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (now - WpreviousMillis >= Winterval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    WpreviousMillis = now;
  } 



 
  buttonH.loop();
  buttonM.loop();
  buttonL.loop();
  buttonS.loop();

//   if(Step==0){
//  ServerUpdatedata(CardID,"Pay");
// Step=1;
// }

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }



  // do {   
  //  if(temp==0) {
  //    draw();
  //    } //else DisplayHandeler();
  // } while( u8g2.nextPage() );





   if (millis() - previousMillis2 >= 15000) {
    previousMillis2 = millis();
    OldCardID="";
  }
  //delay(50);






// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   
    // Serial.println(F("The NUID tag is:"));
    // Serial.print(F("In hex: "));
    // printHex(rfid.uid.uidByte, rfid.uid.size);
    // Serial.println();
    // Serial.print(F("In dec: "));
    // printDec(rfid.uid.uidByte, rfid.uid.size);
    // Serial.println();
  }
  else Serial.println(F("Card read previously."));




CardID="";


  
  for (byte i = 0; i < rfid.uid.size; i++) {
    CardID += rfid.uid.uidByte[i];
  }
  //---------------------------------------------
  if( CardID == OldCardID ){
    return;
  }
  else{
    OldCardID = CardID;
     Serial.println(CardID);
  }
  //---------------------------------------------
  //Serial.println(CardID);
  buzzer(150);

ServerUpdatedata(CardID,"RFID");


  if(Id[0] == CardID){

      Serial.println("Teacher");
  } else if(Id[1] == CardID){

      //     digitalWrite(Relay, HIGH);
      // digitalWrite(Pump1, HIGH);
      
      // delay(8000);
      // digitalWrite(Relay, LOW);
      //   digitalWrite(Pump1, LOW);

Serial.println("Student");
  } else if(Id[2] == CardID){
temp=1;
Serial.println("Admin");
  } else Serial.println("No Record");
 // SendCardID(CardID);


  //delay(1000);



  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();


}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
}

void buzzer(int T){
 digitalWrite(Buzzer, HIGH);
      delay(T);
      digitalWrite(Buzzer, LOW);

}




void handler(Button2& btn) {
  switch (btn.getClickType()) {
    case SINGLE_CLICK:
if(Step<4)
 ++Step;


  u8g2.firstPage();

do{


if(Step==2){
 
CoffeeType();

} else if(Step==1){
 // ++Step;
  SugerType();
} else if(Step==3) {
 // ++Step;

  Final();
//Serial.println(Type);
//Serial.println(Sugger);
   
} else if(Step==4){
  
 
int time= 1600;

     sensorStatus = digitalRead(IRSensor); // Set the GPIO as Input
  if (sensorStatus == 0) 
  {
   
    if(Type==1){
   digitalWrite(Pump2, HIGH); //Milk+ Suger    
     delay(Sugger*700);
         digitalWrite(Pump2, LOW);

} else time=2200;
    
digitalWrite(Pump1, HIGH);
 delay(time);
 digitalWrite(Pump1, LOW);
  digitalWrite(Relay, LOW);

  ThansHave();
  ServerUpdatedata(CardID,"Pay");


  }
  else  {
    buzzer(600);
      Final();
    //Serial.println("Motion Ended!");
  }










}

} while (u8g2.nextPage());






//  u8g2.firstPage();   
// do{
// Final();
// } while (u8g2.nextPage());

buzzer(300);

 
Serial.println("Mode 01");

      break;
    case DOUBLE_CLICK:
      Serial.print("double ");

  u8g2.drawStr( 15, 13, "Tea Making Started");   
  u8g2.drawStr( 35, 28, "500");   
  u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "C");

  u8g2.drawStr(27,46, "Tea Level");         
  u8g2.drawStr( 37, 61, "30"); 
  u8g2.drawStr(75,61, "%"); 

      break;
    case TRIPLE_CLICK:
      Serial.print("triple ");
  //      u8g2.drawStr( 15, 13, "Tea Making Done");   
  // u8g2.drawStr( 35, 28, "Serve");   
  // u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  // u8g2.drawUTF8(76, 28, "C");

  // u8g2.drawStr(27,46, "Tea Level");         
  // u8g2.drawStr( 37, 61, "30"); 
  // u8g2.drawStr(75,61, "%"); 

  // digitalWrite(Relay, HIGH);
      digitalWrite(Pump1, HIGH);
       digitalWrite(Pump2, HIGH);
      
     delay(5000);
   //   digitalWrite(Relay, LOW);
       digitalWrite(Pump1, LOW);
         digitalWrite(Pump2, LOW);

      


      break;
    case LONG_CLICK:
      Serial.print("long");
buzzer(600);
  u8g2.firstPage();

do{


     sensorStatus = digitalRead(IRSensor); // Set the GPIO as Input
  if (sensorStatus == 0) 
  {
   
    if(Type==1){
   digitalWrite(Pump2, HIGH); //Milk+ Suger    
     delay(Sugger*600);
         digitalWrite(Pump2, LOW);

}
    
digitalWrite(Pump1, HIGH);
 delay(1200);
 digitalWrite(Pump1, LOW);
  digitalWrite(Relay, LOW);

  ThansHave();
  ServerUpdatedata(CardID,"Pay");


  }
  else  {
    buzzer(600);
      Final();
    //Serial.println("Motion Ended!");
  }

} while (u8g2.nextPage());




      
      break;
  }
  Serial.print("click");
  Serial.print(" (");
  Serial.print(btn.getNumberOfClicks());
  Serial.println(")");
}






void click(Button2& btn) {
    if (btn == buttonH) {

    u8g2.firstPage();
     do{

if(Step==1){
SugerType(3);
} else if(Step==2) CoffeeType(2);



} while (u8g2.nextPage());


      buzzer(300);
      Serial.println("High clicked");


    } else if (btn == buttonM) {


    u8g2.firstPage();
     do{

if(Step==1){
SugerType(2);
} 


} while (u8g2.nextPage());

      Serial.println("Mediam clicked");
buzzer(300);
      // digitalWrite(Relay, HIGH);
      // digitalWrite(Pump1, HIGH);
       //digitalWrite(Pump2, HIGH);
      
     // delay(9000);
      // digitalWrite(Relay, LOW);
      //  digitalWrite(Pump1, LOW);
        //  digitalWrite(Pump2, LOW);
      
    } else if (btn == buttonL) {

     u8g2.firstPage();
     do{

if(Step==1){
SugerType(1);
} else if(Step==2) CoffeeType(1);


} while (u8g2.nextPage());

buzzer(300);

      //  digitalWrite(Pump2, HIGH);
      
      // delay(9000);
  
      //     digitalWrite(Pump2, LOW);
      


      Serial.println("Low clicked");
     
    } else if (btn == buttonS) {
      buzzer(300);
      Serial.println("Start clicked");
      
    }
}




void connect_wifi() {
  Serial.println("Waiting for WiFi");
  WiFi.begin(ssid, password);
  for (int i = 0; i < 3; i++) {
    //while (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() != WL_CONNECTED) {

      Serial.print(".");
      buzzer(150);

     // measureAndStore();

    } else {
      break;
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      buzzer(600);
     

    }
  }
}


void ServerUpdatedata(String UUID,String Req) {


  //Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  

    // Prepare your HTTP POST request data distace=100000&latitute=75.89100000000&longitute=1955445500000
    /*  String httpRequestData = "distace=" +  getDistance() + "&latitute=" + lat_str
                          + "&longitute=" + lng_str; */


    String httpRequestData = "UUID="+ UUID +"&Req="+Req +"&Sugger="+Sugger+"&Type="+Type;
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    int httpResponseCode = http.POST(httpRequestData);

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");

    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    if (httpResponseCode > 0) {
      // Serial.print("HTTP Response code: ");
      // Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.print("Data: ");
      Serial.println(payload);



       char JSONMessage[100];  //Original message

       payload.toCharArray(JSONMessage, 100);

 StaticJsonDocument<200> doc; 
  DeserializationError error = deserializeJson(doc, JSONMessage);


  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }




  if(Req=="RFID"){


String Notifi = doc["Msg"];

if(Notifi=="DONE"){


  String Name = doc["Name"];
  int Credit = doc["Credit"];

 Sugger = doc["Suger"];
 Type = doc["Ctype"];



  digitalWrite(Relay, HIGH);

     u8g2.firstPage();
     do{

 Welcome(Name);

} while (u8g2.nextPage());


  delay(5000);


       u8g2.firstPage();
     do{

SugerType();

Step=1;

} while (u8g2.nextPage());

} else if(Notifi=="REG"){


    u8g2.firstPage();
     do{

 WelcomeReg();

} while (u8g2.nextPage());


  delay(5000);


       u8g2.firstPage();
     do{

 draw(String(temppp),String(Tvel));



} while (u8g2.nextPage());

} else if(Notifi=="Insu"){




      u8g2.firstPage();
     do{

  Insufficient();

} while (u8g2.nextPage());


  delay(5000);


       u8g2.firstPage();
     do{

 draw(String(temppp),String(Tvel));



} while (u8g2.nextPage());
}


  } else if(Req=="Temp"){


  String Temp = doc["Name"];
  String level = doc["Credit"];


      u8g2.firstPage();

do{

 // draw(Temp,level);
 

  draw(String(temppp),String(Tvel));

} while (u8g2.nextPage());

  } else if(Req=="Pay"){
 


      u8g2.firstPage();

do{

 ThansHave();

} while (u8g2.nextPage());

    delay(3000);
      u8g2.firstPage();

do{

 Tvel=Tvel-5;
  draw(String(temppp),String(Tvel));

} while (u8g2.nextPage());

    
  }



     





    //   int value = parsed["total_count"];
    //   Serial.println(value);





    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

}


