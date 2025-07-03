/*
 * sensor_error_handler.c
 *
 *  Created on: Jun 19, 2025
 *      Author: kloud
 */
// File: Core/Src/sensor_error_handler.c
#include "sensor_error_handler.h"
#include <string.h>

/**
 * @brief  將錯誤碼轉換為字串。
 */
const char* sensor_error_to_string(int error_code) {
    switch (error_code) {
        case SENSOR_OK:                 return "OK";
        case SENSOR_ERROR:              return "GENERAL_ERROR";
        case SENSOR_INIT_FAIL:          return "INIT_FAIL";
        case SENSOR_CONFIG_FAIL:        return "CONFIG_FAIL";
        case SENSOR_COMM_FAIL:          return "COMM_FAIL";
        case SENSOR_NO_RESPONSE:        return "NO_RESPONSE";
        case SENSOR_INVALID_ID:         return "INVALID_ID";
        case SENSOR_DATA_ERR:           return "DATA_ERROR";
        case SENSOR_DATA_NOT_READY:     return "DATA_NOT_READY";
        case SENSOR_RX_NODATA:          return "RX_NO_DATA";
        case SENSOR_RX_BUFFER_OVERFLOW: return "RX_BUFFER_OVERFLOW";
        case SENSOR_GPS_NO_FIX:         return "GPS_NO_FIX";
        case SENSOR_PARAM_INVALID:      return "INVALID_PARAM";
        default:                        return "UNKNOWN_ERROR";
    }
}

/**
 * @brief  處理感測器錯誤的核心函式。
 */
void handle_sensor_error(const char* sensor_id, int error_code) {
    // 如果操作成功，則不印出任何訊息
    if (error_code == SENSOR_OK) {
        return;
    }

    // 準備格式化錯誤訊息的緩衝區
    char error_buffer[128];
    const char* error_string = sensor_error_to_string(error_code);

    // 根據設計文件規範格式化輸出字串
    // 格式: <SensorID>_ERROR:CODE=-X,MSG=ERROR_STRING [N/A]
    snprintf(error_buffer, sizeof(error_buffer),
             "<%s>_ERROR:CODE=%d,MSG=%s [N/A]\r\n",
             sensor_id, error_code, error_string);

    // 透過 UART 印出格式化的錯誤訊息 (假設 printf 已重定向到 UART)
    printf("%s", error_buffer);

    // 可選：在此處添加進一步的錯誤處理邏輯，例如：
    // - 點亮錯誤指示 LED
    // - 嘗試重新初始化感測器
    // - 進入安全模式等
}

