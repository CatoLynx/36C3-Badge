// VFD.h, vacuum fluroescent display driver for Samsung HCS-12SS59T, V0.9 171112 qrt@qland.de

#ifndef _VFD_h
#define _VFD_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>              // 50 MIOSO, 51 MOSI, 52 SCK, 53 SS

#define SPIPARS         2000000, LSBFIRST, SPI_MODE3
#define SUPPLYMODE        1     // 0 Vdisp, 1 supply clock
#define SUPPLYCYC       62      // 16E3^-1 / 2 / (16E6^-1 * DIV8)

#define VFD_DCRAM_WR      0x10    // ccccaaaa dddddddd dddddddd ..
#define VFD_CGRAM_WR      0x20    // "
#define VFD_ADRAM_WR      0x30    // ccccaaaa ******dd ******dd ..
#define VFD_DUTY        0x50    // ccccdddd
#define VFD_NUMDIGIT      0x60    // "
#define VFD_LIGHTS        0x70    // cccc**dd

#define LINORM          0x00    // normal display
#define LIOFF         0x01    // lights OFF
#define LION          0x02    //        ON

#define NUMDIGITS       12      // display digit width
#define BUFSIZE         100     // display/scroll buffer

const int Pin_VFD_RESET   = 7;     // VFD _RESET
const int Pin_VFD_CS    = 8;     //     _CS   chip select, SPI SS

const int Pin_VFD_SUCLK_A = 9;     //     SUCLK A, OC2A
const int Pin_VFD_SUCLK_B = 10;      //           B,    B, inverted

#define PIN_LED_D1 18
#define PIN_LED_D2 19
#define PIN_LED_D3 6
#define PIN_LED_H1 5
#define PIN_LED_H2 15

class VFD
{
  public:
    int16_t scrLen;
    int16_t scrPos;
    int16_t scrMode;

    void init();
    void setCrack(byte crackID, bool state);
    void supplyOn();
    void supplyOff();
    void write(char* text);
    void scroll(int16_t mode);
    void display();
    void beginSPI();
    void endSPI();
    void select(int pin);
    void deSelect(int pin);
    void sendCmdSeq(char cmd, char arg);
    void sendChar(char c);
    char getCode(char c);

  protected:

  private:
    char buf[BUFSIZE];

    void sendCmd(char cmd, char arg);
};

extern VFD Vfd;

#endif
