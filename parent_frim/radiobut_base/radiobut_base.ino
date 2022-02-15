//База брейн системы на радиомодулях NRF24l01+ 
//Arduino Nano
//С коментариями для изучения 15.02.2022

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//Сброс базы и кнопок
//#255,

//База высылает даные ввиде N пробел time
//N - номер кнопки
//time - время от сброса до нажатия

//Команда для управления RGB
//#button,cmd,R,G,B, - запятые обязательно все и самое главное последнюю
//button - адрес кнопки
//cmd - номер команды. Для RGB это 2
//R,G,B - шим по каналам 0-255
//Пример: #1,2,145,12,75,
//кнопка 1 R - 145, G - 12, B - 75,

//Команда для управления LED
//#button,cmd,I,0,0, - запятые обязательно все и самое главное последнюю
//button - адрес кнопки
//cmd - номер команды. Для LED это 1
//I - состояние светодиода (0 выключен, 1 - включен);
//Пример: #1,1,1,0,0,
//кнопка 1 включить светодиод

#define but_num 10    //количество кнопок

String  tmp="";       //временная строка
char f_c=-1, i=0;     //флаг? и индекс
int but=0, cmd=0, c_pwm_R=0, c_pwm_B=0, c_pwm_G=0;


RF24 radio(8, 7);     //инициализация радиомодуля CE - 8, CS - 7
const uint64_t pipes[] = { 0xF0F0F0F000LL, 0xF0F0F0F0FFLL };// адреса каналов (труб) приема и передачи
uint8_t Rdata[5] = {0};   //массив для приема данных / приемный буфер
uint8_t Tdata[5] = {0};   //массив для передачи данных / буфер на передачу
uint8_t buttons[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A}; //адреса кнопок
uint8_t b_cmd[] =   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //команды по умолчанию
uint8_t b_cmd_t[but_num];             // массив  команд
volatile uint8_t b_c = 0, b_first=0;  //
bool f_rx = 0, f_first=0;             //флаг приема?, флаг первого выполнения функции
int ff=0;                             //




void setup() {
  for( char i=0; i<but_num; i++) b_cmd_t[i]=b_cmd[i];   //запись во временный массив 
  pinMode(3, INPUT);                                    //пин кнопки
  digitalWrite(3, HIGH);      //включение пуллапа

  //  attachInterrupt(0, reset_button, LOW);    //прерывание на нажатие кнопки
  Serial.begin(115200);             //запуск уарта на скорости 115200
  printf_begin();                   //запуск вывода данных в порт
  radio.begin();                    //запуск радиомодуля
  radio.setPALevel(RF24_PA_MAX);    //установка максимальной мощности
  radio.openWritingPipe(pipes[0]);  //открытие передающей трубы
  radio.openReadingPipe(1, pipes[1]);//открытие приемной трубы
  radio.printDetails();//вывод в порт данных о модуле
  radio.setAutoAck(0);//отключение автоматического подтверждения приема

  // radiosend();


  attachInterrupt(0, resiveRNF, FALLING);     //включение прерывания от радиомодуля

  //сброс кнопок
  radio.stopListening();              //отключение приема и переключение на передачу
  (*(uint16_t*)Tdata) = 0xFF;         //запись команды сброса в буфер
  // printf("%d\n",Tdata);
  radio.startWrite(Tdata, sizeof(Tdata), 0);//запуск записи Just like write(), but it returns immediately. To find out what happened to the send, catch the IRQ and then call whatHappened().
 // radio.startListening();
  b_c = 0;      //счетчик кнопок

}



void loop() {
  
  if ((but>0)&&(but<but_num))   //если в переменной что то есть (принято из порта) и меньше числа кнопко, то есть является адресоми кнопки
           b_cmd_t[but-1]=cmd;  //запись команды в соответствующий элемент массива команд
    
    

   if((!digitalRead(3))||(but==255))  //если кнопка нажата или из порта пришла команда сброса
   {
    f_first=0;                        //пока нигде не использжуется                        
       radio.stopListening();         //отключение приема и переключение на передачу
       (*(uint16_t*)Tdata) = 0xFF;    //запись команды сброса в буфер
     
       radio.startWrite(Tdata, 5,0);  //запуск записи с запросом подтверждения

       for( char i=0; i<but_num; i++) b_cmd_t[i]=b_cmd[i]; //при сбросе команды по умолчанию
       
       but=0;     //сброс переменной
   }
   else
   {

      radio.stopListening();            //отключение приема и переключение на передачу
      Tdata[0] = buttons[b_c];          //запись адреса очережной кнопки
      Tdata[1] = b_cmd_t[b_c];          //запись команды очередной кнопке
      Tdata[2] = c_pwm_R;               //запись значения ШИМ красного
      Tdata[3] = c_pwm_G;               //--//--//--//зеленый
      Tdata[4] = c_pwm_B;               //////////////синий
      b_cmd_t[b_c]=0x00;                //очистка элемента буфера, означает что команда была отправлена
    //  delay(3);
      radio.startWrite(Tdata, 5,0);     //отправка данных
      delay(7);                         //рабочая задержка
      b_c++;                            //инкремент счетчика
           if (b_c == but_num)          //если максимальное значение, 
               b_c = 0;                 //обнуление счетчика
   } 

    
   

}


void resiveRNF()          //обработка прерывания от радиомодуля
{

  bool tx, fail, rx;      //флаги
  radio.whatHappened(tx, fail, rx); // читаем регистр
  //flag_tx = tx; // 1 если успешно отправили данные
  //flag_fail = fail; // 1 если данные не отправленны
  //flag_rx = rx;  // 1 если пришли данные для чтения
  //Serial.print(tx);
  //Serial.print(fail);
 // Serial.println(rx);
  if(tx)    //если успешно отправили данные
  {
    f_rx=0; //сбрасываем флаг приема?
   
    radio.startListening();   //переключаемся на прием
    
  }
    
  if (rx)   //если пришли данные для чтения
  {
    // f_rx=1;    ///поднимаем флаг приема?
    
    radio.read( &Rdata, 5);   //читаем данные из буфера (5 байт)
    {
        if (Rdata[1] != 0xFF) //если команда не FF, пришли данные
        {
    
          Serial.print(Rdata[0], HEX);  // выводим в порт адрес
          Serial.print("\t");           //табуляция
          Serial.println(*(unsigned long*)&Rdata[1]); //выводим данные - 4 байта слепленные в 1

          //при нажатии на кнопку отсылается команда на зажигание светодиода
          // b_cmd_t[Rdata[0]-1]=0x02;
          
    
        }
        else 
        {     // если FF, выводим адрес кнопки, которая не нажата
              Serial.print(Rdata[0], HEX);  // print as an ASCII-encoded hexadecimal
              Serial.print("\t"); Serial.println("np");
         
        }
           
     rx=0;          //сбрасываем флаг
    }
    f_rx=1;   //поднимаем флаг приема
    
    
 }

  
    //delay(3);} //вспомогательная задержка
}

void serialEvent() {//прием данных из порта
   
    char inChar = (char)Serial.read();  //читаем данные из порта
    
    if (inChar=='#')  //если начало посылки
    {
        f_c=0; //
    }
    if ((inChar>='0')&&((inChar<='9')))
    {
     tmp+=inChar;//если число, собираем его в строку 
     i++; //пока нигде не используется
     
    }
   
    if  (inChar==',')//если конец числа закончилось число, записали данные
      { 
        
        if (f_c==0)
                              //записываем адрес кнопки
            but=tmp.toInt();
        if (f_c==1)
                                  //записываем команду
            cmd=tmp.toInt();    
                                  //записываем данные
        if (f_c==2)
          
            c_pwm_R=tmp.toInt();
        
        if (f_c==3)
          
            c_pwm_G=tmp.toInt();
        
        if (f_c==4)
          
            c_pwm_B=tmp.toInt();
         
        f_c++;      
        i=0;  //
        tmp="";
      }
   
  
}
