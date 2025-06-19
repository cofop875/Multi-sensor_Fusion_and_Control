/*
 * icm42688p.h
 *
 *  Created on: May 26, 2025
 *      Author: kloud
 */

#ifndef ICM42688P_H
#define ICM42688P_H

#include "stm32h7xx_hal.h" // 引用 STM32 HAL 庫

// SPI 通訊超時時間 (ms)
#define ICM42688P_SPI_TIMEOUT 100

// ICM-42688-P WHO_AM_I 暫存器預期值
#define ICM42688P_WHO_AM_I_VALUE 0x47 // Datasheet DS-000347, page 71

// 暫存器庫選擇 (Register Bank Select)
#define ICM42688P_REG_BANK_SEL        0x76 // 用於切換暫存器庫

// --- User Bank 0 Registers ---
// 配置暫存器
#define ICM42688P_REG_DEVICE_CONFIG   0x11 // 設備配置 (SPI 模式等), Datasheet page 56
#define ICM42688P_REG_INT_CONFIG      0x14 // 中斷引腳配置 (INT1/INT2 極性、驅動方式、模式), Datasheet page 58
#define ICM42688P_REG_FIFO_CONFIG     0x16 // FIFO 配置 (未使用於此範例)

// 感測器識別與電源管理
#define ICM42688P_REG_WHO_AM_I        0x75 // WHO_AM_I 識別暫存器 (唯讀)
#define ICM42688P_REG_PWR_MGMT0       0x4E // 電源管理暫存器 0 (啟用/禁用感測器, 設定模式), Datasheet page 69
                                           // Bit [3:2] GYRO_MODE: 00=Off, 01=Standby, 11=Low Noise (LN)
                                           // Bit [1:0] ACCEL_MODE: 00=Off, 01=Low Power (LP), 11=Low Noise (LN)
#define ICM42688P_PWR_MGMT0_ACCEL_LN  (0x03 << 0) // 加速計低噪音模式
#define ICM42688P_PWR_MGMT0_GYRO_LN   (0x03 << 2) // 陀螺儀低噪音模式
#define ICM42688P_PWR_MGMT0_TEMP_DIS  (0x01 << 5) // 禁用溫度感測器 (預設啟用)

// 感測器配置
#define ICM42688P_REG_GYRO_CONFIG0    0x4F // 陀螺儀配置暫存器 0 (ODR, FS_SEL), Datasheet page 70
                                           // Bit [7:4] GYRO_UI_FS_SEL: 0000=±2000dps, 0001=±1000dps,..., 0111=±15.625dps
                                           // Bit [3:0] GYRO_ODR: 0001=32kHz (LN only),..., 0110=1kHz,..., 1111=ODR disabled
#define ICM42688P_REG_ACCEL_CONFIG0   0x50 // 加速計配置暫存器 0 (ODR, FS_SEL), Datasheet page 70
                                           // Bit [7:4] ACCEL_UI_FS_SEL: 0000=±16g, 0001=±8g, 0010=±4g, 0011=±2g
                                           // Bit [3:0] ACCEL_ODR: (同 GYRO_ODR)
#define ICM42688P_GYRO_FS_SEL_2000DPS (0x00 << 4) // 陀螺儀滿量程 ±2000 dps
#define ICM42688P_GYRO_FS_SEL_1000DPS (0x01 << 4)
#define ICM42688P_GYRO_FS_SEL_500DPS  (0x02 << 4)
#define ICM42688P_GYRO_FS_SEL_250DPS  (0x03 << 4)
#define ICM42688P_GYRO_FS_SEL_125DPS  (0x04 << 4)

#define ICM42688P_ACCEL_FS_SEL_16G    (0x00 << 4) // 加速計滿量程 ±16g
#define ICM42688P_ACCEL_FS_SEL_8G     (0x01 << 4)
#define ICM42688P_ACCEL_FS_SEL_4G     (0x02 << 4)
#define ICM42688P_ACCEL_FS_SEL_2G     (0x03 << 4)

#define ICM42688P_ODR_1KHZ            0x06        // 輸出數據速率 1kHz
#define ICM42688P_ODR_200HZ           0x08        // 輸出數據速率 200Hz
#define ICM42688P_ODR_50HZ            0x0B        // 輸出數據速率 50Hz

// 中斷狀態與數據暫存器
#define ICM42688P_REG_INT_STATUS_DRDY 0x2D // 中斷狀態 - 資料就緒 (ACCEL_DATA_RDY, GYRO_DATA_RDY), Datasheet page 65
                                           // Bit 0: ACCEL_DATA_RDY_INT
                                           // Bit 1: GYRO_DATA_RDY_INT
#define ICM42688P_REG_ACCEL_DATA_X1_UI 0x1F // 加速計 X 軸高位元組 (MSB)
#define ICM42688P_REG_ACCEL_DATA_X0_UI 0x20 // 加速計 X 軸低位元組 (LSB)
#define ICM42688P_REG_ACCEL_DATA_Y1_UI 0x21 // 加速計 Y 軸高位元組
#define ICM42688P_REG_ACCEL_DATA_Y0_UI 0x22 // 加速計 Y 軸低位元組
#define ICM42688P_REG_ACCEL_DATA_Z1_UI 0x23 // 加速計 Z 軸高位元組
#define ICM42688P_REG_ACCEL_DATA_Z0_UI 0x24 // 加速計 Z 軸低位元組
#define ICM42688P_REG_GYRO_DATA_X1_UI  0x25 // 陀螺儀 X 軸高位元組
#define ICM42688P_REG_GYRO_DATA_X0_UI  0x26 // 陀螺儀 X 軸低位元組
#define ICM42688P_REG_GYRO_DATA_Y1_UI  0x27 // 陀螺儀 Y 軸高位元組
#define ICM42688P_REG_GYRO_DATA_Y0_UI  0x28 // 陀螺儀 Y 軸低位元組
#define ICM42688P_REG_GYRO_DATA_Z1_UI  0x29 // 陀螺儀 Z 軸高位元組
#define ICM42688P_REG_GYRO_DATA_Z0_UI  0x2A // 陀螺儀 Z 軸低位元組

#define ICM42688P_REG_INT_CONFIG1     0x64 // 中斷配置1 (INT_ASYNC_RESET_BIT 位於此), Datasheet page 73
#define ICM42688P_INT_ASYNC_RESET_BIT (1 << 4) // INT_CONFIG1 中的 INT_ASYNC_RESET 位

// 函數原型宣告
uint8_t icm42688p_init(SPI_HandleTypeDef *hspi); // 初始化 ICM-42688-P
uint8_t icm42688p_read_who_am_i(SPI_HandleTypeDef *hspi); // 讀取 WHO_AM_I 暫存器
void icm42688p_read_gyro_raw(SPI_HandleTypeDef *hspi, int16_t* gyro_data); // 讀取陀螺儀原始數據 (3軸)
void icm42688p_read_accel_raw(SPI_HandleTypeDef *hspi, int16_t* accel_data); // 讀取加速計原始數據 (3軸)
// <<< 新增的主處理函數原型 >>>
void ICM42688P_Main(SPI_HandleTypeDef *hspi); // <<<<<<<<<<<<<<< 新增這一行
// (可選) 若要暴露底層讀寫函數，可取消以下註解
// void icm42688p_write_register(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value); // 寫入單個暫存器
// uint8_t icm42688p_read_register(SPI_HandleTypeDef *hspi, uint8_t reg); // 讀取單個暫存器
// void icm42688p_read_multiple_registers(SPI_HandleTypeDef *hspi, uint8_t start_reg, uint8_t *buffer, uint8_t len); // 連續讀取多個暫存器

#endif /* ICM42688P_H */
