/*
 * sensor_error_handler.h
 *
 *  Created on: Jun 19, 2025
 *      Author: kloud
 */

// File: Core/Inc/sensor_error_handler.h
#ifndef INC_SENSOR_ERROR_HANDLER_H_
#define INC_SENSOR_ERROR_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> // 引入，以便在 .c 檔案中使用 printf

/*
 * =================================================================================
 * 錯誤碼定義 (Error Code Definitions)
 * 根據 "STM32H753I-EVAL2 多感測器匯入與硬體驗證 V1.2.docx" 第 5.1 節
 * =================================================================================
 */
#define SENSOR_OK                   0   // 操作成功
#define SENSOR_ERROR                1  // 通用錯誤
#define SENSOR_INIT_FAIL            2  // 初始化失敗
#define SENSOR_CONFIG_FAIL          3  // 配置失敗 (例如量程、速率)
#define SENSOR_COMM_FAIL            4  // 通訊失敗 (SPI/I2C/UART 讀寫錯誤)
#define SENSOR_NO_RESPONSE          5  // 感測器無響應 (超時)
#define SENSOR_INVALID_ID           6  // 讀取的設備 ID 不符
#define SENSOR_DATA_ERR             7  // 數據錯誤 (例如 checksum 錯誤、數值異常)
#define SENSOR_DATA_NOT_READY       8  // 數據尚未準備好 (例如轉換未完成)
#define SENSOR_RX_NODATA            9  // UART 接收緩衝區無新數據
#define SENSOR_RX_BUFFER_OVERFLOW   10 // UART 接收緩衝區溢出
#define SENSOR_GPS_NO_FIX           11 // GPS/RTK 尚未定位
#define SENSOR_PARAM_INVALID        12 // 函數輸入參數無效

/**
 * @brief  將感測器錯誤碼轉換為可讀的字串。
 * @param  error_code 錯誤碼。
 * @retval 指向表示錯誤的靜態字串指標。
 */
const char* sensor_error_to_string(int error_code);

/**
 * @brief  處理感測器錯誤，將其格式化並透過 UART 印出。
 * @note   輸出格式基於文件 4.1 節的通用規範，格式為：
 * <SensorID>_ERROR:CODE=-X,MSG=ERROR_STRING [N/A]
 * @param  sensor_id 標識感測器的字串 (例如 "ICM20948")。
 * @param  error_code 發生的錯誤碼。
 */
void handle_sensor_error(const char* sensor_id, int error_code);

#ifdef __cplusplus
}
#endif

#endif /* INC_SENSOR_ERROR_HANDLER_H_ */
