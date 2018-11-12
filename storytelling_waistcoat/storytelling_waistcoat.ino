#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN1 10
#define PIN2 9

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel indicator = Adafruit_NeoPixel(1, PIN2, NEO_GRB + NEO_KHZ800);


//------------------------------------------
// Parameters you can change
//------------------------------------------

// SNOW mode
// change the color of the snow falling
int snow_R=88; // red value
int snow_G=100; // green value
int snow_B=64; // blue value

// change the number below for the snow mode speed
float snow_fade_speed = 0.03; //smaller this is, longer it falls
float snow_pass_speed=0.17;
int snow_fall_speed=25; // bigger the number, slower the light posisiton falls
int snow_fall_frequency=17; // smaller the number, more frequent

// MOOD MAKER mode
int moodSpeed=40; // the number indicates the speed of change


// RAINBOW sparkle mode
int sparkle_num=64; // change the number of the rainbow sparkle

//------------------------------------------
//------------------------------------------

int pot_val;
int sampleSize = 10;
int avg;

int red;
int green;
int blue;

boolean whiteOn;

int buttonPin[] = {4, 5, 6}; //button top, 2nd, 3rd (4th button does not have switch on it)
boolean buttonState[3];
boolean last_buttonState[3];

int mode = 0;;
int lastMode = 0;;

float intensity[40]; // decides intensity 0-1.0
float fadeSpeed[40]; // number of steps



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

  // store the last mode
  lastMode = mode;


  if (buttonState[1] == 0 && buttonState[1] != last_buttonState[1]) {

    // switch the moe according to the button state
    if (lastMode == 1) {
      mode = 0;
    }
    else {
      mode = 1;
    }

    // when pressed the first time, initialize the fadeFire
    if (mode == 1) {
      for (int i = 0; i < 40; i++) {
        initialize_fadeFire(i);
      }
    }
    delay(10);
  }
  last_buttonState[1] = buttonState[1];

  // button 2 (3rd button) with snow effect
  if (buttonState[2] == 0 && buttonState[2] != last_buttonState[2]) {
    // switch the moe according to the button state
    if (lastMode == 2) {
      mode = 0;
    }
    else {
      mode = 2;
    }

    // when pressed the first time, initialize the snow mode
    if (mode == 2) {
      for (int i = 0; i < 40; i++) {
        initialize_snow(i);
      }
      strip.show();
      drop_snow();
    }
    delay(10);
  }
  last_buttonState[2] = buttonState[2];


  // button 0 (top button) with rainbow effect
  if (buttonState[0] == 0 && buttonState[0] != last_buttonState[0]) {
    // rainbow sparkle!
    rainbow_sparkle(sparkle_num); // the number indicates the number/length of the sparkle
  }
  last_buttonState[0] = buttonState[0];


  // show on LEDs
  indicator.setPixelColor(0, indicator.Color(red, green, blue));
  indicator.show();


  Serial.println(mode);
  switch (mode) {
    case 0:
      fade_out(10);
      break;

    case 1:
      // do the fire thing, the number indicates the speed of change
      fadeFire(moodSpeed);
      break;

    case 2:
    // once in a while drop some snow
      if (millis() % snow_fall_frequency == 0) {
        drop_snow();
      }
      snow();
      break;
  }

  delay(10);
}



void rainbow_sparkle(int number_of_sparkel) {
  for (int i = 0; i < 40; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
  }
  for (int i = 0; i < number_of_sparkel; i++) {
    int star = random(40);
    strip.setPixelColor(star, Wheel((i * (256 / number_of_sparkel)) & 255));
    strip.show();
    delay(20);
    strip.setPixelColor(star, strip.Color(0, 0, 0));
  }
  delay(500);
}

void initialize_snow(int i) {
  // turn all off
  strip.setPixelColor(i, strip.Color(0, 0, 0));
  intensity[i] = 0.0;
}

void drop_snow() {
  // randomly drop the snow on left or right side of the shoulder
  int randomGen = random(13) % 2;
  if (randomGen == 0) {
    intensity[19] = (float)random(60, 100) / 100.0;
  }
  else {
    intensity[20] = (float)random(60, 100) / 100.0;
  }
}

void snow() {
  for (int i = 0; i < 20; i++) {
    intensity[i] = constrain(intensity[i], 0.0, 1.0);
    // calculate the color
    int sR = (int)snow_R * intensity[i];
    int sG = (int)snow_G * intensity[i];
    int sB = (int)snow_B * intensity[i];

    // make sure that the color stays within the range
    sR=constrain(sR,0,255);
    sG=constrain(sG,0,255);
    sB=constrain(sB,0,255);

    // set the color on the pixel
    strip.setPixelColor(i, strip.Color(sR, sG, sB));

    // if the intensity is bigger than 0, pass the intensity to the next pixel
    if (intensity[i] > 0.0) {
      if (i > 0) {
        intensity[i - 1] = intensity[i] - snow_fade_speed; // lower the
      }
      intensity[i] = intensity[i] - snow_pass_speed;
    }
  }

  for (int i = 39; i > 19; i--) {
    intensity[i] = constrain(intensity[i], 0.0, 1.0);
    // calculate the color
    int sR = (int)snow_R * intensity[i];
    int sG = (int)snow_G * intensity[i];
    int sB = (int)snow_B * intensity[i];
    // make sure that the color stays within the range
    sR=constrain(sR,0,255);
    sG=constrain(sG,0,255);
    sB=constrain(sB,0,255);

    // set the color on the led
    strip.setPixelColor(i, strip.Color(sR, sG, sB));

    if (intensity[i] > 0.0) {
      if (i < 39) {
        intensity[i + 1] = intensity[i] - snow_fade_speed;
      }
      intensity[i] = intensity[i] - snow_pass_speed;
    }
  }
  strip.show();
  delay(snow_fall_speed);
}

void initialize_fadeFire(int i) {

  // decide what is addded
  int acc = random(4, 10);
  fadeSpeed[i] = (float)acc / 200.0; // number of steps

}

void fadeFire(int delayAmount) {

  for (int i = 0; i < 40; i++) {
    // make sure that intensity stays within 0-1
    intensity[i] = constrain(intensity[i], 0.0, 1.0);
    // calculate the red ammount in float
    float rr = red * intensity[i];
    // cast to int
    int RR = (int)rr;
    // calculate the green ammount in float
    float gg = green * intensity[i];
    // cast to int
    int GG = (int)gg;
    // calculate the blue ammount in float
    float bb = blue * intensity[i];
    // cast to int
    int BB = (int)bb;
    // set the pixel color
    strip.setPixelColor(i, strip.Color(RR, GG, BB));
    if (whiteOn) {
      if (i % 2 == 0) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }

    //---------------------------
    // culculate the next intensity
    //---------------------------

    intensity[i] = intensity[i] + fadeSpeed[i];
    // if it reaches 100% then reverse the fading
    if (intensity[i] >= 1.0) {
      fadeSpeed[i] = -fadeSpeed[i];
    }
    // if it reaches 0% initialize new
    if (intensity[i] <= 0.0) {
      initialize_fadeFire(i);
    }
    //---------------------------
  }
  strip.show();
  // wait, this will change the speed
  delay(delayAmount);
}

void fade_out(int delayAmount) {
  for (int i = 0; i < 40; i++) {
    if (fadeSpeed[i] > 0) {
      fadeSpeed[i] = -fadeSpeed[i];
    }
    intensity[i] = intensity[i] + fadeSpeed[i];

    // make sure that intensity stays within 0-1
    intensity[i] = constrain(intensity[i], 0.0, 1.0);
    // calculate the red ammount in float
    float rr = red * intensity[i];
    // cast to int
    int RR = (int)rr;
    // calculate the green ammount in float
    float gg = green * intensity[i];
    // cast to int
    int GG = (int)gg;
    // calculate the blue ammount in float
    float bb = blue * intensity[i];
    // cast to int
    int BB = (int)bb;
    // set the pixel color
    strip.setPixelColor(i, strip.Color(RR, GG, BB));
  }
  strip.show();
  // wait, this will change the speed
  delay(delayAmount);
}

void colorSelector(int val) {
  val = map(val, 0, 470, 0, 767);
  val = constrain(val, 0, 767);

  int tempVal;
  if (val < 255) {
    tempVal = val;
    tempVal = constrain(tempVal, 0, 255);
    red = 255 - tempVal;
    green = tempVal;
    blue = 0;
    whiteOn = false;
  }

  if (val > 255 && val < 516) {
    tempVal = val - 255;
    tempVal = constrain(tempVal, 0, 255);
    red = 0;
    green = 255 - tempVal;
    blue = tempVal;
    whiteOn = false;
  }
  if (val > 516 && val < 760) {
    tempVal = val - 516;
    tempVal = constrain(tempVal, 0, 255);
    red = tempVal;
    green = 0;
    blue = 255 - tempVal;
    whiteOn = false;
  }
  if (val > 760) {
    red = 128;
    green = 128;
    blue = 128;
    whiteOn = true;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(WheelPos * 3, 0, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(0, 255 - WheelPos * 3, WheelPos * 3);
}

void rainbow() {
  // turn all of them off
  for (int i = 0; i < 40; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();

  // turn the pixel one by one in rainbow color
  for (int i = 0; i < 40; i++) {
    strip.setPixelColor(i, Wheel((i * 256 / 40) & 255));
    strip.show();
    delay(20);
  }

  for (int j = 0; j < 128; j++) {
    for (int i = 0; i < 40; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / 40) + j) & 255));
    }
    strip.show();
    delay(5);
  }


  // turn the pixel one by one in rainbow color
  for (int i = 40; i > 0; i--) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(20);
  }


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
