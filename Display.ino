#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>

#include "Button2.h"


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define BUTTON_PIN  2
Button2 button;



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


U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=18*/ 25, /* data=23*/ 33, /* CS=5*/ 26, /* reset=*/ 22); // ESP32



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
  u8g2.begin();
    u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf); 
  u8g2.setColorIndex(1); 

    button.begin(button_boot.PIN);
  button.setClickHandler(handler);
  button.setLongClickHandler(handler);
  button.setDoubleClickHandler(handler);
  button.setTripleClickHandler(handler);


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

void draw(){

  readTemperature();
  readHumidity();
  
  u8g2.drawFrame(0,0,128,31);         
  u8g2.drawFrame(0,33,128,31);           
  
  u8g2.drawStr( 15, 13, "Temperature");   
  u8g2.drawStr( 35, 28, temperature);   
  u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "C");

  u8g2.drawStr(27,46, "Tea Level");         
  u8g2.drawStr( 37, 61, humidity); 
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


void loop() {  

    button.loop();
  u8g2.firstPage();
  do {   
    draw();
  } while( u8g2.nextPage() );
}






void handler(Button2& btn) {
  switch (btn.getClickType()) {
    case SINGLE_CLICK:
u8g2.drawStr( 15, 13, "Temperature");   
  u8g2.drawStr( 35, 28, "100");   
  u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "C");

  u8g2.drawStr(27,46, "Tea Level");         
  u8g2.drawStr( 37, 61, "50"); 
  u8g2.drawStr(75,61, "%"); 
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
       u8g2.drawStr( 15, 13, "Tea Making Done");   
  u8g2.drawStr( 35, 28, "Serve");   
  u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "C");

  u8g2.drawStr(27,46, "Tea Level");         
  u8g2.drawStr( 37, 61, "30"); 
  u8g2.drawStr(75,61, "%"); 

      


      break;
    case LONG_CLICK:
      Serial.print("long");


      
      break;
  }
  Serial.print("click");
  Serial.print(" (");
  Serial.print(btn.getNumberOfClicks());
  Serial.println(")");
}
