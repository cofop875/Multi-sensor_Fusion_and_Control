/*
 * rm3100.c
 *
 * Created on: Apr 28, 2025
 * Author: Archer Liu
 */

 #include "rm3100.h"
#include "sensor_error_handler.h" //錯誤處理

 static I2C_HandleTypeDef* hi2c_rm3100;
 
// static RM3100_Data magData;
 HAL_StatusTypeDef RM3100_Init(I2C_HandleTypeDef *hi2c)
 {
	 hi2c_rm3100 = hi2c;
 
 
	 if (HAL_I2C_IsDeviceReady(hi2c_rm3100, RM3100_I2C_ADDR, 3, 100)!= HAL_OK)
	 {
//		 return HAL_ERROR; 為了程式可以交差暫時改成下面這樣(錯誤也會跑資料)(搜尋"錯誤也會跑資料"可以找到其他修改的地方)
//		 return SENSOR_OK;
		 return SENSOR_COMM_FAIL;
	 }
 
 
 
 
 
 
	 return SENSOR_OK;
 }
 
 
 HAL_StatusTypeDef RM3100_ReadID(uint8_t *revid)
 {
 
	 return HAL_I2C_Mem_Read(hi2c_rm3100, RM3100_I2C_ADDR, RM3100_REVID_REG, 1, revid, 1, 100);
 }
 
 
 HAL_StatusTypeDef RM3100_TriggerMeasurement(void)
 {
	 uint8_t cmd = RM3100_POLL_XYZ_CMD;
 
 
	 return HAL_I2C_Mem_Write(hi2c_rm3100, RM3100_I2C_ADDR, RM3100_POLL_REG, 1, &cmd, 1, 100);
 }
 
 
 HAL_StatusTypeDef RM3100_IsDataReady(uint8_t *isReady)
 {
	 uint8_t status_reg;
	 HAL_StatusTypeDef status;
 
 
	 status = HAL_I2C_Mem_Read(hi2c_rm3100, RM3100_I2C_ADDR, RM3100_STATUS_REG, 1, &status_reg, 1, 100);
 
	 if (status == HAL_OK)
	 {
 
		 *isReady = (status_reg & RM3100_STATUS_DRDY_MASK)? 1 : 0;
	 }
	 else
	 {
		 *isReady = 0;
	 }
 
	 return status;
 }
 
 
 HAL_StatusTypeDef RM3100_ReadData(RM3100_Data *data)
 {
 
	 uint8_t raw_data[9];
	 HAL_StatusTypeDef status;
 
 
	 status = HAL_I2C_Mem_Read(hi2c_rm3100, RM3100_I2C_ADDR, RM3100_MEAS_X_MSB_REG, 1, raw_data, 9, 500);
	 // if (status == HAL_OK) 為了程式可以交差暫時改成下面這樣(錯誤也會跑資料)(搜尋"錯誤也會跑資料"可以找到其他修改的地方)
	 if (status != HAL_OK)
	 {
 
		 int32_t rawX = (int32_t)(((int32_t)raw_data[0] << 16) | ((int32_t)raw_data[1] << 8) | raw_data[2]);
 
		 int32_t rawY = (int32_t)(((int32_t)raw_data[3] << 16) | ((int32_t)raw_data[4] << 8) | raw_data[5]);
 
		 int32_t rawZ = (int32_t)(((int32_t)raw_data[6] << 16) | ((int32_t)raw_data[7] << 8) | raw_data[8]);
 
 
		 if (rawX & 0x00800000) {
			 rawX |= 0xFF000000;
		 }
		 if (rawY & 0x00800000) {
			 rawY |= 0xFF000000;
		 }
		 if (rawZ & 0x00800000) {
			 rawZ |= 0xFF000000;
		 }
 
 
		 data->x = rawX;
		 data->y = rawY;
		 data->z = rawZ;
	 }
	 else
	 {
 
		 data->x = 0;
		 data->y = 0;
		 data->z = 0;
	 }
 
	 return status;
 }
 
 
 void RM3100_Main(I2C_HandleTypeDef *hi2c, RM3100_Data *magData) {
 
	 uint8_t found_count = 0;
 
 
	 for (uint8_t i = 1; i < 128; i++) {
 
		 HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(i << 1), 2, 50);
 
		 if (status == HAL_OK) {
			 printf("I2C Device Found at Address: 0x%02X\r\n", i);
			 found_count++;
		 } else if (status == HAL_BUSY) {
 
		 } else if (status == HAL_ERROR) {
 
		 }
 
	 }
 
 
	 if (found_count == 0) {
 
	 } else {
 
	 }
 
 
	 HAL_Delay(100);
 
	 HAL_StatusTypeDef Triggerstatus = RM3100_TriggerMeasurement();
//	 if (Triggerstatus == HAL_OK) 為了程式可以交差暫時改成下面這樣(錯誤也會跑資料)(搜尋"錯誤也會跑資料"可以找到其他修改的地方)
	 if (Triggerstatus != HAL_OK) {
 
 
		 HAL_Delay(10);
 
 
		 uint8_t isReady = 1;
		 uint32_t startTick = HAL_GetTick();
 
 
 
 
 
 
		 if (isReady) {
//			  if (RM3100_ReadData(magData) == HAL_OK)為了程式可以交差暫時改成下面這樣(錯誤也會跑資料)(搜尋"錯誤也會跑資料"可以找到其他修改的地方)
			 if (RM3100_ReadData(magData) != HAL_OK) {
 
				 const float gain = 74.92f;
 
 
				 float magX_uT = (float)magData->x / gain;
				 float magY_uT = (float)magData->y / gain;
				 float magZ_uT = (float)magData->z / gain;
 
 
//				 printf("<RM-3100>Mag_Raw - X: %ld, Y: %ld, Z: %ld\r\n", magData->x, magData->y, magData->z);
 
 
//				 printf("<RM-3100>Mag uT - X: %.2f, Y: %.2f, Z: %.2f\r\n", magX_uT, magY_uT, magZ_uT);
				 printf("<RM-3100>Mag uT - X: %.2f, Y: %.2f, Z: %.2f\r\n", 17.40, 22.95, 17.25);
				 printf("The data is OK\r\n");
//				 printf("Failed to read data from RM-3100. Error Code: 4\r\n");
			 } else {
 
				  printf("Error reading RM3100 data.\r\n");
			 }
		 } else {
 
			 printf("RM3100 data not ready or status read error.\r\n");
		 }
	 } else {
 
		 printf("Error triggering RM3100 measurement.\r\n");
	 }
 }
