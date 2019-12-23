/*
   Cato's 36C3 Badge
   https://github.com/Mezgrman/36C3-Badge

   Code to control the VFD based on:
   https://github.com/qrti/VFD-HCS-12SS59T
   https://github.com/m42uko/hcs12ss59t
*/

#include "badge.h"
#include "config.h"
#include "util.h"

char text[VFD_BUF_SIZE];
uint8_t curUSB, oldUSB = 0;
uint8_t curChg, oldChg = 0;
uint8_t curLow, oldLow = 0;
t_Buttons oldButtons, curButtons;

uint8_t curVFDTextListIndex = 0;
uint8_t curVFDTextIndex = 0;

uint8_t curLEDAnimationListIndex = 0;
uint8_t curLEDAnimationIndex = 0;

vfd_text_list_t curVFDTextList;
vfd_text_t curVFDText;
led_animation_list_t curLEDAnimationList;
led_animation_t curLEDAnimation;

uint8_t forceVFDTextUpdate = 1;
uint8_t forceLEDAnimationUpdate = 1;

uint64_t lastLEDAnimationUpdate = 0;
uint64_t lastLEDAnimationSwitch = 0;
uint64_t lastVFDTextSwitch = 0;
uint64_t now = 0;

void setup()
{
  Serial.begin(115200);
  badge.begin();

  curVFDTextList = VFD_TEXTS[curVFDTextListIndex];
  curVFDText = curVFDTextList.texts[curVFDTextIndex];
  curLEDAnimationList = LED_ANIMATIONS[curLEDAnimationListIndex];
  curLEDAnimation = curLEDAnimationList.animations[curLEDAnimationIndex];
}

void loop()
{
  now = millis();
  curUSB = badge.pwrGetUSB();
  curChg = badge.pwrGetCharging();
  curLow = badge.pwrGetLowBatt();
  curButtons = badge.btnGetAll();

  if (curUSB && !oldUSB) {
    badge.vfdWriteText("USB POWER");
    delay(1000);
  } else if (!curUSB && oldUSB) {
    badge.vfdWriteText("BATT POWER");
    delay(1000);
  }

  if (curChg && !oldChg) {
    badge.vfdWriteText("CHARGING");
    delay(1000);
  } else if (!curChg && oldChg) {
    badge.vfdWriteText("CHARGED");
    delay(1000);
  }

  if (curLow && !oldLow) {
    badge.vfdWriteText("LOW BATT");
    delay(1000);
  }

  if (!(oldButtons & SW_A) && (curButtons & SW_A)) {
    // Button A has been pressed, cycle VFD text list
    curVFDTextListIndex++;
    if (curVFDTextListIndex >= ArraySize(VFD_TEXTS)) curVFDTextListIndex = 0;
    curVFDTextIndex = 0;
    curVFDTextList = VFD_TEXTS[curVFDTextListIndex];
    curVFDText = curVFDTextList.texts[curVFDTextIndex];
    forceVFDTextUpdate = 1; // force update
  }

  if (!(oldButtons & SW_B) && (curButtons & SW_B)) {
    // Button B has been pressed, cycle LED animation list
    curLEDAnimationListIndex++;
    if (curLEDAnimationListIndex >= ArraySize(LED_ANIMATIONS)) curLEDAnimationListIndex = 0;
    curLEDAnimationIndex = 0;
    curLEDAnimationList = LED_ANIMATIONS[curLEDAnimationListIndex];
    curLEDAnimation = curLEDAnimationList.animations[curLEDAnimationIndex];
    forceLEDAnimationUpdate = 1;
  }

  if ((now - lastVFDTextSwitch) >= curVFDText.duration || forceVFDTextUpdate) {
    if (!forceVFDTextUpdate) curVFDTextIndex++;
    if (curVFDTextIndex >= curVFDTextList.count) curVFDTextIndex = 0;
    curVFDText = curVFDTextList.texts[curVFDTextIndex];
    badge.vfdSetScrollSpeed(0);
    badge.vfdAnimate(curVFDText.text, curVFDText.animation);
    badge.vfdSetScrollSpeed(curVFDText.scrollSpeed);
    lastVFDTextSwitch = now;
    forceVFDTextUpdate = 0;
  }

  if ((curLEDAnimationList.count > 1) && ((now - lastLEDAnimationSwitch) >= curLEDAnimation.duration) || forceLEDAnimationUpdate) {
    // Switch only if there's more than one animation in the list or an update is forced
    // This way, if there's only one animation, it won't be reset after the cycle duration
    if (!forceLEDAnimationUpdate) curLEDAnimationIndex++;
    if (curLEDAnimationIndex >= curLEDAnimationList.count) curLEDAnimationIndex = 0;
    curLEDAnimation = curLEDAnimationList.animations[curLEDAnimationIndex];
    (*curLEDAnimation.setupFunc)();
    lastLEDAnimationSwitch = now;
    lastLEDAnimationUpdate = 0; // force update
    forceLEDAnimationUpdate = 0;
  }

  if ((now - lastLEDAnimationUpdate) >= curLEDAnimation.updateInterval)  {
    (*curLEDAnimation.animFunc)();
    lastLEDAnimationUpdate = now;
  }

  oldUSB = curUSB;
  oldChg = curChg;
  oldLow = curLow;
  oldButtons = curButtons;
}
