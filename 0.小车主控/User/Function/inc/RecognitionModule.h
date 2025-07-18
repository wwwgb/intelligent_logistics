#ifndef __RecognitionModule_H__
#define __RecognitionModule_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "mytype.h"
/* 类型定义 ------------------------------------------------------------------*/
typedef enum 
{
	RM_leisure=0,	//空闲的
	RM_start,
	RM_Colorstart,
	RM_starting,
	RM_Identify,   //二维码
	RM_error,
	RM_succeed,
	RM_Circle,      //圆识别
	RM_Location, //定位
}RecognitionModuleSte_e;
typedef struct
{
	volatile uint8_t err;
	volatile RecognitionModuleSte_e state;
	volatile RecognitionModuleSte_e RecognitionModuleSte;
	volatile uint16_t time;
	void (*RecognitionModule_Transmit)(uint8_t *pData, uint16_t Size);
}RecognitionModule_s;
/* 宏定义 --------------------------------------------------------------------*/
#define ERR_disconnect 1
#define ERR_Identification_failure 2
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void RecognitionModule_Init(RecognitionModule_s *hand,
							void (*pQ_Transmit)(uint8_t *pData, uint16_t Size));
void RecognitionModule_Scan1Ms(RecognitionModule_s *hand);
void RecognitionModule_ReceivingProcess(RecognitionModule_s *hand,uint8_t *datahend,uint16_t datalen);//接收程序
void RecognitionModule_Start(RecognitionModule_s *hand);   
void ColorRecognitionModule_Start(RecognitionModule_s *hand);
void RecognitionModule_Stop(RecognitionModule_s *hand);
void CircleRecognitionModule_Start(RecognitionModule_s *hand);
void LocationRecognitionModule_Start(RecognitionModule_s *hand);                    
#endif  // __RecognitionModule_H__
