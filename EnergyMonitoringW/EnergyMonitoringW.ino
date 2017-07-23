#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <EmonLib.h>

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

EnergyMonitor emon1;

typedef struct Node {
  unsigned long cmd;
  float value = NULL;
} Node;

typedef struct Emon{
  float realPower;
  float Vrms;
  float Irms;
};

Node node;
Emon data;

void setup() {
  Serial.begin(57600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  //radio.setPayloadSize(32);     //размер пакета, в байтах
  radio.enableDynamicPayloads();
  radio.openReadingPipe(2, address[0]);     //хотим слушать трубу 0
  radio.openWritingPipe(address[2]);
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)
  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

  emon1.voltage(3, 233, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(2, 111.1);       // Current: input pin, calibration.
}

void RadioSend(float Value){
  node.value = Value;
  radio.stopListening();
  radio.write(&node, sizeof(node) );
  radio.startListening();
}

void loop() {
  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  data.realPower       = emon1.realPower;        //extract Real Power into variable
  data.Vrms            = emon1.Vrms;             //extract Vrms into Variable
  data.Irms            = emon1.Irms;             //extract Irms into Variable
  node.cmd = NULL;
  node.value = NULL;
  // связь с миром
  while ( radio.available()) {
    //uint8_t len = radio.getDynamicPayloadSize();
    //Serial.println(len);
    radio.read( &node, sizeof(node) );
    switch (node.cmd) {
      // get voltage
      case 0xAF0:
        adioSend(data.Vrms);
        break;
      // get current
      case 0xAF1:
        RadioSend(data.Irms);
        break;
      // get power
      case 0xAF2:
        RadioSend(data.realPower);
        break;
    }
  }
}
