#pragma once

#include <SPI.h>

#define VCC_VOLTAGE   5060  // Calibration value (actual value of 5V rail in mV)

#define SPI_PARAMS    2000000, LSBFIRST, SPI_MODE3
#define SUPPLY_CLK    62    // Clocked VFD anode & filament supply, ~16 kHz

#define VFD_DCRAM_WR  0x10  // ccccaaaa dddddddd dddddddd ..
#define VFD_CGRAM_WR  0x20  // ccccaaaa dddddddd dddddddd ..
#define VFD_ADRAM_WR  0x30  // ccccaaaa ******dd ******dd ..
#define VFD_DUTY      0x50  // ccccdddd
#define VFD_NUMDIGIT  0x60  // ccccdddd
#define VFD_LIGHTS    0x70  // cccc**dd

#define VFD_LI_NORM   0x00  // Normal operation
#define VFD_LI_OFF    0x01  // All segments off
#define VFD_LI_ON     0x02  // All segments on

#define VFD_NUM_CHARS 12    // Number of characters on VFD
#define VFD_BUF_SIZE  100   // Scroll buffer for VFD

#define VFD_ANI_DELAY 15    // Animation frame delay in milliseconds

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

typedef enum Buttons {
  SW_NONE = 0,
  SW_STBY = 1,
  SW_A = 2,
  SW_B = 4
} t_Buttons;

typedef enum VFDAnimations {
  ANIMATION_NONE,
  ANIMATION_RANDOM,
  ANIMATION_FLIP,
  ANIMATION_SLIDE
} t_VFDAnimation;

class Badge
{
  public:
    static const int PIN_VFD_RST = 7;  // active low
    static const int PIN_VFD_CS = 8;   // active low
    static const int PIN_VFD_SUP_CLK_A = 9;
    static const int PIN_VFD_SUP_CLK_B = 10;
    static const int PIN_LED_D1 = 18;
    static const int PIN_LED_D2 = 19;
    static const int PIN_LED_D3 = 6;
    static const int PIN_LED_H1 = 5;
    static const int PIN_LED_H2 = 15;
    static const int PIN_BATT_ADC = A0;
    static const int PIN_SW_STBY = 2;
    static const int PIN_SW_A = 3;
    static const int PIN_SW_B = 4;
    static const int PIN_PMIC_STAT1_LBO = 16; // LBO active low
    static const int PIN_PMIC_PG = 17;  // active low

    volatile uint8_t pwmCounter = 0;
    volatile uint16_t timer2InterruptCounter = 0;
    uint8_t pwmValueDestruction1 = 0;
    uint8_t pwmValueDestruction2 = 0;
    uint8_t pwmValueDestruction3 = 0;
    uint8_t pwmValueHope1 = 0;
    uint8_t pwmValueHope2 = 0;

    Badge();
    void begin();
    void wakeUp();
    void sleep();
    void vfdSetBrightness(uint8_t level);
    void vfdSetSupply(uint8_t state);
    void vfdSetTestMode(t_VFDTestMode mode);
    void vfdWriteText(char* text);
    void vfdAnimate(char *text, t_VFDAnimation animation);
    void vfdSetCharacter(uint8_t addr, char* charData);
    void vfdSetScrollSpeed(uint32_t speed);
    void vfdDoScroll();
    char vfdGetCode(char c);
    void setCrack(t_Crack crack, uint8_t value);
    void battUpdateAverage();
    uint16_t battGetVoltage();
    uint8_t battGetLevel();
    t_Buttons btnGetAll();
    uint8_t pwrGetUSB();
    uint8_t pwrGetCharging();
    uint8_t pwrGetLowBatt();

  protected:

  private:
    char vfdBuffer[VFD_BUF_SIZE];
    const SPISettings spiConfig;

    int16_t vfdScrollLen;
    int16_t vfdScrollPos;
    uint32_t vfdScrollSpeed;

    uint16_t vfdSetScrollSpeedTickCount = 0;

    static const uint8_t BATT_AVG_NUM_VALUES = 10;
    uint16_t battAverage = 0;
    uint32_t battAvgSum = 0;
    uint16_t battAvgPos = 0;
    uint16_t battAvgValues[BATT_AVG_NUM_VALUES] = {0};

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
    void startTimer2();
    void stopTimer2();
};

extern Badge badge;
