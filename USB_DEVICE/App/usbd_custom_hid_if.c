/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */
#define LSB(_x) ((_x) & 0xFF)
#define MSB(_x) ((_x) >> 8)

#define RAWHID_USAGE_PAGE	0xFFC0
#define RAWHID_USAGE		0x0C00
#define RAWHID_TX_SIZE 64
#define RAWHID_RX_SIZE 64
/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
    {
        0x05, 0x01,         //   Usage Page (Generic Desktop),
        0x09, 0x06,         //   Usage (Keyboard),
        0xA1, 0x01,         //   Collection (Application),
        0x85, 0x01,                    //     REPORT_ID (1)
        // bitmap of modifiers
        0x75, 0x01,         //   Report Size (1),
        0x95, 0x08,         //   Report Count (8),
        0x05, 0x07,       //   Usage Page (Key Codes),
        0x19, 0xE0,       //   Usage Minimum (224),
        0x29, 0xE7,       //   Usage Maximum (231),
        0x15, 0x00,       //   Logical Minimum (0),
        0x25, 0x01,       //   Logical Maximum (1),
        0x81, 0x02,       //   Input (Data, Variable, Absolute), ;Modifier byte
        // bitmap of keys
        0x95, 0x78,       //   Report Count (120),
        0x75, 0x01,       //   Report Size (1),
        0x15, 0x00,       //   Logical Minimum (0),
        0x25, 0x01,       //   Logical Maximum(1),
        0x05, 0x07,       //   Usage Page (Key Codes),
        0x19, 0x00,       //   Usage Minimum (0),
        0x29, 0x77,       //   Usage Maximum (),
        0x81, 0x02,       //   Input (Data, Variable, Absolute),
#if 1
        // LED output report
        0x95, 0x05,       //   Report Count (5),
        0x75, 0x01,       //   Report Size (1),
        0x05, 0x08,       //   Usage Page (LEDs),
        0x19, 0x01,       //   Usage Minimum (1),
        0x29, 0x05,       //   Usage Maximum (5),
        0x91, 0x02,       //   Output (Data, Variable, Absolute),
        0x95, 0x01,       //   Report Count (1),
        0x75, 0x03,       //   Report Size (3),
        0x91, 0x03,       //   Output (Constant),
#endif
        0xC0  ,                 //   End Collection

#if 1
        //	RAW HID
	0x06, LSB(RAWHID_USAGE_PAGE), MSB(RAWHID_USAGE_PAGE),	// 30
	0x0A, LSB(RAWHID_USAGE), MSB(RAWHID_USAGE),

	0xA1, 0x01,				// Collection 0x01
    0x85, 0x02,             // REPORT_ID (3)
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,		// logical maximum = 255

	0x95, 32,				// report count TX
	0x09, 0x01,				// usage
	0x81, 0x02,				// Input (array)

	0x95, 32,				// report count RX
	0x09, 0x02,				// usage
	0x91, 0x02,				// Output (array)
	0xC0,					// end collection
#endif
#if 1
                      //?????????Consumer??????
        0x05,0x0C,   // Usage Pg (Consumer Devices)
        0x09, 0x01,   // Usage (Consumer Control)
                       //?????????ID???3
        0xA1, 0x01,   // Collection (Application)
        0x85, 0x03,   // Report Id (3)
                //??????????????????????????????
                0x09, 0x02,   //   Usage (Numeric KeyPad)
                // ????????????????????????
                0xA1, 0x02,   //   Collection (Logical)
                0x05, 0x09,   //     Usage Pg (Button)
                //?????????Button1~Button10
                0x19, 0x01,   //     Usage Min (Button1)
                0x29, 0x0A,   //     Usage Max (Button10)
                //??????????????????????????????1~10
                0x15, 0x01,   //     Logical Min (1)
                0x25, 0x0A,   //     Logical Max (10)
                //1???4bit????????????????????????1~10????????????????????????????????????????????????
                0x75, 0x04,   //     Report Size (4)
                0x95, 0x01,   //     Report Count (1)
                //??????4bit?????????????????????
                0x81, 0x00,   //     Input (Data, Ary, Abs)
                //????????????????????????
                0xC0,         //   End Collection
                0x05, 0x0C,   //   Usage Pg (ConsumerDevices)
                0x09, 0x86,   //   Usage (Channel)
                //??????????????????-1~1????????????????????????-1???1???????????????+???-
                0x15, 0xFF,   //   Logical Min (-1)
                0x25, 0x01,   //   Logical Max (1)
                //?????????2bit???????????????1???bit????????????+????????????????????????-
                0x75, 0x02,   //   Report Size (2)
                0x95, 0x01,   //   Report Count (1)
                //?????????2bit??????????????????
                0x81, 0x46,   //   Input (Data, Var,Rel, Null)
                //??????????????????????????????????????????
                0x09, 0xE9,   //   Usage (Volume Up)
                0x09, 0xEA,   //   Usage (Volume Down)
                //????????????0~1???????????????LogicalMax,???????????????LogicalMax=1
                0x15, 0x00,   //   Logical Min (0)
                //??????2???1bit?????????bit???????????????
                0x75, 0x01,   //   Report Size (1)
                0x95, 0x02,   //   Report Count (2)
                //???2???1bit?????????????????????
                0x81, 0x02,   //   Input (Data, Var,Abs)
                //??????12?????????
                0x09, 0xE2,   //   Usage (Mute)
//                0x09, 0x30,   //   Usage (Power)
                0x09, 0x6F,   //   Usage (SYS_BRIGHTNESS_UP)
//                0x09, 0x40,   //   Usage (Menu)
                0x09, 0x70,   //   Usage (SYS_BRIGHTNESS_DOWN)
//                0x09, 0xB1,   //   Usage (Pause)
                0x09, 0xCD,   //   Usage (Play/Pause)
//                0x09, 0xB2,   //   Usage (Record)
                0x0a, 0x83, 0x01,   //   Usage (Media Select)
                0x0a, 0x23, 0x02,   //   Usage (Home)
//                0x0a, 0x24, 0x02,   //   Usage (Back)
                0x0a, 0x92, 0x01,   //   Usage (Calculator)
//                0x09, 0xB3,   //   Usage (Fast Forward)
                0x0a, 0x94, 0x01,   //   Usage (My Computer)
//                0x09, 0xB4,   //   Usage (Rewind)
                0x0a, 0x8A, 0x01,   //   Usage (Mail)
                0x09, 0xB5,   //   Usage (Scan Next)
                0x09, 0xB6,   //   Usage (Scan Prev)
                0x09, 0xB7,   //   Usage (Stop)
                //?????????4????????????1~12???1??????Mute ??? 12??????Stop
                0x15, 0x01,   //   Logical Min (1)
                0x25, 0x0C,   //   Logical Max (12)
                0x75, 0x04,   //   Report Size (4)
                0x95, 0x01,   //   Report Count (1)
                //?????????4bit??????????????????
                0x81, 0x00,   //   Input (Data, Ary,Abs)
                //????????????
                0x09, 0x80,   //   Usage (Selection)
                0xA1, 0x02,   //   Collection (Logical)
                0x05, 0x09,   //     Usage Pg (Button)
                //?????????Button1~Button3
                0x19, 0x01,   //     Usage Min (Button1)
                0x29, 0x03,   //     Usage Max (Button3)
                //???????????????????????????1~3
                0x15, 0x01,   //     Logical Min (1)
                0x25, 0x03,   //     Logical Max (3)
                //???????????????Report Count??????????????????Report Count =1???????????????1???2bit?????????1~3
                0x75, 0x02,   //     Report Size (2)
                //???1??????????????????????????????
                0x81, 0x00,   //     Input (Data, Ary,Abs)
                //?????????2???bit?????????????????????????????????ReportSize=2???Report Count=1??????????????????
                0xC0,         //   End Collection
                0x81, 0x03,   //   Input (Const, Var,Abs)
                0xC0        // End Collection
#endif
    };

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
    {
        CUSTOM_HID_ReportDesc_FS,
        CUSTOM_HID_Init_FS,
        CUSTOM_HID_DeInit_FS,
        CUSTOM_HID_OutEvent_FS
    };

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
    /* USER CODE BEGIN 4 */
    return (USBD_OK);
    /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
    /* USER CODE BEGIN 5 */
    return (USBD_OK);
    /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
    /* USER CODE BEGIN 6 */


    return (USBD_OK);
    /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

