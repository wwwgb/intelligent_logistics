#include<string.h>
#define ID 4
#define NMB 11
#define FOOD 5
char info;
char temp[20];
char id[ID];
char nmb[NMB];
char food[FOOD];
int i=0;



void setup() {
  Serial.begin(9600,SERIAL_8N1); //设置串口波特率9600
  Serial.println("START");
  }
 void loop() {
  if (Serial.available() > 0){
      i++;
      info= Serial.read();
      temp[i-1]=(char)info;
      if(i==20){
          divID(temp);
          divNMB(temp);
          divFOOD(temp);
          memset(temp,0,21);          
          Serial.println(id);
          memset(id,0,ID);
          Serial.println(nmb);
          memset(nmb,0,NMB);
          Serial.println(food);
      }     
   }
   delay(100);
}    


void divID(char a[]){
  for(int j=0;j<ID;j++){
    id[j]=a[j];
  }
}
void divNMB(char a[]){
  for(int j=0;j<NMB;j++){
   nmb[j]=a[j+ID];
  }
}
void divFOOD(char a[]){
  for(int j=0;j<FOOD;j++){
    food[j]=a[j+ID+NMB];
  }
}
