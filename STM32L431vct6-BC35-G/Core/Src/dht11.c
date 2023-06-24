/*******************************************************************************
* Copyright (c) <Zhejiang Huawei Telecommunication Technologies Co., Ltd>
* All rights reserved.
* @�ļ���        : dht11
* @����          : DHT11�������ļ�
* @����          : ��������Ŀ��
* @����          : 2020/11/01
* @�汾          : V2.0.0
--------------------------------------------------------------------------------
  ����������ʹ��ָ�����̣�
  1����ӱ�������.c��.h�ļ�
  2��ͨ������DHT11_Read_Data�������õ���ʪ��ֵ
  3���û�Ҫ���ݳ���ϵͳʱ���޸�SYSCLOCK��ֵ��Ҫ����Ӱ����ʱ����
*******************************************************************************/
#include "DHT11.h"

/********************************�ڲ��궨��*************************************/
#define SYSCLOCK   80         // ϵͳʱ�ӣ�������ʱ,��Ҫ�û�����ϵͳʱ�ӽ����޸ģ�Ҫ����Ӱ�쾫ȷ��ʱ����λMhz

/********************************�ڲ����Ŷ���***********************************/	   

#define DHT11_Pin              GPIO_PIN_8        
#define DHT11_GPIO_Port        GPIOA

/********************************IOģʽ����*************************************/	
#define DHT11_IN()             {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=0<<8*2;}	   // PA8����ģʽ
#define DHT11_OUT()            {GPIOA->MODER&=~(3<<(8*2));GPIOA->MODER|=1<<8*2;}       // PB8���ģʽ

/********************************IO�ں�������***********************************/				   
#define DHT11_Clr()    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET)   
#define DHT11_Set()    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET)

#define DHT11_READ()   HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)

//******************************�ڲ���������************************************/
static void delay_us(uint16_t Nus);
static void delay_ms(uint16_t Nms);
static void DHT11_GPIO_Init(void);
static void DHT11_Reset(void);
static uint8_t DHT11_Read_Bit(void);
static uint8_t DHT11_Read_Byte(void);
    
/*******************************************************************************
* ��    �ƣ�Delay_us
* ��    �ܣ�΢�뼶��ʱ
* ����˵����Nus:��ʱ��ʱ�䣬��λus
* �� �� ֵ����
* ˵    ��������ϵͳʱ�ӣ�������ʱ
* ���÷������ڲ�����
*******************************************************************************/
static void delay_us(uint16_t Nus)
{
    uint32_t i;
    i = Nus * (SYSCLOCK/3);        
    while(i--);
}

/*******************************************************************************
* ��    �ƣ�Delay_ms
* ��    �ܣ�΢�뼶��ʱ
* ����˵����Nus:��ʱ��ʱ�䣬��λms
* �� �� ֵ����
* ˵    ��������ϵͳʱ�ӣ�������ʱ
* ���÷������ڲ�����
*******************************************************************************/
static void delay_ms(uint16_t Nms)
{
    uint32_t i;
    i = Nms * (SYSCLOCK/3) * 1000;        
    while(i--);
}

/*******************************************************************************
* ��    �ƣ�DHT11_GPIO_Init
* ��    �ܣ���ʼ��DHT11����
* ����˵������
* �� �� ֵ����
* ˵    ������������DHT11���Ź���ģʽ
* ���÷������ڲ�����
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
* ��    �ƣ�DHT11_Reset
* ��    �ܣ���λDHT11ģ�飬׼����ȡ����
* ����˵������
* �� �� ֵ����
* ˵    ���������ֲ���ʾ��IO��������18ms
* ���÷������ڲ�����
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
* ��    �ƣ�DHT11_Read_Bit
* ��    �ܣ���ȡDHT11һ��λ������
* ����˵������
* �� �� ֵ��1/0
* ˵    ��������ֵ��λ��ǰ����λ�ں�
* ���÷������ڲ�����
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
* ��    �ƣ�DHT11_Read_Byte
* ��    �ܣ���ȡDHT11һ���ֽڵ�����
* ����˵������
* �� �� ֵ��data:DHT11������
* ˵    ������λ��ǰ����λ�ں�
* ���÷������ڲ�����
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
* ��    �ƣ�DHT11_Read_Data
* ��    �ܣ���ȡDHT11������
* ����˵������
* �� �� ֵ��data:DHT11������
* ˵    ������λ���ݣ�ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ+У��λ
* ���÷������ڲ�����
*******************************************************************************/
uint8_t DHT11_Read_Data(float *temp,float *humidity)
{
    uint8_t i = 0;
    uint8_t buffer[5]={0};
    
    /****************************** code start 2 ******************************/
    DHT11_Reset();                      //��λ������
    
    if (DHT11_READ() == RESET)          //����ⲿ�͵�ƽ�ź�
    {
        //��⵽DHT11��Ӧ
        while (DHT11_READ() == RESET);  //��⴫������Ӧ�ĵ͵�ƽ�ź�
        while (DHT11_READ() == SET);    //��⴫������Ӧ�ĸߵ�ƽ�ź�
        for (i = 0; i < 5; i++)
        {
            buffer[i] = DHT11_Read_Byte();  //��ȡ����������
        }  
        while (DHT11_READ() == RESET);      //��⴫���������ź�
        DHT11_OUT();
        DHT11_Set();
        
        uint8_t checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
        if (checksum != buffer[4])          //�ж�У��λ
        {            
            return 1;   //�õ������쳣
        }
        *humidity = (float)(buffer[0] + buffer[1] / 10.0);  //����ʪ��ֵ
        *temp = (float)(buffer[2] + buffer[3] / 10.0);      //�����¶�ֵ
    }
    /******************************* code end 2 *******************************/
    
    return 0;    
}   
void DHT_Init(void)
{
    DHT11_GPIO_Init();   
}    












