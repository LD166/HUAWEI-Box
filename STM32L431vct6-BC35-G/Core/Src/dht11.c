/*******************************************************************************
* Copyright (c) <Zhejiang Huawei Telecommunication Technologies Co., Ltd>
* All rights reserved.
* @文件名        : dht11
* @描述          : DHT11测驱动文件
* @作者          : 物联网项目组
* @日期          : 2020/11/01
* @版本          : V2.0.0
--------------------------------------------------------------------------------
  本驱动函数使用指导流程：
  1、添加本驱动的.c和.h文件
  2、通过调用DHT11_Read_Data函数，得到温湿度值
  3、用户要根据程序系统时钟修改SYSCLOCK的值，要不会影响延时精度
*******************************************************************************/
#include "DHT11.h"

/********************************内部宏定义*************************************/
#define SYSCLOCK   80         // 系统时钟，用于延时,需要用户根据系统时钟进行修改，要不会影响精确延时，单位Mhz

/********************************内部引脚定义***********************************/	   

#define DHT11_Pin              GPIO_PIN_8        
#define DHT11_GPIO_Port        GPIOA

/********************************IO模式设置*************************************/	
#define DHT11_IN()             {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=0<<8*2;}	   // PA8输入模式
#define DHT11_OUT()            {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=1<<8*2;}       // PB8输出模式

/********************************IO口函数操作***********************************/				   
#define DHT11_Clr()    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET)   
#define DHT11_Set()    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET)

#define DHT11_READ()   HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)

//******************************内部函数定义************************************/
static void delay_us(uint16_t Nus);
static void delay_ms(uint16_t Nms);
static void DHT11_GPIO_Init(void);
static void DHT11_Reset(void);
static uint8_t DHT11_Read_Bit(void);
static uint8_t DHT11_Read_Byte(void);
    
/*******************************************************************************
* 名    称：Delay_us
* 功    能：微秒级延时
* 参数说明：Nus:延时的时间，单位us
* 返 回 值：无
* 说    明：根据系统时钟，进行延时
* 调用方法：内部函数
*******************************************************************************/
static void delay_us(uint16_t Nus)
{
    uint32_t i;
    i = Nus * (SYSCLOCK/3);        
    while(i--);
}

/*******************************************************************************
* 名    称：Delay_ms
* 功    能：微秒级延时
* 参数说明：Nus:延时的时间，单位ms
* 返 回 值：无
* 说    明：根据系统时钟，进行延时
* 调用方法：内部函数
*******************************************************************************/
static void delay_ms(uint16_t Nms)
{
    uint32_t i;
    i = Nms * (SYSCLOCK/3) * 1000;        
    while(i--);
}

/*******************************************************************************
* 名    称：DHT11_GPIO_Init
* 功    能：初始化DHT11引脚
* 参数说明：无
* 返 回 值：无
* 说    明：用于配置DHT11引脚工作模式
* 调用方法：内部函数
*******************************************************************************/
static void DHT11_GPIO_Init(void)
{					     
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(DHT11_GPIO_Port,GPIO_PIN_8,GPIO_PIN_RESET);   
}

/*******************************************************************************
* 名    称：DHT11_Reset
* 功    能：复位DHT11模块，准备读取数据
* 参数说明：无
* 返 回 值：无
* 说    明：根据手册显示，IO至少拉低18ms
* 调用方法：内部函数
*******************************************************************************/
static void DHT11_Reset(void)
{
    DHT11_OUT();    
    DHT11_Clr();
    delay_ms(19);
    DHT11_Set();
    delay_us(30);
    DHT11_IN();
}

/*******************************************************************************
* 名    称：DHT11_Read_Bit
* 功    能：读取DHT11一个位的数据
* 参数说明：无
* 返 回 值：1/0
* 说    明：数据值高位在前，低位在后
* 调用方法：内部函数
*******************************************************************************/
static uint8_t DHT11_Read_Bit(void)
{
    while (DHT11_READ() == RESET);
    delay_us(40);
    if (DHT11_READ() == SET)
    {
        while (DHT11_READ() == SET);
        return 1;
    }
    else
    {
        return 0;
    }
}

/*******************************************************************************
* 名    称：DHT11_Read_Byte
* 功    能：读取DHT11一个字节的数据
* 参数说明：无
* 返 回 值：data:DHT11的数据
* 说    明：高位在前，低位在后
* 调用方法：内部函数
*******************************************************************************/
static uint8_t DHT11_Read_Byte(void)
{
    uint8_t i;
    uint8_t data = 0;
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= DHT11_Read_Bit();
    }
    return data;
}

/*******************************************************************************
* 名    称：DHT11_Read_Data
* 功    能：读取DHT11的数据
* 参数说明：无
* 返 回 值：data:DHT11的数据
* 说    明：五位数据，湿度高位+湿度低位+温度高位+温度低位+校验位
* 调用方法：内部函数
*******************************************************************************/
uint8_t DHT11_Read_Data(float *temp,float *humidity)
{
    uint8_t i = 0;
    uint8_t buffer[5]={0};
    
    /****************************** code start 2 ******************************/
    DHT11_Reset();                      //复位传感器
    
    if (DHT11_READ() == RESET)          //检测外部低电平信号
    {
        //检测到DHT11响应
        while (DHT11_READ() == RESET);  //检测传感器响应的低电平信号
        while (DHT11_READ() == SET);    //检测传感器响应的高电平信号
        for (i = 0; i < 5; i++)
        {
            buffer[i] = DHT11_Read_Byte();  //读取传感器数据
        }  
        while (DHT11_READ() == RESET);      //检测传感器结束信号
        DHT11_OUT();
        DHT11_Set();
        
        uint8_t checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
        if (checksum != buffer[4])          //判断校验位
        {            
            return 1;   //得到数据异常
        }
        *humidity = (float)(buffer[0] + buffer[1] / 10.0);  //计算湿度值
        *temp = (float)(buffer[2] + buffer[3] / 10.0);      //计算温度值
    }
    /******************************* code end 2 *******************************/
    
    return 0;    
}   
void DHT_Init(void)
{
    DHT11_GPIO_Init();   
}    












