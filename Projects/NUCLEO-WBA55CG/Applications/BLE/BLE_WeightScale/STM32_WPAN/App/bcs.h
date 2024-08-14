/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4.h
  * @author  MCD Application Team
  * @brief   Header for service4.c
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
#ifndef BCS_H
#define BCS_H

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
  BCS_BCF,
  BCS_BCM,
  /* USER CODE BEGIN Service3_CharOpcode_t */

  /* USER CODE END Service3_CharOpcode_t */
  BCS_CHAROPCODE_LAST
} BCS_CharOpcode_t;

typedef enum
{
  BCS_BCF_READ_EVT,
  BCS_BCM_INDICATE_ENABLED_EVT,
  BCS_BCM_INDICATE_DISABLED_EVT,
  /* USER CODE BEGIN Service3_OpcodeEvt_t */

  /* USER CODE END Service3_OpcodeEvt_t */
  BCS_BOOT_REQUEST_EVT
} BCS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service3_Data_t */

  /* USER CODE END Service3_Data_t */
} BCS_Data_t;

typedef struct
{
  BCS_OpcodeEvt_t       EvtOpcode;
  BCS_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service3_NotificationEvt_t */

  /* USER CODE END Service3_NotificationEvt_t */
} BCS_NotificationEvt_t;

/* USER CODE BEGIN ET */
typedef enum
{
 /*
  * bit 0
  * Time Stamp Supported Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_TIME_STAMP           = 1<<0,
 /*
  * bit 1
  * Multiple Users Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_MULTIPLE_USERS       = 1<<1,
 /*
  * bit 2
  * Basal Metabolism Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_BASAL_METABOLISM     = 1<<2,
 /*
  * bit 3
  * Muscle Percentage Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_MUSCLE_PERCENTAGE    = 1<<3,
 /*
  * bit 4
  * Muscle Mass Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_MUSCLE_MASS          = 1<<4,
 /*
  * bit 5
  * Fat Free Mass Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_FAT_FREE_MASS        = 1<<5,
 /*
  * bit 6
  * Soft Lean Mass Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_SOFT_LEAN_MASS       = 1<<6,
 /*
  * bit 7
  * Body Water Mass Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_BODY_WATER_MASS      = 1<<7,
 /*
  * bit 8
  * Impedance Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_SUPPORTED_IMPEDANCE            = 1<<8,
 /*
  * bit 9
  * Weight Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_WEIGHT               = 1<<9,
 /*
  * bit 10
  * Height Supported
  * 0 = False 1 = True
  */
 BCS_FEATURE_HEIGHT               = 1<<10,
 /*
  * bit 11 - 14
  * Weight Measurement Resolution
  */
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_5KG   =       1<<11,
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_2KG   =       2<<11,
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_1KG   =       3<<11,
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_05KG  =       4<<11,
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_02KG  =       5<<11,
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_01KG  =       6<<11,
 BCS_FEATURE_WEIGHT_MEASUREMENT_RESOLUTION_0_005KG =       7<<11,
 /*
  * bit 15 - 17
  * Height Measurement Resolution
  */
 BCS_FEATURE_HEIGHT_MEASUREMENT_RESOLUTION_0_01M   =       1<<15,
 BCS_FEATURE_HEIGHT_MEASUREMENT_RESOLUTION_0_005M  =       2<<15,
 BCS_FEATURE_HEIGHT_MEASUREMENT_RESOLUTION_0_001M  =       3<<15,
} BCS_FeatureBCS_FEATURE_SUPPORTED_t;

typedef enum
{
  BCS_FLAG_MEASUREMENT_UNITS_SI       = 0,
  BCS_FLAG_MEASUREMENT_UNITS_IMPERIAL = 1<<0,
  BCS_FLAG_TIME_STAMP_PRESENT         = 1<<1,  /* 0 = False, 1 = True */
  BCS_FLAG_USER_ID_PRESENT            = 1<<2,  /* 0 = False, 1 = True */
  BCS_FLAG_BASAL_METABOLISM_PRESENT   = 1<<3,  /* 0 = False, 1 = True */
  BCS_FLAG_MUSCLE_PERCENTAGE_PRESENT  = 1<<4,  /* 0 = False, 1 = True */
  BCS_FLAG_MUSCLE_MASS_PRESENT        = 1<<5,  /* 0 = False, 1 = True */
  BCS_FLAG_FAT_FREE_MASS_PRESENT      = 1<<6,  /* 0 = False, 1 = True */
  BCS_FLAG_SOFT_LEAN_MASS_PRESENT     = 1<<7,  /* 0 = False, 1 = True */
  BCS_FLAG_BODY_WATER_MASS_PRESENT    = 1<<8,  /* 0 = False, 1 = True */
  BCS_FLAG_IMPEDANCE_PRESENT          = 1<<9,  /* 0 = False, 1 = True */
  BCS_FLAG_WEIGHT_PRESENT             = 1<<10, /* 0 = False, 1 = True */
  BCS_FLAG_HEIGHT_PRESENT             = 1<<11, /* 0 = False, 1 = True */
  BCS_FLAG_MULTIPLE_PACKET_MEASUREMENT = 1<<12,/* 0 = False, 1 = True */
} BCS_MeasurementFlags_t;

typedef struct
{
  uint32_t Value;
} BCS_FeatureValue_t;

typedef struct
{
  uint16_t  Year;
  uint8_t   Month;
  uint8_t   Day;
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
}BCS_TimeStamp_t;  /* Size (in octets) = 7 */

typedef struct 
{
  uint16_t Flags;
  uint16_t BodyFatPercentage;
  BCS_TimeStamp_t TimeStamp;
  uint8_t UserID;
  uint16_t BasalMetabolism;
  uint16_t MusclePercentage;
  uint16_t MuscleMass;
  uint16_t FatFreeMass;
  uint16_t SoftLeanMass;
  uint16_t BodyWaterMass;
  uint16_t Impedance;
  uint16_t Weight;
  uint16_t Height;
} BCS_MeasurementValue_t;
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
void BCS_Init(void);
void BCS_Notification(BCS_NotificationEvt_t *p_Notification);
tBleStatus BCS_UpdateValue(BCS_CharOpcode_t CharOpcode, BCS_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BCS_H */
