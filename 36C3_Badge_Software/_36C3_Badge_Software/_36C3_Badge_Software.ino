// VFD.cpp, vacuum fluroescent example for Samsung HCS-12SS59T, V0.9 171112 qrt@qland.de

#include "badge.h"
#include "animations.h"

void setup()
{
  Serial.begin(9600);
  Vfd.init();

  vfd_animate_to(" Hello 36C3 ", ANIMATION_RANDOM);
  delay(2000);
  vfd_animate_to("Destruction ", ANIMATION_FLIP);
  Vfd.setCrack(0, 1);
  Vfd.setCrack(1, 1);
  Vfd.setCrack(2, 1);
  delay(2000);
  vfd_animate_to("     or     ", ANIMATION_SLIDE);
  Vfd.setCrack(0, 0);
  Vfd.setCrack(1, 0);
  Vfd.setCrack(2, 0);
  delay(2000);
  vfd_animate_to("    Hope?   ", ANIMATION_RANDOM);
  Vfd.setCrack(3, 1);
  Vfd.setCrack(4, 1);
  delay(2000);
  vfd_animate_to("Resource Exhaustion", ANIMATION_FLIP);
  Vfd.write("Resource Exhaustion     ");
  Vfd.setCrack(0, 1);
  Vfd.setCrack(1, 1);
  Vfd.setCrack(2, 1);
  delay(500);
  Vfd.scroll(10);                               // scroll left  (+) 20 * 0.01 chars per second
}                                       //        right (-)
//        stop  (0)
void loop()
{

}
