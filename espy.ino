#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP_SSD1306.h>
#include <Adafruit_GFX.h>
#include <gfxfont.h>



// OLED Display
#define OLED_CS     0  // Pin 19, CS - Chip select
#define OLED_DC     2   // Pin 20 - DC digital signal
#define OLED_RESET  16  // Pin 15 -RESET digital signal
ESP_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

//WIFI
const char *ssid = "lazer";
const char *password = "pewpewpew";
unsigned int OSCPort = 8000;
WiFiUDP Udp;
char packetBuffer[255];
//char cbuffer[255];

//LED strip
#define NUMPIXELS 50
#define PIXELPIN 15
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXELPIN, NEO_GRB + NEO_KHZ800);

int apixl;
int pRed = 255;
int pGreen = 255;
int pBlue = 255;
int pHue = 255;
int pSat = 255;
int pLum = 255;

//Rotary Encoder
#define ENCODERPIN1 12
#define ENCODERPIN2 5
#define ENCODERPINB 4
int lastMSB = 0;
int lastLSB = 0;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
int encoderVal = 0;
int encoderTmp = 0;

//menu
int mMode = 0;
int nModes = 4;
bool mChange = false;
int modeVals[4];
int modeValsMax[4];
int mValTmp;

void chckMaxVals(){
  for (int i = 0; i < nModes; ++i){
    if(modeVals[i] > modeValsMax[i]) modeVals[i] = modeValsMax[i];
    if(modeVals[i] < 0) modeVals[i] = 0;
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void wipePxl(int Hue, int Lumi){
  pixels.setPixelColor(apixl,Wheel(Hue));
  pixels.show();
}

void updateEncoder(){
  int MSB = digitalRead(ENCODERPIN1); //MSB = most significant bit
  int LSB = digitalRead(ENCODERPIN2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  if (encoded == 3) {
    encoderVal = encoderValue>>2;
    modeVals[mMode] = mValTmp + encoderVal;
    wipePxl(modeVals[0],0);
  }

  lastEncoded = encoded; //store this value for next time
}

void setup() {
  
  // OLED Display Init
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("pew");
  display.display();
  delay(1000);

  /*
  //Wifi
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("starting WIFI ...");
  display.display();
  WiFi.begin(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  display.println("--------------------");
  display.print("IP: ");
  display.println(myIP);
  Udp.begin(OSCPort);
  display.print("OSCPort: ");
  display.println(OSCPort);
  display.display();
  */

  delay(2000);

  //LED pixels
  pixels.begin();

  //Encoder
  pinMode(ENCODERPIN1, INPUT_PULLUP);
  pinMode(ENCODERPIN2, INPUT_PULLUP);
  pinMode(ENCODERPINB, INPUT_PULLUP);
  //digitalWrite(ENCODERPIN1, HIGH);
  //digitalWrite(ENCODERPIN2, HIGH);
  //digitalWrite(ENCODERPINB, HIGH);
  attachInterrupt(ENCODERPIN1, updateEncoder, CHANGE);
  attachInterrupt(ENCODERPIN2, updateEncoder, CHANGE);

  //Modes
  for (int i = 0; i < nModes; ++i){
    modeVals[i] = 0;
  }
  modeValsMax[0] = 255;
  modeValsMax[1] = 255;
  modeValsMax[2] = 255;
  modeValsMax[3] = 255;
  
}

void loop() {

  /*
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    IPAddress ip = Udp.remoteIP();
    display.clearDisplay();
    display.setCursor(0,0);
    display.print(">> ");
    display.println(ip);
    display.println(packetBuffer);
    display.display();
  }
  */
  
  for(int i = 0; i < NUMPIXELS; ++i){
    pixels.setPixelColor(i,Wheel(modeVals[0])); 
  }
  pixels.show();

  

  //menu
  if(!digitalRead(ENCODERPINB) && !mChange){
    mChange = true;
    encoderValue=0;
    encoderVal = 0;
    lastEncoded = 0;
    ++mMode;
    if(mMode > 3) mMode = 0;
    mValTmp = modeVals[mMode];
    
  }
  if(digitalRead(ENCODERPINB)) mChange = false;

  

  
  display.clearDisplay();
  display.setCursor(0,0);
  for(int i=0; i<4;++i){
    if(mMode == i) display.print(">> ");
    else display.print("   ");
    display.print(i);
    display.print(": ");
    display.println(modeVals[i]);
  }
  display.display();

  delay(10);

  
  
}


