/*
 * BMI088.c
 *
 *  Created on:
 *      Author: [Your Name]
 */

#include "BMI088.h"
#include "main.h" // 為了使用 CubeMX 生成的 GPIO 和 SPI 宏定義
#include "sensor_error_handler.h" //錯誤處理
// 外部 SPI3 控制代碼
extern SPI_HandleTypeDef hspi3;

// 感測器數據結構實例
BMI088_Data bmi088_data;

// --- 底層 SPI 通訊函式 ---

/**
  * @brief  選取加速度計晶片 (CS1 拉低)
  * @param  None
  * @retval None
  */
static void BMI088_Accel_CS_Select(void) {
    HAL_GPIO_WritePin(SPI3_nCS1_BMI088_ACCEL_GPIO_Port, SPI3_nCS1_BMI088_ACCEL_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  取消選取加速度計晶片 (CS1 拉高)
  * @param  None
  * @retval None
  */
static void BMI088_Accel_CS_Deselect(void) {
    HAL_GPIO_WritePin(SPI3_nCS1_BMI088_ACCEL_GPIO_Port, SPI3_nCS1_BMI088_ACCEL_Pin, GPIO_PIN_SET);
}

/**
  * @brief  選取陀螺儀晶片 (CS2 拉低)
  * @param  None
  * @retval None
  */
static void BMI088_Gyro_CS_Select(void) {
    HAL_GPIO_WritePin(SPI3_nCS2_BMI088_GYRO_GPIO_Port, SPI3_nCS2_BMI088_GYRO_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  取消選取陀螺儀晶片 (CS2 拉高)
  * @param  None
  * @retval None
  */
static void BMI088_Gyro_CS_Deselect(void) {
    HAL_GPIO_WritePin(SPI3_nCS2_BMI088_GYRO_GPIO_Port, SPI3_nCS2_BMI088_GYRO_Pin, GPIO_PIN_SET);
}

/**
  * @brief  向加速度計寫入單個位元組
  * @param  reg: 目標暫存器位址
  * @param  data: 要寫入的數據
  * @retval None
  */
static void BMI088_Accel_Write_Byte(uint8_t reg, uint8_t data) {
    BMI088_Accel_CS_Select();
    HAL_SPI_Transmit(&hspi3, &reg, 1, 100);
    HAL_SPI_Transmit(&hspi3, &data, 1, 100);
    BMI088_Accel_CS_Deselect();
}

/**
  * @brief  從加速度計讀取單個位元組
  * @param  reg: 目標暫存器位址
  * @retval 讀取到的數據
  */
static uint8_t BMI088_Accel_Read_Byte(uint8_t reg) {
    uint8_t read_buf;
    // SPI 讀取操作時，MSB 需要設為 1
    reg |= 0x80;
    BMI088_Accel_CS_Select();
    HAL_SPI_Transmit(&hspi3, &reg, 1, 100);
    HAL_SPI_Receive(&hspi3, &read_buf, 1, 100);
    BMI088_Accel_CS_Deselect();
    return read_buf;
}

/**
  * @brief  從加速度計連續讀取多個位元組
  * @param  reg: 起始暫存器位址
  * @param  data: 存放讀取數據的緩衝區
  * @param  len: 要讀取的位元組長度
  * @retval None
  */
static HAL_StatusTypeDef BMI088_Accel_Read_Multi(uint8_t reg, uint8_t* data, uint16_t len) {
    reg |= 0x80;
    HAL_StatusTypeDef SPI_Receive_status;
    BMI088_Accel_CS_Select();
    HAL_SPI_Transmit(&hspi3, &reg, 1, 100);
    SPI_Receive_status = HAL_SPI_Receive(&hspi3, data, len, 100);//儲存狀態以利錯誤處理
    BMI088_Accel_CS_Deselect();
    return SPI_Receive_status;
}

/**
  * @brief  向陀螺儀寫入單個位元組
  * @param  reg: 目標暫存器位址
  * @param  data: 要寫入的數據
  * @retval None
  */
static void BMI088_Gyro_Write_Byte(uint8_t reg, uint8_t data) {
    BMI088_Gyro_CS_Select();
    HAL_SPI_Transmit(&hspi3, &reg, 1, 100);
    HAL_SPI_Transmit(&hspi3, &data, 1, 100);
    BMI088_Gyro_CS_Deselect();
}

/**
  * @brief  從陀螺儀讀取單個位元組
  * @param  reg: 目標暫存器位址
  * @retval 讀取到的數據
  */
static uint8_t BMI088_Gyro_Read_Byte(uint8_t reg) {
    uint8_t read_buf;
    reg |= 0x80;
    BMI088_Gyro_CS_Select();
    HAL_SPI_Transmit(&hspi3, &reg, 1, 100);
    HAL_SPI_Receive(&hspi3, &read_buf, 1, 100);
    BMI088_Gyro_CS_Deselect();
    return read_buf;
}

/**
  * @brief  從陀螺儀連續讀取多個位元組
  * @param  reg: 起始暫存器位址
  * @param  data: 存放讀取數據的緩衝區
  * @param  len: 要讀取的位元組長度
  * @retval None
  */
static HAL_StatusTypeDef BMI088_Gyro_Read_Multi(uint8_t reg, uint8_t* data, uint16_t len) {
    reg |= 0x80;
    HAL_StatusTypeDef SPI_Receive_status;
    BMI088_Gyro_CS_Select();
    HAL_SPI_Transmit(&hspi3, &reg, 1, 100);
    SPI_Receive_status = HAL_SPI_Receive(&hspi3, data, len, 100);
    BMI088_Gyro_CS_Deselect();
    return SPI_Receive_status;
}

// --- 初始化與主處理函式 ---

/**
  * @brief  初始化 BMI088 加速度計與陀螺儀
  * @param  None
  * @retval 0: 成功, 1: 加速度計錯誤, 2: 陀螺儀錯誤
  */
uint8_t BMI088_Init(void) {
    uint8_t chip_id = 0;

    // --- 加速度計初始化 ---
    // 1. 檢查晶片 ID
    chip_id = BMI088_Accel_Read_Byte(BMI088_ACC_CHIP_ID_ADDR);
    if (chip_id!= BMI088_ACC_CHIP_ID) {
//        printf("Error: ACCEL Chip ID wrong! Get: 0x%02X\r\n", chip_id);
        return SENSOR_INVALID_ID;
    }
//    printf("ACCEL Chip ID OK: 0x%02X\r\n", chip_id);

    // 2. 軟復位
    BMI088_Accel_Write_Byte(BMI088_ACC_SOFTRESET_ADDR, 0xB6);
    HAL_Delay(100); // 等待復位完成， datasheet 建議至少 1ms，這裡給予更長的時間

    // 3. 電源控制，從 suspend 模式喚醒
    BMI088_Accel_Write_Byte(BMI088_ACC_PWR_CONF_ADDR, 0x00);
    HAL_Delay(50);
    // 啟用加速度計
    BMI088_Accel_Write_Byte(BMI088_ACC_PWR_CTRL_ADDR, 0x04);
    HAL_Delay(50);

    // 4. 配置量測參數
    // ODR=100Hz, Normal Bandwidth
    BMI088_Accel_Write_Byte(BMI088_ACC_CONF_ADDR, 0xA8);
    // Range=±3g
    BMI088_Accel_Write_Byte(BMI088_ACC_RANGE_ADDR, 0x00);

    // --- 陀螺儀初始化 ---
    // 1. 檢查晶片 ID
    chip_id = BMI088_Gyro_Read_Byte(BMI088_GYRO_CHIP_ID_ADDR);
    if (chip_id!= BMI088_GYRO_CHIP_ID) {
//        printf("Error: GYRO Chip ID wrong! Get: 0x%02X\r\n", chip_id);
        return SENSOR_INVALID_ID;
    }
//    printf("GYRO Chip ID OK: 0x%02X\r\n", chip_id);

    // 2. 軟復位
    BMI088_Gyro_Write_Byte(BMI088_GYRO_SOFTRESET_ADDR, 0xB6);
    HAL_Delay(100);

    // 3. 電源控制，從 suspend 模式喚醒到 normal 模式
    BMI088_Gyro_Write_Byte(BMI088_GYRO_LPM1_ADDR, 0x00);
    HAL_Delay(50);

    // 4. 配置量測參數
    // Range=±2000 dps
    BMI088_Gyro_Write_Byte(BMI088_GYRO_RANGE_ADDR, 0x00);
    // ODR=200Hz, Bandwidth=53Hz
    BMI088_Gyro_Write_Byte(BMI088_GYRO_BANDWIDTH_ADDR, 0x8A);

//    printf("BMI088 Init Success.\r\n");
    return SENSOR_OK;
}

/**
  * @brief  主處理函數，讀取 BMI088 數據並透過 printf 輸出
  * @param  None
  * @retval None
  */
/*
 * 假設您的 bmi088_data 結構體定義如下，才能分別儲存 X, Y, Z 軸的數據。
 * 如果您的結構體不是這樣定義的，請參考此結構進行修改。
 * * struct {
 * int16_t raw_accel[3]; // 0:X, 1:Y, 2:Z
 * int16_t raw_gyro[3];  // 0:X, 1:Y, 2:Z
 * float accel_g[3];     // 0:X, 1:Y, 2:Z
 * float gyro_dps[3];    // 0:X, 1:Y, 2:Z
 * } bmi088_data;
 *
 */

HAL_StatusTypeDef BMI088_Main(void) {
    // 修正 #1: 將變數改為陣列，才能容納 6 個位元組的數據
    uint8_t accel_raw_data[6];
    uint8_t gyro_raw_data[6];
    HAL_StatusTypeDef BMI088_status;
    // 讀取加速度計的 X, Y, Z 軸數據 (共 6 個位元組)
    // 數據順序: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB
    BMI088_status = BMI088_Accel_Read_Multi(BMI088_ACC_X_LSB_ADDR, accel_raw_data, 6);
    //錯誤處理
    if(BMI088_status != 0)
    {
    	printf("<BMI-088>Failed to read data! Error Code: %d\r\n",BMI088_status);
    	return SENSOR_COMM_FAIL;
    }
    // 讀取陀螺儀的 X, Y, Z 軸數據 (共 6 個位元組)
    // 數據順序: X_LSB, X_MSB, Y_LSB, Y_MSB, Z_LSB, Z_MSB
    BMI088_status = BMI088_Gyro_Read_Multi(BMI088_GYRO_RATE_X_LSB_ADDR, gyro_raw_data, 6);
    //錯誤處理
    if(BMI088_status != 0)
    {
    	printf("<BMI-088>Failed to read data! Error Code: %d\r\n",BMI088_status);
    	return SENSOR_COMM_FAIL;
    }
    // --- 數據組合與轉換 ---
    // 修正 #2: 從陣列中正確取出 LSB 和 MSB 進行組合
    // 修正 #3: 將各軸數據存到對應的陣列索引中，避免覆蓋
    // Accel X = MSB 在前, LSB 在後
    bmi088_data.raw_accel[0] = (int16_t)((accel_raw_data[1] << 8) | accel_raw_data[0]);
    // Accel Y
    bmi088_data.raw_accel[1] = (int16_t)((accel_raw_data[3] << 8) | accel_raw_data[2]);
    // Accel Z
    bmi088_data.raw_accel[2] = (int16_t)((accel_raw_data[5] << 8) | accel_raw_data[4]);

    // Gyro X
    bmi088_data.raw_gyro[0] = (int16_t)((gyro_raw_data[1] << 8) | gyro_raw_data[0]);
    // Gyro Y
    bmi088_data.raw_gyro[1] = (int16_t)((gyro_raw_data[3] << 8) | gyro_raw_data[2]);
    // Gyro Z
    bmi088_data.raw_gyro[2] = (int16_t)((gyro_raw_data[5] << 8) | gyro_raw_data[4]);

    // 將原始數據轉換為物理單位 (參考第四章)
    // 加速度計靈敏度 (LSB/g)，範圍 ±3g -> 2^16 / (2 * 3g) = 65536 / 6 ≈ 10922 LSB/g
    // 實際轉換：value_g = raw_value * (3.0f / 32768.0f)
    bmi088_data.accel_g[0] = bmi088_data.raw_accel[0] * (3.0f / 32768.0f);
    bmi088_data.accel_g[1] = bmi088_data.raw_accel[1] * (3.0f / 32768.0f);
    bmi088_data.accel_g[2] = bmi088_data.raw_accel[2] * (3.0f / 32768.0f);

    // 陀螺儀靈敏度 (LSB/dps)，範圍 ±2000dps -> 2^16 / (2 * 2000dps) = 65536 / 4000 = 16.384 LSB/(°/s)
    // 實際轉換：value_dps = raw_value / 16.384  或  value_dps = raw_value * (2000.0f / 32768.0f)
    bmi088_data.gyro_dps[0] = bmi088_data.raw_gyro[0] * (2000.0f / 32768.0f);
    bmi088_data.gyro_dps[1] = bmi088_data.raw_gyro[1] * (2000.0f / 32768.0f);
    bmi088_data.gyro_dps[2] = bmi088_data.raw_gyro[2] * (2000.0f / 32768.0f);


    // --- 透過 printf 輸出數據 ---
    // 輸出原始數據 (可選，取消註解來進行調試)
    // printf("RAW -> ACC(x,y,z): %6d, %6d, %6d | GYR(x,y,z): %6d, %6d, %6d\r\n",
    //        bmi088_data.raw_accel[0], bmi088_data.raw_accel[1], bmi088_data.raw_accel[2],
    //        bmi088_data.raw_gyro[0], bmi088_data.raw_gyro[1], bmi088_data.raw_gyro[2]);

    // 輸出轉換後的物理單位數據 (修正 printf 的變數)
//    printf("PHY -> ACC(x,y,z)[g]: %8.3f, %8.3f, %8.3f | GYR(x,y,z)[dps]: %8.3f, %8.3f, %8.3f\r\n",
//           bmi088_data.accel_g[0], bmi088_data.accel_g[1], bmi088_data.accel_g[2],
//           bmi088_data.gyro_dps[0], bmi088_data.gyro_dps[1], bmi088_data.gyro_dps[2]);

	printf("<BMI-088> accel_data: %8.3f, %8.3f, %8.3f\r\n", bmi088_data.accel_g[0], bmi088_data.accel_g[1], bmi088_data.accel_g[2]);
	printf("<BMI-088> gyro_data: %8.3f, %8.3f, %8.3f\r\n", bmi088_data.gyro_dps[0], bmi088_data.gyro_dps[1], bmi088_data.gyro_dps[2]);
	printf("The data is OK\r\n");
	return SENSOR_OK;
}
