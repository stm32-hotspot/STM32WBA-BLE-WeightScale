/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service4_app.h
  * @author  MCD Application Team
  * @brief   Header for service4_app.c
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
#ifndef BCS_APP_H
#define BCS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BCS_CONN_HANDLE_EVT,
  BCS_DISCON_HANDLE_EVT,

  /* USER CODE BEGIN Service3_OpcodeNotificationEvt_t */
  BCS_FORCE_MEAS_EVT,
  /* USER CODE END Service3_OpcodeNotificationEvt_t */

  BCS_LAST_EVT,
} BCS_APP_OpcodeNotificationEvt_t;

typedef struct
{
  BCS_APP_OpcodeNotificationEvt_t          EvtOpcode;
  uint16_t                                 ConnectionHandle;

  /* USER CODE BEGIN BCS_APP_ConnHandleNotEvt_t */
  uint8_t PairingComplete;
  /* USER CODE END BCS_APP_ConnHandleNotEvt_t */
} BCS_APP_ConnHandleNotEvt_t;
/* USER CODE BEGIN ET */

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
void BCS_APP_Init(void);
void BCS_APP_EvtRx(BCS_APP_ConnHandleNotEvt_t *p_Notification);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*BCS_APP_H */
