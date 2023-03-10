#include "common_inc.h"
#include "configurations.h"
#include "HelloWord/hw_keyboard.h"
#include "time.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"

/* Component Definitions -----------------------------------------------------*/
KeyboardConfig_t config;
HWKeyboard keyboard(&hspi1);
HWKeyboard_RGBLED RGBLED;
EEPROM eeprom;


void UpdateFNColor();

void KeymapModeSet();

/* Main Entry ----------------------------------------------------------------*/
void Main() {
    eeprom.Pull(0, config);
    if (config.configStatus != CONFIG_OK) {
        // Use default settings
        config = KeyboardConfig_t{
                .configStatus = CONFIG_OK,
                .serialNum=123,
                .KeyboardBrightness = 0.7,
                .LightON = 1,
                .FilterTime = 100,
                .TouchEndable = 0,
                .RGBLEDMode = 0,
                .keyMap={},
                .TouchVolumeEnable = 0,
                .LED_R = 242,
                .LED_G = 157,
                .LED_B = 0,
                .KeymapMode = 0,
        };
        memset(config.keyMap, -1, 128);
        eeprom.Push(0, config);
    }
    if (config.KeyboardBrightness < 0.3) {
        config.KeyboardBrightness = 0.7;
        eeprom.Push(0, config);
    }
    if (config.FilterTime < 100 || config.FilterTime > 600) {
        config.FilterTime = 100;
        eeprom.Push(0, config);
    }
    UpdateFNColor();
    if (config.TouchEndable > 1) {
        config.TouchEndable = 0;
        eeprom.Push(0, config);
    }
    if (config.TouchVolumeEnable > 1) {
        config.TouchVolumeEnable = 0;
        eeprom.Push(0, config);
    }
    if (config.RGBLEDMode >= HWKeyboard_RGBLED::ModeTypeMax) {
        config.RGBLEDMode = 0;
        eeprom.Push(0, config);
    }
    if (config.LED_R > 254) {
        config.LED_R = 242;
        eeprom.Push(0, config);
    }
    if (config.LED_G > 254) {
        config.LED_G = 157;
        eeprom.Push(0, config);
    }
    if (config.LED_B > 254) {
        config.LED_B = 0;
        eeprom.Push(0, config);
    }
    if (config.KeymapMode > 1) {
        config.KeymapMode = 1;
        eeprom.Push(0, config);
    }
    KeymapModeSet();
    RGBLED.KeyboardBrightness = config.KeyboardBrightness;
    if (config.LightON != 1) {
        config.LightON = 1;
        eeprom.Push(0, config);
    }
//    config.LightON ? RGBLED.LightON = true : RGBLED.LightON = false;
    RGBLED.LightON = true;
    RGBLED.RGBMode = config.RGBLEDMode;
    // Keyboard Report Start
    HAL_TIM_Base_Start_IT(&htim4);
    RGBLED.RGBLED_Init();
    RGBLED.MonochromeColorSet(HWKeyboard_RGBLED::Color_t{config.LED_R, config.LED_G, config.LED_B});

    while (true) {

        RGBLED.RGBLED_Handle();

        if (keyboard.isEepPush) {     //??????eep
            eeprom.Push(0, config);
            keyboard.isEepPush = false;
        }
        if (config.TouchEndable) {
            switch (keyboard.TouchBarSlide()) {
                case 1:

                    break;
                case 2:

                    break;
                case 0:

                    break;
            }
        }
    }
}

bool Keyrstatus = false;
bool WinLock = false;
uint8_t lastTouch = 0;
HWKeyboard::KeyCode_t lastVolumnTouch = HWKeyboard::RESERVED;
bool TpLock = false;
uint16_t loop = 0;
uint16_t lastLoop = 0;

/* Event Callbacks -----------------------------------------------------------*/
void UpdateFNColor();

void VolumeUp(uint8_t diff);

void VolumeDown(uint8_t diff);

extern "C" void OnTimerCallback() // 1000Hz callback
{
    keyboard.ScanKeyStates();  // Around 40us use 4MHz SPI clk
    keyboard.ApplyDebounceFilter(config.FilterTime);
    keyboard.Remap(keyboard.FnPressed() ? 2 : 1);  // When Fn pressed use layer-2
    if (keyboard.FnPressed()) {
        keyboard.isFNPress = true;
        if (keyboard.KeyPressed(HWKeyboard::UP_ARROW)) {
            if (RGBLED.LightON) {
                if (!Keyrstatus) {
                    if (RGBLED.KeyboardBrightness < 1) {
                        RGBLED.KeyboardBrightness += 0.1;
                    }
                    Keyrstatus = true;
                    config.KeyboardBrightness = RGBLED.KeyboardBrightness;
                    keyboard.isEepPush = true;
                }
            }
            keyboard.Release(HWKeyboard::UP_ARROW);
        } else if (keyboard.KeyPressed(HWKeyboard::DOWN_ARROW)) {
            if (RGBLED.LightON) {
                if (!Keyrstatus) {
                    if (RGBLED.KeyboardBrightness > 0.3) {
                        RGBLED.KeyboardBrightness -= 0.1;
                    }
                    Keyrstatus = true;
                    config.KeyboardBrightness = RGBLED.KeyboardBrightness;
                    keyboard.isEepPush = true;
                }
            }
            keyboard.Release(HWKeyboard::DOWN_ARROW);
        } else if (keyboard.KeyPressed(HWKeyboard::LEFT_ARROW)) {
            if (RGBLED.RGBMode != HWKeyboard_RGBLED::Illusorycolor) {
                if (!Keyrstatus) {
                    HWKeyboard_RGBLED::Color_t color = RGBLED.MonochromeColorGet();
                    uint8_t r = color.r;
                    uint8_t g = color.g;
                    uint8_t b = color.b;
                    if (r > 50) {
                        r -= 50;
                    } else if (g > 50) {
                        g -= 50;
                    } else if (b > 50) {
                        b -= 50;
                    }
                    config.LED_R = r;
                    config.LED_G = g;
                    config.LED_B = b;
                    RGBLED.MonochromeColorSet(HWKeyboard_RGBLED::Color_t{r, g, b});
                    Keyrstatus = true;
                    keyboard.isEepPush = true;
                }
            }
            keyboard.Release(HWKeyboard::LEFT_ARROW);
        } else if (keyboard.KeyPressed(HWKeyboard::RIGHT_ARROW)) {
            if (RGBLED.RGBMode != HWKeyboard_RGBLED::Illusorycolor) {
                if (!Keyrstatus) {
                    HWKeyboard_RGBLED::Color_t color = RGBLED.MonochromeColorGet();
                    uint8_t r = color.r;
                    uint8_t g = color.g;
                    uint8_t b = color.b;
                    if (r < 250) {
                        r += 50;
                    } else if (g < 250) {
                        g += 50;
                    } else if (b < 250) {
                        b += 50;
                    }
                    config.LED_R = r;
                    config.LED_G = g;
                    config.LED_B = b;
                    RGBLED.MonochromeColorSet(HWKeyboard_RGBLED::Color_t{r, g, b});
                    Keyrstatus = true;
                    keyboard.isEepPush = true;
                }
            }
            keyboard.Release(HWKeyboard::RIGHT_ARROW);
        } else if (keyboard.KeyPressed(HWKeyboard::SPACE)) {
            if (!Keyrstatus) {
                if (RGBLED.RGBMode < HWKeyboard_RGBLED::ModeTypeMax) {
                    RGBLED.RGBMode++;
                } else {
                    RGBLED.RGBMode = 0;
                }
                if (RGBLED.RGBMode == HWKeyboard_RGBLED::ModeTypeMax) {
                    RGBLED.RGBLED_ctrl(&RGBLED, HWKeyboard_RGBLED::RGBLEDOFF, 1);
                } else {
                    RGBLED.RGBLED_ctrl(&RGBLED, HWKeyboard_RGBLED::RGBLEDON, 1);
                }
                config.RGBLEDMode = RGBLED.RGBMode;
                Keyrstatus = true;
                keyboard.isEepPush = true;
            }
            keyboard.Release(HWKeyboard::SPACE);

        } else if (keyboard.KeyPressed(HWKeyboard::EQUAL)) {
            if (!Keyrstatus) {
                if (config.FilterTime < 600) {
                    config.FilterTime += 100;
                }
                UpdateFNColor();
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::EQUAL);
        } else if (keyboard.KeyPressed(HWKeyboard::MINUS)) {
            if (!Keyrstatus) {
                if (config.FilterTime > 100) {
                    config.FilterTime -= 100;
                }
                UpdateFNColor();
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::MINUS);
        } else if (keyboard.KeyPressed(HWKeyboard::RIGHT_U_BRACE)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::SYS_BRI_UP);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::RIGHT_U_BRACE);
        } else if (keyboard.KeyPressed(HWKeyboard::LEFT_U_BRACE)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::SYS_BRI_DOWN);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::LEFT_U_BRACE);
        } else if (keyboard.KeyPressed(HWKeyboard::LEFT_GUI)) {
            if (!Keyrstatus) {
                WinLock = !WinLock;
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::LEFT_GUI);
        } else if (keyboard.KeyPressed(HWKeyboard::ESC)) {
            if (!Keyrstatus) {
                TpLock = !TpLock;
//                keyboard.TouchTestLED = !keyboard.TouchTestLED;
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::ESC);

        } else if (keyboard.KeyPressed(HWKeyboard::VOLUME_UP)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::VOLUME_UP);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::VOLUME_UP);

        } else if (keyboard.KeyPressed(HWKeyboard::VOLUME_DOWN)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::VOLUME_DOWN);

        } else if (keyboard.KeyPressed(HWKeyboard::MUTE)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::MUTE);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::MUTE);

        } else if (keyboard.KeyPressed(HWKeyboard::APPLICATION)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::APPLICATION);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::APPLICATION);

        } else if (keyboard.KeyPressed(HWKeyboard::WWW_HOME)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::WWW_HOME);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::WWW_HOME);

        } else if (keyboard.KeyPressed(HWKeyboard::MAIL)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::MAIL);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::MAIL);

        } else if (keyboard.KeyPressed(HWKeyboard::MEDIA_SELECT)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::MEDIA_SELECT);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::MEDIA_SELECT);

        } else if (keyboard.KeyPressed(HWKeyboard::SCAN_PREVIOUS_TRACK)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::SCAN_PREVIOUS_TRACK);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::SCAN_PREVIOUS_TRACK);

        } else if (keyboard.KeyPressed(HWKeyboard::SCAN_NEXT_TRACK)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::SCAN_NEXT_TRACK);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::SCAN_NEXT_TRACK);

        } else if (keyboard.KeyPressed(HWKeyboard::PLAY_PAUSE)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::PLAY_PAUSE);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::PLAY_PAUSE);

        } else if (keyboard.KeyPressed(HWKeyboard::STOP)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::STOP);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::STOP);

        } else if (keyboard.KeyPressed(HWKeyboard::CALCULATOR)) {
            if (!Keyrstatus) {
                keyboard.MediaPress(HWKeyboard::CALCULATOR);
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::CALCULATOR);

        } else if (keyboard.KeyPressed(HWKeyboard::TAB)) {
            if (!Keyrstatus) {
                config.KeymapMode++;
                if (config.KeymapMode > 1) {
                    config.KeymapMode = 0;
                }
                KeymapModeSet();
                keyboard.isEepPush = true;
                Keyrstatus = true;
            }
            keyboard.Release(HWKeyboard::TAB);
        } else {
            keyboard.MediaRelease();
            Keyrstatus = false;
        }
    } else {
        keyboard.MediaRelease();
        keyboard.isFNPress = false;
        Keyrstatus = false;
    }
    if (keyboard.KeyPressed(HWKeyboard::LEFT_GUI) && WinLock) {
        keyboard.Release(HWKeyboard::LEFT_GUI);
    }
    if (TpLock) {
        loop++;
        uint8_t thisTouch = 0;
        if (keyboard.GetTouchBarState(1)) {
            thisTouch = 1;
        }
        if (keyboard.GetTouchBarState(2)) {
            thisTouch = 2;
        }
        if (keyboard.GetTouchBarState(3)) {
            thisTouch = 3;
        }
        if (keyboard.GetTouchBarState(4)) {
            thisTouch = 4;
        }
        if (keyboard.GetTouchBarState(5)) {
            thisTouch = 5;
        }
        if (keyboard.GetTouchBarState(6)) {
            thisTouch = 6;
        }
        if (lastTouch != 0 && thisTouch != 0) {
            if (loop - lastLoop < 5) {
                uint8_t diff;
                if (thisTouch > lastTouch) {
                    diff = thisTouch - lastTouch;
                    VolumeUp(diff);
                } else if (thisTouch < lastTouch) {
                    diff = lastTouch - thisTouch;
                    VolumeDown(diff);
                } else {}
            }
            lastLoop = loop;
        }
        if (thisTouch != 0) {
            lastTouch = thisTouch;
        }
    }
    if (keyboard.KeyDelayCnt) {
        keyboard.KeyDelayCnt--;
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,
                                   keyboard.GetHidReportBuffer(1),
                                   HWKeyboard::KEY_REPORT_SIZE);
    }

}

void VolumeUp(uint8_t diff) {
    switch (diff) {
        default:
        case 5:
            keyboard.MediaPress(HWKeyboard::VOLUME_UP);
        case 4:
            keyboard.MediaPress(HWKeyboard::VOLUME_UP);
        case 3:
            keyboard.MediaPress(HWKeyboard::VOLUME_UP);
        case 2:
            keyboard.MediaPress(HWKeyboard::VOLUME_UP);
        case 1:
            keyboard.MediaPress(HWKeyboard::VOLUME_UP);
    }
}

void VolumeDown(uint8_t diff) {
    switch (diff) {
        default:
        case 5:
            keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
        case 4:
            keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
        case 3:
            keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
        case 2:
            keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
        case 1:
            keyboard.MediaPress(HWKeyboard::VOLUME_DOWN);
    }
}


extern "C"
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    RGBLED.isRgbTxBusy = false;
}
extern "C"
void HID_RxCpltCallback(uint8_t *_data) {
    //??????LED??????
    RGBLED.isNumLocked = _data[1] & 0x01 ? true : false;
    RGBLED.isCapsLocked = _data[1] & 0x02 ? true : false;
    RGBLED.isScrlkLocked = _data[1] & 0x04 ? true : false;
    RGBLED.isComposeLocked = _data[1] & 0x08 ? true : false;
    RGBLED.isKanaLocked = _data[1] & 0x10 ? true : false;
    for (int i = 0; i < sizeof(_data) / sizeof(_data[0]); ++i) {
        RGBLED.SetAuraBuffer(_data[i], i);
    }
}

void UpdateFNColor() {
    switch (config.FilterTime) {
        case 100:
            RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_t{(uint8_t) 187, 167, 16});
            break;
        case 200:
            RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_t{(uint8_t) 72, 200, 224});
            break;
        case 300:
            RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_t{(uint8_t) 45, 23, 222});
            break;
        case 400:
            RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_t{(uint8_t) 213, 18, 232});
            break;
        case 500:
            RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_t{(uint8_t) 147, 112, 219});
            break;
        case 600:
            RGBLED.FNColorSet(HWKeyboard_RGBLED::Color_t{(uint8_t) 255, 0, 255});
            break;
        default:
            break;
    }
}

void KeymapModeSet() {
    switch (config.KeymapMode) {
        default:
        case 0:
            keyboard.keyMap[1][13] = HWKeyboard::DELETE;
            keyboard.keyMap[1][28] = HWKeyboard::HOME;
            keyboard.keyMap[1][43] = HWKeyboard::END;
            keyboard.keyMap[1][57] = HWKeyboard::PAGE_UP;
            keyboard.keyMap[1][71] = HWKeyboard::PAGE_DOWN;
            keyboard.keyMap[2][13] = HWKeyboard::INSERT;
            keyboard.keyMap[2][28] = HWKeyboard::PAD_NUM_LOCK;
            keyboard.keyMap[2][43] = HWKeyboard::MUTE;
            keyboard.keyMap[2][57] = HWKeyboard::PAGE_UP;
            keyboard.keyMap[2][71] = HWKeyboard::PAGE_DOWN;
            break;
        case 1:
            keyboard.keyMap[1][13] = HWKeyboard::PRINT;
            keyboard.keyMap[1][28] = HWKeyboard::INSERT;
            keyboard.keyMap[1][43] = HWKeyboard::DELETE;
            keyboard.keyMap[1][57] = HWKeyboard::HOME;
            keyboard.keyMap[1][71] = HWKeyboard::END;
            keyboard.keyMap[2][13] = HWKeyboard::PAUSE;
            keyboard.keyMap[2][57] = HWKeyboard::PAGE_UP;
            keyboard.keyMap[2][71] = HWKeyboard::PAGE_DOWN;
            keyboard.keyMap[2][28] = HWKeyboard::INSERT;
            keyboard.keyMap[2][43] = HWKeyboard::DELETE;
            break;
    }
}