#include <CayenneMQTTESP8266.h>
#include <esp8266wifi.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#include "FastLED.h"
// Pride2015
// Animated, ever-changing rainbows.
// by Mark Kriegsman
// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY

#ifdef __AVR__
 #include <avr power.h="">
#endif
#define DHTPIN 4 // setting dht11 pin
#define DHTTYPE DHT11// setting type of the dht
#define PIN D5 // neopixel pin conected to d5
#define NUMPIXELS 75 // number of leds in neopixel ring
#define buzzer D7// set buzzer pin

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    D5
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
#define BRIGHTNESS  255
#define FRAMES_PER_SECOND 60
#define COOLING  55
#define SPARKING 120

CRGB leds[NUM_LEDS];

bool gReverseDirection = false;

DHT dht(DHTPIN, DHTTYPE);// object for dht11
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);// object for neopixel
char ssid[] = "class1";  
char password[] = "11111111";
char username[] = "4630a800-fca5-11ec-8df2-dd50487e509b";  
char mqtt_password[] = "2c75884416b0c0abc0355608931202f0276af83c";
char client_id[] = "d454d3b0-fcb2-11ec-bbc1-5d0b0fa0a668";
const int LDRpin = A0;// ldr pin
const int PIRpin = D0;// pir pin
int LDRval = 0;// for reading ldr value
int PIRval = 0;// for reading pir value  
int temp;// for reading temperature value from dht11
int hum;// for reading humidity value from dht11
int r,g,b;// variables for storing red green and blue led mix values
int Cmode;// variable for storing security mode activate or deactivate
int TURN;// variable for manuale mode
int SAFE_MODE;// variable for false safe mode
int rainbow;
int fire;


void setup()  
 {  
    pinMode(LDRpin,INPUT);// .....|
    pinMode(PIRpin,INPUT);//......| Seting sensors and modules input or output
    pinMode(buzzer,OUTPUT);//.....|
    dht.begin();// starting dht11 sensor
    Serial.begin(9600);
    Cayenne.begin(username,mqtt_password,client_id,ssid,password);// starting cayenne  
    pixels.begin(); // start neopixel ring

 //   delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip)
      .setDither(BRIGHTNESS < 255);

  // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

    Serial.println("FIRE FLY BOOTED");
  }

  
void loop()
  {   
  
    Cayenne.loop();  
  
  // checking security mode activated!
    if(Cmode==1)
    {
    buglerMode();// if secuirity mode active will call bugler mode function
    }
    if(rainbow==1)
     {
       pride();  
       FastLED.show();
//        setColor(100,0,0);
     }
    if (fire==1){
       Fire2012();
       FastLED.show();
//       FastLED.delay(1000 / FRAMES_PER_SECOND);
    }
    if(Cmode<1)
    {
    setLight();// if not active then call setlight function
    }
    
    hum = dht.readHumidity();// storing humidity value to hum variable
    temp = dht.readTemperature();//storing temperature value to temp variable
    LDRval = analogRead(LDRpin);// read and store ldr value
    PIRval = digitalRead(PIRpin); // read and store pir value
    Serial.println(PIRval);
    Serial.println(hum);
    Serial.println(temp);
    Serial.println(LDRval);
    
    // send all sensor values to cayenne
          Cayenne.virtualWrite(0,temp);
          Cayenne.virtualWrite(1,hum);
          Cayenne.virtualWrite(2,LDRval);
          Cayenne.virtualWrite(3,PIRval);
    // checking temperature or humidity is in dangerous level   

     
    if(hum>80||hum<15){
     digitalWrite(buzzer,HIGH);
    }
    if(temp>40||temp<10)
     {
       digitalWrite(buzzer,HIGH);
     }
    // checking false safe mode      
    if(SAFE_MODE==1)
     {
       digitalWrite(buzzer,LOW);
     }
  }

 


void setColor(int red,int green,int blue)
{
 for(int i =0;i<=NUMPIXELS;i++)// set every led into the color
 {
   pixels.setPixelColor(i,pixels.Color(red,green,blue));// seting color neopixel
   pixels.show();// activate neopixel
 }
}


void setLight()// checking manual mode active or not
{
  if(TURN==1)
  {
  manual();
  }
  else
  {
    if(PIRval>0)
    {
     setColor(r,g,b);
    }
    else if(LDRval<30)
    {
    setColor(r,g,b);
    }
    else if(PIRval<1&&LDRval>10)
    {
     setColor(0,0,0);
    }
   }
}


void buglerMode()// bugler mode's function
{
 if(SAFE_MODE==1)
 {
   digitalWrite(buzzer,LOW);
 }
 else if(Cmode==1 && PIRval>0)
 {
  digitalWrite(buzzer,HIGH);
  Serial.println("bugler on");
 }
}


void pride() 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[pixelnumber], newcolor, 64);
  }
}


void Fire2012()
{
// Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}


void manual()// manual mod's function
{
  setColor(r,g,b);
}
//recieve values from cayenne
CAYENNE_IN(4)
{
 r= getValue.asInt();// recieve red value for neopixel from cayenne
}
CAYENNE_IN(5)
{
 g= getValue.asInt();// recieve green value for neopixel from cayenne
}
CAYENNE_IN(6)
{
   b= getValue.asInt();// recieve blue value for neopixel from cayenne
}
CAYENNE_IN(7)
{
 Cmode =getValue.asInt();// recieve commands for security mode
 Serial.println(Cmode);
}
CAYENNE_IN(8)
{
 TURN = getValue.asInt();// recieve commands for manual mode
}
CAYENNE_IN(9)
{
 SAFE_MODE = getValue.asInt();// recieve commands for false safe
 Serial.println(SAFE_MODE);
}
CAYENNE_IN(10)
{
 rainbow = getValue.asInt();// recieve commands for rainbow button
}
CAYENNE_IN(11)
{
  fire = getValue.asInt();// recieve commands for fire button
}  
