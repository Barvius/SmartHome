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

typedef struct HeatingSystem {
  float TempOn;
  float TempOff;
  bool Mode;
};

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);     //размер пакета, в байтах


  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
}

Node node;
HeatingSystem HS;

void loop() {
  byte pipeNo;
  while ( radio.available(&pipeNo)) {  // слушаем эфир со всех труб
    radio.read( &node, sizeof(node) );         // чиатем входящий сигнал
    //Serial.print("From: "); Serial.print(pipeNo);
    //Serial.print(" Recieved: "); Serial.print(node.value);
    //Serial.print(" Params: "); Serial.println(node.cmd);

    switch (node.cmd) {
      case 0xCEA:
        HS.TempOn = node.value;
        break;
      case 0xCFA:
        radio.writeAckPayload(pipeNo, &HS.TempOn, sizeof(float) );
        break;
      case 0xCEB:
        HS.TempOff = node.value;
        break;
      case 0xCFB:
        radio.writeAckPayload(pipeNo, &HS.TempOff, sizeof(float) );
        break;
      case 0xCEF:
        HS.Mode = (bool)node.value;
        break;
      case 0xCFF:
        radio.writeAckPayload(pipeNo, &HS.Mode, sizeof(bool) );
        break;
      case 0xCEC:
        
        break;
      case 0xCFC:
        
        break;
      case 0xCFD:
        
        break;
    }
  }
}