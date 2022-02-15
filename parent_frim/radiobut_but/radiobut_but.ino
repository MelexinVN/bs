//Кнопка брейн системы на радиомодулях NRF24l01+
//Arduino Nano
//С коментариями для изучения 15.02.2022

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"




#define GRBLED_PIN_R 6    // пин для канала R
#define GRBLED_PIN_G 5    // пин для канала G
#define GRBLED_PIN_B 9    // пин для канала B


#define butadr 0x02 // Адрес кнопки


#define butLED 6 //пин единичного светодиода

RF24 radio(8, 7);//инициализация радиомодуля CE - 8, CS - 7
const uint64_t pipes[] = { 0xF0F0F0F000LL, 0xF0F0F0F0FFLL };// адреса каналов (труб) приема и передачи
uint8_t Rdata[5] = {0};//массив для приема данных / приемный буфер
uint8_t Tdata[5] = {0};//массив для передачи данных / буфер на передачу
//uint8_t buttons[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};

volatile unsigned long time_ms = 0, time_str = 0; //время 
unsigned int time_out=0;  

bool isPushed = 0;    //флаг нажатия кнопки
volatile bool flag_rx = 0;  //флаг приема



void setup() {

  pinMode(3, INPUT);            //пин кнопки на вход
  pinMode(butLED, OUTPUT);      //светодиод кнопки на выход

 // pinMode(GRBLED_PIN_R, OUTPUT);  //красный вывод ШИМ
 // pinMode(GRBLED_PIN_G, OUTPUT);  //зеленый
 // pinMode(GRBLED_PIN_B, OUTPUT);  //синий
  
  digitalWrite(3, HIGH);  //включение пуллапа
  //  attachInterrupt(0, reset_button, LOW);
  Serial.begin(115200);//запуск уарта
  //  printf_begin();
  radio.begin();//запуск радиомодуля
  radio.setPALevel(RF24_PA_MAX);//установка максимальной мощности
  radio.openWritingPipe(pipes[1]);//открытие передающей трубы
  radio.openReadingPipe(1, pipes[0]);//открытие приемной трубы
  //radio.printDetails();
  radio.setAutoAck(0);//отключение автоматического подтверждения приема
  
Tdata[0] = butadr;  //запись адреса кнопки в нулевой элемент



  attachInterrupt(0, resiveRNF, FALLING);//прерывание от радиомодуля
  radio.startListening();//включение приема



}




void loop() {

  if (!isPushed)                //если кнопка не нажата
  {
    isPushed = !digitalRead(3); //обновление флага нажатия
    
    time_ms = millis() - time_str;  //запись времени прошедшего с прошлого сброса
   
  }

 
  
  if (flag_rx)    //если флаг приема поднят, то есть есть данные на прием
  {
    flag_rx = 0;  //сбрасываем флаг
    radio.read(Rdata, 5);   //читаем данные из буфера
  //  Serial.println(Rdata[0], HEX);
  //  Serial.println(Rdata[1], HEX);

    if (Rdata[0] == 0xFF)   //если команда сброса
    {
      //Serial.println("FF");
      time_str = millis();  //сохраняем старое значениевремени
      isPushed = 0;         //сбрасываем флаг нажатия
   


    }

    if (Rdata[0] == butadr)   //если в принятой посылке адрес этой кнопки
    {
      //Serial.print(Rdata[0]);
      //Serial.println(Rdata[1]);
    //  if (Rdata[1] == 0x00)
     // {
       

          if (isPushed)   //если кнопка считается нажатой
          {
            //Serial.print("time: ");
            Tdata[0] = butadr;  //записываем адрес
            radio.stopListening();  //остановить прием, переключение на передачу
            delay(3);               //рабочая задержка
            (*(unsigned long*)&Tdata[1]) = time_ms; //отправка времени
            radio.startWrite(Tdata, 5, 0);          //начинаем отправку
          //  delay(3);
          //  radio.startListening();
          //  delay(3);
          //  Serial.println(time_ms);
  
          }
          else
          {
            radio.stopListening();  //если кнопка не нажата, 
            Tdata[1] = 0xFF;        //отправляем FF
           // Tdata[1] =  butadr;
            radio.startWrite(Tdata, 5, 0);  //начинаем отправку
           // delay(3);
            //radio.startListening();
           // delay(3);
           
  
          }
          time_out = millis();  //сохранение времени
    //  }
      if (Rdata[1] == 0x01) //если команда - управление светодиодом
      {
        if (Rdata[2]==0x01) //если команда - включить светодиод
            digitalWrite(butLED, HIGH);
        if (Rdata[2]==0x00) //если команда - включить светодиод
            digitalWrite(butLED, LOW);
      
      }

       if (Rdata[1] == 0x02)  // если команда - ШИМ
      {//
         analogWrite(GRBLED_PIN_R, Rdata[2]);
         //Serial.println(Rdata[2]);
         analogWrite(GRBLED_PIN_G, Rdata[3]);
         //Serial.println(Rdata[3]);
         analogWrite(GRBLED_PIN_B, Rdata[4]);
         //Serial.println(Rdata[4]);
               
      }



    }

   
  }

/* if (millis()- time_out > 20)
  {
    radio.startListening();
    time_out = millis();
    Serial.println("to");
  }
*/
}


void resiveRNF()    //обработка прерывания от радиомодуля
{
  bool tx, fail, rx;//флаги

  radio.whatHappened(tx, fail, rx); // читаем регистр
  //flag_tx = tx; // 1 если успешно отправили данные
  //flag_fail = fail; // 1 если данные не отправленны
  flag_rx = rx;  // 1 если пришли данные для чтения
  /*Serial.print(tx);
  Serial.print(fail);
  Serial.println(rx);
  */
  
  
if(tx) //если успешно отправили данные
{
    radio.startListening();   // переключаемся на прием
    
}
//delay(3);

  //  radio.read( &Rdata, 2);
}
