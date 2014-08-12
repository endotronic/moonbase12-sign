#include <Adafruit_NeoPixel.h>
#define STRIP_LENGTH 110
#define MS_DELAY 40
#define CHANCE_COLOR 750
#define CHANCE_DRIP 0
#define BLEND_FACTOR 25
#define FADE_FACTOR 99

// derived:
#define BUFFER_LENGTH (STRIP_LENGTH / 2)
#define BLEND_FACTOR_R (100 - (2 * BLEND_FACTOR))

struct CRGB
{
  uint8_t r, g, b;

    // allow construction from R, G, B
    inline CRGB(uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
        : r(ir), g(ig), b(ib)
    {
    }

    // allow construction from uint32_t
    inline CRGB(uint32_t color)  __attribute__((always_inline))
        : r(color >> 16), g((color >> 8) & 0xFF), b(color & 0xFF)
    {
    }

    inline CRGB()  __attribute__((always_inline))
      : r(0), g(0), b(0)
    {
    }
};


Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LENGTH, 6, NEO_GRB + NEO_KHZ800);
CRGB buffer[BUFFER_LENGTH], buffer_copy[BUFFER_LENGTH], dripColor;

void setup() {
  strip.begin();
  strip.show();

  // Turn "working" indicator on
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  memset(buffer, 0, BUFFER_LENGTH * sizeof(CRGB));
  pickNewDrip();
}

void loop() {
  if (random(0, CHANCE_COLOR) == 0) {
    pickNewDrip();
  }

  if (random(0, CHANCE_DRIP) == 0) {
    drip();
  }

  blend();
  fade();
  render();
  delay(MS_DELAY);
}

void pickNewDrip() {
  uint32_t r = random(0, 255), g = random(0, 255), b = random(0, 255);
  uint32_t m = max(max(r, g), b);
  uint32_t s = 250000 / m;

  dripColor.r = (r * s) / 1000;
  dripColor.g = (g * s) / 1000;
  dripColor.b = (b * s) / 1000;
}

void render() {
  for (uint8_t i = 0; i < BUFFER_LENGTH; i++) {
    strip.setPixelColor(i, buffer[i].r, buffer[i].g, buffer[i].b);
    strip.setPixelColor(STRIP_LENGTH - i - 1, buffer[i].r, buffer[i].g, buffer[i].b);
  }

  strip.show();
}

void drip() {
  buffer[random(0, BUFFER_LENGTH)] = dripColor;
}

void fade() {
  for (uint8_t i = 0; i < BUFFER_LENGTH; i++) {
    buffer[i].r = ((uint16_t)buffer[i].r * FADE_FACTOR) / 100;
    buffer[i].g = ((uint16_t)buffer[i].g * FADE_FACTOR) / 100;
    buffer[i].b = ((uint16_t)buffer[i].b * FADE_FACTOR) / 100;
  }
}

void blend() {
  memcpy(buffer_copy, buffer, BUFFER_LENGTH * sizeof(CRGB));

  for (uint8_t i = 0; i < BUFFER_LENGTH; i++) {
    CRGB *m1 = (i != 0) ? &buffer_copy[i - 1] : &buffer_copy[i];
    CRGB *p1 = (i != BUFFER_LENGTH - 1) ? &buffer_copy[i + 1] : &buffer_copy[i];

    buffer[i].r = ((BLEND_FACTOR_R * (uint16_t)buffer_copy[i].r) + (BLEND_FACTOR * (uint16_t)m1->r) + (BLEND_FACTOR * (uint16_t)p1->r)) / 100;
    buffer[i].g = ((BLEND_FACTOR_R * (uint16_t)buffer_copy[i].g) + (BLEND_FACTOR * (uint16_t)m1->g) + (BLEND_FACTOR * (uint16_t)p1->g)) / 100;
    buffer[i].b = ((BLEND_FACTOR_R * (uint16_t)buffer_copy[i].b) + (BLEND_FACTOR * (uint16_t)m1->b) + (BLEND_FACTOR * (uint16_t)p1->b)) / 100;
  }
}

