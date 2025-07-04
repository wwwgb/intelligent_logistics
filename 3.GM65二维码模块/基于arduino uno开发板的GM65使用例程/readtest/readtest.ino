
char info;
void setup() {
Serial.begin(9600,SERIAL_8N1); //设置串口波特率9600
Serial2.begin(9600,SERIAL_8N1); //设置串口波特率9600

}

void loop() {
  if (Serial2.available() > 0){
  info= Serial2.read();//Serial.read()串口读取为串口读取函数
  Serial.println(info);
  delay(100);

}
}
//#include<string.h>
//#define ID 1
//#define NMB 11
//#define FOOD 5
//char id[ID];
//char nmb[NMB];
//char food[FOOD];
