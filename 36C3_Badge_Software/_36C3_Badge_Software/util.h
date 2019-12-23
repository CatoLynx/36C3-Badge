#pragma once

#include <stdint.h>
#include <stdlib.h>

// number of items in an array
template< typename T, size_t N > size_t ArraySize (T (&) [N]) {
  return N;
}

uint16_t movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, uint16_t pos, uint16_t len, uint16_t nextNum);
