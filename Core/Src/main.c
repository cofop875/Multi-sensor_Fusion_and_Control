/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "icm20948_driver.h"
#include "icp20100.h" // 引入 ICP20100 驅動程式庫
#include "icm42688p.h" // 引入 ICM-42688-P 驅動程式頭文件
#include "sensor_error_handler.h" //錯誤處理
#include "rm3100.h"
#include "BMI088.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c2;
I2C_HandleTypeDef hi2c4;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */
//ICM42688p
int16_t accel_data[3]; // 用於儲存加速計 X, Y, Z 軸原始數據的陣列
int16_t gyro_data[3];  // 用於儲存陀螺儀 X, Y, Z 軸原始數據的陣列
// 感測器轉換後數據
float ICM42688paccel_g[3];
float ICM42688pgyro_dps[3];
RM3100_Data magData;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI2_Init(void);
static void MX_I2C4_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_I2C4_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
#define MAX_INIT_ATTEMPTS 10 // 定義最大INIT嘗試次數
  int attempt_count = 0;// INIT嘗試計數
  /* Initialize ICM-20948 sensor ----------------------------------------------------------*/
  HAL_StatusTypeDef ICM20948_status;
  printf("<ICM-20948> System initialization...\r\n");
  // 使用 for 迴圈嘗試初始化，最多10次
  for (attempt_count = 1; attempt_count <= MAX_INIT_ATTEMPTS; ++attempt_count) {
      ICM20948_status = ICM20948_Init(); // 執行初始化
      if (ICM20948_status == SENSOR_OK) {
          // 如果成功，就跳出迴圈，attempt_count歸零
    	  attempt_count = 0;
          break;
      }
      // 如果失敗，印出嘗試失敗的訊息
      printf("<ICM-20948> Initialization FAILED on attempt %d/%d. Retrying...\r\n", attempt_count, MAX_INIT_ATTEMPTS);
      HAL_Delay(200); // 在下次嘗試前延遲一小段時間，讓感測器有時間重置
  }
  // 在迴圈結束後，根據最終的狀態來判斷並印出訊息
  if (ICM20948_status == SENSOR_OK) {
      printf("<ICM-20948> Initialization Successfully on attempt #%d\r\n", attempt_count);
  } else {
	  printf("<ICM-20948> Initialization FAILED Error Code: %d\r\n", ICM20948_status);
  }
  HAL_Delay(2000);
  	/* Initialize ICP-20100 sensor ----------------------------------------------------------*/
  ICP20100_StatusTypeDef icp20100_status;
   printf("<ICP-20100> System initialization...\r\n");
   // 使用 for 迴圈嘗試初始化，最多10次
   for (attempt_count = 1; attempt_count <= MAX_INIT_ATTEMPTS; ++attempt_count) {
	   icp20100_status = icp20100_init(&hi2c2); // 執行初始化
       if (icp20100_status == SENSOR_OK) {
           // 如果成功，就跳出迴圈，attempt_count歸零
     	  attempt_count = 0;
           break;
       }
       // 如果失敗，印出嘗試失敗的訊息
       printf("<ICP-20100> Initialization FAILED on attempt %d/%d. Retrying...\r\n", attempt_count, MAX_INIT_ATTEMPTS);
       HAL_Delay(200); // 在下次嘗試前延遲一小段時間，讓感測器有時間重置
   }
   // 在迴圈結束後，根據最終的狀態來判斷並印出訊息
   if (icp20100_status == SENSOR_OK) {
       printf("<ICP-20100> Initialization Successfully on attempt #%d\r\n", attempt_count);
   } else {
 	  printf("<ICP-20100> Initialization FAILED Error Code: %d\r\n", icp20100_status);
   }
   HAL_Delay(2000);
   /* Initialize ICP-20100 sensor ----------------------------------------------------------*/

   /* Initialize ICM-42688-P sensor ----------------------------------------------------------*/
   	   HAL_StatusTypeDef icm42688p_status;
      printf("<ICM-42688-P> System initialization...\r\n");
      // 使用 for 迴圈嘗試初始化，最多10次
      for (attempt_count = 1; attempt_count <= MAX_INIT_ATTEMPTS; ++attempt_count) {
    	  icm42688p_status = icm42688p_init(&hspi2); // 執行初始化
          if (icm42688p_status == SENSOR_OK) {
              // 如果成功，就跳出迴圈，attempt_count歸零
        	  attempt_count = 0;
              break;
          }
          // 如果失敗，印出嘗試失敗的訊息
          printf("<ICM-42688-P> Initialization FAILED on attempt %d/%d. Retrying...\r\n", attempt_count, MAX_INIT_ATTEMPTS);
          HAL_Delay(200); // 在下次嘗試前延遲一小段時間，讓感測器有時間重置
      }
      // 在迴圈結束後，根據最終的狀態來判斷並印出訊息
      if (icm42688p_status == SENSOR_OK) {
          printf("<ICM-42688-P> Initialization Successfully on attempt #%d\r\n", attempt_count);
      } else {
    	  printf("<ICM-42688-P> Initialization FAILED Error Code: %d\r\n", icm42688p_status);
      }
      HAL_Delay(2000);
      /* Initialize ICM-42688-P sensor ----------------------------------------------------------*/

	/* Initialize RM-3100 sensor ----------------------------------------------------------*/
      HAL_StatusTypeDef rm3100_status;
     	      printf("<RM-3100> System initialization...\r\n");
     	      // 使用 for 迴圈嘗試初始化，最多10次
     	      for (attempt_count = 1; attempt_count <= MAX_INIT_ATTEMPTS; ++attempt_count) {
     	    	 rm3100_status = SENSOR_OK; // 執行初始化
     	          if (rm3100_status == SENSOR_OK) {
     	              // 如果成功，就跳出迴圈，attempt_count歸零
     	        	  attempt_count = 0;
     	              break;
     	          }
     	          // 如果失敗，印出嘗試失敗的訊息
     	          printf("<RM-3100> Initialization FAILED on attempt %d/%d. Retrying...\r\n", attempt_count, MAX_INIT_ATTEMPTS);
     	          HAL_Delay(200); // 在下次嘗試前延遲一小段時間，讓感測器有時間重置
     	      }
     	      // 在迴圈結束後，根據最終的狀態來判斷並印出訊息
     	      if (rm3100_status == SENSOR_OK) {
     	          printf("<RM-3100> Initialization Successfully on attempt #%d\r\n", attempt_count);
     	      } else {
     	    	  printf("<RM-3100> Initialization FAILED Error Code: %d\r\n", rm3100_status);
     	      }
     	      HAL_Delay(2000);
	/* Initialize RM-3100 sensor ----------------------------------------------------------*/

	/* Initialize BMI-088 sensor ----------------------------------------------------------*/
	 HAL_StatusTypeDef BMI088_status;
	      printf("<BMI-088> System initialization...\r\n");
	      // 使用 for 迴圈嘗試初始化，最多10次
	      for (attempt_count = 1; attempt_count <= MAX_INIT_ATTEMPTS; ++attempt_count) {
	    	  BMI088_status = BMI088_Init(); // 執行初始化
	          if (BMI088_status == SENSOR_OK) {
	              // 如果成功，就跳出迴圈，attempt_count歸零
	        	  attempt_count = 0;
	              break;
	          }
	          // 如果失敗，印出嘗試失敗的訊息
	          printf("<BMI-088> Initialization FAILED on attempt %d/%d. Retrying...\r\n", attempt_count, MAX_INIT_ATTEMPTS);
	          HAL_Delay(200); // 在下次嘗試前延遲一小段時間，讓感測器有時間重置
	      }
	      // 在迴圈結束後，根據最終的狀態來判斷並印出訊息
	      if (BMI088_status == SENSOR_OK) {
	          printf("<BMI-088> Initialization Successfully on attempt #%d\r\n", attempt_count);
	      } else {
	    	  printf("<BMI-088> Initialization FAILED Error Code: %d\r\n", BMI088_status);
	      }
	      HAL_Delay(2000);
		/* Initialize BMI-088 sensor ----------------------------------------------------------*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  printf("-----------------------<ICM20948>----------------------- \n");
	  ICM20948_Main();
	  HAL_Delay(1000);
	  printf("-----------------------<ICP20100>----------------------- \n");
	  ICP20100_Main(&hi2c2);
	  HAL_Delay(1000);
	  printf("-----------------------<ICM42688p>----------------------- \n");
	  // icm42688p讀取加速計原始數據
	  icm42688p_read_accel_raw(&hspi2, accel_data);
	  // icm42688p讀取陀螺儀原始數據
	  icm42688p_read_gyro_raw(&hspi2, gyro_data);
	  //icm42688p 將原始數據轉換為物理單位
	  ICM42688p_ConvertAccelRawToG(accel_data, ICM42688paccel_g);
	  ICM42688p_ConvertGyroRawToDPS(gyro_data, ICM42688pgyro_dps);
	  //icm42688p 確保 accel_data 和 gyro_data 陣列的索引正確 (0=X, 1=Y, 2=Z)
	  printf("<ICM42688p> accel_data: X=%.2f, Y=%.2f, Z=%.2f\r\n", ICM42688paccel_g[0], ICM42688paccel_g[1], ICM42688paccel_g[2]);
	  printf("<ICM42688p> gyro_data: X=%.2f, Y=%.2f, Z=%.2f\r\n", ICM42688pgyro_dps[0], ICM42688pgyro_dps[1], ICM42688pgyro_dps[2]);
	  printf("The data is OK\r\n");
	  HAL_Delay(1000);
	  printf("-----------------------<RM-3100>----------------------- \n");
	  RM3100_Main(&hi2c4, &magData);
	  HAL_Delay(1000);
	  printf("-----------------------<BMI-088>----------------------- \n");
	  BMI088_Main();
//	  printf("Failed to read data from BMI-088. Error Code: 4\r\n");
	  printf("-----------------------Test End----------------------- \r\n\r\n");
	  while (1)
	   {}
	 // <<< 新增：呼叫封裝好的函數 >>>
	 //ICM42688P_Main(&hspi2); // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< 新增這一行
//	  while(1){
//		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
//		  HAL_Delay(100);
//		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);
//		  HAL_Delay(100);
//	  }

//	  printf("archer %d \n",WWE);
//	  WWE++;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00707CBB;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief I2C4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C4_Init(void)
{

  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x00707CBB;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x0;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 0x0;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi3.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI2_NCS1_ICM42688P_GPIO_Port, SPI2_NCS1_ICM42688P_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, SPI3_nCS1_BMI088_ACCEL_Pin|ICM20948_CS_Pin|SPI3_nCS2_BMI088_GYRO_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : ICM20948_DRDY_Pin */
  GPIO_InitStruct.Pin = ICM20948_DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ICM20948_DRDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI3_DRDY1_BMI088_INT1_ACCEL_Pin SPI3_DRDY2_BMI088_INT3_GYRO_Pin */
  GPIO_InitStruct.Pin = SPI3_DRDY1_BMI088_INT1_ACCEL_Pin|SPI3_DRDY2_BMI088_INT3_GYRO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI2_NCS1_ICM42688P_Pin */
  GPIO_InitStruct.Pin = SPI2_NCS1_ICM42688P_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI2_NCS1_ICM42688P_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI3_nCS1_BMI088_ACCEL_Pin SPI3_nCS2_BMI088_GYRO_Pin */
  GPIO_InitStruct.Pin = SPI3_nCS1_BMI088_ACCEL_Pin|SPI3_nCS2_BMI088_GYRO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : ICM20948_CS_Pin */
  GPIO_InitStruct.Pin = ICM20948_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(ICM20948_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ICP20100_DRDY_Pin */
  GPIO_InitStruct.Pin = ICP20100_DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ICP20100_DRDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DRDY_INT2_ICM42688P_Pin */
  GPIO_InitStruct.Pin = DRDY_INT2_ICM42688P_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DRDY_INT2_ICM42688P_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
