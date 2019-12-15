#ifndef VFD_FUN_H
#define VFD_FUN_H

#include "badge.h"

enum VFD_FUN_ANIMATIONS {
  ANIMATION_RANDOM = 0,
  ANIMATION_FLIP,
  ANIMATION_SLIDE
};

void vfd_animate_to(char *text, enum VFD_FUN_ANIMATIONS animation);

#endif // VFD_FUN_H
