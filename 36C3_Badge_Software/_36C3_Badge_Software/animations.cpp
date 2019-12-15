#include "animations.h"
#include "badge.h"
#include <stdlib.h>
#include <string.h>

#define VFD_ANIM_DELAY 20

void vfd_animate_to(char *text, enum VFD_FUN_ANIMATIONS animation)
{
  int i, j;
  int done = 0;
  static char last[NUMDIGITS];

  switch (animation) {
    case ANIMATION_RANDOM:
      for (int j = 0; j < 25; ++j) {
        Vfd.beginSPI();
        Vfd.select(Pin_VFD_CS);
        Vfd.sendCmdSeq(VFD_DCRAM_WR, 0);
        for (i = 0; i < NUMDIGITS; ++i) {
          Vfd.sendChar(rand() % 26 + 'A');
        }
        Vfd.deSelect(Pin_VFD_CS);
        Vfd.endSPI();
        _delay_ms(VFD_ANIM_DELAY);
      }

      Vfd.write(text);
      break;

    case ANIMATION_FLIP:
      while (!done) {
        for (int j = 0; j < 25; ++j) {
          done = 1;
          for (i = 0; i < NUMDIGITS; ++i) {
            if (last[i] > text[i]) {
              while (Vfd.getCode(--last[i]) == 79) {
                if (last[i] == '?')
                  break;
              }
              done = 0;
            } else if (last[i] < text[i]) {
              while (Vfd.getCode(++last[i]) == 79) {
                if (last[i] == '?')
                  break;
              }
              done = 0;
            }
          }
          Vfd.write(last);
          _delay_ms(VFD_ANIM_DELAY);
        }
      }
      break;

    case ANIMATION_SLIDE:
      for (j = 0; j < NUMDIGITS; ++j) {
        for (i = 1; i < NUMDIGITS; ++i) {
          last[i - 1] = last[i];
        }
        last[NUMDIGITS - 1] = text[j];
        Vfd.write(last);
        _delay_ms(VFD_ANIM_DELAY);
      }
      break;

    default:
      Vfd.write(text);
      break;
  }

  for (i = 0; i < NUMDIGITS; ++i) {
    last[i] = text[i];
  }
}
