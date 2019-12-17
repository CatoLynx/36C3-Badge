#include "util.h"

uint16_t movingAvg(uint16_t *ptrArrNumbers, uint32_t *ptrSum, uint16_t pos, uint16_t len, uint16_t nextNum) {
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return *ptrSum / len;
}
