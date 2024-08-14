/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4_app.c
  * @author  MCD Application Team
  * @brief   service4_app application definition.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "bcs_app.h"
#include "bcs.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum 
{
  MeasurementUnits_SI = 0,   /* Weight and Mass in units of kilogram (kg) and Height in units of meter */
  MeasurementUnits_Imperial  /* Weight and Mass in units of pound (lb) and Height in units of inch (in) */
} Measurement_Unit_t;
/* USER CODE END PTD */

typedef enum
{
  Bcm_INDICATION_OFF,
  Bcm_INDICATION_ON,
  /* USER CODE BEGIN Service3_APP_SendInformation_t */

  /* USER CODE END Service3_APP_SendInformation_t */
  BCS_APP_SENDINFORMATION_LAST
} BCS_APP_SendInformation_t;

typedef struct
{
  BCS_APP_SendInformation_t     Bcm_Indication_Status;
  /* USER CODE BEGIN Service3_APP_Context_t */
  BCS_MeasurementValue_t MeasurementChar;
  BCS_FeatureValue_t FeatureChar;
  
  uint8_t Indication_Status;
  UTIL_TIMER_Object_t TimerMeasurement_Id;
  uint32_t StartTick;
  uint8_t Paired;
  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} BCS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_WEIGHT_IN_KG                                                  70
#define DEFAULT_HEIGHT_IN_METERS                                             1.7f
#define DEFAULT_BCS_TIME_STAMP_YEAR                                         2022
#define DEFAULT_BCS_TIME_STAMP_MONTH                                           7
#define DEFAULT_BCS_TIME_STAMP_DAY                                             5
#define DEFAULT_BCS_TIME_STAMP_HOURS                                           0
#define DEFAULT_BCS_TIME_STAMP_MINUTES                                         0
#define DEFAULT_BCS_TIME_STAMP_SECONDS                                         0

#define DEFAULT_BCS_BASAL_METALBOLISM                                       1700
#define DEFAULT_BCS_BODY_FAT_PERCENTAGE                                       10
#define DEFAULT_BCS_BODY_WATER_MASS                                           35
#define DEFAULT_BCS_FAT_FREE_MASS                                             25
#define DEFAULT_BCS_IMPEDANCE                                        (2000 << 2)
#define DEFAULT_BCS_MUSCLE_MASS                                               40
#define DEFAULT_BCS_MUSCLE_PERCENTAGE                                         80
#define DEFAULT_BCS_SOFT_LEAN_MASS                                            63

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BCS_APP_MEASUREMENT_INTERVAL   (5000)  /**< 5s */

/* Field Weight of Body Composition Measurement */
#define POUNDS_TO_KILOGRAMS        0.4536
#define INCHES_TO_METERS           0.3048
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static BCS_APP_Context_t BCS_APP_Context;

uint8_t a_BCS_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void BCS_Bcm_SendIndication(void);

/* USER CODE BEGIN PFP */
static void BCS_APP_Measure_timCB(void *arg);
/* Measurement */
static void BCS_APP_Measurement(void);
void BCSAPP_RemoveFeature(uint16_t feature);
void BCSAPP_Reset(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void BCS_Notification(BCS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */

  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case BCS_BCF_READ_EVT:
      /* USER CODE BEGIN Service3Char1_READ_EVT */

      /* USER CODE END Service3Char1_READ_EVT */
      break;

    case BCS_BCM_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char2_INDICATE_ENABLED_EVT */
      if(BCS_APP_Context.Paired == TRUE)
      {  
        BCS_APP_Context.Indication_Status = 1;
        
        UTIL_TIMER_Stop(&(BCS_APP_Context.TimerMeasurement_Id));
        UTIL_TIMER_StartWithPeriod(&(BCS_APP_Context.TimerMeasurement_Id), BCS_APP_MEASUREMENT_INTERVAL);
      }
      /* USER CODE END Service3Char2_INDICATE_ENABLED_EVT */
      break;

    case BCS_BCM_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char2_INDICATE_DISABLED_EVT */
      if(BCS_APP_Context.Paired == TRUE)
      {  
        BCS_APP_Context.Indication_Status = 0;
        
        UTIL_TIMER_Stop(&(BCS_APP_Context.TimerMeasurement_Id));
      }
      /* USER CODE END Service3Char2_INDICATE_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_Notification_default */

      /* USER CODE END Service3_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service3_Notification_2 */

  /* USER CODE END Service3_Notification_2 */
  return;
}

void BCS_APP_EvtRx(BCS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */
    case BCS_FORCE_MEAS_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */
      /* WSP PTS test when notification is requested */
      BCS_APP_Context.Indication_Status = 1;
      
      UTIL_TIMER_Stop(&(BCS_APP_Context.TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod(&(BCS_APP_Context.TimerMeasurement_Id), BCS_APP_MEASUREMENT_INTERVAL);
      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;
    /* USER CODE END Service3_APP_EvtRx_Service3_EvtOpcode */
    case BCS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_CONN_HANDLE_EVT */
      BCS_APP_Context.Paired = p_Notification->PairingComplete;
      /* USER CODE END Service3_APP_CONN_HANDLE_EVT */
      break;

    case BCS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */
      BCS_APP_Context.Indication_Status = 0;
      
      UTIL_TIMER_Stop(&(BCS_APP_Context.TimerMeasurement_Id));
      /* USER CODE END Service3_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_APP_EvtRx_default */

      /* USER CODE END Service3_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service3_APP_EvtRx_2 */

  /* USER CODE END Service3_APP_EvtRx_2 */

  return;
}

void BCS_APP_Init(void)
{
  UNUSED(BCS_APP_Context);
  BCS_Init();

  /* USER CODE BEGIN Service3_APP_Init */
  BCS_Data_t msg_conf;
  uint8_t length = 0;

  APP_DBG_MSG("BCS_APP_Init\n\r");
  
  /*
   * No Indication by default
   */
  BCS_APP_Context.Indication_Status                 = 0;
  
  /*
   * Ticks
   */
  BCS_APP_Context.StartTick                         = HAL_GetTick();
  
  /**
   * Initialize Body Composition Features
   */
  BCS_APP_Context.FeatureChar.Value = (BCS_FEATURE_SUPPORTED_TIME_STAMP        |
                                       BCS_FEATURE_SUPPORTED_BASAL_METABOLISM  |
                                       BCS_FEATURE_SUPPORTED_MUSCLE_PERCENTAGE |
                                       BCS_FEATURE_SUPPORTED_MUSCLE_MASS       |
                                       BCS_FEATURE_SUPPORTED_FAT_FREE_MASS     |
                                       BCS_FEATURE_SUPPORTED_SOFT_LEAN_MASS    |
                                       BCS_FEATURE_SUPPORTED_BODY_WATER_MASS   |
                                       BCS_FEATURE_SUPPORTED_IMPEDANCE);
  
  a_BCS_UpdateCharData[length++] = (BCS_APP_Context.FeatureChar.Value) & 0xFF;
  a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.FeatureChar.Value) >> 8) & 0xFF;
  a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.FeatureChar.Value) >> 16) & 0xFF;
  a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.FeatureChar.Value) >> 24) & 0xFF;
  msg_conf.Length = length;
  msg_conf.p_Payload = a_BCS_UpdateCharData;
  BCS_UpdateValue(BCS_BCF, &msg_conf);

  /**
   * Initialize Fields of BCS Measurement
   */
  /* Body Composition Measurement value unit */
  BCS_APP_Context.MeasurementChar.Flags             = BCS_FLAG_MEASUREMENT_UNITS_SI;
  
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MULTIPLE_USERS) == BCS_FEATURE_SUPPORTED_MULTIPLE_USERS)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_USER_ID_PRESENT;
  }

  /* Add support for Weight */
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_WEIGHT) == BCS_FEATURE_WEIGHT)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_WEIGHT_PRESENT;
    BCS_APP_Context.MeasurementChar.Weight            = 0;
  }
  
  /* Add support for Time Stamp */
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_TIME_STAMP) == BCS_FEATURE_SUPPORTED_TIME_STAMP)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_TIME_STAMP_PRESENT;
    BCS_APP_Context.MeasurementChar.TimeStamp.Year    = DEFAULT_BCS_TIME_STAMP_YEAR;
    BCS_APP_Context.MeasurementChar.TimeStamp.Month   = DEFAULT_BCS_TIME_STAMP_MONTH;
    BCS_APP_Context.MeasurementChar.TimeStamp.Day     = DEFAULT_BCS_TIME_STAMP_DAY;
    BCS_APP_Context.MeasurementChar.TimeStamp.Hours   = DEFAULT_BCS_TIME_STAMP_HOURS;
    BCS_APP_Context.MeasurementChar.TimeStamp.Minutes = DEFAULT_BCS_TIME_STAMP_MINUTES;
    BCS_APP_Context.MeasurementChar.TimeStamp.Seconds = DEFAULT_BCS_TIME_STAMP_SECONDS;
  }

  /* Add support for Height */
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_HEIGHT) == BCS_FEATURE_HEIGHT)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_HEIGHT_PRESENT;
    BCS_APP_Context.MeasurementChar.Height            = 0;
  }

  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_BASAL_METABOLISM) == BCS_FEATURE_SUPPORTED_BASAL_METABOLISM)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_BASAL_METABOLISM_PRESENT;
    BCS_APP_Context.MeasurementChar.BasalMetabolism   = DEFAULT_BCS_BASAL_METALBOLISM;
  }
  BCS_APP_Context.MeasurementChar.BodyFatPercentage = DEFAULT_BCS_BODY_FAT_PERCENTAGE * 10;
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_BODY_WATER_MASS) == BCS_FEATURE_SUPPORTED_BODY_WATER_MASS)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_BODY_WATER_MASS_PRESENT;
    BCS_APP_Context.MeasurementChar.BodyWaterMass     = DEFAULT_BCS_BODY_WATER_MASS * 200;
  }
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_FAT_FREE_MASS) == BCS_FEATURE_SUPPORTED_FAT_FREE_MASS)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_FAT_FREE_MASS_PRESENT;
    BCS_APP_Context.MeasurementChar.FatFreeMass       = DEFAULT_BCS_FAT_FREE_MASS * 200;
  }
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_IMPEDANCE) == BCS_FEATURE_SUPPORTED_IMPEDANCE)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_IMPEDANCE_PRESENT;
    BCS_APP_Context.MeasurementChar.Impedance         = DEFAULT_BCS_IMPEDANCE;
  }
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MUSCLE_MASS) == BCS_FEATURE_SUPPORTED_MUSCLE_MASS)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_MUSCLE_MASS_PRESENT;
    BCS_APP_Context.MeasurementChar.MuscleMass        = DEFAULT_BCS_MUSCLE_MASS * 200;
  }
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MUSCLE_PERCENTAGE) == BCS_FEATURE_SUPPORTED_MUSCLE_PERCENTAGE)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_MUSCLE_PERCENTAGE_PRESENT;
    BCS_APP_Context.MeasurementChar.MusclePercentage  = DEFAULT_BCS_MUSCLE_PERCENTAGE * 10;
  }
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_SOFT_LEAN_MASS) == BCS_FEATURE_SUPPORTED_SOFT_LEAN_MASS)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_SOFT_LEAN_MASS_PRESENT;
    BCS_APP_Context.MeasurementChar.SoftLeanMass      = DEFAULT_BCS_SOFT_LEAN_MASS * 200;
  }
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MULTIPLE_USERS) == BCS_FEATURE_SUPPORTED_MULTIPLE_USERS)
  {
    BCS_APP_Context.MeasurementChar.Flags            |= BCS_FLAG_USER_ID_PRESENT;
    BCS_APP_Context.MeasurementChar.UserID            = 0;
  }
  /*
   * Create timer for Body Composition Measurement
   */
  UTIL_TIMER_Create(&(BCS_APP_Context.TimerMeasurement_Id),
                    BCS_APP_MEASUREMENT_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &BCS_APP_Measure_timCB, 0);
  
  /*
   * Register task for Body Composition Measurment
   */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_BCS_MEAS_REQ_ID, UTIL_SEQ_RFU, BCS_APP_Measurement );
  
  /* Start Measurement */
  UTIL_TIMER_StartWithPeriod(&(BCS_APP_Context.TimerMeasurement_Id), BCS_APP_MEASUREMENT_INTERVAL);
  /* USER CODE END Service3_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
void BCSAPP_RemoveFeature(uint16_t feature)
{
  BCS_Data_t msg_conf;
  uint8_t length = 0;

  if(feature & BCS_FEATURE_WEIGHT)
  {
    BCS_APP_Context.FeatureChar.Value &= (uint16_t)((~BCS_FEATURE_WEIGHT) & 0xFFFF);

    BCS_APP_Context.MeasurementChar.Flags &= (uint16_t)((~BCS_FLAG_WEIGHT_PRESENT) & 0xFFFF);
  }
  if(feature & BCS_FEATURE_HEIGHT)
  {
    BCS_APP_Context.FeatureChar.Value &= (uint16_t)((~BCS_FEATURE_HEIGHT) & 0xFFFF);

    BCS_APP_Context.MeasurementChar.Flags &= (uint16_t)((~BCS_FLAG_HEIGHT_PRESENT) & 0xFFFF);
  }

  a_BCS_UpdateCharData[length++] = (BCS_APP_Context.FeatureChar.Value) & 0xFF;
  a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.FeatureChar.Value) >> 8) & 0xFF;
  a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.FeatureChar.Value) >> 16) & 0xFF;
  a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.FeatureChar.Value) >> 24) & 0xFF;
  msg_conf.Length = length;
  msg_conf.p_Payload = a_BCS_UpdateCharData;
  BCS_UpdateValue(BCS_BCF, &msg_conf);
}

void BCSAPP_Reset(void)
{
  APP_DBG_MSG("BCSAPP_Reset\n\r");

  /*
  * Reset Application Context
  */
  BCS_APP_Context.Indication_Status = 0;  /* disable */
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void BCS_Bcm_SendIndication(void) /* Property Indication */
{
  BCS_APP_SendInformation_t indication_on_off = Bcm_INDICATION_OFF;
  BCS_Data_t bcs_indication_data;

  bcs_indication_data.p_Payload = (uint8_t*)a_BCS_UpdateCharData;
  bcs_indication_data.Length = 0;

  /* USER CODE BEGIN Service3Char2_IS_1*/

  /* USER CODE END Service3Char2_IS_1*/

  if (indication_on_off != Bcm_INDICATION_OFF)
  {
    BCS_UpdateValue(BCS_BCM, &bcs_indication_data);
  }

  /* USER CODE BEGIN Service3Char2_IS_Last*/

  /* USER CODE END Service3Char2_IS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void BCS_APP_Measure_timCB(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_BCS_MEAS_REQ_ID, CFG_SEQ_PRIO_0);

  return;
}

/*
 * Get Weight Measurement Resolution from Body Composition Feature field
 */
static uint8_t BcFeatureField_WMR(void)
{
  return ((BCS_APP_Context.FeatureChar.Value >> 3) & 0x0F);
}

/*
 * Get Measurement Units from Flags of Body Composition Measurement
 */
static Measurement_Unit_t BcMeasurementUnits_Weight(void)
{
  return (Measurement_Unit_t)(BCS_APP_Context.MeasurementChar.Flags & 1);
}

/*
 * Weight Measurement Resolution from Body Composition Feature field
 * Return value is the resolution for kilograms (kg) or resolution for pounds
 */
static float BcMeasurementResolution_Weight(uint8_t feature, Measurement_Unit_t unit)
{
  float resolution = 1.0f; /* the default is Not specified */
  
  switch (feature)
  {
  case 1:
    /* Resolution of 0.5 kg or 1 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.5f : 1.0f;
    break;
  case 2:
    /* Resolution of 0.2 kg or 0.5 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.2f : 0.5f;
    break;
  case 3:
    /* Resolution of 0.1 kg or 0.2 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.1f : 0.2f;
    break;
  case 4:
    /* Resolution of 0.05 kg or 0.1 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.05f : 0.1f;
    break;
  case 5:
    /* Resolution of 0.02 kg or 0.05 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.02f : 0.05f;
    break;
  case 6:
    /* Resolution of 0.01 kg or 0.02 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.01f : 0.02f;
    break;
  case 7:
    /* Resolution of 0.005 kg or 0.01 lb */
    resolution = (unit == MeasurementUnits_SI) ? 0.005f : 0.01f;
    break;
  default:
    /* do nothing */
    break;
  }
  
  return resolution;
}

/*
 * This function will transfer weight value with appoint unit
 * The formular: 1 pound = 0.4536 kilogram
 */
static uint16_t BcConvert_Weight(Measurement_Unit_t unit_from, Measurement_Unit_t unit_to, float weight)
{
  uint32_t v;
  float resolution = BcMeasurementResolution_Weight(BcFeatureField_WMR(), BcMeasurementUnits_Weight());
  
  if((unit_from == MeasurementUnits_SI) && (unit_to == MeasurementUnits_Imperial))
  {
    /* kilograms (kg) -> pounds */
    weight /= POUNDS_TO_KILOGRAMS;
  } else if ((unit_from == MeasurementUnits_Imperial) && (unit_to == MeasurementUnits_SI))
  {
    /* pounds -> kilograms (kg) */
    weight *= POUNDS_TO_KILOGRAMS;
  }
  
  weight /= resolution;
  /* floor value */
  v = (uint32_t)(weight * 10);
  return (uint16_t)((v + 5) / 10);
}

/*
 * Get Weight Measurement Resolution from Body Composition Feature field
 */
static uint8_t BcFeatureField_HMR(void)
{
  return ((BCS_APP_Context.FeatureChar.Value >> 7) & 0x07);
}


/*
 * Get Measurement Units from Flags of Body Composition Measurement
 */
static Measurement_Unit_t BcMeasurementUnits_Height(void)
{
  return (Measurement_Unit_t)(BCS_APP_Context.MeasurementChar.Flags & 1);
}


/*
 * Height Measurement Resolution from Body Composition Feature field
 * Return value is the resolution for meters or resolution for inches
 */
static float BcMeasurementResolution_Height(uint8_t feature, Measurement_Unit_t unit)
{
  float resolution = 1.0f; /* the default is Not specified */
  
  switch (feature){
  case 1:
    /* Resolution of 0.01 meter or 1 inch */
    resolution = (unit == MeasurementUnits_SI) ? 0.01f : 1.0f;
    break;
  case 2:
    /* Resolution of 0.005 meter or 0.5 inch */
    resolution = (unit == MeasurementUnits_SI) ? 0.005f : 0.5f;
    break;
  case 3:
    /* Resolution of 0.001 meter or 0.1 inch */
    resolution = (unit == MeasurementUnits_SI) ? 0.001f : 0.1f;
    break;
  default:
    /* do nothing */
    break;
  }
  
  return resolution;
}


/*
 * This function will transfer weight value with appoint unit
 * The formular: 1 pound = 0.4536 kilogram
 */
static uint16_t BcConvert_Height(Measurement_Unit_t unit_from, Measurement_Unit_t unit_to, float height)
{
  uint32_t v;
  float resolution = BcMeasurementResolution_Height(BcFeatureField_HMR(), BcMeasurementUnits_Height());
  
  if((unit_from == MeasurementUnits_SI) && (unit_to == MeasurementUnits_Imperial)){
    /* meters -> inches */
    height /= INCHES_TO_METERS;
  } else if ((unit_from == MeasurementUnits_Imperial) && (unit_to == MeasurementUnits_SI)){
    /* inches -> meters */
    height *= INCHES_TO_METERS;
  }
  
  height /= resolution;
  /* floor value */
  v = (uint32_t)(height * 10);
  return (uint16_t)((v + 5) / 10);
}

static void BCS_APP_Measurement(void)
{
  /*Weight, BMI,  Height Initialization*/
  uint16_t weight, height;
  
  weight = 0;
  height = 0;

  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_WEIGHT) == BCS_FEATURE_WEIGHT)
  {
    /* E.g. weight = 0x36B0 for 70kg with resolution = 0.0005 */
    float weight_si = (float)(DEFAULT_WEIGHT_IN_KG + (rand() % 10));
    weight = BcConvert_Weight(MeasurementUnits_SI,
                                      ((BCS_APP_Context.MeasurementChar.Flags & BCS_FLAG_MEASUREMENT_UNITS_IMPERIAL) ? MeasurementUnits_Imperial : MeasurementUnits_SI),
                                      weight_si);
  }
  
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_HEIGHT) == BCS_FEATURE_HEIGHT)
  {
    float height_si = (float)(DEFAULT_HEIGHT_IN_METERS + (rand() % 10));
    height = BcConvert_Height(MeasurementUnits_SI,
                                      ((BCS_APP_Context.MeasurementChar.Flags & BCS_FLAG_MEASUREMENT_UNITS_IMPERIAL) ? MeasurementUnits_Imperial : MeasurementUnits_SI),
                                      height_si);
  }

  
  uint32_t ticks = (HAL_GetTick() - BCS_APP_Context.StartTick) / 1000;
  
  if(BCS_APP_Context.Indication_Status == 0)
  {
    APP_DBG_MSG("Stop BCS Measurement\n\r");
    UTIL_TIMER_Stop(&(BCS_APP_Context.TimerMeasurement_Id));
    return;
  }
  
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_WEIGHT) == BCS_FEATURE_WEIGHT)
  {
    /* update Weight */
    BCS_APP_Context.MeasurementChar.Weight = weight;
  }
  
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_HEIGHT) == BCS_FEATURE_HEIGHT)
  {
    /* update Height */
    BCS_APP_Context.MeasurementChar.Height = height;
  }
  
  if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_TIME_STAMP) == BCS_FEATURE_SUPPORTED_TIME_STAMP)
  {
    /* update Time Stamp */
    BCS_APP_Context.MeasurementChar.TimeStamp.Year    = DEFAULT_BCS_TIME_STAMP_YEAR;
    BCS_APP_Context.MeasurementChar.TimeStamp.Month   = DEFAULT_BCS_TIME_STAMP_MONTH;
    BCS_APP_Context.MeasurementChar.TimeStamp.Day     = DEFAULT_BCS_TIME_STAMP_DAY;
    BCS_APP_Context.MeasurementChar.TimeStamp.Hours   = DEFAULT_BCS_TIME_STAMP_HOURS;
    BCS_APP_Context.MeasurementChar.TimeStamp.Minutes = DEFAULT_BCS_TIME_STAMP_MINUTES;
    BCS_APP_Context.MeasurementChar.TimeStamp.Seconds = DEFAULT_BCS_TIME_STAMP_SECONDS;

    while(ticks >= 60)
    {
      ticks -= 60;
      BCS_APP_Context.MeasurementChar.TimeStamp.Minutes += 1;
    }
    BCS_APP_Context.MeasurementChar.TimeStamp.Seconds = ticks;
    
    while(BCS_APP_Context.MeasurementChar.TimeStamp.Minutes >= 60)
    {
      BCS_APP_Context.MeasurementChar.TimeStamp.Minutes -= 60;
      BCS_APP_Context.MeasurementChar.TimeStamp.Hours += 1;
    }
    
    while(BCS_APP_Context.MeasurementChar.TimeStamp.Hours >= 24)
    {
      BCS_APP_Context.MeasurementChar.TimeStamp.Hours -= 24;
      BCS_APP_Context.MeasurementChar.TimeStamp.Day += 1;
    }
    
    /* let's handle date simply without permanment calendar */
    while(BCS_APP_Context.MeasurementChar.TimeStamp.Day >= 31)
    {
      BCS_APP_Context.MeasurementChar.TimeStamp.Day -= 31;
      BCS_APP_Context.MeasurementChar.TimeStamp.Month += 1;
    }
    
    while(BCS_APP_Context.MeasurementChar.TimeStamp.Month >= 12)
    {
      BCS_APP_Context.MeasurementChar.TimeStamp.Month -= 11;
      BCS_APP_Context.MeasurementChar.TimeStamp.Year += 1;
    }
  }
  
  {
    BCS_Data_t msg_conf;
    uint8_t length = 0;
    
    a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.Flags) & 0xFF;
    a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.Flags) >> 8) & 0xFF;
    
    a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.BodyFatPercentage) & 0xFF;
    a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.BodyFatPercentage) >> 8) & 0xFF;
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_TIME_STAMP) == BCS_FEATURE_SUPPORTED_TIME_STAMP)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.TimeStamp.Year) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.TimeStamp.Year) >> 8) & 0xFF;
      a_BCS_UpdateCharData[length++] = BCS_APP_Context.MeasurementChar.TimeStamp.Month;
      a_BCS_UpdateCharData[length++] = BCS_APP_Context.MeasurementChar.TimeStamp.Day;
      a_BCS_UpdateCharData[length++] = BCS_APP_Context.MeasurementChar.TimeStamp.Hours;
      a_BCS_UpdateCharData[length++] = BCS_APP_Context.MeasurementChar.TimeStamp.Minutes;
      a_BCS_UpdateCharData[length++] = BCS_APP_Context.MeasurementChar.TimeStamp.Seconds;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MULTIPLE_USERS) == BCS_FEATURE_SUPPORTED_MULTIPLE_USERS)
    {
      a_BCS_UpdateCharData[length++] = BCS_APP_Context.MeasurementChar.UserID;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_BASAL_METABOLISM) == BCS_FEATURE_SUPPORTED_BASAL_METABOLISM)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.BasalMetabolism) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.BasalMetabolism) >> 8) & 0xFF;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MUSCLE_PERCENTAGE) == BCS_FEATURE_SUPPORTED_MUSCLE_PERCENTAGE)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.MusclePercentage) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.MusclePercentage) >> 8) & 0xFF;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_MUSCLE_MASS) == BCS_FEATURE_SUPPORTED_MUSCLE_MASS)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.MuscleMass) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.MuscleMass) >> 8) & 0xFF;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_FAT_FREE_MASS) == BCS_FEATURE_SUPPORTED_FAT_FREE_MASS)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.FatFreeMass) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.FatFreeMass) >> 8) & 0xFF;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_SOFT_LEAN_MASS) == BCS_FEATURE_SUPPORTED_SOFT_LEAN_MASS)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.SoftLeanMass) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.SoftLeanMass) >> 8) & 0xFF;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_BODY_WATER_MASS) == BCS_FEATURE_SUPPORTED_BODY_WATER_MASS)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.BodyWaterMass) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.BodyWaterMass) >> 8) & 0xFF;
    }
    
    if(((BCS_APP_Context.FeatureChar.Value) & BCS_FEATURE_SUPPORTED_IMPEDANCE) == BCS_FEATURE_SUPPORTED_IMPEDANCE)
    {
      a_BCS_UpdateCharData[length++] = (BCS_APP_Context.MeasurementChar.Impedance) & 0xFF;
      a_BCS_UpdateCharData[length++] = ((BCS_APP_Context.MeasurementChar.Impedance) >> 8) & 0xFF;
    }
      
    msg_conf.Length = length;
    msg_conf.p_Payload = a_BCS_UpdateCharData;
    BCS_UpdateValue(BCS_BCM, &msg_conf);
  }
  
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
