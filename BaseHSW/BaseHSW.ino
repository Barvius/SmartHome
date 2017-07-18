
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

typedef struct Node {
  unsigned int cmd;
  float value;
} Node;

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  //radio.setPayloadSize(32);     //размер пакета, в байтах
  radio.enableDynamicPayloads();

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.openReadingPipe(1, address[1]);

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
  byte pipeNo;
  node.value = random(1, 10);
  node.cmd = 0xcfe0;
  Serial.println(node.cmd); Serial.println(node.value);
  //if ( radio.write(&node, sizeof(node)) ) {                        // Send the counter variable to the other radio
  // radio.write(&node, sizeof(node));
  //delay(1000);
  //      radio.flush_rx();
  radio.write(&node, sizeof(node), 0);
  
  //radio.openReadingPipe(0, address[1]);
  radio.startListening();
  delay(1000);
  if ( radio.available(&pipeNo)) {  // слушаем эфир со всех труб
    radio.read( &node, sizeof(node) );
    Serial.print(" From: "); Serial.print(pipeNo);
    Serial.print(" Recieved: "); Serial.print(node.value);
     Serial.print(" CMD: "); Serial.println(node.cmd, HEX);
  } else {
    Serial.println("no data");
  }

  radio.stopListening();



  delay(5000);
}
