/* AudioLinkSync, Teensy 4.0
 * by DrAndyHaas, June 2023
 * 
 * Compile with Tools->CPU Speed = 150 MHz for lower power consumption 
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <EEPROM.h>
#include <InternalTemperature.h>

//For SSD1306 0.96" SPI Screen
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_MOSI  17
#define OLED_CLK   16
#define OLED_DC    19
#define OLED_CS    20
#define OLED_RESET 18
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AsyncAudioInputSPDIF3    spdif_async_in; //xy=261,205
AudioEffectDelay         delay0;         //xy=655,200
AudioEffectDelay         delay1;         //xy=655,331
AudioOutputSPDIF3        spdif3_out;     //xy=992,201
AudioAnalyzeRMS          rms0;           //xy=996,372
AudioAnalyzePeak         peak0;          //xy=997,297
AudioAnalyzePeak         peak1;          //xy=998,453
AudioAnalyzeRMS          rms1;           //xy=999,528
AudioConnection          patchCord1(spdif_async_in, 0, delay0, 0);
AudioConnection          patchCord2(spdif_async_in, 1, delay1, 0);
AudioConnection          patchCord3(delay0, 0, spdif3_out, 0);
AudioConnection          patchCord4(delay0, 0, peak0, 0);
AudioConnection          patchCord5(delay0, 0, rms0, 0);
AudioConnection          patchCord6(delay1, 0, spdif3_out, 1);
AudioConnection          patchCord7(delay1, 0, peak1, 0);
AudioConnection          patchCord8(delay1, 0, rms1, 0);
// GUItool: end automatically generated code

bool changingdelay = false;
bool firstloop = true;
short mydelay = 0; // delay in ms
short maxdelay = 750; // max allowed delay
#define ledPin  13
#define knobPin 23
#define buttPin 22

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttPin, INPUT_PULLUP);
  AudioMemory(2*333); //333 should be about 1 second max, and there's two channels
  Serial.begin(115200);

  //read the saved delay
  mydelay = EEPROM.read(0)*256 + EEPROM.read(1);
  if (mydelay<0 || mydelay>maxdelay) mydelay=0;

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer but draw some pixels, because why not
  display.clearDisplay();
  for (int i=0; i<10; ++i) display.drawPixel(10*i+1, 10*i+1, SSD1306_WHITE);
  display.display();
  delay(1000);
}

void loop() {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  char buf[16];

  display.write("AudioLinkSync v1.1\n");

  display.write("locked: ");
  if (spdif_async_in.isLocked()) display.write("yes\n");
  else display.write("no\n");
  
  display.write("in freq: ");
  double inputFrequency=spdif_async_in.getInputFrequency();
  dtostrf(inputFrequency,6,3,buf);
  display.write(buf);
  display.write("\n");
  
  double pUsageIn=spdif_async_in.processorUsage(); 
  display.write("cpu use [%]: ");
  dtostrf(pUsageIn,5,1,buf);
  display.write(buf);
  display.write("\n");

  float temp = InternalTemperature.readTemperatureF();
  display.write("temp [F]: ");
  dtostrf(temp,5,1,buf);
  display.write(buf);
  display.write("\n");
  
  display.write("used memblk: ");
  dtostrf(AudioMemoryUsageMax(),5,1,buf);
  display.write(buf);
  display.write("\n");
  
  /*
  double bufferedTime=spdif_async_in.getBufferedTime();
  dtostrf(bufferedTime*1e3,7,4,buf);
  display.write("buf (ms): ");
  display.write(buf);
  display.write("\n");
  */

  double peakL=peak0.read();
  double peakR=peak1.read();
  dtostrf(max(peakL,peakR),5,4,buf);
  display.write("peak vol: ");
  display.write(buf);
  display.write("\n");

  /*
  double rmsL=rms0.read();
  double rmsR=rms1.read();
  dtostrf(max(rmsL,rmsR),5,4,buf);
  display.write("rms vol:  ");
  display.write(buf);
  display.write("\n");
  */
  
  if (digitalRead(buttPin)==false) { // button pressed
    if (changingdelay) { // we stop changing, save the value
      EEPROM.write(0, mydelay/256);//high byte
      EEPROM.write(1, mydelay%256);//low byte
    }
    changingdelay = !changingdelay;
  }

  display.write("delay (ms): ");
  if (!changingdelay && !firstloop) digitalWrite(ledPin, LOW); // set the LED off
  else{
    if (changingdelay) mydelay=analogRead(knobPin)*(maxdelay/1023.0); // read the delay from the knob, normalize to the max delay
    digitalWrite(ledPin, HIGH); // set the LED on
    delay0.delay(0, mydelay);
    delay1.delay(0, mydelay);
    display.write(" * ");
  }
  dtostrf(mydelay,5,1,buf);
  display.write(buf);
  display.write("\n");
  
  display.display();
  firstloop = false;
  delay(3000); // wait this many ms
}
