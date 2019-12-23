#pragma once

// Put your texts and LED setups here!
#include "badge.h"

typedef void (*func_t)(void);

// Special flags to display data instead of a normal text
typedef enum VFDTextFlags {
  TF_NONE,        // Regular text
  TF_BAT_VOLT,    // Display battery voltage (~3000-4200; requires %d in the text)
  TF_BAT_PERCENT, // Display battery level in percent (0-100; requires %d in the text)
  TF_PWR_SRC,     // Display power source (USB/BAT; requires %s in the text)
  TF_CHG_STAT,    // Display charging status (YES/NO; requires %s in the text)
  TF_LOW_BAT_STAT // Display low battery status (YES/NO; requires %s in the text)
} vfd_text_flags_t;

typedef struct VFDText {
  const char *text;
  vfd_animation_t animation;
  uint8_t scrollSpeed;
  uint16_t duration;
  vfd_text_flags_t flags;
} vfd_text_t;

typedef struct VFDTexts {
  uint8_t count;
  vfd_text_t *texts;
} vfd_text_list_t;

typedef struct LEDAnimation {
  func_t setupFunc;
  func_t animFunc;
  uint16_t updateInterval;
  uint16_t duration;
} led_animation_t;

typedef struct LEDAnimations {
  uint8_t count;
  led_animation_t *animations;
} led_animation_list_t;

const vfd_text_list_t VFD_TEXTS[] = {
  { 5, (vfd_text_t[]) {
      { "NO ANIMATION", ANIMATION_NONE, 0, 3000 },
      { "FADING ANIMATION WITH SCROLLING TEXT              ", ANIMATION_FADE, 8, 5000, TF_NONE },
      { "FLIP ANIMATION GOTTA GO FAST     ", ANIMATION_FLIP, 4, 4000, TF_NONE },
      { "RANDOM ANIM ", ANIMATION_RANDOM, 0, 2000, TF_NONE },
      { "SLIDING ANIM", ANIMATION_SLIDE, 0, 2000, TF_NONE },
    }
  },
  { 2, (vfd_text_t[]) {
      { "Another text", ANIMATION_FADE, 0, 2000, TF_NONE },
      { "and another ", ANIMATION_FADE, 0, 2000, TF_NONE },
    }
  },
  { 5, (vfd_text_t[]) {
      { "BAT %d MV", ANIMATION_NONE, 0, 1000, TF_BAT_VOLT },
      { "BAT LVL %d", ANIMATION_NONE, 0, 1000, TF_BAT_PERCENT },
      { "PWR SRC %s", ANIMATION_NONE, 0, 1000, TF_PWR_SRC },
      { "CHARGING %s", ANIMATION_NONE, 0, 1000, TF_CHG_STAT },
      { "LOW BATT %s", ANIMATION_NONE, 0, 1000, TF_LOW_BAT_STAT },
    }
  }
};

uint8_t ledD1Level, ledD1LevelOld = 0;
uint8_t ledD2Level, ledD2LevelOld = 0;
uint8_t ledD3Level, ledD3LevelOld = 0;
uint8_t ledH1Level, ledH1LevelOld = 0;
uint8_t ledH2Level, ledH2LevelOld = 0;

uint8_t ledD1Fading = 0;
uint8_t ledD2Fading = 0;
uint8_t ledD3Fading = 0;
uint8_t ledH1Fading = 0;
uint8_t ledH2Fading = 0;

void updateLEDs() {
  badge.setCrack(DESTRUCTION1, ledD1Level);
  badge.setCrack(DESTRUCTION2, ledD2Level);
  badge.setCrack(DESTRUCTION3, ledD3Level);
  badge.setCrack(HOPE1, ledH1Level);
  badge.setCrack(HOPE2, ledH2Level);
  ledD1LevelOld = ledD1Level;
  ledD2LevelOld = ledD2Level;
  ledD3LevelOld = ledD3Level;
  ledH1LevelOld = ledH1Level;
  ledH2LevelOld = ledH2Level;
}

void anim11Setup() {
  ledD1Level = 63;
  ledD2Level = 0;
  ledD3Level = 0;
  ledH1Level = 0;
  ledH2Level = 0;
  ledD1Fading = 0;
  ledD2Fading = 1;
  ledD3Fading = 0;
  ledH1Fading = 0;
  ledH2Fading = 0;
  updateLEDs();
}

void anim11Loop() {
  if (ledD1Level == 63) {
    ledD2Fading = 1;
    ledD1Fading = 0;
  } else if (ledD2Level == 63) {
    ledH1Fading = 1;
    ledD2Fading = 0;
  } else if (ledH1Level == 63) {
    ledD3Fading = 1;
    ledH1Fading = 0;
  } else if (ledD3Level == 63) {
    ledH2Fading = 1;
    ledD3Fading = 0;
  } else if (ledH2Level == 63) {
    ledD1Fading = 1;
    ledH2Fading = 0;
  }
  if (ledD1Fading) {
    ledD1Level++;
    ledH2Level--;
  } else if (ledD2Fading) {
    ledD2Level++;
    ledD1Level--;
  } else if (ledH1Fading) {
    ledH1Level++;
    ledD2Level--;
  } else if (ledD3Fading) {
    ledD3Level++;
    ledH1Level--;
  } else if (ledH2Fading) {
    ledH2Level++;
    ledD3Level--;
  }
  updateLEDs();
}

void anim21Setup() {
  ledD1Level = 63;
  ledD2Level = 63;
  ledD3Level = 63;
  ledH1Level = 0;
  ledH2Level = 0;
  updateLEDs();
}

void anim21Loop() {

}

void anim22Setup() {
  ledD1Level = 0;
  ledD2Level = 0;
  ledD3Level = 0;
  ledH1Level = 63;
  ledH2Level = 63;
  updateLEDs();
}

void anim22Loop() {

}

const led_animation_list_t LED_ANIMATIONS[] = {
  { 1, (led_animation_t[]) {
      { anim11Setup, anim11Loop, 5, 2000 },
    }
  },
  { 2, (led_animation_t[]) {
      { anim21Setup, anim21Loop, 5, 500 },
      { anim22Setup, anim22Loop, 5, 500 },
    }
  }
};
