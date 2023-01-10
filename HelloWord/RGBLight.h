//
// Created by Administrator on 2022/11/4.
//

#ifndef HELLOWORD_KEYBOARD_FW_RGBLIGHT_H
#define HELLOWORD_KEYBOARD_FW_RGBLIGHT_H

//#include "main.h"
//#include "hw_keyboard.h"

class HWKeyboard_RGBLED {

public:
    static const uint8_t LED_NUMBER = 104;

    int16_t RGBLEDKeyBuf[11]={0};
    int16_t RGBLEDKeymap[82]={
       13,12,11,10,9,8,7,6,5,4,3,2,1,0,
       14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,
       43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,
       44,45,46,47,48,49,50,51,52,53,54,55,56,57,
       71,70,69,68,67,66,65,64,63,62,61,60,59,58,
       72,73,74,75,76,77,78,79,80,81,
    };
    struct Color_t
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    enum SpiWs2812Byte_t : uint8_t
    {
        //WS_HIGH = 0xFE,
       // WS_LOW = 0xE03333

        WS_HIGH = 0xF8,
        WS_LOW = 0xC0
    };

    enum ModeType{
        monochrome = 0,
        breathing,
        Illusorycolor,
        Follow,
        ModeTypeMax,
    };

    enum RGBLEDTrigger{
        RGBLEDON = 0,
        RGBLEDOFF,
        RGBLEDNegate,
    };

    float KeyboardBrightness = 0.7;
    bool LightON = false;
    volatile bool isRgbTxBusy;
    bool isCapsLocked = false;
    bool isNumLocked = false;
    bool isScrlkLocked = false;
    bool isComposeLocked = false;
    bool isKanaLocked = false;
    bool isUartGet = false;
    uint8_t RGBMode = monochrome;

    void RGBLED_Init(void);
    void SyncLights();
    void RGBLED_Handle();
    void SetRgbBufferByID(uint8_t _keyId, Color_t _color, float _brightness);
    void FNColorSet(HWKeyboard_RGBLED::Color_t Color);
    void CapsColorSet(HWKeyboard_RGBLED::Color_t Color);
    void NumColorSet(HWKeyboard_RGBLED::Color_t Color);
    void MonochromeColorSet(HWKeyboard_RGBLED::Color_t Color);
    HWKeyboard_RGBLED::Color_t MonochromeColorGet();
    void RGBLED_ModeHandle(uint8_t mode);
    void RGBLED_ctrl(HWKeyboard_RGBLED *rgb,uint8_t trigger,bool  iswriteEep);
private:

    uint8_t rgbBuffer[LED_NUMBER][3][8]{};
    uint8_t wsCommit[64] = {0};
    uint8_t brightnessPreDiv = 2; // 1/4
    Color_t FNColor;
    Color_t CapsColor;
    Color_t NumColor;
    Color_t MonochromeColor;

};
#endif //HELLOWORD_KEYBOARD_FW_RGBLIGHT_H
