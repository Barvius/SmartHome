
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

typedef struct Node {
  int cmd;
  float value;
} Node;

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
}

Node node;

void loop() {

  float value;
  bool f = false;
  char inData[20]; // Allocate some space for the string
  char inChar; // Where to store the character read
  byte index = 0; // Index into array; where to store the character

    
  

  while (Serial.available() > 0) {
    if (index < 19){
      inChar = Serial.read(); // Read a character
      inData[index] = inChar; // Store it
      index++; // Increment where to write next
      inData[index] = '\0'; // Null terminate the string
      
    }
    f = true;
  }
  
  
  
  if (f) {
    node.value = random(1,10);
    sscanf(inData, "%x", &node.cmd);
    Serial.println(inData);
    Serial.println(node.cmd);Serial.println(node.value);
    if ( radio.write(&node, sizeof(node)) ) {                        // Send the counter variable to the other radio
      if (!radio.available()) {                           // если не получаем ответ
        Serial.println("No answer");
      } else {
        while (radio.available() ) {
          radio.read( &value, sizeof(value) );
          Serial.print(" Recieved: "); Serial.println(value);
        }
      }
    }
  }
  delay(10);
}
