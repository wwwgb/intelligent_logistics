#include "data.h"
#include "stm32f4xx.h"
#include "SCServo.h"
#include "usart.h"
#include "Servo.h"
#include "thread_rccu.h"
typedef struct{
	int16_t Num1;//���ID
	int16_t Num2;
	int16_t Num3;
	int16_t Num4;
}Servo;
/* ------------------------------------------------------------*/
Servo bowl_1 		= {//1����
								784,
								2142,
								2070,//>>>>>2050
								};
Servo bowl_2 		= {//2����
								1020,
								2139,
								2083,//>>>>>2050
								};
Servo bowl_3 		= {//3����
								1268,
								2141,
								2072,//>>>>>2050
								};
/* ------------------------------------------------------------*/
Servo Red 		= {//��һ��
								3580,
								2446,
								1466
								};
Servo Green 	= {//��һ��
								3039,
								2264,
								1317
								};
Servo Blue 		= {//��һ��
								2544,
								2470,
								1498
								};
/* ------------------------------------------------------------*/
Servo Red_Stacking 		= {//�����
								3585,
								2237,
								1791	
								};
Servo Green_Stacking 	= {//�����
								3028,
								1937,
								1773
								};
Servo Blue_Stacking 	= {//�����
								2546,
								2263,
								1808
								};
/* ------------------------------------------------------------*/
		void Rest_action()//��λ����
		{
			RegWritePosEx(2, 1802, 2250, 30);
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
		}
		void Rest_action_S2()//��ʼ����
		{
			RegWritePosEx(1, 1024, 2250, 30);
			RegWritePosEx(2, 2150, 2250, 30);
			RegWritePosEx(3, 2274, 2250, 30);
			RegWritePosEx(4, 2047, 2250, 30);
			RegWriteAction();
		}
		void IdentifyColor_action()//ʶ��ԭ����ɫ����
		{
			RegWritePosEx(1, 3071, 2250, 30);
			RegWritePosEx(2, 2047, 2250, 30);
			RegWritePosEx(3, 2047, 2250, 30);
			RegWritePosEx(4, 2047, 2250, 30);
			RegWriteAction();
		}
		void IdentifyCircle_action()//�ּӹ���ʶ��Բ������
		{
			RegWritePosEx(1, 3071, 2250, 30);
			RegWritePosEx(2, 1915, 2250, 30);
			RegWritePosEx(3, 2200, 2250, 30);//2154
			RegWritePosEx(4, 2047, 2250, 30);
			RegWriteAction();
		}
		void Get_A()//ץȡԭ��������
		{
			RegWritePosEx(1, 3071, 4250, 60);
			RegWritePosEx(2, 2094, 4250, 60);
			RegWritePosEx(3, 1800, 4250, 60);
			RegWriteAction();
			HAL_Delay(100);
			WritePosEx(4, 1500, 4096, 100);//ץ
			HAL_Delay(500);
			Rest_action();//��λ
			HAL_Delay(500);
		}
		void Put_1()//�ŵ�1���루�죩
		{
			WritePosEx(1, bowl_1.Num1, 2250, 30);//ת��1���뷽��
			HAL_Delay(1800);
			RegWritePosEx(2, bowl_1.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_1.Num3, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 2047, 4095, 100);//��
			HAL_Delay(500);
		}
		void Put_2()//�ŵ�2���루�̣�
		{
			WritePosEx(1, bowl_2.Num1, 2250, 30);//ת��2���뷽��
			HAL_Delay(1400);
			RegWritePosEx(2, bowl_2.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_2.Num3, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 2047, 4096, 100);//��
			HAL_Delay(500);
		}
		void Put_3()//�ŵ�3���루����
		{
			WritePosEx(1, bowl_3.Num1, 2250, 30);//ת��3���뷽��
			HAL_Delay(800);
			RegWritePosEx(2, bowl_3.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_3.Num3, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 2047, 4096, 100);//��
			HAL_Delay(1000);
		}
		void Get_1()//ץ1�������ϣ��죩
		{
			WritePosEx(4, 2047, 4096, 100);//��
			WritePosEx(1, bowl_1.Num1, 2250, 30);//ת��1���뷽��
			HAL_Delay(500);
			RegWritePosEx(2, bowl_1.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(500);
			RegWritePosEx(3, bowl_1.Num3, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(400);
			WritePosEx(4, 1500, 4096, 100);//ץ
			HAL_Delay(200);
			RegWritePosEx(3, 2274, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(300);
			Rest_action();//��λ
			HAL_Delay(500);
		}
		void Get_2()//ץ2�������ϣ��̣�
		{
			WritePosEx(4, 2047, 4096, 100);//��
			WritePosEx(1, bowl_2.Num1, 2250, 30);//ת��2���뷽��
			HAL_Delay(800);
			RegWritePosEx(2, bowl_2.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_2.Num3, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(300);
			WritePosEx(4, 1500, 4096, 100);//ץ
			HAL_Delay(200);
			RegWritePosEx(3, 2274, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(300);
			Rest_action();//��λ
			HAL_Delay(500);
		}
		void Get_3()//ץ3�������ϣ�����
		{
			WritePosEx(4, 2047, 4096, 100);//��
			WritePosEx(1, bowl_3.Num1, 2250, 30);//ת��3���뷽��
			HAL_Delay(800);
			RegWritePosEx(2, bowl_3.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_3.Num3, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(300);
			WritePosEx(4, 1500, 4096, 100);//ץ
			HAL_Delay(200);
			RegWritePosEx(3, 2274, 1897, 30);//������
			RegWriteAction();
			HAL_Delay(300);
			Rest_action();//��λ
			HAL_Delay(500);
		}
		void Put_Red()//�ź컷������һ��
		{
			WritePosEx(1, Red.Num1, 2250, 30);//ת���컷����
			HAL_Delay(1800);
			RegWritePosEx(2, Red.Num2, 2250, 30);//��ǰ��2445
			RegWritePosEx(3, Red.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			WritePosEx(4, 2047, 4095, 50);//צ���ɿ�
			HAL_Delay(800);//300
			Rest_action();
		}
		void Put_Green()//���̻�������һ��
		{
			WritePosEx(1, Green.Num1, 2250, 30);//ת���̻�����
			HAL_Delay(1400);
			WritePosEx(3, Green.Num3, 4250, 60);
			WritePosEx(2, Green.Num2, 2250, 30);//��ǰ��
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//צ���ɿ�
			HAL_Delay(300);
			Rest_action();
		}
		void Put_Blue()//������������һ��
		{
			WritePosEx(1, Blue.Num1, 2250, 30);//ת����������
			HAL_Delay(800);
			RegWritePosEx(2, Blue.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, Blue.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//צ���ɿ�
			HAL_Delay(300);
			Rest_action();
		}
		void Put_Red_Stacking()//�ź컷�������
		{
			WritePosEx(1, Red_Stacking.Num1, 2250, 30);//ת���컷����
			HAL_Delay(1800);
			RegWritePosEx(2, Red_Stacking.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, Red_Stacking.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			WritePosEx(4, 2047, 4095, 50);//צ���ɿ�
			HAL_Delay(800);//300
			Rest_action();
		}
		void Put_Green_Stacking()//���̻��������
		{
			WritePosEx(1, Green_Stacking.Num1, 2250, 30);//ת���̻�����
			HAL_Delay(1400);
			WritePosEx(3, Green_Stacking.Num3, 4250, 60);
			WritePosEx(2, Green_Stacking.Num2, 2250, 30);//��ǰ��
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//צ���ɿ�
			HAL_Delay(300);
			Rest_action();
		}
		void Put_Blue_Stacking()//�������������
		{
			WritePosEx(1, Blue_Stacking.Num1, 2250, 30);//ת����������
			HAL_Delay(800);
			RegWritePosEx(2, Blue_Stacking.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, Blue_Stacking.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//צ���ɿ�
			HAL_Delay(300);
			Rest_action();
		}
		void Get_Red()//ץ�컷����
		{
			WritePosEx(4, 2047, 2250, 30);//צ���ɿ�
			HAL_Delay(100);
			WritePosEx(1, Red.Num1, 2250, 30);//ת���컷����
			HAL_Delay(1500);
			WritePosEx(2, Red.Num2, 2250, 30);//��ǰ��
			WritePosEx(3, Red.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 1500, 0, 0);//צ��ץ��
			HAL_Delay(500);
			Rest_action();
			HAL_Delay(500);
		}
		void Get_Green()//ץ�̻�����
		{
			WritePosEx(4, 2047, 2250, 30);//צ���ɿ�
			HAL_Delay(100);
			WritePosEx(1, Green.Num1, 2250, 30);//ת���̻�����
			HAL_Delay(1500);
			WritePosEx(3, Green.Num3, 2250, 30);
			WritePosEx(2, Green.Num2, 2250, 30);//��ǰ��
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 1500, 0, 0);//צ��ץ��
			HAL_Delay(500);
			Rest_action();
			HAL_Delay(500);
		}
		void Get_Blue()//ץ��������
		{
			WritePosEx(4, 2047, 2250, 30);//צ���ɿ�
			HAL_Delay(100);
			WritePosEx(1, Blue.Num1, 2250, 30);//ת����������
			HAL_Delay(1500);
			RegWritePosEx(2, Blue.Num2, 2250, 30);//��ǰ��
			RegWritePosEx(3, Blue.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 1500, 0, 0);//צ��ץ��
			HAL_Delay(500);
			Rest_action();
			HAL_Delay(500);
		}