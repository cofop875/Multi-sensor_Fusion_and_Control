/*
 * BMI088.h
 *
 *  Created on:
 *      Author: [Your Name]
 */

#ifndef INC_BMI088_H_
#define INC_BMI088_H_

#include "stm32h7xx_hal.h"
#include <stdio.h>

/* 加速度計暫存器位址定義 */
#define BMI088_ACC_CHIP_ID_ADDR         0x00
#define BMI088_ACC_ERR_REG_ADDR         0x02
#define BMI088_ACC_STATUS_ADDR          0x03
#define BMI088_ACC_X_LSB_ADDR           0x12
#define BMI088_ACC_CONF_ADDR            0x40
#define BMI088_ACC_RANGE_ADDR           0x41
#define BMI088_ACC_PWR_CONF_ADDR        0x7C
#define BMI088_ACC_PWR_CTRL_ADDR        0x7D
#define BMI088_ACC_SOFTRESET_ADDR       0x7E

/* 陀螺儀暫存器位址定義 */
#define BMI088_GYRO_CHIP_ID_ADDR        0x00
#define BMI088_GYRO_RATE_X_LSB_ADDR     0x02
#define BMI088_GYRO_RANGE_ADDR          0x0F
#define BMI088_GYRO_BANDWIDTH_ADDR      0x10
#define BMI088_GYRO_LPM1_ADDR           0x11
#define BMI088_GYRO_SOFTRESET_ADDR      0x14
#define BMI088_GYRO_INT_CTRL_ADDR       0x15

/* 晶片 ID */
#define BMI088_ACC_CHIP_ID              0x1E
#define BMI088_GYRO_CHIP_ID             0x0F

/* 結構體用於儲存感測器數據 */
typedef struct {

    int16_t raw_accel[3]; // 0:X, 1:Y, 2:Z X, Y, Z 軸加速度原始數據
    int16_t raw_gyro[3];  // 0:X, 1:Y, 2:Z X, Y, Z 軸角速度原始數據
    float accel_g[3];     // 0:X, 1:Y, 2:Z 轉換為 g 單位的加速度數據
    float gyro_dps[3];    // 0:X, 1:Y, 2:Z 轉換為 dps (度/秒) 單位的角速度數據
} BMI088_Data;

/* 函數原型宣告 */

/**
  * @brief  初始化 BMI088 加速度計與陀螺儀
  * @param  None
  * @retval 0: 成功, -1: 加速度計錯誤, -2: 陀螺儀錯誤
  */
uint8_t BMI088_Init(void);

/**
  * @brief  主處理函數，讀取 BMI088 數據並透過 printf 輸出
  * @param  None
  * @retval None
  */
HAL_StatusTypeDef BMI088_Main(void);


#endif /* INC_BMI088_H_ */
