/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "DHT11.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
float temp_f;
float humidity_f = 0;
int	num=500;
char D_a[100];               // 拼接字符串
char D_b[100];	
const char* Data_Head = "AT+QMTPUB=0,0,0,0,\"mqhome\",";				//发布消息
//{\"temp\":%.2f,\"humi\":%.2f}
const char* Data_t = "{\"temp\":";
const char* Data_h = ",\"humi\":";
const char* Data_tail = "}";
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
huart2_rx_struct huart2_rx_struct_t;


/*******************************************************************************
* 名    称：get_temp_Humidity
* 功    能：获取温度函数
* 参数说明：无
* 返 回 值：无
* 说    明：无
* 调用方法：外部函数
*******************************************************************************/
void get_temp_Humidity(void)
{
    float f_buffer_temp = 0;
    float f_buffer_humi = 0;
		HAL_UART_Transmit(&huart3,"hum-temp-data get-------\n",28,20);
    /****************************** code start 3 ******************************/
    if(DHT11_Read_Data(&f_buffer_temp, &f_buffer_humi) == 0)    //读取温湿度值
    {
        if((f_buffer_temp <= 50) && (f_buffer_temp > 0))    //检测温度是否在范围内
        {
            temp_f = f_buffer_temp; //将温度值赋给全局变量temp_f   
//					printf("temp_f=%.2f \n",temp_f);
            /*将温度值赋给*HomeView.Temp,用于TFT屏显示*/         
//            snprintf(buffer, 5, "%2.1f", temp_f);   
//            /*将温度值拷贝到上传华为物联网平台的结构体中，用于上传给华为物联网平台*/
//            memcpy(g_pltSendData.Temperature, HomeView.Temp, 5);
        }
        if((f_buffer_humi <= 100) && (f_buffer_humi > 0))   //获取湿度值，可省略
        {
            humidity_f = f_buffer_humi;
//            snprintf(buffer, 5, "%2.1f", humidity_f);
//					printf("humidity_f=%.2f \n",humidity_f);
        }
    }
    /******************************* code end 3 *******************************/
}

void send_data(void)
{

	  sprintf((char*)D_b,"%s%.2f%s%.2f%s",
												Data_t,temp_f,Data_h,humidity_f,Data_tail);
					//计算数据长度拼接到M_a
		sprintf((char*)D_a,"%s%d\r\n",Data_Head,strlen(D_b));
		printf("D-A=%s",D_a);
		printf("D-B=%s",D_b);
		HAL_UART_Transmit(&huart2, (unsigned char *)D_a, strlen(D_a), 500);
		HAL_Delay(1000); 
//		HAL_UART_Transmit(&huart3,"send temp\n",11,20);
//		while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, ">") == NULL) //收到 > 回复开始发送数据
// 		{
//			memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组  //清空接受缓冲区
			HAL_UART_Transmit(&huart2, (unsigned char *)D_b, strlen(D_b), 300);
//		}
//		else{
//		HAL_UART_Transmit(&huart3,"send error\n",11,20);
//		}
		memset(D_a,0x00,100);//清空
		memset(D_b,0x00,100);//清空
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

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
	DHT_Init();								//DHT11 初始化
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_DMA(&huart2,huart2_rx_struct_t.uart_rx_buf, UART_RX_LEN);         // 启动DMA接收
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);                                      // 使能空闲中断 
	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);  //引脚写入
	 HAL_UART_Transmit(&huart3,"delay10s\r\n",8,20);	
	HAL_Delay(10000);
	memset(huart2_rx_struct_t.uart_rx_buf,0,100);//清空
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
				HAL_UART_Transmit(&huart3,"AT+CSQ\r\n",8,20);	
//			HAL_Delay(1000);
		
//			if(strstr((char*)huart2_rx_struct_t.uart_rx_buf,"OPEN")!=NULL)
//				{
//					HAL_UART_Transmit(&huart2,"AT-error\r\n",7,10);
////					HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
////					HAL_Delay(1000);
//				}	
//				else if(strstr((char*)huart2_rx_struct_t.uart_rx_buf,"CLOSE")!=NULL)
//					{
////									HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
//										HAL_UART_Transmit(&huart2,"AT\r\n",4,10);
////									HAL_Delay(1000);
//				
//					}
			memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空
			HAL_Delay(100);
		 HAL_UART_Transmit(&huart2, (unsigned char *)"AT\r\n", 4, 300);
		 HAL_Delay(100); 
		 while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "OK") == NULL)
		 {
			 memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空
		   HAL_Delay(1000); 
			  HAL_UART_Transmit(&huart3,"AT-error\r\n",10,10);
			  HAL_UART_Transmit(&huart2, (unsigned char *)"AT\r\n", 4, 300);
				HAL_Delay(100); 
				if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "OK") != NULL)
				{
					break ;
				}
		 } 	 
		 HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CGATT?\r\n", 11, 300);
		  HAL_Delay(100);
		  while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "CGATT:1") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空
		     HAL_Delay(4000); 
				 HAL_UART_Transmit(&huart3, (unsigned char *)"CGATT\r\n", 7, 300);
				  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CGATT?\r\n", 11, 300);
						HAL_Delay(1000);
					if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "CGATT:1") != NULL)
						{
							break ;
						}
		 } 
		 
		  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CEREG?\r\n", 11, 300);
		 HAL_Delay(100);
		  while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "CEREG,0,1") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				HAL_Delay(4000);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
				HAL_Delay(4000); 
				HAL_UART_Transmit(&huart3, (unsigned char *)"CEREG\r\n", 6, 300);
				 HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CEREG?\r\n", 11, 300);
				 HAL_Delay(1000); 
					if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "CEREG:0,1") != NULL)
					{
					break ;
					}
			} 
		 	 
		  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CEDRXS=0,5\r\n", 15, 300);
			HAL_Delay(100);
		  while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "OK") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
		   HAL_Delay(3000); 
				 HAL_UART_Transmit(&huart3, (unsigned char *)"CEDRXS\r\n", 11, 300);
				 HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CEDRXS=0,5\r\n", 15, 300);
			if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "OK") != NULL)
				{
					break ;
				}
		 } 
		 
		 
		  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CPSMS=0\r\n", 12, 300);
		 HAL_Delay(100);
		  while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "OK") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
		   HAL_Delay(4000); 
				 HAL_UART_Transmit(&huart3, (unsigned char *)"CPSMS\r\n", 6, 300);
				  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CPSMS=0\r\n", 12, 300);
			if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "OK") != NULL)
				{
					break ;
				}
			}
		 
		
		 
		  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTOPEN=0,\"lbsmqtt.airm2m.com\",1883\r\n", 50, 500);
//			HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTOPEN=0,'broker.emqx.io',1883\r\n", 36, 300);
			HAL_Delay(100);
			while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTOPEN: 0,0") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
		   HAL_Delay(10000); 
			 HAL_UART_Transmit(&huart3, (unsigned char *)"QMTOPEN\r\n", 6, 300);
//				   HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTOPEN=0,'lbsmqtt.airm2m.com',1883\r\n", 38, 300);
//							HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTOPEN=0,'broker.emqx.io',1883\r\n", 36, 300);
			if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTOPEN: 0,0") != NULL)
			{
					break ;
			}
		 }
		   
		  HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTCONN=0,\"aiot\"\r\n", 22, 300);
		 HAL_Delay(100);
			while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTCONN: 0,0,0") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
		    HAL_Delay(4000); 
				HAL_UART_Transmit(&huart3, (unsigned char *)"QMTCONN\r\n", 12, 300);
//				   HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTCONN=0,'ZHOU'\r\n", 19, 300);
			if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTCONN: 0,0,0") != NULL)
				{
						break ;
				}
			}
		 
		   HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTSUB=0,1,\"LED\",0\r\n", 24, 300);
			HAL_Delay(100);
		while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTSUB: 0,1,0,0") == NULL)
		  {
				HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
				memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
		   HAL_Delay(3000); 
				 HAL_UART_Transmit(&huart3, (unsigned char *)"QMTSUB\r\n", 12, 300);
//				   HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTSUB=0,1,'LED',0\r\n", 19, 300);
			if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTSUB: 0,1,0,0") != NULL)
			{
					break ;
			}
		}
		 
		while(1)
		{
			while(num--)
			{
				if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "inled:1") != NULL)
					{
						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);
						HAL_Delay(10);
//						HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
//						HAL_Delay(15);
						printf("---------LED-OPEN--------\r\n");
						memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
					}
					else if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "inled:0") != NULL)
					{
						HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_SET);
						HAL_Delay(10);
//						HAL_UART_Transmit(&huart3, huart2_rx_struct_t.uart_rx_buf, 20, 300);
//						HAL_Delay(15);
						printf("---------LED-CLOSE--------\r\n");
						memset(huart2_rx_struct_t.uart_rx_buf,0x00,100);//清空接收缓冲区数组
					}
				HAL_Delay(10);
			}
			num=500;
				 get_temp_Humidity();
				send_data();
		}
		 
//		   HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTPUB=0,0,0,0,\"LED\",5\r\n", 19, 300);
////		HAL_UART_Transmit(&huart2, (unsigned char *)"ABCDE\r\n", 7, 300);
//		while(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTPUB:0,0,0") == NULL)
//		  {
//		   HAL_Delay(10000); 
//				 HAL_UART_Transmit(&huart3, (unsigned char *)"+QMTPUB=0,0,0,0\r\n", 17, 300);
//				   HAL_UART_Transmit(&huart2, (unsigned char *)"AT+QMTPUB=0,0,0,0,'LED',5\r\n", 19, 300);
//					HAL_UART_Transmit(&huart3, (unsigned char *)"ABCDE\r\n", 7, 300);
//			if(strstr((char *)huart2_rx_struct_t.uart_rx_buf, "+QMTPUB:0,0,0") != NULL)
//				{
//					break ;
//				}
//			}
					
		
  }
  /* USER CODE END 3 */
}

/*******************************************************************************
* 名    称：fputc
* 功    能：支持printf输出函数
* 参数说明：无
* 返 回 值：无
* 说    明：无
* 调用方法：内部函数
********************************************************************************/
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);         //串口3发送函数
    return ch;
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_USART3;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin : PA1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD2 PD3 PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
