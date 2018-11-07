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
  Serial.println(pot_val);

  // select the color
  colorSelector(pot_val);

  // read buttons
  for (int t = 0; t < 3; t++) {
    buttonState[t] = digitalRead(buttonPin[t]);
  }

 

  if (buttonState[1] == 0 && buttonState[1] != last_buttonState[1]) {
    fire_mode = !fire_mode;
    // when pressed the first time, initialize the 
    if (fire_mode) {
      for (int i = 0; i < 40; i++) {
        initialize_fadeFire(i);
      }
    }
  }
  last_buttonState[1] = buttonState[1];

// button 2 (3rd button) with sparkle effect
  if (buttonState[2] == 0 && buttonState[2] != last_buttonState[2]) {
    // sparkle!
    sparkle(80); // the number indicates the number/length of the sparkle
  }
  last_buttonState[2] = buttonState[2];

  
// button 0 (top button) with rainbow effect
   if (buttonState[0] == 0 && buttonState[0] != last_buttonState[0]) {
    // rainbow!
    rainbow(72);
  }
  last_buttonState[0] = buttonState[0];



  // show on LEDs
  indicator.setPixelColor(0, indicator.Color(red, green, blue));
  indicator.show();



  if (fire_mode) {
    // do the fire thing, the number indicates the speed of change
    fadeFire(40);
  }
  else {
    // fade off
    fade_out(10);
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

void rainbow(int number_of_sparkel) {
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

void fade_out(int delayAmount){
   for (int i = 0; i < 40; i++) {
    if (fadeSpeed[i] >0){
      fadeSpeed[i]=-fadeSpeed[i];
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
val=map(val,0,470,0,767);
val=constrain(val,0,767);
  
  int tempVal;
  if (val < 255) {
    tempVal = val;
    tempVal = constrain(tempVal, 0, 255);
    red = 255 - tempVal;
    green = tempVal;
    blue = 0;
  }

  if (val > 255 && val < 516) {
    tempVal = val - 255;
    tempVal = constrain(tempVal, 0, 255);
    red = 0;
    green = 255 - tempVal;
    blue = tempVal;
  }
  if (val > 516 && val < 768) {
    tempVal = val - 516;
    tempVal = constrain(tempVal, 0, 255);
    red = tempVal;
    green = 0;
    blue = 255 - tempVal;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(WheelPos * 3, 0,255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(0,255 - WheelPos * 3,WheelPos * 3);
}

