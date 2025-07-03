/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ICP20100_SDA_Pin GPIO_PIN_0
#define ICP20100_SDA_GPIO_Port GPIOF
#define ICP20100_SCL_Pin GPIO_PIN_1
#define ICP20100_SCL_GPIO_Port GPIOF
#define ICM20948_DRDY_Pin GPIO_PIN_2
#define ICM20948_DRDY_GPIO_Port GPIOF
#define SPI2_MISO_ICM_Pin GPIO_PIN_2
#define SPI2_MISO_ICM_GPIO_Port GPIOC
#define SPI2_MOSI_ICM_Pin GPIO_PIN_3
#define SPI2_MOSI_ICM_GPIO_Port GPIOC
#define ICM20948_SCK_Pin GPIO_PIN_5
#define ICM20948_SCK_GPIO_Port GPIOA
#define SPI3_MOSI_SENSOR3_BMI088_Pin GPIO_PIN_2
#define SPI3_MOSI_SENSOR3_BMI088_GPIO_Port GPIOB
#define SPI3_DRDY1_BMI088_INT1_ACCEL_Pin GPIO_PIN_7
#define SPI3_DRDY1_BMI088_INT1_ACCEL_GPIO_Port GPIOE
#define SPI3_DRDY2_BMI088_INT3_GYRO_Pin GPIO_PIN_8
#define SPI3_DRDY2_BMI088_INT3_GYRO_GPIO_Port GPIOE
#define SPI2_NCS1_ICM42688P_Pin GPIO_PIN_11
#define SPI2_NCS1_ICM42688P_GPIO_Port GPIOB
#define SPI3_nCS1_BMI088_ACCEL_Pin GPIO_PIN_13
#define SPI3_nCS1_BMI088_ACCEL_GPIO_Port GPIOD
#define ICM20948_CS_Pin GPIO_PIN_14
#define ICM20948_CS_GPIO_Port GPIOD
#define SPI3_nCS2_BMI088_GYRO_Pin GPIO_PIN_15
#define SPI3_nCS2_BMI088_GYRO_GPIO_Port GPIOD
#define ICP20100_DRDY_Pin GPIO_PIN_5
#define ICP20100_DRDY_GPIO_Port GPIOG
#define SPI2_SCK_ICM_Pin GPIO_PIN_9
#define SPI2_SCK_ICM_GPIO_Port GPIOA
#define DRDY_INT2_ICM42688P_Pin GPIO_PIN_10
#define DRDY_INT2_ICM42688P_GPIO_Port GPIOA
#define SPI3_SCK_SENSOR3_BMI088_Pin GPIO_PIN_10
#define SPI3_SCK_SENSOR3_BMI088_GPIO_Port GPIOC
#define SPI3_MISO_SENSOR3_BMI088_Pin GPIO_PIN_11
#define SPI3_MISO_SENSOR3_BMI088_GPIO_Port GPIOC
#define ICM20948_MISO_Pin GPIO_PIN_9
#define ICM20948_MISO_GPIO_Port GPIOG
#define ICM20948_MOSI_Pin GPIO_PIN_5
#define ICM20948_MOSI_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
