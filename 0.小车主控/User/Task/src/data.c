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
/*******直流电机驱动模块数据声明*******/
#ifdef __SLAVE_DCMotorMiniwatt_H__
DCMotorMiniwattDef_t DCMotorMiniwatt_S[4];
#endif
/*******陀螺仪数据声明*******/
GyroData_Typedef GyroData_Struct = {0};
/*******二维码模块数据声明*******/
RecognitionModule_s RecognitionModule_t;
/*******二维码颜色、圆位置模块数据声明*******/
RecognitionModule_s PCModule_t;
RM_cartesian cartesian_loc;
/*******颜色缓存数据声明*******/
uint8_t bool_recognitionflag = 0; 
uint8_t Recognition_Buffer[Recognition_RX_LEN] = {0};
uint16_t ColorSequence[2][3] = {RED,GREEN,BLUE,BLUE,GREEN,RED};
uint8_t bool_colorsequenceflag = 0;
uint16_t CurrentMaterialColor = WHITE;
/*******遥控数据声明*******/
_s_RemoteCtrl_Info sRemoteCtrl_Info = {0};
/* Exported functions --------------------------------------------------------*/
















