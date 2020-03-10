// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>
#include <VirtualWire.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN_LEFT     3
#define LED_PIN_CENTER     6
#define LED_PIN_RIGHT     7
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  1

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 150

//Unity Conection
#define SERIAL_USB

/********************
  Code for Heartbeat
 ********************/

//Variables for heartbeat where shows BPM and IBI
int UpperThreshold = 515;
int LowerThreshold = 495;
int reading = 0;
float BPM = 0.0;
bool IgnoreReading = false;
bool FirstPulseDetected = false;
unsigned long FirstPulseTime = 0;
unsigned long SecondPulseTime = 0;
unsigned long PulseInterval = 0;
const unsigned long delayTime = 10;
const unsigned long delayTime2 = 1000;
const unsigned long baudRate = 9600;
unsigned long previousPulseMillis = 0;
unsigned long previousMillis2 = 0;


/****************************
  Code for leds RGB Neopixel
 ****************************/


int brake;
const int pot = 3;
int algo = 50;
int count = 0;
int turnLeft = 0;
int turnRight = 0;
int blinker = 0;
int daytime = 0;
unsigned long previousMillis = 0;
unsigned long timeToAction = 500;
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN_CENTER, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN_RIGHT, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel strip3(LED_COUNT, LED_PIN_LEFT, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


/*************************
  Code for RF433 Receiver
 *************************/
const int pin_de_recepcion = 11;

void setup() {
  delay(1000);

#ifdef SERIAL_USB
  Serial.begin(baudRate); // You can choose any baudrate, just need to also change it in Unity.
  while (!Serial); // wait for Leonardo enumeration, others continue immediately
#endif
  pinMode(13, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip2.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip2.show();            // Turn OFF all pixels ASAP
  strip2.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip3.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip3.show();            // Turn OFF all pixels ASAP
  strip3.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)

  // Inicializa IO y ISR
  vw_set_rx_pin(pin_de_recepcion);
  vw_setup(2000); // Bits por segundo
  vw_rx_start(); // Comienzo de recepción

}

void loop() {

  /******************************************
    Loop for receive data from RF Transmitter
   ******************************************/
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  //Serial.print("BUFLEN: ");
  //Serial.println(buflen);

  if (vw_get_message(buf, &buflen)) { // Non-blocking


    brake = convertToInt(0, 1, buf);
    turnLeft = convertToInt(2, 3, buf);
    turnRight = convertToInt(4, 5, buf);
    blinker = convertToInt(6, 7, buf);
    daytime = convertToInt(8, 9, buf);


    Serial.print("brake: ");
    Serial.print(brake);
    Serial.print(" left: ");
    Serial.print(turnLeft);
    Serial.print(" right: ");
    Serial.print(turnRight);
    Serial.print(" blink: ");
    Serial.print(blinker);
    Serial.print(" daytime: ");
    Serial.println(daytime);

  }

  /***************
    HeartBeat Loop
   ***************/

  // Get current time
  unsigned long currentPulseMillis = millis();

  // First event
  if (myTimer1(delayTime, currentPulseMillis) == 1) {

    reading = analogRead(4);

    // Heart beat leading edge detected.
    if (reading > UpperThreshold && IgnoreReading == false) {
      if (FirstPulseDetected == false) {
        FirstPulseTime = millis();
        FirstPulseDetected = true;
      }
      else {
        SecondPulseTime = millis();
        PulseInterval = SecondPulseTime - FirstPulseTime;
        FirstPulseTime = SecondPulseTime;
      }
      IgnoreReading = true;
      digitalWrite(LED_BUILTIN, HIGH);
    }

    // Heart beat trailing edge detected.
    if (reading < LowerThreshold && IgnoreReading == true) {
      IgnoreReading = false;
      digitalWrite(LED_BUILTIN, LOW);
    }

    // Calculate Beats Per Minute.
    BPM = (1.0 / PulseInterval) * 60.0 * 1000;
  }

  // Second event
  if (myTimer2(delayTime2, currentPulseMillis) == 1) {
    //Serial.print(reading);
    //Serial.print("\t");
    //Serial.print(PulseInterval);
    //Serial.print("\t");
    //Serial.print(BPM);
    //Serial.println(" BPM");
    //Serial.flush();
  }

  /****************************
    Loop for LED's RGB Neopixel
   ****************************/

  digitalWrite(13, HIGH);






  if (!brake)
    Stop();
  else
    colorWipe(0    , BRIGHTNESS); // Menta 25, 217,220
  count = 0;
  if (turnLeft)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= timeToAction && count < 5) {
      BlinkLeft(strip2.Color(0, 0, 255), 50);
      algo = abs(algo - 50);
      strip2.setBrightness(algo);
      previousMillis = currentMillis;
      count++;
    }
    else if (count >= 5) {
      turnLeft = false;

    }
  }
  else {
    count = 0;
    if (turnRight)
    {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= timeToAction && count < 6) {
        BlinkRight(strip3.Color(0,   0,   255)     , 50);
        algo = abs(algo - 50);
        strip3.setBrightness(algo);
        previousMillis = currentMillis;
        count++;
      }
      else if (count >= 5)
        turnRight = false;

    }
    else {
      count = 0;
      if (blinker) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= timeToAction && count < 6) {
          Blink(strip2.Color(250,  0, 80)     , 50);
          algo = abs(algo - 50);
          strip2.setBrightness(algo);
          strip3.setBrightness(algo);
          previousMillis = currentMillis;
          count++;
        }
        else if (count >= 5)
          turnLeft = false;
        turnRight = false;
      }
      else {
        count = 0;
        Blink(strip2.Color(0,  0,   255)     , 50);
        strip2.setBrightness(50);
        strip3.setBrightness(50);

      }
    }
  }
}

/*******************************
  Methods for Neopixel LED's RGB
 *******************************/

void Stop() {
  colorWipe(1 , 100); // Menta 25, 217,220

  strip.setBrightness(100);
}
// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(int color, int wait) {
  Serial.println();

  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    if (color) {
      strip.setPixelColor(i, 255, 0, 0, 0);   //  Set pixel's color (in RAM)
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 255));       //  Set pixel's color (in RAM)
    }
    strip.show();                          //  Update strip to match
    delay(25);                           //  Pause for a moment
  }
}


void BlinkLeft(uint32_t color, int wait) {
  for (int i = 0; i < strip2.numPixels(); i++) { // For each pixel in strip...
    strip2.setPixelColor(i, color);        //  Set pixel's color (in RAM)
    strip2.show();                          //  Update strip to match
    //delay(wait);                           //  Pause for a moment
  }
}

void Blink(uint32_t color, int wait) {
  for (int i = 0; i < strip2.numPixels(); i++) { // For each pixel in strip...
    strip2.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip2.show();                          //  Update strip to match
    strip3.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip3.show();                          //  Update strip to match
    //delay(wait);                           //  Pause for a moment
  }
}
void BlinkRight(uint32_t color, int wait) {
  for (int i = 0; i < strip3.numPixels(); i++) { // For each pixel in strip...
    strip3.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip3.show();                          //  Update strip to match
    //delay(wait);                           //  Pause for a moment
  }
}

/*****************************
  Methods for HeartBeat Sensor
 *****************************/

// First event timer
int myTimer1(long delayTime, long currentPulseMillis) {
  if (currentPulseMillis - previousPulseMillis >= delayTime) {
    previousPulseMillis = currentPulseMillis;
    return 1;
  }
  else {
    return 0;
  }
}

// Second event timer
int myTimer2(long delayTime2, long currentPulseMillis) {
  if (currentPulseMillis - previousMillis2 >= delayTime2) {
    previousMillis2 = currentPulseMillis;
    return 1;
  }
  else {
    return 0;
  }
}

int convertToInt(int a, int b, char const* buf) {
  char buf_aux[2];
  buf_aux[0] = buf[a];
  buf_aux[1] = buf[b];

  return atoi((char*)buf_aux);
}

void sendData(String data) {
#ifdef NATIVE_USB
  Serial.println(data); // need a end-line because wrmlh.csharp use readLine method to receive data
#endif

#ifdef SERIAL_USB
  Serial.println(data); // need a end-line because wrmlh.csharp use readLine method to receive data
#endif
}
void sendData(int data) {
#ifdef NATIVE_USB
  Serial.println(data); // need a end-line because wrmlh.csharp use readLine method to receive data
#endif

#ifdef SERIAL_USB
  Serial.println(data); // need a end-line because wrmlh.csharp use readLine method to receive data
#endif
}
