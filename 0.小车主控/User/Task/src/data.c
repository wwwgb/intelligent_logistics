/* Includes ------------------------------------------------------------------*/
#include "data.h"
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/*******ֱ���������ģ����������*******/
#ifdef __SLAVE_DCMotorMiniwatt_H__
DCMotorMiniwattDef_t DCMotorMiniwatt_S[4];
#endif
/*******��������������*******/
GyroData_Typedef GyroData_Struct = {0};
/*******��ά��ģ����������*******/
RecognitionModule_s RecognitionModule_t;
/*******��ά����ɫ��Բλ��ģ����������*******/
RecognitionModule_s PCModule_t;
RM_cartesian cartesian_loc;
/*******��ɫ������������*******/
uint8_t bool_recognitionflag = 0; 
uint8_t Recognition_Buffer[Recognition_RX_LEN] = {0};
uint16_t ColorSequence[2][3] = {RED,GREEN,BLUE,BLUE,GREEN,RED};
uint8_t bool_colorsequenceflag = 0;
uint16_t CurrentMaterialColor = WHITE;
/*******ң����������*******/
_s_RemoteCtrl_Info sRemoteCtrl_Info = {0};
/* Exported functions --------------------------------------------------------*/
















