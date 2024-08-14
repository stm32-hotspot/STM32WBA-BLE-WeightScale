/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service2_app.c
  * @author  MCD Application Team
  * @brief   service2_app application definition.
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
#include "wss_app.h"
#include "wss.h"
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
  Mnbs_INDICATION_OFF,
  Mnbs_INDICATION_ON,
  /* USER CODE BEGIN Service2_APP_SendInformation_t */

  /* USER CODE END Service2_APP_SendInformation_t */
  WSS_APP_SENDINFORMATION_LAST
} WSS_APP_SendInformation_t;

typedef struct
{
  WSS_APP_SendInformation_t     Mnbs_Indication_Status;
  /* USER CODE BEGIN Service2_APP_Context_t */
  WSS_MeasurementValue_t MeasurementChar;
  WSS_FeatureValue_t FeatureChar;
  
  uint8_t Indication_Status;
  UTIL_TIMER_Object_t TimerMeasurement_Id;
  uint32_t StartTick;
  uint8_t Paired;
  /* USER CODE END Service2_APP_Context_t */
  uint16_t              ConnectionHandle;
} WSS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEFAULT_WEIGHT_IN_KG                                             70.500f
#define DEFAULT_HEIGHT_IN_METERS                                           1.72f
#define DEFAULT_WSS_TIME_STAMP_YEAR                                         2022
#define DEFAULT_WSS_TIME_STAMP_MONTH                                           7
#define DEFAULT_WSS_TIME_STAMP_DAY                                             5
#define DEFAULT_WSS_TIME_STAMP_HOURS                                           0
#define DEFAULT_WSS_TIME_STAMP_MINUTES                                         0
#define DEFAULT_WSS_TIME_STAMP_SECONDS                                         0
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define DEFAULT_ONE_SECOND_INTERVAL (1000) /**< 1s */
#define WSS_MEASUREMENT_INTERVAL   (DEFAULT_ONE_SECOND_INTERVAL*4)  /**< 4s */

/* Field Weight of Weight Scale Measurement */
#define POUNDS_TO_KILOGRAMS        0.4536
#define INCHES_TO_METERS           0.3048
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static WSS_APP_Context_t WSS_APP_Context;

uint8_t a_WSS_UpdateCharData[247];

/* USER CODE BEGIN PV */
static WSS_MeasurementValue_t a_WSS_Measurement[100];
static int8_t storeIndex;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void WSS_Mnbs_SendIndication(void);

/* USER CODE BEGIN PFP */
static void WSS_APP_Measurement_timCB( void *arg );
/* Weight */
static uint8_t WSS_APP_FeatureField_WMR(void);
static Measurement_Unit_t WSS_APP_MeasurementUnits_Weight(void);
static float WSS_APP_MeasurementResolution_Weight(uint8_t feature, Measurement_Unit_t unit);
static uint16_t WSS_APP_Convert_Weight(Measurement_Unit_t unit_from, Measurement_Unit_t unit_to, float weight);
/* Height */
static uint8_t WSS_APP_FeatureField_HMR(void);
static Measurement_Unit_t WSS_APP_MeasurementUnits_Height(void);
static float WSS_APP_MeasurementResolution_Height(uint8_t feature, Measurement_Unit_t unit);
static uint16_t WSS_APP_Convert_Height(Measurement_Unit_t unit_from, Measurement_Unit_t unit_to, float height);
/* BMI */
static uint16_t WSS_APP_Calculate_BMI(float weight, float height);
/* Measurement */
static void WSS_APP_Measurement(void);
static void WSS_APP_Store(void);
static void WSS_APP_Suppress(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void WSS_Notification(WSS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_Notification_1 */

  /* USER CODE END Service2_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_Notification_Service2_EvtOpcode */

    /* USER CODE END Service2_Notification_Service2_EvtOpcode */

    case WSS_WSF_READ_EVT:
      /* USER CODE BEGIN Service2Char1_READ_EVT */

      /* USER CODE END Service2Char1_READ_EVT */
      break;

    case WSS_MNBS_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service2Char2_INDICATE_ENABLED_EVT */
      if(WSS_APP_Context.Paired == TRUE)
      {
        WSS_APP_Context.Indication_Status = 1;
      }
      /* USER CODE END Service2Char2_INDICATE_ENABLED_EVT */
      break;

    case WSS_MNBS_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service2Char2_INDICATE_DISABLED_EVT */
      if(WSS_APP_Context.Paired == TRUE)
      {
        WSS_APP_Context.Indication_Status = 0;
      }
      /* USER CODE END Service2Char2_INDICATE_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_Notification_default */

      /* USER CODE END Service2_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service2_Notification_2 */

  /* USER CODE END Service2_Notification_2 */
  return;
}

void WSS_APP_EvtRx(WSS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service2_APP_EvtRx_1 */

  /* USER CODE END Service2_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service2_APP_EvtRx_Service2_EvtOpcode */
    case WSS_FORCE_MEAS_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */
      /* WSP PTS test when notification is requested */
      WSS_APP_Context.Indication_Status = 1;
      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;
    /* USER CODE END Service2_APP_EvtRx_Service2_EvtOpcode */
    case WSS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_CONN_HANDLE_EVT */
      WSS_APP_Context.Paired = p_Notification->PairingComplete;
      /* USER CODE END Service2_APP_CONN_HANDLE_EVT */
      break;

    case WSS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service2_APP_DISCON_HANDLE_EVT */
      WSS_APP_Context.Indication_Status = 0;
      /* USER CODE END Service2_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service2_APP_EvtRx_default */

      /* USER CODE END Service2_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service2_APP_EvtRx_2 */

  /* USER CODE END Service2_APP_EvtRx_2 */

  return;
}

void WSS_APP_Init(void)
{
  UNUSED(WSS_APP_Context);
  WSS_Init();

  /* USER CODE BEGIN Service2_APP_Init */
  WSS_Data_t msg_conf;
  uint8_t length = 0;

  APP_DBG_MSG("WSS_APP_Init\n\r");
  
  /*
   * No Indication by default
   */
  WSS_APP_Context.Indication_Status = 0;
  
  /*
   * Ticks
   */
  WSS_APP_Context.StartTick = HAL_GetTick();
  
  /**
   * Initialize Weight Scale Features
   */
  WSS_APP_Context.FeatureChar.Value = (WSS_FEATURE_BMI | 
                                       WSS_FEATURE_RESOLUTION_0_5KG |
                                       WSS_FEATURE_RESOLUTION_0_01m |
                                       WSS_FEATURE_TIME_STAMP);

  a_WSS_UpdateCharData[length++] = (WSS_APP_Context.FeatureChar.Value) & 0xFF;
  a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.FeatureChar.Value) >> 8) & 0xFF;
  a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.FeatureChar.Value) >> 16) & 0xFF;
  a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.FeatureChar.Value) >> 24) & 0xFF;
  msg_conf.Length = length;
  msg_conf.p_Payload = a_WSS_UpdateCharData;
  WSS_UpdateValue(WSS_WSF, &msg_conf);

  /**
   * Initialize Fields of WSS Measurement
   */
  /* Weight Scale Measurement value unit */
  WSS_APP_Context.MeasurementChar.Flags             = WSS_NO_FLAGS; /* unit as SI */
  
  /* Weight */
  WSS_APP_Context.MeasurementChar.Weight            = 0;
  
  /* Add support for Time Stamp */
  WSS_APP_Context.MeasurementChar.Flags            |= WSS_FLAGS_TIME_STAMP_PRESENT;
  WSS_APP_Context.MeasurementChar.TimeStamp.Year    = DEFAULT_WSS_TIME_STAMP_YEAR;
  WSS_APP_Context.MeasurementChar.TimeStamp.Month   = DEFAULT_WSS_TIME_STAMP_MONTH;
  WSS_APP_Context.MeasurementChar.TimeStamp.Day     = DEFAULT_WSS_TIME_STAMP_DAY;
  WSS_APP_Context.MeasurementChar.TimeStamp.Hours   = DEFAULT_WSS_TIME_STAMP_HOURS;
  WSS_APP_Context.MeasurementChar.TimeStamp.Minutes = DEFAULT_WSS_TIME_STAMP_MINUTES;
  WSS_APP_Context.MeasurementChar.TimeStamp.Seconds = DEFAULT_WSS_TIME_STAMP_SECONDS;
  
  /* Add support for BMI & Height pair */
  WSS_APP_Context.MeasurementChar.Flags            |= WSS_FLAGS_BMI_AND_HEIGHT_PRESENT;
  WSS_APP_Context.MeasurementChar.BMI               = 0;
  WSS_APP_Context.MeasurementChar.Height            = 0;
  
  /*
   * Create timer for Weight Scale Measurement
   */
  UTIL_TIMER_Create(&(WSS_APP_Context.TimerMeasurement_Id),
                    WSS_MEASUREMENT_INTERVAL,
                    UTIL_TIMER_PERIODIC,
                    &WSS_APP_Measurement_timCB, 0);
  
  /*
   * Register task for Weight Scale Measurment
   */
  UTIL_SEQ_RegTask( 1<< CFG_TASK_WSS_MEAS_REQ_ID, UTIL_SEQ_RFU, WSS_APP_Measurement );
  
  /* Start Measurement */
  UTIL_TIMER_StartWithPeriod(&(WSS_APP_Context.TimerMeasurement_Id), WSS_MEASUREMENT_INTERVAL);

  /* USER CODE END Service2_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void WSS_Mnbs_SendIndication(void) /* Property Indication */
{
  WSS_APP_SendInformation_t indication_on_off = Mnbs_INDICATION_OFF;
  WSS_Data_t wss_indication_data;

  wss_indication_data.p_Payload = (uint8_t*)a_WSS_UpdateCharData;
  wss_indication_data.Length = 0;

  /* USER CODE BEGIN Service2Char2_IS_1*/

  /* USER CODE END Service2Char2_IS_1*/

  if (indication_on_off != Mnbs_INDICATION_OFF)
  {
    WSS_UpdateValue(WSS_MNBS, &wss_indication_data);
  }

  /* USER CODE BEGIN Service2Char2_IS_Last*/

  /* USER CODE END Service2Char2_IS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void WSS_APP_Measurement_timCB( void *arg )
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask( 1<<CFG_TASK_WSS_MEAS_REQ_ID, CFG_SEQ_PRIO_0);

  return;
}

/*
 * Get Weight Measurement Resolution from Weight Scale Feature field
 */
static uint8_t WSS_APP_FeatureField_WMR(void)
{
  return ((WSS_APP_Context.FeatureChar.Value >> 3) & 0x0F);
}

/*
 * Get Measurement Units from Flags of Weight Scale Measurement
 */
static Measurement_Unit_t WSS_APP_MeasurementUnits_Weight(void)
{
  return (Measurement_Unit_t)(WSS_APP_Context.MeasurementChar.Flags & 1);
}

/*
 * Weight Measurement Resolution from Weight Scale Feature field
 * Return value is the resolution for kilograms (kg) or resolution for pounds
 */
static float WSS_APP_MeasurementResolution_Weight(uint8_t feature, Measurement_Unit_t unit)
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
static uint16_t WSS_APP_Convert_Weight(Measurement_Unit_t unit_from, Measurement_Unit_t unit_to, float weight)
{
  float resolution = WSS_APP_MeasurementResolution_Weight(WSS_APP_FeatureField_WMR(), WSS_APP_MeasurementUnits_Weight());
  
  APP_DBG_MSG("Convert Weight %f with resolution = %f, unit %d -> %d\n\r", weight, resolution, unit_from, unit_to);
  
  if((unit_from == MeasurementUnits_SI) && (unit_to == MeasurementUnits_Imperial))
  {
    /* kilograms (kg) -> pounds */
    weight /= POUNDS_TO_KILOGRAMS;
  } 
  else if ((unit_from == MeasurementUnits_Imperial) && (unit_to == MeasurementUnits_SI))
  {
    /* pounds -> kilograms (kg) */
    weight *= POUNDS_TO_KILOGRAMS;
  }
  
  weight /= resolution;

  return (uint16_t)weight;
}


/*
 * Get Weight Measurement Resolution from Height Scale Feature field
 */
static uint8_t WSS_APP_FeatureField_HMR(void)
{
  return ((WSS_APP_Context.FeatureChar.Value >> 7) & 0x07);
}


/*
 * Get Measurement Units from Flags of Weight Scale Measurement
 */
static Measurement_Unit_t WSS_APP_MeasurementUnits_Height(void)
{
  return (Measurement_Unit_t)(WSS_APP_Context.MeasurementChar.Flags & 1);
}


/*
 * Height Measurement Resolution from Weight Scale Feature field
 * Return value is the resolution for meters or resolution for inches
 */
static float WSS_APP_MeasurementResolution_Height(uint8_t feature, Measurement_Unit_t unit)
{
  float resolution = 1.0f; /* the default is Not specified */
  
  switch (feature)
  {
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
static uint16_t WSS_APP_Convert_Height(Measurement_Unit_t unit_from, Measurement_Unit_t unit_to, float height)
{
  float resolution = WSS_APP_MeasurementResolution_Height(WSS_APP_FeatureField_HMR(), WSS_APP_MeasurementUnits_Height());
  
  APP_DBG_MSG("Convert Height %f with resolution = %f, unit %d -> %d\n\r", height, resolution, unit_from, unit_to);
  
  if((unit_from == MeasurementUnits_SI) && (unit_to == MeasurementUnits_Imperial))
  {
    /* meters -> inches */
    height /= INCHES_TO_METERS;
  } 
  else if ((unit_from == MeasurementUnits_Imperial) && (unit_to == MeasurementUnits_SI))
  {
    /* inches -> meters */
    height *= INCHES_TO_METERS;
  }
  
  height /= resolution;
  
  return (uint16_t)height;
}


/*
 * Calcular: BMI = Weight / (Height ^ 2) with Unit is 0.1 kg/m2
 */
static uint16_t WSS_APP_Calculate_BMI(float weight, float height)
{
  float resolution = 0.1f;
  float value = weight / (height * height);
  uint32_t v;
  
  value /= resolution;
  v = (uint32_t)(value * 10);
  return (uint16_t)((v + 5) / 10);
}


static void WSS_APP_Measurement(void)
{
  /* Weight, BMI,  Height Initialization */
  
  /* E.g. weight = 0x36B0 for 70kg with resolution = 0.0005 */
  float weight_si = (float)(DEFAULT_WEIGHT_IN_KG /*+ (rand() % 10)*/);
  uint16_t weight = WSS_APP_Convert_Weight(MeasurementUnits_SI,
                                           ((WSS_APP_Context.MeasurementChar.Flags & WSS_FLAGS_VALUE_UNIT_IMPERIAL) ? MeasurementUnits_Imperial : MeasurementUnits_SI),
                                           weight_si);
  float height_si = (float)(DEFAULT_HEIGHT_IN_METERS /*+ (rand() % 10)*/);
  uint16_t height = WSS_APP_Convert_Height(MeasurementUnits_SI,
                                           ((WSS_APP_Context.MeasurementChar.Flags & WSS_FLAGS_VALUE_UNIT_IMPERIAL) ? MeasurementUnits_Imperial : MeasurementUnits_SI),
                                           height_si);
  uint16_t BMI = WSS_APP_Calculate_BMI(weight_si, height_si);
  uint32_t ticks = (HAL_GetTick() - WSS_APP_Context.StartTick) / 1000;
  
  APP_DBG_MSG("weight_si = %f -> weight = %02X, height_si = %f -> height = %02X\n\r", weight_si, weight, height_si, height);

  /* update Weight */
  WSS_APP_Context.MeasurementChar.Weight = weight;
  
  if((WSS_APP_Context.MeasurementChar.Flags) & WSS_FLAGS_BMI_AND_HEIGHT_PRESENT)
  {
    /* update Height */
    WSS_APP_Context.MeasurementChar.Height = height;
    
    /* update BMI */
    WSS_APP_Context.MeasurementChar.BMI    = BMI;
  }
  
  if((WSS_APP_Context.MeasurementChar.Flags) & WSS_FLAGS_TIME_STAMP_PRESENT)
  {
    /* update Time Stamp */
    WSS_APP_Context.MeasurementChar.TimeStamp.Year    = DEFAULT_WSS_TIME_STAMP_YEAR;
    WSS_APP_Context.MeasurementChar.TimeStamp.Month   = DEFAULT_WSS_TIME_STAMP_MONTH;
    WSS_APP_Context.MeasurementChar.TimeStamp.Day     = DEFAULT_WSS_TIME_STAMP_DAY;
    WSS_APP_Context.MeasurementChar.TimeStamp.Hours   = DEFAULT_WSS_TIME_STAMP_HOURS;
    WSS_APP_Context.MeasurementChar.TimeStamp.Minutes = DEFAULT_WSS_TIME_STAMP_MINUTES;
    WSS_APP_Context.MeasurementChar.TimeStamp.Seconds = DEFAULT_WSS_TIME_STAMP_SECONDS;
      
    while(ticks >= 60)
    {
      ticks -= 60;
      WSS_APP_Context.MeasurementChar.TimeStamp.Minutes += 1;
    }
    WSS_APP_Context.MeasurementChar.TimeStamp.Seconds = ticks;
    
    while(WSS_APP_Context.MeasurementChar.TimeStamp.Minutes >= 60)
    {
      WSS_APP_Context.MeasurementChar.TimeStamp.Minutes -= 60;
      WSS_APP_Context.MeasurementChar.TimeStamp.Hours += 1;
    }
    
    while(WSS_APP_Context.MeasurementChar.TimeStamp.Hours >= 24)
    {
      WSS_APP_Context.MeasurementChar.TimeStamp.Hours -= 24;
      WSS_APP_Context.MeasurementChar.TimeStamp.Day += 1;
    }
    
    /* let's handle date simply without permanment calendar */
    while(WSS_APP_Context.MeasurementChar.TimeStamp.Day >= 31)
    {
      WSS_APP_Context.MeasurementChar.TimeStamp.Day -= 31;
      WSS_APP_Context.MeasurementChar.TimeStamp.Month += 1;
    }
    
    while(WSS_APP_Context.MeasurementChar.TimeStamp.Month >= 12)
    {
      WSS_APP_Context.MeasurementChar.TimeStamp.Month -= 11;
      WSS_APP_Context.MeasurementChar.TimeStamp.Year += 1;
    }
  }
  
  if(WSS_APP_Context.Indication_Status)
  {
    if(storeIndex >= 0)
    {
      BLE_DBG_APP_MSG ("WSS_APP_Measurement(): send stored measurement %d\n", storeIndex);
      WSS_APP_Suppress();
      UTIL_TIMER_Stop(&(WSS_APP_Context.TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod(&(WSS_APP_Context.TimerMeasurement_Id), DEFAULT_ONE_SECOND_INTERVAL);
    }
    else
    {
      WSS_Data_t msg_conf;
      uint8_t length = 0;
      
      UTIL_TIMER_Stop(&(WSS_APP_Context.TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod(&(WSS_APP_Context.TimerMeasurement_Id), WSS_MEASUREMENT_INTERVAL);

      a_WSS_UpdateCharData[length++] = WSS_APP_Context.MeasurementChar.Flags;
      a_WSS_UpdateCharData[length++] = (WSS_APP_Context.MeasurementChar.Weight) & 0xFF;
      a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.MeasurementChar.Weight) >> 8) & 0xFF;
      if((WSS_APP_Context.MeasurementChar.Flags) & WSS_FLAGS_TIME_STAMP_PRESENT)
      {
        a_WSS_UpdateCharData[length++] = (WSS_APP_Context.MeasurementChar.TimeStamp.Year) & 0xFF;
        a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.MeasurementChar.TimeStamp.Year) >> 8) & 0xFF;
        a_WSS_UpdateCharData[length++] = WSS_APP_Context.MeasurementChar.TimeStamp.Month;
        a_WSS_UpdateCharData[length++] = WSS_APP_Context.MeasurementChar.TimeStamp.Day;
        a_WSS_UpdateCharData[length++] = WSS_APP_Context.MeasurementChar.TimeStamp.Hours;
        a_WSS_UpdateCharData[length++] = WSS_APP_Context.MeasurementChar.TimeStamp.Minutes;
        a_WSS_UpdateCharData[length++] = WSS_APP_Context.MeasurementChar.TimeStamp.Seconds;
      }
      if((WSS_APP_Context.MeasurementChar.Flags) & WSS_FLAGS_BMI_AND_HEIGHT_PRESENT)
      {
        a_WSS_UpdateCharData[length++] = (WSS_APP_Context.MeasurementChar.BMI) & 0xFF;
        a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.MeasurementChar.BMI) >> 8) & 0xFF;
        a_WSS_UpdateCharData[length++] = (WSS_APP_Context.MeasurementChar.Height) & 0xFF;
        a_WSS_UpdateCharData[length++] = ((WSS_APP_Context.MeasurementChar.Height) >> 8) & 0xFF;
      }
      msg_conf.Length = length;
      msg_conf.p_Payload = a_WSS_UpdateCharData;
      WSS_UpdateValue(WSS_MNBS, &msg_conf);
    }
  }
  else
    WSS_APP_Store();
}

static void WSS_APP_Store(void)
{
  memcpy(&a_WSS_Measurement[storeIndex++],
         &(WSS_APP_Context.MeasurementChar),
         sizeof(WSS_MeasurementValue_t));
  if(storeIndex == 100)
    storeIndex = 0;
}

static void WSS_APP_Suppress(void)
{
  uint8_t i;
  WSS_Data_t msg_conf;
  uint8_t length = 0;
  
  a_WSS_UpdateCharData[length++] = a_WSS_Measurement[0].Flags;
  a_WSS_UpdateCharData[length++] = (a_WSS_Measurement[0].Weight) & 0xFF;
  a_WSS_UpdateCharData[length++] = ((a_WSS_Measurement[0].Weight) >> 8) & 0xFF;
  if((WSS_APP_Context.MeasurementChar.Flags) & WSS_FLAGS_TIME_STAMP_PRESENT)
  {
    a_WSS_UpdateCharData[length++] = (a_WSS_Measurement[0].TimeStamp.Year) & 0xFF;
    a_WSS_UpdateCharData[length++] = ((a_WSS_Measurement[0].TimeStamp.Year) >> 8) & 0xFF;
    a_WSS_UpdateCharData[length++] = a_WSS_Measurement[0].TimeStamp.Month;
    a_WSS_UpdateCharData[length++] = a_WSS_Measurement[0].TimeStamp.Day;
    a_WSS_UpdateCharData[length++] = a_WSS_Measurement[0].TimeStamp.Hours;
    a_WSS_UpdateCharData[length++] = a_WSS_Measurement[0].TimeStamp.Minutes;
    a_WSS_UpdateCharData[length++] = a_WSS_Measurement[0].TimeStamp.Seconds;
  }
  if((WSS_APP_Context.MeasurementChar.Flags) & WSS_FLAGS_BMI_AND_HEIGHT_PRESENT)
  {
    a_WSS_UpdateCharData[length++] = (a_WSS_Measurement[0].BMI) & 0xFF;
    a_WSS_UpdateCharData[length++] = ((a_WSS_Measurement[0].BMI) >> 8) & 0xFF;
    a_WSS_UpdateCharData[length++] = (a_WSS_Measurement[0].Height) & 0xFF;
    a_WSS_UpdateCharData[length++] = ((a_WSS_Measurement[0].Height) >> 8) & 0xFF;
  }
  msg_conf.Length = length;
  msg_conf.p_Payload = a_WSS_UpdateCharData;
  WSS_UpdateValue(WSS_MNBS, &msg_conf);

  for(i = 1; i < storeIndex; i++)
  {
    memcpy(&a_WSS_Measurement[i-1],
           &a_WSS_Measurement[i],
           sizeof(WSS_MeasurementValue_t));
    memcpy(&a_WSS_Measurement[i-1],
           &a_WSS_Measurement[i],
           sizeof(WSS_MeasurementValue_t));
  }
  storeIndex--;
}
/* USER CODE END FD_LOCAL_FUNCTIONS*/
