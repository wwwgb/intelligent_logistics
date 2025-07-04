#ifndef __THREAD_RCCU_H_
#define __THREAD_RCCU_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* Includes ------------------------------------------------------------------*/
#include "mytype.h"
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void ChassisModle_Set(int model);
void ChassisCoord_Set(float _x_diff, float _y_diff, float _yaw_diff);
void ChassisSpeed_Set(float _x_spd, float _y_spd);
void ChassisRELAX_set(void);
void ChassisCoord_WaitStop(void);
int32_t Read_Position_x_mm(void);
int32_t Read_Position_y_mm(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __THREAD_RCCU_H_ */


