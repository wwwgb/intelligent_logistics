#ifndef _BUS_SERVO_H_
#define _BUS_SERVO_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
/* Private macros ------------------------------------------------------------*/
//SCS    SMSBL
#define SERVO_POTENTIOMETER      0 //���ֽ���ǰ���ֽ��ں�
#define SERVO_MAGNETICENCODER    1 //���ֽ���ǰ���ֽ��ں�

#define SERVO_UART_Tx_BuffSize   43

#define SERVO_FH1_DATA           0xFF
#define SERVO_FH2_DATA           0xFF

#define SERVO_ID_BRO             0xFE

#define SERVO_INS_PING           0x01  //����ѯ����ѯ����״̬
#define SERVO_INS_READ           0x02  //��������ѯ���Ʊ�����ַ�
#define SERVO_INS_WRITE          0x03  //��д�������Ʊ���д���ַ�
#define SERVO_INS_REGWRITE       0x04  //���첽д��������WRITE DATA�����ǿ����ַ�д��󲢲����϶�����ֱ��ACTIONָ���
#define SERVO_INS_ACTION         0x05  //��ִ���첽д������REG WRITEд��Ķ�
#define SERVO_INS_SYCNREAD       0x82  //��ͬ����������ͬʱ��ѯ������
#define SERVO_INS_SYCNWRITE      0x83  //��ͬ��д������ͬʱ���ƶ�����
#define SERVO_INS_RESET          0x06  //����λ���ѿ��Ʊ�λΪ����ֵ

#define SERVO_CTRLLIST_SETID     0x05  //����ID�ŵĵ�ַ
#define SERVO_CTRLLIST_TARLOC    0x2A  //Ŀ��λ�õ��׵�ַ
/* Private types -------------------------------------------------------------*/
typedef union
{
	uint8_t Datum[SERVO_UART_Tx_BuffSize];
	struct{
		uint8_t FH1;         //֡ͷ1
		uint8_t FH2;         //֡ͷ2
		uint8_t id;          //ID
		uint8_t length;      //���ݳ���
		uint8_t instruction; //ָ��
		uint8_t data[SERVO_UART_Tx_BuffSize-5];
	}DATE;
}BusServo_TypeDef;
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
