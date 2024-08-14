/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2.h
  * @author  MCD Application Team
  * @brief   Header for service2.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WSS_H
#define WSS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  WSS_WSF,
  WSS_MNBS,
  /* USER CODE BEGIN Service2_CharOpcode_t */

  /* USER CODE END Service2_CharOpcode_t */
  WSS_CHAROPCODE_LAST
} WSS_CharOpcode_t;

typedef enum
{
  WSS_WSF_READ_EVT,
  WSS_MNBS_INDICATE_ENABLED_EVT,
  WSS_MNBS_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service2_OpcodeEvt_t */

  /* USER CODE END Service2_OpcodeEvt_t */
  WSS_BOOT_REQUEST_EVT
} WSS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} WSS_Data_t;

typedef struct
{
  WSS_OpcodeEvt_t       EvtOpcode;
  WSS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service2_NotificationEvt_t */

  /* USER CODE END Service2_NotificationEvt_t */
} WSS_NotificationEvt_t;

/* USER CODE BEGIN ET */
 typedef enum
{
  WSS_FEATURE_TIME_STAMP           = 1<<0,
  WSS_FEATURE_MULTIPLE_USERS       = 1<<1,
  WSS_FEATURE_BMI                  = 1<<2,
  WSS_FEATURE_RESOLUTION_0_5KG     = 1<<3,
  WSS_FEATURE_RESOLUTION_0_2KG     = 2<<3,
  WSS_FEATURE_RESOLUTION_0_1KG     = 3<<3,
  WSS_FEATURE_RESOLUTION_0_05KG    = 4<<3,
  WSS_FEATURE_RESOLUTION_0_02KG    = 5<<3,
  WSS_FEATURE_RESOLUTION_0_01KG    = 6<<3,
  WSS_FEATURE_RESOLUTION_0_005KG   = 7<<3,
  WSS_FEATURE_RESOLUTION_0_01m     = 1<<7,
  WSS_FEATURE_RESOLUTION_0_005m    = 2<<7,
  WSS_FEATURE_RESOLUTION_0_001m    = 3<<7,
} WSS_WeightScaleFeatureOption_t;

typedef enum
{
  WSS_MEASUREMENT_IND_DISABLED_EVT=0,
  WSS_MEASUREMENT_IND_ENABLED_EVT
} WSS_App_Opcode_Notification_evt_t;

typedef enum
{
  WSS_NO_FLAGS = 0,
  WSS_FLAGS_VALUE_UNIT_IMPERIAL= (1<<0),
  WSS_FLAGS_TIME_STAMP_PRESENT = (1<<1),
  WSS_FLAGS_USER_ID_PRESENT = (1<<2),
  WSS_FLAGS_BMI_AND_HEIGHT_PRESENT = (1<<3),
} WSS_WM_Flags_t;

typedef struct
{
  uint32_t Value;  
} WSS_FeatureValue_t;

typedef struct
{
  uint16_t  Year;
  uint8_t   Month;
  uint8_t   Day;
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
}WSS_TimeStamp_t;

typedef struct
{
  uint16_t Weight;
  WSS_TimeStamp_t TimeStamp;
  uint16_t BMI;
  uint16_t Height;
  uint8_t Flags;
} WSS_MeasurementValue_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void WSS_Init(void);
void WSS_Notification(WSS_NotificationEvt_t *p_Notification);
tBleStatus WSS_UpdateValue(WSS_CharOpcode_t CharOpcode, WSS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*WSS_H */
