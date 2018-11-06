#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN1 9
#define PIN2 10

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel indicator = Adafruit_NeoPixel(1, PIN2, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


int pot_val;
int sampleSize = 10;
int avg;


int red;
int green;
int blue;

int buttonPin[] = {6, 5, 4};
boolean buttonState[3];
boolean last_buttonState[3];

boolean fire_mode;

int fadeSpeed[40]; // number of steps
int fadeStep_R[40]; // how much it adds each time
int fadeStep_G[40]; // how much it adds each time
int fadeStep_B[40]; // how much it adds each time
boolean drc[40];// fade direction
int fadeRed[40];
int fadeGreen[40];
int fadeBlue[40];




void setup() {

  // setup neo pixels. strip is the jacket
  strip.begin();
  strip.setBrightness(128);
  strip.show(); // Initialize all pixels to 'off'

  // setup neo pixels. indicator is the one LED on the board
  indicator.begin();
  indicator.setBrightness(16);
  indicator.show(); // Initialize all pixels to 'off'

  for (int i = 0; i < 3; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  // set initial value for potentiometer reading
  avg = analogRead(A1);

  // set the random with analog pin4 reading
  randomSeed(analogRead(4));


  // start serial communication for debugging
  Serial.begin(9600);
  Serial.println("hello");
}

void loop() {

  // read the potentiometer to decide on the color
  pot_val = analogRead(A1);
  avg = (avg * (sampleSize - 1) + pot_val) / sampleSize;
  pot_val = avg;

  // select the color
  colorSelector(pot_val);

  // read buttons
  for (int t = 0; t < 3; t++) {
    buttonState[t] = digitalRead(buttonPin[t]);

  }
  //Serial.println();

  if (buttonState[1] == 0 && buttonState[1] != last_buttonState[1]) {
    fire_mode = !fire_mode;
    if (fire_mode) {
      for (int i = 0; i < 40; i++) {
        initialize_fadeFire(i);
      }
    }
  }
  last_buttonState[1] = buttonState[1];

  if (buttonState[2] == 0 && buttonState[2] != last_buttonState[2]) {
    // sparkle!
    sparkle(100); // the number indicates the number/length of the sparkle
  }
  last_buttonState[2] = buttonState[2];


  // show on LEDs
  indicator.setPixelColor(0, indicator.Color(red, green, blue));
  indicator.show();



  if (fire_mode) {
    // do the fire thing, the number indicates the speed of change
    fadeFire(10);
  }
  else {
    // turn off
    for (int i = 0; i < 40; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
  }

  delay(10);
}

void sparkle(int number_of_sparkel) {
  for (int i = 0; i < 40; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
  for (int i = 0; i < number_of_sparkel; i++) {
    int star = random(40);
    strip.setPixelColor(star, strip.Color(255, 255, 255));
    strip.show();
    delay(10);
    strip.setPixelColor(star, strip.Color(0, 0, 0));
  }
}

void randomFire(int delayAmount) {
  for (int i = 0; i < 40; i++) {
    float intensity = random(100);
    int t = red * intensity;
    float tempC = (float)t / 100.0;
    int rrr = (int)tempC;
    t = green * intensity;
    tempC = (float)t / 100.0;
    int ggg = (int)tempC;
    t = blue * intensity;
    tempC = (float)t / 100.0;
    int bbb = (int)tempC;
    strip.setPixelColor(i, strip.Color(rrr, ggg, bbb));
  }
  strip.show();
  delay(delayAmount);
}

void initialize_fadeFire(int i) {
  int fadeSpeed = random(10, 100); // randomly set how many steps it needs to fade
  int currentStep = random (2, fadeSpeed);
  drc[i] = random(1);

  fadeStep_R[i] = red / fadeSpeed;
  fadeStep_G[i] = green / fadeSpeed;
  fadeStep_B[i] = blue / fadeSpeed;

  fadeRed[i] = fadeStep_R[i] * currentStep;
  fadeGreen[i] = fadeStep_G[i] * currentStep;
  fadeBlue[i] = fadeStep_B[i] * currentStep;

  if (!drc) {
    fadeStep_R[i] = -fadeStep_R[i];
    fadeStep_G[i] = -fadeStep_G[i];
    fadeStep_B[i] = -fadeStep_B[i];
  }

}

void fadeFire(int delayAmount) {
  for (int i = 0; i < 40; i++) {
    fadeRed[i] = fadeRed[i] + fadeStep_R[i];
    fadeGreen[i] = fadeGreen[i] + fadeStep_G[i];
    fadeStep_B[i] = fadeStep_B[i] + fadeStep_B[i];

    if (fadeRed[i] >= red) {
      fadeStep_R[i] = -fadeStep_R[i];
      fadeStep_G[i] = -fadeStep_G[i];
      fadeStep_B[i] = -fadeStep_B[i];
    }

    fadeRed[i] = constrain(fadeRed[i], 0, red);
    fadeGreen[i] = constrain(fadeGreen[i], 0, green);
    fadeBlue[i] = constrain(fadeBlue[i], 0, blue);
    strip.setPixelColor(i, strip.Color(fadeRed[i], fadeGreen[i], fadeBlue[i]));

    if (fadeRed[i] == 0) {
      initialize_fadeFire(i);
    }
  }
  strip.show();
  delay(delayAmount);
}

void colorSelector(int val) {
  int tempVal;
  if (val < 384) {
    tempVal = val - 128;
    tempVal = constrain(tempVal, 0, 255);
    red = 255 - tempVal;
    green = tempVal;
    blue = 0;
  }

  if (val > 384 && val < 640) {
    tempVal = val - 384;
    tempVal = constrain(tempVal, 0, 255);
    red = 0;
    green = 255 - tempVal;
    blue = tempVal;
  }
  if (val > 640 && val < 896) {
    tempVal = val - 640;
    tempVal = constrain(tempVal, 0, 255);
    red = tempVal;
    green = 0;
    blue = 255 - tempVal;
  }
  if (val > 896) {
    tempVal = val - 768;
    tempVal = constrain(tempVal, 0, 255);
    red = tempVal;
    green = tempVal;
    blue = tempVal;
  }

}

