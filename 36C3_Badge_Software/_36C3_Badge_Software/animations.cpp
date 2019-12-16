#include "animations.h"
#include "badge.h"
#include <stdlib.h>
#include <string.h>

#define VFD_ANIM_DELAY 15

void vfd_animate_to(char *text, enum VFD_FUN_ANIMATIONS animation)
{
  int i, j;
  int done = 0;
  static char last[VFD_NUM_CHARS];
  static char next[VFD_NUM_CHARS + 1];

  switch (animation) {
    case ANIMATION_RANDOM:
      for (int j = 0; j < 25; ++j) {
        for (i = 0; i < VFD_NUM_CHARS; ++i) {
          next[i] = rand() % 26 + 'A';
        }
        next[VFD_NUM_CHARS] = 0x00;
        badge.vfdWriteText(next);
        _delay_ms(VFD_ANIM_DELAY);
      }

      badge.vfdWriteText(text);
      break;

    case ANIMATION_FLIP:
      while (!done) {
        for (int j = 0; j < 25; ++j) {
          done = 1;
          for (i = 0; i < VFD_NUM_CHARS; ++i) {
            if (last[i] > text[i]) {
              while (badge.vfdGetCode(--last[i]) == 79) {
                if (last[i] == '?')
                  break;
              }
              done = 0;
            } else if (last[i] < text[i]) {
              while (badge.vfdGetCode(++last[i]) == 79) {
                if (last[i] == '?')
                  break;
              }
              done = 0;
            }
          }
          badge.vfdWriteText(last);
          _delay_ms(VFD_ANIM_DELAY);
        }
      }
      break;

    case ANIMATION_SLIDE:
      for (j = 0; j < VFD_NUM_CHARS; ++j) {
        for (i = 1; i < VFD_NUM_CHARS; ++i) {
          last[i - 1] = last[i];
        }
        last[VFD_NUM_CHARS - 1] = text[j];
        badge.vfdWriteText(last);
        _delay_ms(VFD_ANIM_DELAY);
      }
      break;

    default:
      badge.vfdWriteText(text);
      break;
  }

  for (i = 0; i < VFD_NUM_CHARS; ++i) {
    last[i] = text[i];
  }
}
