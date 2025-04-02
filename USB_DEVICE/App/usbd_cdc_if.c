/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define MAX_CMD_LENGTH  10
static uint8_t rx_buffer[MAX_CMD_LENGTH];
static uint32_t rx_index = 0;
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */
#define NUM_LEDS 8

typedef struct {
  uint8_t number;
  GPIO_TypeDef* port;
  uint16_t pin;
  uint32_t blinkPeriod;
  uint32_t lastToggleTime;
  uint8_t blinkEnabled;
} LED_BlinkTypeDef;

LED_BlinkTypeDef ledBlink[NUM_LEDS] = {
  {3, GPIOE, LD3_Pin, 0, 0, 0},
  {4, GPIOE, LD4_Pin, 0, 0, 0},
  {5, GPIOE, LD5_Pin, 0, 0, 0},
  {6, GPIOE, LD6_Pin, 0, 0, 0},
  {7, GPIOE, LD7_Pin, 0, 0, 0},
  {8, GPIOE, LD8_Pin, 0, 0, 0},
  {9, GPIOE, LD9_Pin, 0, 0, 0},
  {10, GPIOE, LD10_Pin, 0, 0, 0}
};

void Process_Command(uint8_t *command, uint32_t length) {
  if (length < 3) return;

  if (command[0] == 'B') {
    int led_number = command[1] - '0';
    if (led_number < 3 || led_number > 10) {
      return;
    }

    for (uint8_t i = 0; i < NUM_LEDS; i++) {
      if (ledBlink[i].number == led_number) {
        char *comma = (char *)strchr((char *)command, ',');
        if (comma != NULL) {
          uint32_t period = (uint32_t)atoi(comma + 1);
          ledBlink[i].blinkPeriod = period;
          ledBlink[i].blinkEnabled = (period > 0) ? 1 : 0;
          ledBlink[i].lastToggleTime = HAL_GetTick();

          char response[50];
          sprintf(response, "LED%d blink period set to %lu ms\r\n", led_number, period);
          CDC_Transmit_FS((uint8_t *)response, strlen(response));
        }
        break;
      }
    }
  }
  else if (command[0] == 'E') {
    int led_number = command[1] - '0';
    if (led_number >= 3 && led_number <= 10) {
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        if (ledBlink[i].number == led_number) {
          ledBlink[i].blinkEnabled = 0;
          ledBlink[i].blinkPeriod = 0;
          break;
        }
      }
      switch (led_number) {
        case 3:
          HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_SET);
          break;
        case 4:
          HAL_GPIO_WritePin(GPIOE, LD4_Pin, GPIO_PIN_SET);
          break;
        case 5:
          HAL_GPIO_WritePin(GPIOE, LD5_Pin, GPIO_PIN_SET);
          break;
        case 6:
          HAL_GPIO_WritePin(GPIOE, LD6_Pin, GPIO_PIN_SET);
          break;
        case 7:
          HAL_GPIO_WritePin(GPIOE, LD7_Pin, GPIO_PIN_SET);
          break;
        case 8:
          HAL_GPIO_WritePin(GPIOE, LD8_Pin, GPIO_PIN_SET);
          break;
        case 9:
          HAL_GPIO_WritePin(GPIOE, LD9_Pin, GPIO_PIN_SET);
          break;
        case 10:
          HAL_GPIO_WritePin(GPIOE, LD10_Pin, GPIO_PIN_SET);
          break;
      }
    }
  }
  else if (command[0] == 'D') {
    int led_number = command[1] - '0';
    if (led_number >= 3 && led_number <= 10) {
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        if (ledBlink[i].number == led_number) {
          ledBlink[i].blinkEnabled = 0;
          ledBlink[i].blinkPeriod = 0;
          break;
        }
      }
      switch (led_number) {
        case 3:
          HAL_GPIO_WritePin(GPIOE, LD3_Pin, GPIO_PIN_RESET);
          break;
        case 4:
          HAL_GPIO_WritePin(GPIOE, LD4_Pin, GPIO_PIN_RESET);
          break;
        case 5:
          HAL_GPIO_WritePin(GPIOE, LD5_Pin, GPIO_PIN_RESET);
          break;
        case 6:
          HAL_GPIO_WritePin(GPIOE, LD6_Pin, GPIO_PIN_RESET);
          break;
        case 7:
          HAL_GPIO_WritePin(GPIOE, LD7_Pin, GPIO_PIN_RESET);
          break;
        case 8:
          HAL_GPIO_WritePin(GPIOE, LD8_Pin, GPIO_PIN_RESET);
          break;
        case 9:
          HAL_GPIO_WritePin(GPIOE, LD9_Pin, GPIO_PIN_RESET);
          break;
        case 10:
          HAL_GPIO_WritePin(GPIOE, LD10_Pin, GPIO_PIN_RESET);
          break;
      }
    }
  }
}

void Update_LED_Blink(void) {
  uint32_t currentTime = HAL_GetTick();
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    if (ledBlink[i].blinkEnabled && ledBlink[i].blinkPeriod > 0) {
      if ((currentTime - ledBlink[i].lastToggleTime) >= ledBlink[i].blinkPeriod) {
        HAL_GPIO_TogglePin(ledBlink[i].port, ledBlink[i].pin);
        ledBlink[i].lastToggleTime = currentTime;
      }
    }
  }
}

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}


/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  for (uint32_t i = 0; i < *Len; i++) {
      if (rx_index < MAX_CMD_LENGTH) {
         rx_buffer[rx_index++] = Buf[i];
         CDC_Transmit_FS(Buf, *Len);
     }
  }
  if (rx_index > 0 && (rx_buffer[rx_index - 1] == '\n' || rx_buffer[rx_index - 1] == '\r')) {
      Process_Command(rx_buffer, rx_index);
      rx_index = 0;
  }
  return (USBD_OK);
  /* USER CODE END 6 */
}


/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
