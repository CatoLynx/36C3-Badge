/*
   Cato's 36C3 Badge
   https://github.com/Mezgrman/36C3-Badge

   Code to control the VFD based on:
   https://github.com/qrti/VFD-HCS-12SS59T
   https://github.com/m42uko/hcs12ss59t
*/

#include "badge.h"
#include "animations.h"

void setup()
{
  Serial.begin(9600);
  badge.begin();

  return;

  vfd_animate_to(" Hello 36C3 ", ANIMATION_RANDOM);
  delay(1000);
  vfd_animate_to("Destruction ", ANIMATION_FLIP);
  badge.setCrack(DESTRUCTION1, 64);
  badge.setCrack(DESTRUCTION2, 64);
  badge.setCrack(DESTRUCTION3, 64);
  delay(500);
  vfd_animate_to("     or     ", ANIMATION_SLIDE);
  badge.setCrack(DESTRUCTION1, 0);
  badge.setCrack(DESTRUCTION2, 0);
  badge.setCrack(DESTRUCTION3, 0);
  delay(500);
  vfd_animate_to("    Hope?   ", ANIMATION_RANDOM);
  badge.setCrack(HOPE1, 64);
  badge.setCrack(HOPE2, 64);
  delay(1000);
  badge.setCrack(HOPE1, 0);
  badge.setCrack(HOPE2, 0);
  badge.vfdWriteText("AND NOW - A MESSAGE FROM OUR SPONSORS            ");
  delay(500);
  badge.vfdSetScrollSpeed(1);
  delay(3700);
  badge.vfdSetScrollSpeed(0);
}

char text[VFD_NUM_CHARS + 1];
void loop()
{
  sprintf(text, "BAT %d SW %d", (uint16_t)round(badge.battGetLevel() / 10.0), badge.btnGetAll());
  badge.vfdWriteText(text);
  for (uint8_t i = 0; i <= 64; i++) {
    badge.setCrack(DESTRUCTION1, i);
    badge.setCrack(HOPE2, 64 - i);
    delay(5);
  }
  for (uint8_t i = 0; i <= 64; i++) {
    badge.setCrack(DESTRUCTION2, i);
    badge.setCrack(DESTRUCTION1, 64 - i);
    delay(5);
  }
  for (uint8_t i = 0; i <= 64; i++) {
    badge.setCrack(HOPE1, i);
    badge.setCrack(DESTRUCTION2, 64 - i);
    delay(5);
  }
  for (uint8_t i = 0; i <= 64; i++) {
    badge.setCrack(DESTRUCTION3, i);
    badge.setCrack(HOPE1, 64 - i);
    delay(5);
  }
  for (uint8_t i = 0; i <= 64; i++) {
    badge.setCrack(HOPE2, i);
    badge.setCrack(DESTRUCTION3, 64 - i);
    delay(5);
  }
}
