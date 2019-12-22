/*
   Cato's 36C3 Badge
   https://github.com/Mezgrman/36C3-Badge

   Code to control the VFD based on:
   https://github.com/qrti/VFD-HCS-12SS59T
   https://github.com/m42uko/hcs12ss59t
*/

#include "badge.h"
#include "animations.h"

#include <MD_Menu.h>

bool display(MD_Menu::userDisplayAction_t, char* = nullptr);
MD_Menu::userNavAction_t navigation(uint16_t &incDelta);

MD_Menu::value_t *mnuValueRqst(MD_Menu::mnuId_t id, bool bGet);

MD_Menu::value_t vBuf;  // interface buffer for values

// Menu Headers --------
const PROGMEM MD_Menu::mnuHeader_t mnuHdr[] =
{
  { 10, "Main Menu",    10, 11, 0 },
  { 11, "VFD Settings", 20, 21, 0 },
  { 12, "LED Settings", 30, 31, 0 },
  { 13, "Crack Settgs", 40, 44, 0 },
};

// Menu Items ----------
const PROGMEM MD_Menu::mnuItem_t mnuItm[] =
{
  { 10, "VFD Settings", MD_Menu::MNU_MENU,  11 },
  { 11, "LED Settings", MD_Menu::MNU_MENU,  12 },

  { 20, "Set Message",  MD_Menu::MNU_INPUT, 20 },
  { 21, "Set Brightns", MD_Menu::MNU_INPUT, 21 },

  { 30, "Set Cracks",   MD_Menu::MNU_MENU,  13 },
  { 31, "Crack Bright", MD_Menu::MNU_INPUT, 31 },

  { 40, "Destruction1", MD_Menu::MNU_INPUT, 40 },
  { 41, "Destruction2", MD_Menu::MNU_INPUT, 41 },
  { 42, "Destruction3", MD_Menu::MNU_INPUT, 42 },
  { 43, "Hope1",        MD_Menu::MNU_INPUT, 43 },
  { 44, "Hope2",        MD_Menu::MNU_INPUT, 44 },
};

// Input Items ---------
const PROGMEM char listMsgs[] = "msg1|msg2|msg3";

const PROGMEM MD_Menu::mnuInput_t mnuInp[] =
{
  { 20, "",  MD_Menu::INP_LIST, mnuValueRqst, 6,    0, 0,     0, 0,  0, listMsgs }, // VFD Message
  { 21, "", MD_Menu::INP_INT,  mnuValueRqst, 2,    0, 0,    15, 0, 10, nullptr }, // VFD Brightness

  { 31, "", MD_Menu::INP_INT,  mnuValueRqst, 2,    0, 0,    63, 0, 10, nullptr }, // Crack Brightness

  { 40, "Dest1", MD_Menu::INP_BOOL, mnuValueRqst, 1,    0, 0,     0, 0,  0, nullptr },
  { 41, "Dest2", MD_Menu::INP_BOOL, mnuValueRqst, 1,    0, 0,     0, 0,  0, nullptr },
  { 42, "Dest3", MD_Menu::INP_BOOL, mnuValueRqst, 1,    0, 0,     0, 0,  0, nullptr },
  { 43, "Hope1",        MD_Menu::INP_BOOL, mnuValueRqst, 1,    0, 0,     0, 0,  0, nullptr },
  { 44, "Hope2",        MD_Menu::INP_BOOL, mnuValueRqst, 1,    0, 0,     0, 0,  0, nullptr },
};

// bring it all together in the global menu object
MD_Menu M(navigation, display,        // user navigation and display
          mnuHdr, ARRAY_SIZE(mnuHdr), // menu header data
          mnuItm, ARRAY_SIZE(mnuItm), // menu item data
          mnuInp, ARRAY_SIZE(mnuInp));// menu input data

uint8_t curVFDMessage = 0;
uint8_t curVFDBrightness = 15;
uint8_t curCrackBrightness = 63;
uint8_t crkD1State = 1;
uint8_t crkD2State = 1;
uint8_t crkD3State = 1;
uint8_t crkH1State = 1;
uint8_t crkH2State = 1;

MD_Menu::value_t *mnuValueRqst(MD_Menu::mnuId_t id, bool bGet) {
  switch (id) {
    default: {
        return nullptr;
      }

    case 20: { // VFD Message
        if (bGet) {
          vBuf.value = curVFDMessage;
        } else {
          curVFDMessage = vBuf.value;
        }
        break;
      }

    case 21: { // VFD Brightness
        if (bGet) {
          vBuf.value = curVFDBrightness;
        } else {
          curVFDBrightness = vBuf.value;
        }
        break;
      }

    case 31: { // Crack Brightness
        if (bGet) {
          vBuf.value = curCrackBrightness;
        } else {
          curCrackBrightness = vBuf.value;
        }
        break;
      }

    case 40: { // Crack Destruction1
        if (bGet) {
          vBuf.value = crkD1State;
        } else {
          crkD1State = vBuf.value;
        }
        break;
      }

    case 41: { // Crack Destruction2
        if (bGet) {
          vBuf.value = crkD2State;
        } else {
          crkD2State = vBuf.value;
        }
        break;
      }

    case 42: { // Crack Destruction3
        if (bGet) {
          vBuf.value = crkD3State;
        } else {
          crkD3State = vBuf.value;
        }
        break;
      }

    case 43: { // Crack Hope1
        if (bGet) {
          vBuf.value = crkH1State;
        } else {
          crkH1State = vBuf.value;
        }
        break;
      }

    case 44: { // Crack Hope2
        if (bGet) {
          vBuf.value = crkH2State;
        } else {
          crkH2State = vBuf.value;
        }
        break;
      }
  }
}

t_Buttons oldButtons, buttons;
MD_Menu::userNavAction_t navigation(uint16_t &incDelta) {
  MD_Menu::userNavAction_t nav = MD_Menu::NAV_NULL;
  buttons = badge.btnGetAll();
  if (!(oldButtons & SW_A) && (buttons & SW_A)) {
    nav = MD_Menu::NAV_SEL;
  }
  if (!(oldButtons & SW_B) && (buttons & SW_B)) {
    nav = MD_Menu::NAV_INC;
  }
  oldButtons = buttons;
  return nav;
}

bool display(MD_Menu::userDisplayAction_t action, char *msg) {
  switch (action) {
    case MD_Menu::DISP_INIT: {
        break;
      }

    case MD_Menu::DISP_CLEAR: {
        badge.vfdWriteText("");
        break;
      }

    case MD_Menu::DISP_L0: {
        break;
      }

    case MD_Menu::DISP_L1: {
        badge.vfdWriteText(msg);
        break;
      }
  }
  return true;
}

void setup()
{
  Serial.begin(115200);
  badge.begin();

  M.begin();
  M.setMenuWrap(true);
  M.setAutoStart(false);
  M.setTimeout(5000);

  return;

  vfd_animate_to(" Hello 36C3 ", ANIMATION_RANDOM);
  delay(1000);
  vfd_animate_to("Destruction ", ANIMATION_FLIP);
  badge.setCrack(DESTRUCTION1, 64);
  badge.setCrack(DESTRUCTION2, 64);
  badge.setCrack(DESTRUCTION3, 64);
  delay(500);
  vfd_animate_to("     or     ", ANIMATION_SLIDE);
  badge.setCrack(DESTRUCTION1, 0);
  badge.setCrack(DESTRUCTION2, 0);
  badge.setCrack(DESTRUCTION3, 0);
  delay(500);
  vfd_animate_to("    Hope?   ", ANIMATION_RANDOM);
  badge.setCrack(HOPE1, 64);
  badge.setCrack(HOPE2, 64);
  delay(1000);
  badge.setCrack(HOPE1, 0);
  badge.setCrack(HOPE2, 0);
  badge.vfdWriteText("AND NOW - A MESSAGE FROM OUR SPONSORS            ");
  delay(500);
  badge.vfdSetScrollSpeed(1);
  delay(3700);
  badge.vfdSetScrollSpeed(0);
}

char text[VFD_NUM_CHARS + 1];
uint8_t curUSB, oldUSB = 0;
uint8_t curChg, oldChg = 0;
uint8_t curLow, oldLow = 0;
void loop()
{
  // If we are not running and not autostart
  // check if there is a reason to start the menu
  if (!M.isInMenu())
  {
    uint16_t dummy;

    if (navigation(dummy) == MD_Menu::NAV_SEL)
      M.runMenu(true);
  }

  M.runMenu();   // just run the menu code
  
  badge.setCrack(DESTRUCTION1, crkD1State * curCrackBrightness);
  badge.setCrack(DESTRUCTION2, crkD2State * curCrackBrightness);
  badge.setCrack(DESTRUCTION3, crkD3State * curCrackBrightness);
  badge.setCrack(HOPE1, crkH1State * curCrackBrightness);
  badge.setCrack(HOPE2, crkH2State * curCrackBrightness);
  return;
  
  curUSB = badge.pwrGetUSB();
  curChg = badge.pwrGetCharging();
  curLow = badge.pwrGetLowBatt();

  if (curUSB && !oldUSB) {
    badge.vfdWriteText("USB POWER");
    delay(1000);
  } else if (!curUSB && oldUSB) {
    badge.vfdWriteText("BATT POWER");
    delay(1000);
  }

  if (curChg && !oldChg) {
    badge.vfdWriteText("CHARGING");
    delay(1000);
  } else if (!curChg && oldChg) {
    badge.vfdWriteText("CHARGED");
    delay(1000);
  }

  if (curLow && !oldLow) {
    badge.vfdWriteText("LOW BATT");
    delay(1000);
  }

  sprintf(text, "BATTERY %d", (uint16_t)round(badge.battGetLevel() / 10.0));
  badge.vfdWriteText(text);

  if (curChg) {
    for (uint8_t i = 15; i <= 64; i++) {
      badge.setCrack(HOPE2, i);
      delay(50);
    }
    for (uint8_t i = 64; i >= 15; i--) {
      badge.setCrack(HOPE2, i);
      delay(50);
    }
  } else {
    for (uint8_t i = 0; i <= 64; i++) {
      badge.setCrack(DESTRUCTION1, i);
      badge.setCrack(HOPE2, 64 - i);
      delay(5);
    }
    for (uint8_t i = 0; i <= 64; i++) {
      badge.setCrack(DESTRUCTION2, i);
      badge.setCrack(DESTRUCTION1, 64 - i);
      delay(5);
    }
    for (uint8_t i = 0; i <= 64; i++) {
      badge.setCrack(HOPE1, i);
      badge.setCrack(DESTRUCTION2, 64 - i);
      delay(5);
    }
    for (uint8_t i = 0; i <= 64; i++) {
      badge.setCrack(DESTRUCTION3, i);
      badge.setCrack(HOPE1, 64 - i);
      delay(5);
    }
    for (uint8_t i = 0; i <= 64; i++) {
      badge.setCrack(HOPE2, i);
      badge.setCrack(DESTRUCTION3, 64 - i);
      delay(5);
    }
  }

  oldUSB = curUSB;
  oldChg = curChg;
  oldLow = curLow;
}
