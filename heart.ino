#include <FastLED.h>

#define NUM_LEDS    19
#define SPARKING          120
#define NUM_LEDS          19
#define COOLING           55
#define RUN_PATTERNS      1
#define RUN_PALETTE         2
#define RUN_FIRE          3

#define UPDATES_PER_SECOND 100

#define BUTTON_1          23
#define BUTTON_2          22
#define BUTTON_3          21

CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

CRGB strip[NUM_LEDS];

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
bool gReverseDirection = false;
int g_program;

// Params for width and height
const uint8_t kMatrixWidth = NUM_LEDS;
const uint8_t kMatrixHeight = 1;
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)
//#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
// Param for different pixel layouts
const bool kMatrixSerpentineLayout = true;

// The 32bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

void rainbow(CRGB *leds, int count);
void rainbowWithGlitter(CRGB *leds, int count);
void confetti(CRGB *leds, int count);
void sinelon(CRGB *leds, int count);
void juggle(CRGB *leds, int count);
void bpm(CRGB *leds, int count);

typedef void (*SimplePatternList[])(CRGB*, int);
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        strip[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void runColorPalette()
{
  EVERY_N_MILLISECONDS(100) {
    ChangePalettePeriodically();
    
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors(startIndex);
    
    FastLED.show();
  }
}

void rainbow(CRGB *leds, int count) 
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, count, gHue, 7);
}

void rainbowWithGlitter(CRGB *leds, int count) 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow(leds, count);
  addGlitter(leds, count, 80);
}

void addGlitter(CRGB *leds, int count, fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[random16(count) ] += CRGB::White;
  }
}

void confetti(CRGB *leds, int count) 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, count, 10);
  int pos = random16(count);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(CRGB *leds, int count)
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, count, 20);
  int pos = beatsin16(13, 0, count - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm(CRGB *leds, int count)
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < count; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle(CRGB *leds, int count)
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, count, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, count - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void Fire2012(CRGB *strip, int count)
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < count; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / count) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= count - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < count; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      strip[pixelnumber] = color;
    }
}

void cylon(CRGB *leds, int count)
{
  static uint8_t hue = 0;
  
  for(int i = 0; i < count; i++) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show(); 
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall(leds, count);
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
  Serial.print("x");

  // Now go in the other direction.  
  for(int i = (count)-1; i >= 0; i--) {
    // Set the i'th led to red 
    leds[i] = CHSV(hue++, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall(leds, count);
    // Wait a little bit before we loop around and do it again
    delay(10);
  }
}

void runPatterns()
{
  while (g_program == RUN_PATTERNS) {
    gPatterns[gCurrentPatternNumber](strip, NUM_LEDS);
  }
}

void startupDisplay(CRGB *leds, int count)
{
  for (int i = 0; i < count; i++) {
    leds[i] = CRGB::White;
  }
  FastLED.setBrightness(100);
}

void fadeall(CRGB *leds, int count) 
{ 
  for (int i = 0; i < count; i++) { 
    leds[i].nscale8(250); 
  } 
}

void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for (int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if (random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for (int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      strip[pixelnumber] = color;
    }
    FastLED.show();
}

void setPatternProgram()
{
  if (g_program != RUN_PATTERNS) {
    g_program = RUN_PATTERNS;
    gCurrentPatternNumber = 0;
  }
  else {
    if (gCurrentPatternNumber == 5) {
      gCurrentPatternNumber = 0;
    }
    else {
      gCurrentPatternNumber++;
    }
  }
}

void setFireProgram()
{
  g_program = RUN_FIRE;
}

void setPaletteProgram()
{
  g_program = RUN_PALETTE;
}

void setup() 
{
  delay(3000);
  // put your setup code here, to run once:
  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();
  FastLED.addLeds<APA102, 11, 13>(strip, NUM_LEDS);

  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);
  
  attachInterrupt((BUTTON_1), setPatternProgram, RISING);
  attachInterrupt((BUTTON_2), setFireProgram, RISING);
  attachInterrupt((BUTTON_3), setPaletteProgram, RISING);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  switch (g_program) {
    case RUN_PATTERNS:
      runPatterns();
      break;
    case RUN_PALETTE:
      runColorPalette();
      break;
    case RUN_FIRE:
      Fire2012();
      break;
  }

}
