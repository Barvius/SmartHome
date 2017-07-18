#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля
#include <DHT.h>


#define DHTPIN 2
#define ONE_WIRE_BUS 3
#define PUMP_RELAY 8

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

DHT dht(DHTPIN, DHT11);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
//DeviceAddress SystemAddr, outsideThermometer;

DeviceAddress SystemSensor = { 0x28, 0xFF, 0x61, 0x14, 0x74, 0x16, 0x05, 0xAA };
DeviceAddress ExternalSensor = { 0x28, 0xFF, 0x04, 0x16, 0x74, 0xC9, 0x77, 0xFF };

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

typedef struct Node {
  unsigned int cmd;
  float value;
} Node;

typedef struct HeatingSystem {
  float TempOn;
  float TempOff;
  bool Mode;
};

Node node;
HeatingSystem HS;

void setup() {
  Serial.begin(9600); //открываем порт для связи с ПК

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  //radio.setPayloadSize(32);     //размер пакета, в байтах
  radio.enableDynamicPayloads();

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.openWritingPipe(address[1]);

  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль

  dht.begin();

  sensors.begin();
  sensors.setResolution(12);

  pinMode(PUMP_RELAY, OUTPUT);
  digitalWrite(PUMP_RELAY, HIGH); 
}


void loop() {
  bool send = false;
  sensors.requestTemperatures();
  
  float humidity = dht.readHumidity();
  float SystemTemperature = sensors.getTempC(SystemSensor);
  float ExternalTemperature = sensors.getTempC(ExternalSensor);
  Serial.println(sensors.getDeviceCount());
  if(HS.Mode){// auto
    if(SystemTemperature > HS.TempOn && digitalRead(PUMP_RELAY)){
      digitalWrite(PUMP_RELAY, LOW);
    }
    if(SystemTemperature < HS.TempOff && !digitalRead(PUMP_RELAY)){
      digitalWrite(PUMP_RELAY, HIGH);
    }
  }
  
  byte pipeNo;
  while ( radio.available(&pipeNo)) {  // слушаем эфир со всех труб
    radio.read( &node, sizeof(node) );         // чиатем входящий сигнал
    switch (node.cmd) {
      case 0xCEA:
        HS.TempOn = node.value;
        break;
      case 0xCFA:
        send = true;
        node.value = HS.TempOn;
        break;
      case 0xCEB:
        HS.TempOff = node.value;
        break;
      case 0xCFB:
        send = true;
        node.value = HS.TempOff;
        break;
      case 0xCEF:
        HS.Mode = (bool)node.value;
        break;
      case 0xCFF:
        send = true;
        node.value = HS.Mode;
        break;
      case 0xCEC:

        break;
      case 0xCFC:

        break;
      case 0xCFD:

        break;
      case 0xCFE0:
        send = true;
        node.value = SystemTemperature;
        break;
      case 0xCFE1:
        send = true;
        node.value = humidity;
        break;
    }
    if (send) {
      radio.stopListening();
      radio.write(&node, sizeof(node) );
      radio.startListening();
    }
  }

}
