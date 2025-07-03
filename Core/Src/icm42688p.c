#include "icm42688p.h"
#include <stdio.h> // 用於 printf (除錯)
#include "sensor_error_handler.h" //錯誤處理
// ICM-42688-P nCS 引腳定義 (根據 User Label)
#define ICM42688P_NCS_PORT GPIOB
#define ICM42688P_NCS_PIN  GPIO_PIN_11

// --- 內部輔助函數 ---
static float current_accel_sensitivity = ACCEL_SENSITIVITY_2G;
static float current_gyro_sensitivity = GYRO_SENSITIVITY_250DPS;
// 選中 ICM-42688-P (nCS 拉低)
static void icm42688p_select(void) {
    HAL_GPIO_WritePin(ICM42688P_NCS_PORT, ICM42688P_NCS_PIN, GPIO_PIN_RESET);
}

// 取消選中 ICM-42688-P (nCS 拉高)
static void icm42688p_deselect(void) {
    HAL_GPIO_WritePin(ICM42688P_NCS_PORT, ICM42688P_NCS_PIN, GPIO_PIN_SET);
}

// 寫入單個暫存器
// 根據 ICM-42688-P SPI 協議, 寫入時, 位址位元組的 MSB (bit 7) 為 0
static HAL_StatusTypeDef icm42688p_write_register(SPI_HandleTypeDef *hspi, uint8_t reg, uint8_t value) {
    HAL_StatusTypeDef status1, status2;
    uint8_t command = reg & 0x7F; // 清除 MSB (bit 7) 表示寫入操作

    icm42688p_select();
    status1 = HAL_SPI_Transmit(hspi, &command, 1, ICM42688P_SPI_TIMEOUT);
    if (status1 == HAL_OK) {
        status2 = HAL_SPI_Transmit(hspi, &value, 1, ICM42688P_SPI_TIMEOUT);
    } else {
        status2 = status1; // 如果第一次傳輸失敗，直接返回該狀態
    }
    icm42688p_deselect();

    if (status1!= HAL_OK) return status1;
    return status2;
}

// 讀取單個暫存器
// 根據 ICM-42688-P SPI 協議, 讀取時, 位址位元組的 MSB (bit 7) 為 1
static uint8_t icm42688p_read_register(SPI_HandleTypeDef *hspi, uint8_t reg) {
    uint8_t command = reg | 0x80; // 設定 MSB (bit 7) 表示讀取操作
    uint8_t data_read = 0;

    icm42688p_select();
    HAL_SPI_Transmit(hspi, &command, 1, ICM42688P_SPI_TIMEOUT);
    HAL_SPI_Receive(hspi, &data_read, 1, ICM42688P_SPI_TIMEOUT);
    icm42688p_deselect();

    return data_read;
}

// 連續讀取多個暫存器
static HAL_StatusTypeDef icm42688p_read_multiple_registers(SPI_HandleTypeDef *hspi, uint8_t start_reg, uint8_t *buffer, uint8_t len) {
    HAL_StatusTypeDef status;
    uint8_t command = start_reg | 0x80; // 設定 MSB (bit 7) 表示讀取操作

    icm42688p_select();
    HAL_SPI_Transmit(hspi, &command, 1, ICM42688P_SPI_TIMEOUT); // 發送起始位址
    status = HAL_SPI_Receive(hspi, buffer, len, ICM42688P_SPI_TIMEOUT); // 連續讀取數據
    icm42688p_deselect();

    return status;
}

// 切換暫存器庫 (0 for User Bank 0, 1 for User Bank 1, etc.)
static HAL_StatusTypeDef icm42688p_set_bank(SPI_HandleTypeDef *hspi, uint8_t bank) {
    return icm42688p_write_register(hspi, ICM42688P_REG_BANK_SEL, bank);
}

// --- 公開函數實作 ---

// 初始化 ICM-42688-P
uint8_t icm42688p_init(SPI_HandleTypeDef *hspi) {
    uint8_t who_am_i_value;
    uint8_t temp_reg_val;
    uint8_t test_reg_val;
    icm42688p_deselect(); // 確保 nCS 初始為高電位
    HAL_Delay(10); // 感測器上電後短暫延遲

    // 1. 切換到 User Bank 0 (大部分配置暫存器位於此)
    if (icm42688p_set_bank(hspi, 0)!= HAL_OK) {
        printf("ICM Error: Set Bank 0 failed\r\n");
        return SENSOR_INIT_FAIL; // 返回 0 表示初始化失敗
    }
    HAL_Delay(100); // 切換 Bank 後延遲(此處數值原為1，某些情況貌似太快導致who_am_i_value = 0x0，初始化失敗，可視情況調整!)

    // 2. 讀取 WHO_AM_I 暫存器進行驗證
    who_am_i_value = icm42688p_read_who_am_i(hspi); // 此函數內部已處理 Bank 切換
    if (who_am_i_value!= ICM42688P_WHO_AM_I_VALUE) {
        printf("ICM Error: WHO_AM_I check failed. Expected 0x%02X, Got 0x%02X\r\n", ICM42688P_WHO_AM_I_VALUE, who_am_i_value);
        return SENSOR_INVALID_ID;
    }
    // 移除此處的 printf，移至 main.c 中判斷後打印
    // printf("ICM Info: WHO_AM_I = 0x%02X (OK)\r\n", who_am_i_value);

    // 3. 配置電源管理 PWR_MGMT0 (0x4E)
    //    啟用加速計和陀螺儀，均設為低噪音 (LN) 模式
    //    ACCEL_MODE[1:0] = 11 (LN), GYRO_MODE[1:0] = 11 (LN) -> 0b00001111 = 0x0F

    if (icm42688p_write_register(hspi, ICM42688P_REG_PWR_MGMT0, ICM42688P_PWR_MGMT0_ACCEL_LN | ICM42688P_PWR_MGMT0_GYRO_LN)!= HAL_OK) {
        // printf("ICM Error: PWR_MGMT0 config failed\r\n"); // 移至 main.c
        return SENSOR_CONFIG_FAIL;
    }
    HAL_Delay(1); // 模式更改後需要穩定時間 (datasheet 建議 1ms for gyro, 1ms for accel from sleep to LN)
//    test_reg_val = icm42688p_read_register(hspi, ICM42688P_REG_PWR_MGMT0);
//    printf("ICM42688P_REG_PWR_MGMT0: 0x%02X\r\n", test_reg_val);

    // 4. 配置陀螺儀 GYRO_CONFIG0 (0x4F)
    //    設定 ODR = 1kHz (0x06), FS_SEL = ±2000 dps (0x00 << 4)
    if (icm42688p_write_register(hspi, ICM42688P_REG_GYRO_CONFIG0, ICM42688P_GYRO_FS_SEL_2000DPS | ICM42688P_ODR_1KHZ)!= HAL_OK) {
        // printf("ICM Error: GYRO_CONFIG0 failed\r\n"); // 移至 main.c
        return SENSOR_CONFIG_FAIL;
    }
    HAL_Delay(1);

    // 5. 配置加速計 ACCEL_CONFIG0 (0x50)
    //    設定 ODR = 1kHz (0x06), FS_SEL = ±16g (0x00 << 4)
    if (icm42688p_write_register(hspi, ICM42688P_REG_ACCEL_CONFIG0, ICM42688P_ACCEL_FS_SEL_16G | ICM42688P_ODR_1KHZ)!= HAL_OK) {
        // printf("ICM Error: ACCEL_CONFIG0 failed\r\n"); // 移至 main.c
        return SENSOR_CONFIG_FAIL;
    }
    HAL_Delay(1);

    // 6. (重要) 清除 INT_ASYNC_RESET 位 (INT_CONFIG1, 0x64, bit 4)
    //    根據 datasheet 建議，為確保 INT1/INT2 正常工作，應清除此位
    //    此暫存器位於 User Bank 0
    temp_reg_val = icm42688p_read_register(hspi, ICM42688P_REG_INT_CONFIG1); // 內部已處理 Bank 切換
    if (icm42688p_write_register(hspi, ICM42688P_REG_INT_CONFIG1, temp_reg_val & ~ICM42688P_INT_ASYNC_RESET_BIT)!= HAL_OK) {
        // printf("ICM Error: INT_CONFIG1 (clear INT_ASYNC_RESET) failed\r\n"); // 移至 main.c
        return SENSOR_DATA_NOT_READY;
    }
    HAL_Delay(1);

    // printf("ICM-42688-P 初始化成功。\r\n"); // 移至 main.c
    return SENSOR_OK; // 返回 1 表示初始化成功
}

// 讀取 WHO_AM_I 暫存器
uint8_t icm42688p_read_who_am_i(SPI_HandleTypeDef *hspi) {
    // 確保在 Bank 0 讀取
    // 注意: icm42688p_read_register 內部不處理 bank 切換，調用者需確保 bank 正確
    // 為安全起見，在此處明確設定 bank
    if (icm42688p_set_bank(hspi, 0)!= HAL_OK) {
         // 實際應用中應處理此錯誤
//        printf("ICM Error: Set Bank 0 failed in read_who_am_i\r\n");
        return 0; // 返回錯誤值
    }
    HAL_Delay(10); // Bank 切換後延遲
    return icm42688p_read_register(hspi, ICM42688P_REG_WHO_AM_I);
}

// 讀取陀螺儀原始數據 (3軸)
void icm42688p_read_gyro_raw(SPI_HandleTypeDef *hspi, int16_t* gyro_data) {
    uint8_t raw_data_buffer[3]; // 用於儲存 X_MSB, X_LSB, Y_MSB, Y_LSB, Z_MSB, Z_LSB

    // 確保在 Bank 0 讀取
    if (icm42688p_set_bank(hspi, 0)!= HAL_OK) {
//        printf("ICM Error: Set Bank 0 failed in read_gyro_raw\r\n");
        gyro_data[0] = 0; // Gyro X 清零
        gyro_data[1] = 0; // Gyro Y 清零
        gyro_data[2] = 0; // Gyro Z 清零
        return;
    }
    HAL_Delay(1); // 短暫延遲確保 Bank 切換完成

    if(icm42688p_read_multiple_registers(hspi, ICM42688P_REG_GYRO_DATA_X1_UI, raw_data_buffer, 6)!= HAL_OK) {
//        printf("ICM Error: Failed to read gyro data\r\n");
        gyro_data[0] = 0; // Gyro X 清零
        gyro_data[1] = 0; // Gyro Y 清零
        gyro_data[2] = 0; // Gyro Z 清零
        return;
    }

    gyro_data[0] = (int16_t)(raw_data_buffer[0] << 8 | raw_data_buffer[1]); // Gyro X
    gyro_data[1] = (int16_t)(raw_data_buffer[2] << 8 | raw_data_buffer[4]); // Gyro Y
    gyro_data[2] = (int16_t)(raw_data_buffer[5] << 8 | raw_data_buffer[6]); // Gyro Z
}

// 讀取加速計原始數據 (3軸)
void icm42688p_read_accel_raw(SPI_HandleTypeDef *hspi, int16_t* accel_data) {
    uint8_t raw_data_buffer[3]; // 用於儲存 X_MSB, X_LSB, Y_MSB, Y_LSB, Z_MSB, Z_LSB

    // 確保在 Bank 0 讀取
    if (icm42688p_set_bank(hspi, 0)!= HAL_OK) {
//        printf("ICM Error: Set Bank 0 failed in read_accel_raw\r\n");
        accel_data[0] = 0; // Accel X 清零
        accel_data[1] = 0; // Accel Y 清零
        accel_data[2] = 0; // Accel Z 清零
        return;
    }
    HAL_Delay(1);

    if(icm42688p_read_multiple_registers(hspi, ICM42688P_REG_ACCEL_DATA_X1_UI, raw_data_buffer, 6)!= HAL_OK) {
//        printf("ICM Error: Failed to read accel data\r\n");
        accel_data[0] = 0; // Accel X 清零
        accel_data[1] = 0; // Accel Y 清零
        accel_data[2] = 0; // Accel Z 清零
        return;
    }

    accel_data[0] = (int16_t)(raw_data_buffer[0] << 8 | raw_data_buffer[1]); // Accel X
    accel_data[1] = (int16_t)(raw_data_buffer[2] << 8 | raw_data_buffer[4]); // Accel Y
    accel_data[2] = (int16_t)(raw_data_buffer[5] << 8 | raw_data_buffer[6]); // Accel Z
}
// <<< 新增的主處理函數實作 >>>
/**
  * @brief  讀取 ICM-42688-P 的數據並透過 printf 輸出。
  * @param  hspi: 指向 SPI_HandleTypeDef 結構的指標，包含 SPI 配置信息。
  * @retval None
  */
/**
 * @brief 將加速計原始數據轉換為 g (重力加速度)。
 * @param pAccelRaw: 指向加速計原始數據陣列 (int16_t[3])。
 * @param pAccelG:   指向儲存轉換後數據的陣列 (float[3])。
 */
void ICM42688p_ConvertAccelRawToG(const int16_t* pAccelRaw, float* pAccelG) {
    pAccelG[0] = (float)pAccelRaw[0] / current_accel_sensitivity;
    pAccelG[1] = (float)pAccelRaw[1] / current_accel_sensitivity;
    pAccelG[2] = (float)pAccelRaw[2] / current_accel_sensitivity;
}

/**
 * @brief 將陀螺儀原始數據轉換為 dps (度/秒)。
 * @param pGyroRaw: 指向陀螺儀原始數據陣列 (int16_t[3])。
 * @param pGyroDPS: 指向儲存轉換後數據的陣列 (float[3])。
 */
void ICM42688p_ConvertGyroRawToDPS(const int16_t* pGyroRaw, float* pGyroDPS) {
    pGyroDPS[0] = (float)pGyroRaw[0] / current_gyro_sensitivity;
    pGyroDPS[1] = (float)pGyroRaw[1] / current_gyro_sensitivity;
    pGyroDPS[2] = (float)pGyroRaw[2] / current_gyro_sensitivity;
}
void ICM42688P_Main(SPI_HandleTypeDef *hspi) {
    int16_t accel_data[3]; // 在此函數內部宣告陣列
    int16_t gyro_data[3];  // 在此函數內部宣告陣列

    // 讀取加速計原始數據
    icm42688p_read_accel_raw(hspi, accel_data);

    // 讀取陀螺儀原始數據
    icm42688p_read_gyro_raw(hspi, gyro_data);

    // 透過 printf 輸出數據 (繁體中文標籤)
    // *** 注意：修正了原始程式碼中 accel_data 和 gyro_data 的第一個參數錯誤 ***
//    printf("accel_data: X=%6d, Y=%6d, Z=%6d\r\n", accel_data[0], accel_data[1], accel_data[2]);
//    printf("gyro_data: X=%6d, Y=%6d, Z=%6d\r\n\r\n", gyro_data[0], gyro_data[1], gyro_data[2]);

    HAL_Delay(500); // 每 500 毫秒讀取並輸出一組數據
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

