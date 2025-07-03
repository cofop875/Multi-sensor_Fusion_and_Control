/*
 * rm3100.h
 *
 * Created on: Apr 28, 2025
 * Author: Archer Liu
 */

 #ifndef INC_RM3100_H_
 #define INC_RM3100_H_
 
 #include "stm32h7xx_hal.h"
 
 
 #define RM3100_I2C_ADDR         (0x21 << 1)
 
 #define RM3100_POLL_REG         0x00
 #define RM3100_CMM_REG          0x01
 #define RM3100_CCX_REG_L        0x04
 #define RM3100_CCX_REG_H        0x05
 #define RM3100_CCY_REG_L        0x06
 #define RM3100_CCY_REG_H        0x07
 #define RM3100_CCZ_REG_L        0x08
 #define RM3100_CCZ_REG_H        0x09
 #define RM3100_TMRC_REG         0x0B
 #define RM3100_MEAS_X_MSB_REG   0x24
 #define RM3100_MEAS_X_MID_REG   0x25
 #define RM3100_MEAS_X_LSB_REG   0x26
 #define RM3100_MEAS_Y_MSB_REG   0x27
 #define RM3100_MEAS_Y_MID_REG   0x28
 #define RM3100_MEAS_Y_LSB_REG   0x29
 #define RM3100_MEAS_Z_MSB_REG   0x2A
 #define RM3100_MEAS_Z_MID_REG   0x2B
 #define RM3100_MEAS_Z_LSB_REG   0x2C
 #define RM3100_BIST_REG         0x33
 #define RM3100_STATUS_REG       0x34
 #define RM3100_REVID_REG        0x36
 
 
 #define RM3100_STATUS_DRDY_MASK 0x80
 
 
 #define RM3100_POLL_XYZ_CMD     0x70
 
 
 typedef struct {
     int32_t x;
     int32_t y;
     int32_t z;
 } RM3100_Data;
 
 
 HAL_StatusTypeDef RM3100_Init(I2C_HandleTypeDef *hi2c);
 HAL_StatusTypeDef RM3100_ReadID(uint8_t *revid);
 HAL_StatusTypeDef RM3100_TriggerMeasurement(void);
 HAL_StatusTypeDef RM3100_IsDataReady(uint8_t *isReady);
 HAL_StatusTypeDef RM3100_ReadData(RM3100_Data *data);
 void RM3100_Main(I2C_HandleTypeDef *hi2c, RM3100_Data *magData);
 
 
 #endif /* INC_RM3100_H_ */
