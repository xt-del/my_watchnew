#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  
#include <Arduino.h>
#include <U8g2lib.h>
#include <DS1302.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <EEPROM.h> 
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial mySerial(2,0);
Adafruit_BMP085 bmp;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);


DS1302 rtc(13, 12, 14);

int music_num = 0;
int num = 1;
String menudata[9]={"","1.时间","2.气","度和海","步时间","UDP视频","6.手动","","接网络"};
char data[38]={'0','1','2','3','4','5','6','7','8','9','0','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
long timeinterval = 0;

uint8_t day[][32] = 
{
    {0x00,0x00,0xC4,0x1F,0x48,0x10,0x48,0x10,0xC1,0x1F,0x42,0x10,0x42,0x10,0xC8,0x1F,
    0x08,0x00,0xE4,0x3F,0x27,0x25,0x24,0x25,0x24,0x25,0x24,0x25,0xF4,0x7F,0x00,0x00},/*"温",0*/
    
    {0x00,0x00,0xFC,0x7F,0x04,0x00,0x04,0x01,0x04,0x01,0x04,0x01,0x04,0x01,0xF4,0x3F,
    0x04,0x01,0x04,0x01,0x04,0x09,0x04,0x11,0x04,0x11,0x02,0x01,0xFA,0x7F,0x01,0x00},/*"压",1*/
    
    {0x80,0x00,0x80,0x00,0x88,0x1F,0x88,0x00,0x88,0x00,0x88,0x00,0xFF,0x7F,0x80,0x00,
    0x80,0x00,0x88,0x10,0x88,0x08,0x84,0x04,0x02,0x03,0xC0,0x00,0x38,0x00,0x07,0x00},/*"步",2*/
    
    {0x08,0x02,0x08,0x02,0x08,0x02,0xBF,0x1F,0x08,0x12,0x08,0x12,0x38,0x13,0x0C,0x12,
    0x0B,0x55,0x08,0x55,0x8A,0x60,0x44,0x40,0x00,0x00,0x12,0x11,0x22,0x22,0x21,0x22},/*"热",3*/
    
    {0x04,0x02,0x04,0x05,0x84,0x08,0x5F,0x10,0xA4,0x6F,0x02,0x00,0xCA,0x23,0x49,0x2A,
    0x5F,0x2A,0xC8,0x2B,0x58,0x2A,0x4F,0x2A,0xCA,0x2B,0x48,0x22,0x48,0x2A,0x48,0x13},/*"输",4*/
    
    {0x00,0x00,0xFE,0x3F,0x82,0x20,0x82,0x20,0xFA,0x2F,0x82,0x20,0x82,0x20,0xF2,0x27,
    0x12,0x24,0x12,0x24,0x12,0x24,0xF2,0x27,0x12,0x24,0x02,0x20,0xFE,0x3F,0x02,0x20},/*"固",5*/
    
    {0x00,0x02,0x04,0x02,0xE8,0x7F,0x08,0x01,0x00,0x05,0x80,0x04,0xCF,0x3F,0x08,0x04,
    0x08,0x04,0x08,0x04,0xE8,0x7F,0x08,0x04,0x08,0x04,0x14,0x04,0xE2,0x7F,0x00,0x00},/*"连",6*/
    
    {0x08,0x0A,0x08,0x12,0x08,0x12,0x08,0x02,0xDF,0x7F,0x08,0x01,0x28,0x01,0x18,0x3F,
    0x8C,0x22,0x8B,0x22,0x88,0x14,0x88,0x14,0x48,0x08,0x48,0x14,0x2A,0x22,0x84,0x41},/*"拔",7*/
    
    {0x06,0x00,0x89,0x2F,0x69,0x30,0x36,0x20,0x10,0x20,0x18,0x00,0x18,0x00,0x18,0x00,
    0x18,0x00,0x18,0x00,0x18,0x00,0x10,0x00,0x30,0x20,0x60,0x10,0x80,0x0F,0x00,0x00},/*"℃",8*/
    
    {0x80,0x00,0x84,0x10,0x88,0x10,0x90,0x08,0x90,0x04,0x80,0x00,0xFE,0x3F,0xC0,0x01,
    0xA0,0x02,0xA0,0x02,0x90,0x04,0x88,0x08,0x84,0x10,0x83,0x60,0x80,0x00,0x80,0x00},/*"米",9*/
    
    {0x80,0x00,0x40,0x00,0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x08,0x08,0xF8,0x0F,0x08,0x08,
    0xF8,0x0F,0x08,0x08,0x80,0x00,0x10,0x21,0x12,0x49,0x12,0x48,0xE1,0x0F,0x00,0x00},/*"息",10*/

    {0x00,0x04,0x00,0x0F,0xF8,0x00,0x08,0x00,0x88,0x00,0x84,0x00,0x84,0x00,0xFC,0x3F,
    0x80,0x00,0x90,0x04,0x90,0x08,0x88,0x10,0x84,0x20,0x82,0x20,0xA0,0x00,0x40,0x00},/*"乐",11*/

    {0x08,0x05,0x08,0x09,0x04,0x01,0xA4,0x7F,0xA2,0x08,0xDF,0x08,0xA8,0x3F,0x84,0x08,
    0x82,0x08,0xBF,0x3F,0x82,0x08,0x80,0x08,0xB8,0x08,0x87,0x7F,0x82,0x00,0x80,0x00}/*"维",12*/
};
uint8_t idcards[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x1F,
0xE2,0x33,0xC0,0xFF,0x01,0x00,0x80,0xFF,0x0F,0xCF,0x1F,0xFE,0x3F,0xFC,0x1F,0xE2,
0x33,0xC0,0xFF,0x01,0x00,0x80,0xFF,0x0F,0xCF,0x1F,0xFE,0x3F,0x0C,0x10,0x7C,0x0E,
0x40,0x80,0x01,0x00,0x80,0x01,0x0C,0xBC,0x79,0x06,0x30,0xEC,0x93,0xEF,0xCF,0x43,
0xBE,0x01,0x00,0x80,0x01,0x0C,0xBC,0x79,0x06,0x30,0xEC,0x93,0xEF,0xCF,0x43,0xBE,
0x01,0x00,0x80,0xF9,0xCD,0x4F,0x06,0xF6,0x33,0xEC,0x13,0x9E,0xCF,0x43,0xBE,0x01,
0x00,0x80,0xF9,0xCD,0x4F,0x06,0xF6,0x33,0xEC,0x13,0x9E,0xCF,0x43,0xBE,0x01,0x00,
0x80,0xF9,0x0D,0xCF,0x61,0xF6,0x33,0xEC,0x13,0x0C,0x8E,0x43,0xBE,0x01,0x00,0x80,
0xF9,0x0D,0xCF,0x61,0xF6,0x33,0x0C,0x10,0x82,0x8D,0x41,0x80,0x01,0x00,0x80,0xF9,
0x0D,0x0C,0x00,0xF6,0x33,0x0C,0x10,0x82,0x8D,0x41,0x80,0x01,0x00,0x80,0xF9,0x0D,
0x0C,0x00,0xF6,0x33,0xFC,0x9F,0x6D,0xB2,0xCD,0xFF,0x01,0x00,0x80,0x01,0x0C,0x83,
0x67,0x06,0x30,0x00,0x80,0xEF,0x4F,0x00,0x00,0x00,0x00,0x80,0xFF,0xCF,0x4C,0x66,
0xFE,0x3F,0x00,0x80,0xEF,0x4F,0x00,0x00,0x00,0x00,0x80,0xFF,0xCF,0x4C,0x66,0xFE,
0x3F,0x7C,0xFE,0x1D,0x32,0x7E,0x30,0x00,0x00,0x00,0x00,0xC0,0x0F,0x18,0x00,0x00,
0x7C,0xFE,0x1D,0x32,0x7E,0x30,0x00,0x00,0x00,0x00,0xC0,0x0F,0x18,0x00,0x00,0x6C,
0x0C,0x1C,0xB2,0x0D,0x8C,0x01,0x00,0x80,0x9F,0xFF,0x3C,0xF8,0x07,0x03,0x7C,0x7C,
0x82,0x7D,0x4E,0xF0,0x01,0x00,0x80,0x9F,0xFF,0x3C,0xF8,0x07,0x03,0x7C,0x7C,0x82,
0x7D,0x4E,0xF0,0x01,0x00,0x00,0x80,0xC1,0xFC,0x99,0x07,0x30,0x90,0x63,0x10,0x40,
0x30,0x4C,0x00,0x00,0x00,0x80,0xC1,0xFC,0x99,0x07,0x30,0x10,0x9E,0x61,0x4C,0xF2,
0xC3,0x01,0x00,0x00,0x00,0xFC,0xF3,0x7F,0x38,0x3F,0x10,0x9E,0x61,0x4C,0xF2,0xC3,
0x01,0x00,0x00,0x00,0xFC,0xF3,0x7F,0x38,0x3F,0x1C,0xE2,0xF1,0x7D,0xFE,0x8D,0x01,
0x00,0x80,0x1F,0x00,0x30,0x78,0x0E,0x0C,0x1C,0xE2,0xF1,0x7D,0xFE,0x8D,0x01,0x00,
0x80,0x1F,0x00,0x30,0x78,0x0E,0x0C,0x80,0x9F,0x7D,0xBE,0x3F,0xF0,0x01,0x00,0x00,
0x86,0x0D,0x70,0x60,0xF0,0x3C,0x00,0xE2,0xEF,0x7F,0xB0,0x7D,0x00,0x00,0x00,0x18,
0xC0,0xC3,0x61,0xCE,0x30,0x00,0xE2,0xEF,0x7F,0xB0,0x7D,0x00,0x00,0x00,0x18,0xC0,
0xC3,0x61,0xCE,0x30,0x6C,0x9E,0x1F,0x4E,0xFC,0xF1,0x01,0x00,0x80,0x01,0xCE,0x70,
0x60,0x06,0x3F,0x6C,0x9E,0x1F,0x4E,0xFC,0xF1,0x01,0x00,0x80,0x01,0xCE,0x70,0x60,
0x06,0x3F,0x80,0x03,0x1E,0x7E,0xF0,0xB3,0x01,0x00,0x00,0x1E,0xC2,0x30,0xF8,0xCF,
0x0C,0xEC,0x71,0x8E,0xF1,0xFD,0xCD,0x01,0x00,0x00,0x1E,0xC2,0x30,0xF8,0xCF,0x0C,
0xEC,0x71,0x8E,0xF1,0xFD,0xCD,0x01,0x00,0x80,0x99,0xFD,0x0F,0xF8,0x3F,0x00,0x90,
0xE1,0x13,0x4C,0xB2,0x73,0x00,0x00,0x80,0x99,0xFD,0x0F,0xF8,0x3F,0x00,0xEC,0xF3,
0x6D,0x3C,0xFE,0x03,0x00,0x00,0x00,0x00,0xC0,0x7C,0x7E,0xF0,0x3C,0xEC,0xF3,0x6D,
0x3C,0xFE,0x03,0x00,0x00,0x00,0x00,0xC0,0x7C,0x7E,0xF0,0x3C,0x00,0x80,0xF1,0xBD,
0x0F,0xC2,0x01,0x00,0x80,0xFF,0xCF,0xFC,0x79,0x36,0x3F,0x00,0x80,0xF1,0xBD,0x0F,
0xC2,0x01,0x00,0x80,0xFF,0xCF,0xFC,0x79,0x36,0x3F,0xFC,0x9F,0x7F,0x7E,0x4E,0xCE,
0x01,0x00,0x80,0x01,0xCC,0x70,0x60,0xF0,0x0F,0x0C,0x90,0xED,0x03,0x0C,0x72,0x00,
0x00,0x80,0xF9,0xCD,0xBF,0xE1,0x3F,0x3F,0x0C,0x90,0xED,0x03,0x0C,0x72,0x00,0x00,
0x80,0xF9,0xCD,0xBF,0xE1,0x3F,0x3F,0xEC,0x93,0x13,0x72,0xFC,0xCF,0x01,0x00,0x80,
0xF9,0x0D,0x7F,0x98,0x31,0x0F,0xEC,0x13,0x1E,0x7E,0xF0,0x4F,0x00,0x00,0x80,0xF9,
0x0D,0x7F,0x98,0x31,0x0F,0xEC,0x13,0x1E,0x7E,0xF0,0x4F,0x00,0x00,0x80,0xF9,0xCD,
0xC3,0xE7,0xCF,0x30,0xEC,0x93,0x83,0x3D,0xC2,0x83,0x01,0x00,0x80,0xF9,0xCD,0xC3,
0xE7,0xCF,0x30,0xEC,0x93,0x83,0x3D,0xC2,0x83,0x01,0x00,0x80,0x01,0xCC,0x30,0x00,
0x30,0x0F,0x0C,0x90,0x13,0x30,0x70,0x4C,0x00,0x00,0x80,0x01,0xCC,0x30,0x00,0x30,
0x0F,0xFC,0x9F,0x61,0x0C,0x72,0xF2,0x01,0x00,0x80,0xFF,0xCF,0xF3,0x79,0x38,0x3C,
0xFC,0x9F,0x61,0x0C,0x72,0xF2,0x01,0x00,0x80,0xFF,0xCF,0xF3,0x79,0x38,0x3C,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void setup() {
  delay(500);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setFontDirection(0);
  bmp.begin();
  rtc.halt(false);
  rtc.writeProtect(false);
  pinMode(15,INPUT);
  pinMode(16,INPUT);
  mySerial.begin (9600);
  mp3_set_serial (mySerial);
  EEPROM.begin(3);
  music_num = EEPROM.read(2);
  if(EEPROM.read(0)==100){
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);
    u8g2.print("Hotspot Mode");
    u8g2.setCursor(2, 31);
    u8g2.print("ID:");
    u8g2.print(EEPROM.read(0));
    u8g2.sendBuffer();
    //设置为sta模式
    wifi_set_opmode(STATION_MODE);
    //开启混杂模式
    wifi_promiscuous_enable(1); 
  }else{
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);
    u8g2.print("WIFI Mode");
    u8g2.setCursor(2, 31);
    u8g2.print("ID:");
    u8g2.print(EEPROM.read(0));
    u8g2.sendBuffer();
    WiFi.mode(WIFI_AP_STA);
  }
  delay(4000);
  u8g2.clearBuffer();
  Serial.begin(115200);
}

void loop() {
  if(EEPROM.read(1)==7){
    sendfix();
    EEPROM.write(1, 0);
    EEPROM.commit();
  }else if(EEPROM.read(1)==8){
    connecte();
    EEPROM.write(1, 0);
    EEPROM.commit();
  }else if(EEPROM.read(1)==4){
    gettime();
    EEPROM.write(1, 0);
    EEPROM.commit();
  }else if(EEPROM.read(1)==5){
    video();
    EEPROM.write(1, 0);
    EEPROM.commit();
  }
  if(button1()==true && button2()==false)
  {   
      timeinterval = millis();
      while(button1()==true && button2()==false){
        delay(10);
      }
      if(millis() - timeinterval>1000){
          u8g2.clear();
          u8g2.updateDisplay();
          switch(num){
            case 1:delay(500);showtime();break;
            case 2:delay(500);recvbmp1();break;
            case 3:delay(500);recvbmp2();break;
            case 4:delay(500);gettime();break;
            case 5:delay(500);video();break;
            case 6:delay(500);input();break;
            case 7:delay(500);sendfix();break;
            case 8:delay(500);connecte();break;
            case 9:delay(500);time_send();break;
            case 10:delay(500);messages();break;
            case 11:delay(500);card();break;
            case 12:delay(500);music();break;
          }
      }else{
        num--;
      }
  }else if(button2()==true && button1()==false){
      num++;
      if(num==13){
        num=1;
      }
  }
  if(num==0){
      num=1;
  }
  menu(num);
  Serial.println(num);
  delay(200);
}

//按键一
bool button1(){
  return(digitalRead(15));
}

//按键二
bool button2(){
  return(digitalRead(16));
}

//发送热点
void sendBeacon(String ssid) {
    // Randomize channel //
    byte channel = random(1,12); 
    wifi_set_channel(channel);

    uint8_t packet[128] = { 0x80, 0x00, //Frame Control 
                        0x00, 0x00, //Duration
                /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address 
                /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                /*22*/  0xc0, 0x6c, //Seq-ctl
                //Frame body starts here
                /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                /*32*/  0xFF, 0x00, //Beacon interval
                /*34*/  0x01, 0x04, //Capability info
                /* SSID */
                /*36*/  0x00
                };

    //int ssidLen = strlen(ssid);
    int ssidLen = ssid.length();
    packet[37] = ssidLen;

    for(int i = 0; i < ssidLen; i++) {
      packet[38+i] = ssid[i];
    }

    uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                        0x03, 0x01, 0x04 /*DSSS (Current Channel)*/ };

    for(int i = 0; i < 12; i++) {
      packet[38 + ssidLen + i] = postSSID[i];
    }

    packet[50 + ssidLen] = channel;

    // Randomize SRC MAC
    packet[10] = packet[16] = random(256);
    packet[11] = packet[17] = random(256);
    packet[12] = packet[18] = random(256);
    packet[13] = packet[19] = random(256);
    packet[14] = packet[20] = random(256);
    packet[15] = packet[21] = random(256);

    int packetSize = 51 + ssidLen;

    wifi_send_pkt_freedom(packet, packetSize, 0);
    wifi_send_pkt_freedom(packet, packetSize, 0);
    wifi_send_pkt_freedom(packet, packetSize, 0);
    delay(1);
}

//ADC读取
int adcrecvs(){
    int adcrecv = analogRead(A0);
    if(adcrecv==1024){
      return 5;
    }else if(adcrecv>50&&adcrecv<60){
      return 1;
    }else if(adcrecv>350&&adcrecv<400){
      return 2;
    }else if(adcrecv>950&&adcrecv<1000){
      return 3;
    }else if(adcrecv>670&&adcrecv<690){
      return 4;
    }else{
      return 0;
    }
}

//固定发送
void sendfix(){
  if(EEPROM.read(0)!=100){
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);//设置显示位置
    u8g2.print("Strat rest");
    delay(2000);
    u8g2.sendBuffer();
    u8g2.updateDisplay();
    EEPROM.write(0, 100);
    EEPROM.write(1, 7);
    EEPROM.commit();
    ESP.restart();
  }else{
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);//设置显示位置
    u8g2.print("Strat send");
    delay(2000);
    u8g2.sendBuffer();
    u8g2.updateDisplay();
    for(int i=100;i<=120;i++)
    {
      sendBeacon("我要冰墩墩*"+String(i));
      delay(100);
    }
  }
}

//音乐播放
void music(){
  int voice = 20;
  long timeinterval1 = 0;
  long timeinterval2 = 0;
  mp3_set_volume(voice);
  u8g2.clearBuffer();
  u8g2.updateDisplay();
  while(true){
      if(button1()==true && button2()==false)
      {
          timeinterval1 = millis();
          while(button1()==true && button2()==false){
            delay(10);
            if(millis() - timeinterval1>3000){
              voice++;
              if(voice>30){
                voice=0;
              }
              u8g2.setCursor(32, 15);//设置显示位置
              u8g2.print(voice);
              u8g2.sendBuffer();
              u8g2.updateDisplay();
              mp3_set_volume(voice);
              delay(100);
            }
          }
          if(millis() - timeinterval1>1000 && millis() - timeinterval1<3000){
            break;
          }else if(millis() - timeinterval1<=1000){
              music_num++;
              mp3_play(music_num);
              u8g2.setCursor(2, 15);//设置显示位置
              u8g2.print("next");
              u8g2.sendBuffer();
              u8g2.updateDisplay();
              delay(2000);  
         }
      }else if(button1()==false && button2()==true)
      {
          timeinterval2 = millis();
          while(button1()==false && button2()==true){
            delay(10);
          if(millis() - timeinterval2>500&&millis() - timeinterval2<2000){
            u8g2.clearBuffer();
              u8g2.setCursor(2, 15);//设置显示位置
              u8g2.print("prev");
              u8g2.sendBuffer();
              u8g2.updateDisplay();
              delay(100);
          }else if(millis() - timeinterval2>2000&&millis() - timeinterval2<3000){
            u8g2.clearBuffer();
              u8g2.setCursor(2, 15);//设置显示位置
              u8g2.print("play");
              u8g2.sendBuffer();
              u8g2.updateDisplay();
              delay(100);
          }else if(millis() - timeinterval2<=500){
            u8g2.clearBuffer();
              u8g2.setCursor(2, 15);
              u8g2.print("stop");
              u8g2.sendBuffer();
              u8g2.updateDisplay();
              delay(100);
           }else if(millis() - timeinterval2 > 3000){
                  music_num++;
                  if(music_num>27){
                    music_num=0;
                  }
                  u8g2.setCursor(32, 15);//设置显示位置
                  u8g2.print(music_num);
                  u8g2.sendBuffer();
                  u8g2.updateDisplay();
                  delay(100);
                  u8g2.clearBuffer();
            }
            
          }
          if(millis() - timeinterval2>500&&millis() - timeinterval2<2000){
              music_num--;
              mp3_play(music_num);
          }else if(millis() - timeinterval2>2000&&millis() - timeinterval2<3000){
              mp3_play();
          }else if(millis() - timeinterval2<=500){
              mp3_pause();
           }else if(millis() - timeinterval2 > 3000){
              mp3_play(music_num);
          }
      }
      u8g2.clearBuffer();
      u8g2.setCursor(2, 15);//设置显示位置
      u8g2.print("playing");
      u8g2.setCursor(70, 15);
      u8g2.print(music_num);
      u8g2.setCursor(90, 15);
      u8g2.print(voice);
      u8g2.setCursor(0, 32);
      u8g2.print("For Introducti");
      u8g2.setCursor(2, 48);
      u8g2.print("on:120.27.198.70");
      u8g2.setCursor(2, 63);
      u8g2.print("/use.html");
      u8g2.sendBuffer();
      u8g2.updateDisplay();  
      delay(200);
      u8g2.clearBuffer();
  }
  EEPROM.write(2, music_num);
  EEPROM.commit(); 
}

//手动输入
void input(){
    int num_data[50]={0};
    int x=1,y=1,n=1;
    bool flag = true;
    while(flag){
      while(adcrecvs()!=5){
         if(adcrecvs()==4){
            x++;
            if(x==9){
              x=0;
            }
         }else if(adcrecvs()==1){
            x--;
            if(x==-1){
              x=8;
            }
         }else if(adcrecvs()==2){
            y++;
            if(y==6){
              y=1;
            }
         }else if(adcrecvs()==3){
            y--;
            if(y==0){
              y=5;
            }
         }else if(button2()==true && button1()==false){
              flag = false;
              break; 
         }
         if(y<=4){
            u8g2.clearBuffer();
            for(int i=0;i<4;i++){
              u8g2.setCursor(1,(i+1)*16 - 1);
              for(int j=1;j<=8;j++){
                u8g2.print(data[j+i*8]);
                u8g2.print(" ");
              }
            }
            if(y==4){
              u8g2.drawLine(x*16,y*16-1,(x*16)+8,y*16-1);
            }else{
              u8g2.drawLine(x*16,y*16,(x*16)+8,y*16);
            }
         }else{
            u8g2.clearBuffer();
            u8g2.setCursor(1,15);
            u8g2.print("W X Y Z");
            u8g2.drawLine(x*16,16,(x*16)+8,16);
         }
         u8g2.sendBuffer();
         u8g2.updateDisplay();
         delay(100);
         u8g2.clearBuffer();
         //Serial.println(adcrecvs());
      }
      num_data[n] = x+((y-1)*8)+1;
      u8g2.clearBuffer();
      u8g2.updateDisplay();
      u8g2.setCursor(56, 30);
      u8g2.print(data[num_data[n]]);
      u8g2.updateDisplay();
      u8g2.clearBuffer();
      Serial.println(data[num_data[n]]);
      n++;
      delay(500);
    }
    u8g2.clearBuffer();
    u8g2.updateDisplay();
    u8g2.setCursor(16, 24);
    u8g2.print("You Put:");
    u8g2.setCursor(16, 40);
    char send_ssid[n];
    for(int i=1;i<n-1;i++){
      u8g2.print(data[num_data[i]]);
      send_ssid[i-1] = char(data[num_data[i]]);
    }
    u8g2.sendBuffer();
    u8g2.updateDisplay();
    WiFi.softAP(send_ssid,"mi989898");
    //sendBeacon(send_ssid);
    delay(4000);
}

//连接网络
void connecte(){
  u8g2.clearBuffer();
  u8g2.updateDisplay();
  if(EEPROM.read(0)==100){
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);//设置显示位置
    u8g2.print("Strat rest");
    delay(2000);
    u8g2.sendBuffer();
    u8g2.updateDisplay();
    EEPROM.write(0, 200);
    EEPROM.write(1, 8);
    EEPROM.commit();
    ESP.restart();
  }
  WiFi.disconnect();
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");
  u8g2.clearBuffer();
  u8g2.setCursor(2, 15);//设置显示位置
  u8g2.print("OK!");
  delay(2000);
  u8g2.sendBuffer();
  u8g2.updateDisplay();
}

//获取时间
void gettime(){
  u8g2.updateDisplay();
  if(EEPROM.read(0)==100){
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);//设置显示位置
    u8g2.print("Strat rest");
    delay(2000);
    u8g2.sendBuffer();
    u8g2.updateDisplay();
    EEPROM.write(0, 200);
    EEPROM.write(1, 4);
    EEPROM.commit();
    ESP.restart();
  }
  Serial.println(WiFi.status());
  if(WiFi.status() == WL_CONNECTED){
      WiFiUDP ntpUDP;
      NTPClient timeClient(ntpUDP, "ntp.aliyun.com");
      timeClient.begin();
      timeClient.setTimeOffset(28800);
      u8g2.setCursor(28,15);
      u8g2.print("同");
      u8g2.drawXBM(44,1,16,16,day[2]);
      u8g2.setCursor(60,15);
      u8g2.print("中");
      delay(1000);
      for(int i=1;i<=5;i++){
           u8g2.print(".");
           u8g2.sendBuffer();
           delay(500);
      }
      int into_time[3];
      timeClient.update();
      delay(1000);
      timeClient.update();
      rtc.setTime(timeClient.getHours(),timeClient.getMinutes(),timeClient.getSeconds());
      unsigned long epochTime = timeClient.getEpochTime();
      struct tm *ptm = gmtime((time_t *)&epochTime);
      into_time[0] = ptm->tm_year + 1900;
      into_time[1] = ptm->tm_mon + 1;
      into_time[2] = ptm->tm_mday;
      rtc.setDate(into_time[0],into_time[1],into_time[2]);
      rtc.setDOW(into_time[0],into_time[1],into_time[2]);
      
      u8g2.setCursor(28, 31);//设置显示位置
      u8g2.print("年finish！");
      u8g2.sendBuffer();
      delay(1000);
      u8g2.setCursor(28, 47);//设置显示位置
      u8g2.print("月finish！");
      u8g2.sendBuffer();
      delay(1000);
      u8g2.setCursor(28, 63);//设置显示位置
      u8g2.print("日finish！");
      u8g2.sendBuffer();
      delay(1000);
      
      u8g2.clearBuffer();
      u8g2.updateDisplay();
      u8g2.setCursor(20, 47);//设置显示位置
      u8g2.print("时间finish！");
      u8g2.sendBuffer();
      delay(1000);
      u8g2.clearBuffer();
      u8g2.setCursor(32, 32);//设置显示位置
      u8g2.print("finish!!");
      u8g2.sendBuffer();
      delay(2000);
  }else{
      u8g2.setFont(u8g2_font_unifont_t_chinese2);
      u8g2.setCursor(28, 32);//设置显示位置
      u8g2.print("没有网络！！");
      u8g2.setCursor(8, 48);
      u8g2.print("START CONNECTED");
      connecte();
      u8g2.sendBuffer();
      u8g2.updateDisplay();
      delay(3000);
  }
}

//菜单
void menu(int menu_line){
   u8g2.clearBuffer();
   if(menu_line>4&&menu_line<=8){
     u8g2.setCursor(2, 15);
     u8g2.print("5.");
     u8g2.setCursor(18, 15);
     u8g2.print(menudata[5]);
     
     u8g2.setCursor(2,31);
     u8g2.print(menudata[6]);
     u8g2.drawXBM(50,16,16,16,day[4]);
     u8g2.setCursor(66,31);
     u8g2.print("入");
     
     u8g2.setCursor(2,47);
     u8g2.print("7.");
     u8g2.drawXBM(18,32,16,16,day[5]);
     u8g2.setCursor(34,47);
     u8g2.print("定");
     u8g2.drawXBM(50,34,16,16,day[3]);
     u8g2.setCursor(66,47);
     u8g2.print("点");
     
     u8g2.setCursor(2,63);
     u8g2.print("8.");
     u8g2.drawXBM(18,48,16,16,day[6]);
     u8g2.setCursor(34,63);
     u8g2.print(menudata[8]);
     u8g2.drawBox(100,(menu_line-5)*16+1,28,14);
   }else if(menu_line<=4){
     u8g2.setCursor(2, 15);
     u8g2.print(menudata[1]);
     u8g2.setCursor(2,31);
     u8g2.print(menudata[2]);
     u8g2.drawXBM(34,16,16,16,day[1]);
     u8g2.setCursor(2,47);
     u8g2.print("3.");
     u8g2.drawXBM(18,31,16,16,day[0]);
     u8g2.setCursor(34,47);
     u8g2.print(menudata[3]);
     u8g2.drawXBM(82,31,16,16,day[7]);
     u8g2.setCursor(2,63);
     u8g2.print("4.同");
     u8g2.drawXBM(34,48,16,16,day[2]);
     u8g2.setCursor(50,63);
     u8g2.print(menudata[4]);
     u8g2.drawBox(100,(menu_line-1)*16+1,28,14);
   }else if(menu_line>8){
     u8g2.setCursor(2,15);
     u8g2.print("9.计时");
     u8g2.setCursor(2,31);
     u8g2.print("10.个人信");
     u8g2.drawXBM(76,18,16,16,day[10]);
     u8g2.setCursor(2,47);
     u8g2.print("11.二  码");
     u8g2.drawXBM(42,34,16,16,day[12]);
     u8g2.setCursor(2,63);
     u8g2.print("12.音");
     u8g2.drawXBM(40,50,16,16,day[11]);
     u8g2.drawBox(100,(menu_line-9)*16+1,28,14);
   }
   u8g2.sendBuffer();
}

//视频播放
void video(){
  u8g2.updateDisplay();
  if(EEPROM.read(0)==100){
    u8g2.clearBuffer();
    u8g2.setCursor(2, 15);//设置显示位置
    u8g2.print("Strat rest");
    delay(2000);
    u8g2.sendBuffer();
    u8g2.updateDisplay();
    EEPROM.write(0, 200);
    EEPROM.write(1, 5);
    EEPROM.commit();
    ESP.restart();
  }
  if(WiFi.status() == WL_CONNECTED){
      WiFiUDP Udp;
      unsigned int localUdpPort = 8000; // 本地端口号
      byte imagecode[1024];         // 接收缓冲区
      Udp.begin(localUdpPort);
      u8g2.clearBuffer();
      u8g2.updateDisplay();
      u8g2.setCursor(2,32);
      u8g2.print("IP:");
      u8g2.print(WiFi.localIP());
      u8g2.setCursor(2,48);
      u8g2.print("Port:");
      u8g2.print(localUdpPort);
      u8g2.sendBuffer();
      delay(1000);
      while(true){
          if(button1()==true||button2()==true){
              break;
          }
          int packetSize = Udp.parsePacket(); //获取当前队首数据包长度
          if (packetSize) //如果数据包可用
          {
            int len = Udp.read(imagecode, 1024);
            if (len > 0)
            {
              u8g2.clearBuffer();
              u8g2.drawXBM(0,0,128,64,imagecode);
              u8g2.sendBuffer();
            }
          }
      }
  }else{
      u8g2.setFont(u8g2_font_unifont_t_chinese2);
      u8g2.setCursor(28, 32);//设置显示位置
      u8g2.print("没有网络！！");
      u8g2.setCursor(8, 48);
      u8g2.print("START CONNECTED");
      connecte();
      u8g2.sendBuffer();
      u8g2.updateDisplay();
      delay(3000);
  }
}

//二维码
void card(){
  u8g2.updateDisplay();
  u8g2.clearBuffer();
  u8g2.drawXBM(4,7,120,50,idcards);
  u8g2.sendBuffer();
  while(true){
      if(button1()==true||button2()==true){
          break;
      }
      delay(1000);
  }
}

//计时
void time_send(){
  bool flag = true;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_inb16_mn);
  long int ms = millis();
  while(button2()!=true){
      if(button1()==true){
          flag = !flag;
          delay(500);
      }else if(flag==true){
        u8g2.setFont(u8g2_font_unifont_t_chinese2);
        u8g2.setCursor(34,47);
        u8g2.print("start");
      }else if(flag==false){
        u8g2.setFont(u8g2_font_unifont_t_chinese2);
        u8g2.setCursor(34,47);
        u8g2.print("stop");
      }
      while(flag){
          
          if(button1()==true){
              flag = !flag;
              delay(500);
          }
          
          u8g2.setCursor(28,32);
          u8g2.print(String((millis()-ms)/1000));
          u8g2.print(".");
          u8g2.print(String(int((millis()-ms)%1000)/10));
          u8g2.setFont(u8g2_font_unifont_t_chinese2);
          if(flag==true){
            u8g2.setCursor(34,47);
            u8g2.print("start");
          }else if(flag==false){
            u8g2.setCursor(34,47);
            u8g2.print("stop");
          }
          u8g2.setFont(u8g2_font_inb16_mn);
          u8g2.sendBuffer();
          delay(200);
      }
     u8g2.clearBuffer();
     delay(500);
  }
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
}

//手表信息
void messages(){
  u8g2.clearBuffer();
  uint8_t MAC_array_STA[6];
  uint8_t MAC_array_AP[6];
  char MAC_char_STA[18];
  char MAC_char_AP[18];
  WiFi.macAddress(MAC_array_STA);
  WiFi.softAPmacAddress(MAC_array_AP);
  for (int i = 0; i < sizeof(MAC_array_STA); ++i) {
    sprintf(MAC_char_STA, "%s%02x:", MAC_char_STA, MAC_array_STA[i]);
    sprintf(MAC_char_AP, "%s%02x:", MAC_char_AP, MAC_array_AP[i]);
  }
  u8g2.setCursor(2,15);
  u8g2.print("MAC STA:");
  u8g2.setCursor(2,31);
  u8g2.print(MAC_char_STA);
  u8g2.setCursor(2,47);
  u8g2.print("MAC AP:");
  u8g2.setCursor(2,63);
  u8g2.print(MAC_char_AP);
  u8g2.sendBuffer();
  
  delay(5000);
  u8g2.setPowerSave(1);
}

//显示时间
void showtime(){
  String timedata = "";
  u8g2.updateDisplay();
  Serial.println("OK");
  while(true){
      if(button1()==true||button2()==true){
          break;
      }
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_inb16_mn);
      timedata = rtc.getDateStr();
      u8g2.setCursor(32,16);
      u8g2.print(timedata.substring(6,10));
      u8g2.setCursor(28,32);
      u8g2.print(timedata.substring(3,5));
      u8g2.print("/");
      u8g2.print(timedata.substring(0,2));
      u8g2.setCursor(8,48);
      u8g2.print(rtc.getTimeStr());
      Serial.println(rtc.getTimeStr());
      u8g2.setFont(u8g2_font_unifont_t_chinese2);
      u8g2.setCursor(32, 64);//设置显示位置
      u8g2.print(rtc.getDOWStr());
      u8g2.sendBuffer();
      delay(1000);
  }
}

//气压
void recvbmp1(){
  while(true){
      if(button1()==true||button2()==true){
          break;
      }
      u8g2.clearBuffer();
      u8g2.setCursor(0, 15);
      u8g2.print("大气");
      u8g2.drawXBM(32,0,16,16,day[1]);
      u8g2.setCursor(48, 15);
      u8g2.print(":");
      u8g2.setCursor(0,31);
      u8g2.print(bmp.readPressure());
      u8g2.print(" Pa");
      u8g2.setCursor(0,47);
      u8g2.print("海平面");
      u8g2.drawXBM(48,32,16,16,day[1]);
      u8g2.setCursor(64, 47);
      u8g2.print("力:");
      u8g2.setCursor(0,63);
      u8g2.print(bmp.readSealevelPressure());
      u8g2.print(" Pa");
      u8g2.sendBuffer();
      delay(500);
  }
  
}

//温度/海拔
void recvbmp2(){
  while(true){
      if(button1()==true||button2()==true){
          break;
      }
      u8g2.clearBuffer();
      u8g2.drawXBM(0,0,16,16,day[0]);
      u8g2.setCursor(16, 15);
      u8g2.print("度:");
      u8g2.setCursor(0,31);
      u8g2.print(bmp.readTemperature());
      u8g2.print(" ");
      u8g2.drawXBM(40,16,16,16,day[8]);
      u8g2.setCursor(0,47);
      u8g2.print("高度:");
      u8g2.setCursor(0,63);
      u8g2.print(bmp.readAltitude());
      u8g2.drawXBM(64,48,16,16,day[9]);
      u8g2.sendBuffer();
  } 
}
