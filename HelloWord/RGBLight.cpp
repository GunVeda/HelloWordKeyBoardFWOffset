//
// Created by Administrator on 2022/11/4.
//
#include "spi.h"
#include "RGBLight.h"
#include "HelloWord/hw_keyboard.h"
#include "configurations.h"

extern HWKeyboard keyboard;
extern KeyboardConfig_t config;

void HWKeyboard_RGBLED::RGBLED_Init(void) {
    // Init RGB buffer
    for (uint8_t i = 0; i < LED_NUMBER; i++)
        SetRgbBufferByID(i, Color_t{0, 0, 0}, KeyboardBrightness);

    CapsColorSet(Color_t{200, 0, 0});
    NumColorSet(Color_t{0, 200, 0});
//    MonochromeColorSet(Color_t{242, 157, 0});
    if (config.FilterTime == 100) {
        FNColorSet(Color_t{(uint8_t) 187, 167, 16});
    } else if (config.FilterTime == 200) {
        FNColorSet(Color_t{(uint8_t) 72, 200, 224});
    } else if (config.FilterTime == 300) {
        FNColorSet(Color_t{(uint8_t) 45, 23, 222});
    } else if (config.FilterTime == 400) {
        FNColorSet(Color_t{(uint8_t) 213, 18, 232});
    }
}


void HWKeyboard_RGBLED::SetRgbBufferByID(uint8_t _keyId, Color_t _color, float _brightness) {
    // To ensure there's no sequence zero bits, otherwise will case ws2812b protocol error.
    if (_color.b < 1)_color.b = 1;
    for (int i = 0; i < 8; i++) {
        rgbBuffer[_keyId][0][i] =
                ((uint8_t) ((float) _color.g * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
        rgbBuffer[_keyId][1][i] =
                ((uint8_t) ((float) _color.r * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
        rgbBuffer[_keyId][2][i] =
                ((uint8_t) ((float) _color.b * _brightness) >> brightnessPreDiv) & (0x80 >> i) ? WS_HIGH : WS_LOW;
    }
}


void HWKeyboard_RGBLED::SyncLights() {
    while (isRgbTxBusy);
    isRgbTxBusy = true;
    HAL_SPI_Transmit_DMA(&hspi2, (uint8_t *) rgbBuffer, LED_NUMBER * 3 * 8);
    while (isRgbTxBusy);
    isRgbTxBusy = true;
    HAL_SPI_Transmit_DMA(&hspi2, wsCommit, 64);
}

void HWKeyboard_RGBLED::RGBLED_Handle() {
    /*---- This is a demo RGB effect ----*/
    RGBLED_ModeHandle(RGBMode);
    //右下角三个灯
    if (isNumLocked) {
        //   RGBLED.SetRgbBufferByID(0, Color_t{(uint8_t)  0, 180, 0}, 1);
        SetRgbBufferByID(82, NumColor, 1);
        SetRgbBufferByID(83, NumColor, 1);
        SetRgbBufferByID(84, NumColor, 1);
    }
    if (isCapsLocked) {
        //  RGBLED.SetRgbBufferByID(0, Color_t{(uint8_t) 180, 0, 0}, 1);
        SetRgbBufferByID(82, CapsColor, 1);
        SetRgbBufferByID(83, CapsColor, 1);
        SetRgbBufferByID(84, CapsColor, 1);
    }

    if (keyboard.isFNPress) {
        //   RGBLED.SetRgbBufferByID(0, Color_t{(uint8_t)  0, 0, 180}, 1);
        SetRgbBufferByID(82, FNColor, 1);
        SetRgbBufferByID(83, FNColor, 1);
        SetRgbBufferByID(84, FNColor, 1);
    }

    if (keyboard.TouchTestLED) {
        if (keyboard.GetTouchBarState(1))
            SetRgbBufferByID(12, Color_t{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(12, Color_t{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(2))
            SetRgbBufferByID(11, Color_t{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(11, Color_t{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(3))
            SetRgbBufferByID(10, Color_t{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(10, Color_t{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(4))
            SetRgbBufferByID(9, Color_t{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(9, Color_t{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(5))
            SetRgbBufferByID(8, Color_t{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(8, Color_t{(uint8_t) 255, 180, 0}, 0);
        if (keyboard.GetTouchBarState(6))
            SetRgbBufferByID(7, Color_t{(uint8_t) 0, 0, 255}, 1);
        else SetRgbBufferByID(7, Color_t{(uint8_t) 255, 180, 0}, 0);
    }
    //if(isUartGet){
    //    SetRgbBufferByID(0, Color_t{(uint8_t) 213, 18, 232}, 1);
    //   isUartGet = 0;
    //        OnUartCmd(UartRxBuf, 1);
    // }
    /*-----------------------------------*/
    // Send RGB buffers to LEDs ++
    SyncLights();
}

void HWKeyboard_RGBLED::FNColorSet(HWKeyboard_RGBLED::Color_t Color) {
    FNColor = Color;
}

void HWKeyboard_RGBLED::CapsColorSet(HWKeyboard_RGBLED::Color_t Color) {
    CapsColor = Color;
}

void HWKeyboard_RGBLED::NumColorSet(HWKeyboard_RGBLED::Color_t Color) {
    NumColor = Color;
}

void HWKeyboard_RGBLED::MonochromeColorSet(HWKeyboard_RGBLED::Color_t Color) {
    MonochromeColor = Color;
}

HWKeyboard_RGBLED::Color_t HWKeyboard_RGBLED::MonochromeColorGet() {
    return MonochromeColor;
}

void HWKeyboard_RGBLED::RGBLED_ModeHandle(uint8_t mode) {
    if (LightON) {
        switch (mode) {
            case monochrome: {
                for (uint8_t i = 0; i < 85; i++) {
                    SetRgbBufferByID(i, MonochromeColor, KeyboardBrightness);
                }
                //反贴氛围灯
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, MonochromeColor, KeyboardBrightness);
                }
            }
                break;
            case breathing: {
                static float LEDBreadBrightness = 0.09;
                static bool LEDBreadDir = true;
                static float W = 0;
                if (LEDBreadBrightness >= 1.1)LEDBreadDir = false;
                else if (LEDBreadBrightness <= 0.06)LEDBreadDir = true;
                LEDBreadDir ? LEDBreadBrightness += 0.005 : LEDBreadBrightness -= 0.005;
                if (LEDBreadBrightness > 1)
                    W = 1;
                else
                    W = LEDBreadBrightness;
                //正贴灯
                for (uint8_t i = 0; i < 85; i++) {
//                    SetRgbBufferByID(i, Color_t{255, 255, 255}, KeyboardBrightness * W);
                    SetRgbBufferByID(i, MonochromeColor, KeyboardBrightness * W);
                }
                //反贴氛围灯
                for (uint8_t i = 85; i < 104; i++) {
//                    SetRgbBufferByID(i, Color_t{255, 255, 255}, KeyboardBrightness * W);
                    SetRgbBufferByID(i, MonochromeColor, KeyboardBrightness * W);
                }
            }
                break;
            case Illusorycolor: {
                static uint32_t t = 1;
                static bool fadeDir = true;
                fadeDir ? t++ : t--;

                if (t > 250) fadeDir = false;
                else if (t < 1) fadeDir = true;

                //正贴灯
                for (uint8_t i = 0; i < 85; i++) {
                    SetRgbBufferByID(i, Color_t{(uint8_t) t, 50, 0}, KeyboardBrightness);
                }
                //反贴氛围灯
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, Color_t{(uint8_t) t, 50, 0}, KeyboardBrightness);
                }
            }
                break;
            case Follow:
                for (uint8_t i = 0; i < 82; i++) {
                    if (RGBLEDKeyBuf[i / 8] & (0x80 >> (i % 8))) {
                        SetRgbBufferByID(RGBLEDKeymap[i], MonochromeColor, KeyboardBrightness);
                    } else {
                        SetRgbBufferByID(RGBLEDKeymap[i], Color_t{0, 0, 0}, 0);
                    }
                }
                for (uint8_t i = 82; i < 85; i++) {
                    SetRgbBufferByID(i, MonochromeColor, 0);
                }
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, FNColor, KeyboardBrightness);
                }
                break;
            case Marquee:
                for (uint8_t i = 0; i < 82; i++) {
                    SetRgbBufferByID(i, Color_t{0, 0, 0}, 0);
                }
                if (index > 81) {
                    index = 0;
                }
                SetRgbBufferByID(RGBLEDKeymap[index], MonochromeColor, KeyboardBrightness);
                wait++;
                if (wait > 5) {
                    wait = 0;
                    index++;
                }
                for (uint8_t i = 82; i < 85; i++) {
                    SetRgbBufferByID(i, MonochromeColor, 0);
                }
                for (uint8_t i = 85; i < 104; i++) {
                    SetRgbBufferByID(i, FNColor, KeyboardBrightness);
                }
//                for (uint8_t i: auraBuffer) {
//                    SetRgbBufferByID(i, FNColor, KeyboardBrightness);
//                }
                break;
        }
    } else {
        for (uint8_t i = 0; i < LED_NUMBER; i++) {
            SetRgbBufferByID(i, Color_t{0, 0, 0}, 0);
        }
    }
}

void HWKeyboard_RGBLED::SetAuraBuffer(uint8_t buffer, uint16_t index) {
    auraBuffer[index] = buffer;
}

void HWKeyboard_RGBLED::RGBLED_ctrl(HWKeyboard_RGBLED *rgb, uint8_t trigger, bool iswriteEep) {
    switch (trigger) {
        case RGBLEDON: {
            rgb->LightON = true;
            if (iswriteEep)config.LightON = 1;
        }
            break;
        case RGBLEDOFF: {
            rgb->LightON = false;
            if (iswriteEep) config.LightON = 0;
        }
            break;
        case RGBLEDNegate: {
            if (rgb->LightON == true) {
                rgb->LightON = false;
                if (iswriteEep)config.LightON = 0;
            } else {
                rgb->LightON = true;
                if (iswriteEep)config.LightON = 1;
            }
        }
            break;
    }
    if (iswriteEep)keyboard.isEepPush = true;
}