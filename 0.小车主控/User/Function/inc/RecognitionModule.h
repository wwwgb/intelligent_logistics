#ifndef __RecognitionModule_H__
#define __RecognitionModule_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "mytype.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
typedef enum 
{
	RM_leisure=0,	//���е�
	RM_start,
	RM_Colorstart,
	RM_starting,
	RM_Identify,   //��ά��
	RM_error,
	RM_succeed,
	RM_Circle,      //Բʶ��
	RM_Location, //��λ
}RecognitionModuleSte_e;
typedef struct
{
	volatile uint8_t err;
	volatile RecognitionModuleSte_e state;
	volatile RecognitionModuleSte_e RecognitionModuleSte;
	volatile uint16_t time;
	void (*RecognitionModule_Transmit)(uint8_t *pData, uint16_t Size);
}RecognitionModule_s;
/* �궨�� --------------------------------------------------------------------*/
#define ERR_disconnect 1
#define ERR_Identification_failure 2
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void RecognitionModule_Init(RecognitionModule_s *hand,
							void (*pQ_Transmit)(uint8_t *pData, uint16_t Size));
void RecognitionModule_Scan1Ms(RecognitionModule_s *hand);
void RecognitionModule_ReceivingProcess(RecognitionModule_s *hand,uint8_t *datahend,uint16_t datalen);//���ճ���
void RecognitionModule_Start(RecognitionModule_s *hand);   
void ColorRecognitionModule_Start(RecognitionModule_s *hand);
void RecognitionModule_Stop(RecognitionModule_s *hand);
void CircleRecognitionModule_Start(RecognitionModule_s *hand);
void LocationRecognitionModule_Start(RecognitionModule_s *hand);                    
#endif  // __RecognitionModule_H__
