#include "badge.h"
#include "util.h"

#include <avr/sleep.h>

Badge badge;

void _wakeUp() {
  badge.wakeUp();
}

void _sleep() {
  badge.sleep();
}

ISR(TIMER2_COMPA_vect) {
  // Timer 2 interrupt, running at 8000 Hz

  // Handle PWM
  badge.pwmCounter++;
  if (badge.pwmCounter >= 64) badge.pwmCounter = 0;

  if (badge.pwmCounter >= badge.pwmValueDestruction1) {
    digitalWrite(badge.PIN_LED_D1, 0);
  } else  {
    digitalWrite(badge.PIN_LED_D1, 1);
  }
  if (badge.pwmCounter >= badge.pwmValueDestruction2) {
    digitalWrite(badge.PIN_LED_D2, 0);
  } else  {
    digitalWrite(badge.PIN_LED_D2, 1);
  }
  if (badge.pwmCounter >= badge.pwmValueDestruction3) {
    digitalWrite(badge.PIN_LED_D3, 0);
  } else {
    digitalWrite(badge.PIN_LED_D3, 1);
  }
  if (badge.pwmCounter >= badge.pwmValueHope1) {
    digitalWrite(badge.PIN_LED_H1, 0);
  } else  {
    digitalWrite(badge.PIN_LED_H1, 1);
  }
  if (badge.pwmCounter >= badge.pwmValueHope2) {
    digitalWrite(badge.PIN_LED_H2, 0);
  } else  {
    digitalWrite(badge.PIN_LED_H2, 1);
  }

  badge.timer2InterruptCounter++;
  if (badge.timer2InterruptCounter % 40 == 0) {
    // Called every 5ms
    badge.vfdAnimInterruptCounter++;
    if (badge.vfdAnimInterruptCounter >= 5) {
      // Called every 25ms
      badge.vfdUpdateAnimation();
      badge.vfdAnimInterruptCounter = 0;
    }
  }
  if (badge.timer2InterruptCounter % 80 == 0) {
    // Called every 10ms
    badge.vfdUpdateScroll();
  }
  if (badge.timer2InterruptCounter % 800 == 0) {
    // Called every 100ms
    badge.battUpdateAverage();
  }
  if (badge.timer2InterruptCounter % 8000 == 0) {
    // Called every second
    badge.timer2InterruptCounter = 0;
  }
}

Badge::Badge() {
  vfdClearBuffer();
  spiConfig = SPISettings(SPI_PARAMS);
}

void Badge::begin() {
  pinMode(PIN_VFD_RST, OUTPUT);
  pinMode(PIN_VFD_CS, OUTPUT);

  pinMode(PIN_VFD_SUP_CLK_A, OUTPUT);
  pinMode(PIN_VFD_SUP_CLK_B, OUTPUT);

  pinMode(PIN_LED_D1, OUTPUT);
  pinMode(PIN_LED_D2, OUTPUT);
  pinMode(PIN_LED_D3, OUTPUT);
  pinMode(PIN_LED_H1, OUTPUT);
  pinMode(PIN_LED_H2, OUTPUT);

  pinMode(PIN_BATT_ADC, INPUT);

  pinMode(PIN_SW_STBY, INPUT_PULLUP);
  pinMode(PIN_SW_A, INPUT_PULLUP);
  pinMode(PIN_SW_B, INPUT_PULLUP);

  pinMode(PIN_PMIC_STAT1_LBO, INPUT_PULLUP);
  pinMode(PIN_PMIC_PG, INPUT_PULLUP);

  digitalWrite(PIN_VFD_RST, HIGH);
  digitalWrite(PIN_VFD_CS, HIGH);

  attachInterrupt(digitalPinToInterrupt(PIN_SW_STBY), _sleep, FALLING);

  startTimer2();

  SPI.begin();

  vfdSetSupply(1);

  vfdReset();

  vfdSPIBegin();
  vfdSendCmd(VFD_NUMDIGIT, VFD_NUM_CHARS);
  vfdSPIEnd();
  vfdSetBrightness(vfdBrightness);
  vfdSetTestMode(NONE);
}

void Badge::wakeUp() {
  // Wake up from sleep mode

  detachInterrupt(digitalPinToInterrupt(PIN_SW_STBY));
  attachInterrupt(digitalPinToInterrupt(PIN_SW_STBY), _sleep, FALLING);
  vfdSetSupply(1);
  startTimer2();

  pwrCheckError();
}

void Badge::sleep() {
  // Enter sleep mode

  stopTimer2();
  vfdSetSupply(0);
  digitalWrite(PIN_LED_D1, 0);
  digitalWrite(PIN_LED_D2, 0);
  digitalWrite(PIN_LED_D3, 0);
  digitalWrite(PIN_LED_H1, 0);
  digitalWrite(PIN_LED_H2, 0);
  detachInterrupt(digitalPinToInterrupt(PIN_SW_STBY));
  attachInterrupt(digitalPinToInterrupt(PIN_SW_STBY), _wakeUp, FALLING);
  sei();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
}

void Badge::vfdSetBrightness(uint8_t level) {
  // Set the VFD brightness (0 to 15)

  if (level > 15) level = 15;
  vfdSendCmd(VFD_DUTY, level);
  vfdBrightness = level;
}

void Badge::vfdSetSupply(uint8_t state) {
  // Set the VFD anode & filament power on or off

  if (state) {
    TCCR1A = 1 << COM1A1 | 1 << COM1B1 | 1 << COM1B0; // OC2A:OC2B L:H
    TCCR1C = 1 << FOC1A | 1 << FOC1B;   // force output compare to fix phase

    TCCR1A = 1 << COM1A0 | 1 << COM1B0; // OC2A, OC2B toggle
    TCCR1B = 1 << WGM12;                // CTC

    OCR1A = SUPPLY_CLK - 1;
    OCR1B = SUPPLY_CLK - 1;

    TCCR1B |= 1 << CS11;  // F_CPU/8, start supply clock
    TCCR1C = 0x00;
  } else {
    TCCR1C = 0;
    TCCR1B = 0; // stop supply clock
    TCCR1A = 0; // OC2A:OC2B L:L, normal port operation
  }
  delay(1);
}

void Badge::vfdSetTestMode(vfd_test_mode_t mode) {
  // Set the VFD test mode (all segments on, all off or normal operation)

  switch (mode) {
    case ALL_ON: {
        vfdSendCmd(VFD_LIGHTS, VFD_LI_ON);
        break;
      }
    case ALL_OFF: {
        vfdSendCmd(VFD_LIGHTS, VFD_LI_OFF);
        break;
      }
    case NONE: {
        vfdSendCmd(VFD_LIGHTS, VFD_LI_NORM);
        break;
      }
  }
}

void Badge::vfdWriteText(char* text) {
  // Output a text on the VFD

  vfdStopAnimation();
  vfdSetScrollSpeed(0);
  vfdWriteTextInternal(text);

  if (vfdScrollLen > VFD_BUF_SIZE - 1) {
    vfdScrollLen = VFD_BUF_SIZE - 1;
    text[vfdScrollLen] = '\0';
  }

  vfdClearBuffer();
  strcpy(vfdBuffer, text);
  vfdUpdate();
}

void Badge::vfdAnimate(char *text, vfd_animation_t animation)
{
  memset(vfdAnimTarget, 0x00, VFD_BUF_SIZE);
  strcpy(vfdAnimTarget, text);
  vfdAnimBrightness = vfdBrightness;
  vfdAnimMode = animation;
  vfdAnimActive = 1;
  vfdAnimFrame = 0;
}

void Badge::vfdStopAnimation() {
  // Stop an ongoig animation and restore previous values

  vfdAnimActive = 0;
  vfdSetBrightness(vfdAnimBrightness);
}

void Badge::vfdSetCharacter(uint8_t addr, char* charData) {
  // Set a custom character

  vfdSPIBegin();
  vfdSPISelect();
  vfdSendCmdSeq(VFD_CGRAM_WR, addr & 0x0f);
  vfdSendCmdSeq(charData[0], 0);
  vfdSendCmdSeq(charData[1], 0);
  vfdSPIDeselect();
  vfdSPIEnd();
}

char Badge::vfdGetCode(char c) {
  // Get the VFD character code for a given ASCII character

  if (c >= '@' && c <= '_')   // 64.. -> 16..
    c -= 48;
  else if (c >= ' ' && c <= '?') // 32.. -> 48..
    c += 16;
  else if (c >= 'a' && c <= 'z') // 97.. -> 17..
    c -= 80;
  else if (c > 15)  // invalid and not a custom char -> ?
    c = 79;

  return c;
}

void Badge::vfdSetScrollSpeed(uint32_t speed) {
  // Enable scrolling on the VFD. Speed = number of 10ms intervals between movements

  vfdScrollSpeed = speed;
}

void Badge::vfdUpdateScroll() {
  // Advance the scroll position of the VFD (to be called by a timer interrupt)

  if (vfdScrollSpeed == 0) return;

  vfdSetScrollSpeedTickCount++;
  if (vfdSetScrollSpeedTickCount < badge.vfdScrollSpeed) return;

  vfdSetScrollSpeedTickCount = 0;

  badge.vfdUpdate();

  if (badge.vfdScrollSpeed > 0) {
    if (++badge.vfdScrollPos >= badge.vfdScrollLen)
      badge.vfdScrollPos = 0;
  }
  else {
    if (--badge.vfdScrollPos < 0)
      badge.vfdScrollPos = badge.vfdScrollLen - 1;
  }
}

void Badge::vfdUpdateAnimation() {
  // Render the next animation frame on the VFD (to be called by a timer interrupt)

  if (!vfdAnimActive) return;

  switch (vfdAnimMode) {
    case ANIMATION_RANDOM: {
        if (vfdAnimFrame == 25) {
          vfdWriteTextInternal(vfdAnimTarget);
          vfdAnimActive = 0;
          break;
        }

        for (uint8_t i = 0; i < VFD_NUM_CHARS; i++) {
          vfdAnimBuffer[i] = rand() % 26 + 'A';
        }
        vfdWriteTextInternal(vfdAnimBuffer);
        break;
      }

    case ANIMATION_FLIP: {
        if (vfdAnimFrame == 50) {
          vfdWriteTextInternal(vfdAnimTarget);
          vfdAnimActive = 0;
          break;
        }

        uint8_t done = 1;
        for (uint8_t i = 0; i < VFD_NUM_CHARS; i++) {
          if (vfdAnimBuffer[i] > vfdAnimTarget[i]) {
            while (vfdGetCode(--vfdAnimBuffer[i]) == 79) {
              if (vfdAnimBuffer[i] == '?') break;
            }
            done = 0;
          } else if (vfdAnimBuffer[i] < vfdAnimTarget[i]) {
            while (vfdGetCode(++vfdAnimBuffer[i]) == 79) {
              if (vfdAnimBuffer[i] == '?') break;
            }
            done = 0;
          }
        }

        if (done) {
          vfdWriteTextInternal(vfdAnimTarget);
          vfdAnimActive = 0;
        } else {
          vfdWriteTextInternal(vfdAnimBuffer);
        }
        break;
      }

    case ANIMATION_SLIDE: {
        if (vfdAnimFrame == VFD_NUM_CHARS - 1) {
          vfdWriteTextInternal(vfdAnimTarget);
          vfdAnimActive = 0;
          break;
        }

        for (uint8_t i = 1; i < VFD_NUM_CHARS; i++) {
          vfdAnimBuffer[i - 1] = vfdAnimBuffer[i];
        }
        vfdAnimBuffer[VFD_NUM_CHARS - 1] = vfdAnimTarget[vfdAnimFrame];
        vfdWriteTextInternal(vfdAnimBuffer);
        break;
      }

    case ANIMATION_FADE: {
        int8_t newBrightness = (int8_t)vfdAnimBrightness - (vfdAnimFrame + 1);
        if (newBrightness >= 0) {
          vfdSetBrightness(newBrightness);
        }
        if (newBrightness == 0) {
          vfdWriteTextInternal(vfdAnimTarget);
        }
        if (newBrightness < 0 && newBrightness >= -vfdAnimBrightness) {
          vfdSetBrightness(-newBrightness);
        }
        if (newBrightness == -vfdAnimBrightness) vfdAnimActive = 0;
        break;
      }

    default: {
        vfdWriteTextInternal(vfdAnimTarget);
        vfdAnimActive = 0;
        break;
      }
  }

  vfdAnimFrame++;
}

void Badge::setCrack(crack_t crack, uint8_t value) {
  // Set a PWM value for the given illuminated crack

  switch (crack) {
    case DESTRUCTION1: {
        pwmValueDestruction1 = value;
        break;
      }
    case DESTRUCTION2: {
        pwmValueDestruction2 = value;
        break;
      }
    case DESTRUCTION3: {
        pwmValueDestruction3 = value;
        break;
      }
    case HOPE1: {
        pwmValueHope1 = value;
        break;
      }
    case HOPE2: {
        pwmValueHope2 = value;
        break;
      }
  }
}

void Badge::battUpdateAverage() {
  battAverage = movingAvg(battAvgValues, &battAvgSum, battAvgPos, BATT_AVG_NUM_VALUES, analogRead(PIN_BATT_ADC));
  battAvgPos++;
  if (battAvgPos >= BATT_AVG_NUM_VALUES) battAvgPos = 0;
}

uint16_t Badge::battGetVoltage() {
  // Get the battery level in mV

  return VCC_VOLTAGE * (uint32_t)battAverage / 1024.0;
}

uint8_t Badge::battGetLevel() {
  // Get the battery level in percent

  uint8_t percentage = map(battGetVoltage(), 3000, 4200, 0, 100);
  if (percentage < 0) percentage = 0;
  if (percentage > 100) percentage = 100;
  return percentage;
}

buttons_t Badge::btnGetAll() {
  // Read all buttons

  buttons_t buttons = SW_NONE;
  if (!digitalRead(PIN_SW_STBY)) buttons |= SW_STBY;
  if (!digitalRead(PIN_SW_A)) buttons |= SW_A;
  if (!digitalRead(PIN_SW_B)) buttons |= SW_B;
  return buttons;
}

uint8_t Badge::pwrGetUSB() {
  // Check whether USB power is connected (1) or not (0)

  return !digitalRead(PIN_PMIC_PG);
}

uint8_t Badge::pwrGetCharging() {
  // Check whether the battery is charging (1) or not (0)

  return !digitalRead(PIN_PMIC_STAT1_LBO) && !digitalRead(PIN_PMIC_PG);
}

uint8_t Badge::pwrGetLowBatt() {
  // Check whether the battery level is low (1) or not (0)

  return !digitalRead(PIN_PMIC_STAT1_LBO) && digitalRead(PIN_PMIC_PG);
}

void Badge::pwrCheckError() {
  // Check for an unrealistic battery voltage caused by some design error
  // and display an error message

  if (analogRead(PIN_BATT_ADC) < 920) return;
  setCrack(DESTRUCTION1, 0);
  setCrack(DESTRUCTION2, 0);
  setCrack(DESTRUCTION3, 0);
  setCrack(HOPE1, 0);
  setCrack(HOPE2, 0);
  vfdWriteText("ERR TURN OFF");
  while (1);
}

// PRIVATE PARTS

void Badge::vfdReset() {
  // Perform a hardware reset of the VFD

  digitalWrite(PIN_VFD_RST, LOW);
  delayMicroseconds(1); // tWRES
  digitalWrite(PIN_VFD_RST, HIGH);
  delayMicroseconds(1); // tRSOFF
}

void Badge::vfdSendCmd(char cmd, char arg) {
  // Initialize an SPI transfer and send a command to the VFD

  vfdSPIBegin();
  vfdSPISelect();
  vfdSendCmdSeq(cmd, arg);
  vfdSPIDeselect();
  vfdSPIEnd();
}

void Badge::vfdSendCmdSeq(char cmd, char arg) {
  // Just send a command to the VFD (if a transfer has already been initialized)

  SPI.transfer(cmd | arg);
  delayMicroseconds(8); // tDOFF ; 1/2 tCSH
}

void Badge::vfdSendChar(char c) {
  // Send an ASCII character to the VFD

  SPI.transfer(vfdGetCode(c));
  delayMicroseconds(8); // tDOFF and 1/2 tCSH for last data
}

void Badge::vfdWriteTextInternal(char* text) {
  // Output a text on the VFD

  vfdScrollLen = strlen(text);
  vfdScrollPos = VFD_NUM_CHARS - 1;

  if (vfdScrollLen > VFD_BUF_SIZE - 1) {
    vfdScrollLen = VFD_BUF_SIZE - 1;
    text[vfdScrollLen] = '\0';
  }

  vfdClearBuffer();
  strcpy(vfdBuffer, text);
  vfdUpdate();
}

void Badge::vfdUpdate() {
  // Update the VFD contents

  vfdSPIBegin();
  vfdSPISelect();

  vfdSendCmdSeq(VFD_DCRAM_WR, 0);

  int16_t p = vfdScrollPos;

  for (int16_t i = 0; i < VFD_NUM_CHARS; i++) {
    vfdAnimBuffer[VFD_NUM_CHARS - (i + 1)] = vfdBuffer[p];
    if (vfdBuffer[p] == 0x00) {
      vfdSendChar(' ');
      p--;
    } else {
      vfdSendChar(vfdBuffer[p--]);
    }

    if (p < 0)
      p = vfdScrollLen - 1;
  }
  vfdAnimBuffer[VFD_NUM_CHARS] = 0x00;

  vfdSPIDeselect();
  vfdSPIEnd();
}

void Badge::vfdSPIBegin() {
  // Begin an SPI transfer to the VFD

  SPI.beginTransaction(spiConfig);
}

void Badge::vfdSPIEnd() {
  // End an SPI transfer to the VFD

  SPI.endTransaction();
}

void Badge::vfdSPISelect() {
  // Assert the VFD's chip select input

  digitalWrite(PIN_VFD_CS, LOW);
  delayMicroseconds(1); // tCSS
}

void Badge::vfdSPIDeselect() {
  // De-assert the VFD's chip select input

  delayMicroseconds(8); // 1/2 tCSH
  digitalWrite(PIN_VFD_CS, HIGH);
}

void Badge::vfdClearBuffer() {
  memset(vfdBuffer, 0x00, VFD_BUF_SIZE);
}

void Badge::startTimer2() {
  // Start timer 2 (used for PWM)

  TCCR2A = 0b00000010;  // CTC mode
  TCCR2B = 0b00000010;  // F_CPU/8
  TIMSK2 = 0b00000010;  // OCIE2A (output compare interrupt A) enabled
  OCR2A  = 250;         // Gives 8000 interrupts per second
}

void Badge::stopTimer2() {
  // Stop timer 2

  TCCR2B = 0b00000000;  // Clock disabled
}
