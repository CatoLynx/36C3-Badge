#pragma once

#include <SPI.h>

#define SPI_PARAMS    2000000, LSBFIRST, SPI_MODE3
#define SUPPLY_CLK    62    // Clocked VFD anode & filament supply, ~16 kHz

#define VFD_DCRAM_WR  0x10  // ccccaaaa dddddddd dddddddd ..
#define VFD_CGRAM_WR  0x20  // "
#define VFD_ADRAM_WR  0x30  // ccccaaaa ******dd ******dd ..
#define VFD_DUTY      0x50  // ccccdddd
#define VFD_NUMDIGIT  0x60  // "
#define VFD_LIGHTS    0x70  // cccc**dd

#define VFD_LI_NORM   0x00  // Normal operation
#define VFD_LI_OFF    0x01  // All segments off
#define VFD_LI_ON     0x02  // All segments on

#define VFD_NUM_CHARS 12    // Number of characters on VFD
#define VFD_BUF_SIZE  100   // Scroll buffer for VFD

typedef enum Crack {
  DESTRUCTION1,
  DESTRUCTION2,
  DESTRUCTION3,
  HOPE1,
  HOPE2
} t_Crack;

typedef enum VFDTestMode {
  NONE,
  ALL_ON,
  ALL_OFF
} t_VFDTestMode;

class Badge
{
  public:
    const int PIN_VFD_RST = 7;  // active low
    const int PIN_VFD_CS = 8;   // active low
    const int PIN_VFD_SUP_CLK_A = 9;
    const int PIN_VFD_SUP_CLK_B = 10;
    const int PIN_LED_D1 = 18;
    const int PIN_LED_D2 = 19;
    const int PIN_LED_D3 = 6;
    const int PIN_LED_H1 = 5;
    const int PIN_LED_H2 = 15;
    
    volatile uint8_t pwmCounter = 0;
    uint8_t pwmValueDestruction1 = 0;
    uint8_t pwmValueDestruction2 = 0;
    uint8_t pwmValueDestruction3 = 0;
    uint8_t pwmValueHope1 = 0;
    uint8_t pwmValueHope2 = 0;
    
    Badge();
    void begin();
    void vfdSetBrightness(uint8_t level);
    void vfdSetSupply(uint8_t state);
    void vfdSetTestMode(t_VFDTestMode mode);
    void vfdWriteText(char* text);
    void vfdSetScrollSpeed(uint32_t speed);
    void vfdDoScroll();
    char vfdGetCode(char c);
    void setCrack(t_Crack crack, uint8_t value);

  protected:

  private:
    char vfdBuffer[VFD_BUF_SIZE];
    const SPISettings spiConfig;

    int16_t vfdScrollLen;
    int16_t vfdScrollPos;
    uint32_t vfdScrollSpeed;

    uint16_t vfdSetScrollSpeedTickCount = 0;

    void vfdReset();
    void vfdSendCmd(char cmd, char arg);
    void vfdSendCmdSeq(char cmd, char arg);
    void vfdSendChar(char c);
    void vfdUpdate();
    void vfdSPIBegin();
    void vfdSPIEnd();
    void vfdSPISelect();
    void vfdSPIDeselect();
    void vfdClearBuffer();
};

extern Badge badge;
