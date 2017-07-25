#include <SPI.h>
#include <Wire.h>
#include <OneWire.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DallasTemperature.h>
#include <PCF8574.h>
#include <AM2320.h>
#include <BMP085.h>

// HW pin config
#define AM_PIN 2
#define ONE_WIRE_BUS 3

RF24 radio(9, 10);

PCF8574 PCF(0x27);
BMP085 PressureSensor = BMP085();
AM2320 HumiditySensor;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress TemperatureSensor = { 0x28, 0xFF, 0x61, 0x14, 0x74, 0x16, 0x05, 0xAA };
//DeviceAddress ExternalSensor = { 0x28, 0xFF, 0x04, 0x16, 0x74, 0xC9, 0x77, 0xFF };

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

typedef struct Node {
  unsigned long cmd;
  float value = NULL;
} Node;

Node node;

void setup() {
  Serial.begin(57600);

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address[0]);
  radio.openWritingPipe(address[3]);
  radio.setChannel(0x60);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_250KBPS);
  radio.powerUp();
  radio.startListening();

  sensors.begin();
  sensors.setResolution(12);

  PCF.write8(1); // off all

  Wire.begin();
  ///dps.init(MODE_STANDARD, 3200, true);
  PressureSensor.init();
}

void RadioSend(float Value){
  node.value = Value;
  radio.stopListening();
  radio.write(&node, sizeof(node) );
  radio.startListening();
}

void loop() {
  sensors.requestTemperatures();
  HumiditySensor.Read();
  float Temperature = sensors.getTempC(TemperatureSensor);
  long Pressure;
  PressureSensor.getPressure(&Pressure);

  node.cmd = NULL;
  node.value = NULL;
  // связь с миром
  while ( radio.available()) {
    //uint8_t len = radio.getDynamicPayloadSize();
    //Serial.println(len);
    radio.read( &node, sizeof(node) );
    switch (node.cmd) {
      // set lamp state
      case 0xBEF0:
        PCF.write(0, (bool)node.value);
        break;
      case 0xBEF1:
        PCF.write(1, (bool)node.value);
        break;
      case 0xBEF2:
        PCF.write(2, (bool)node.value);
        break;
      case 0xBEF3:
        PCF.write(3, (bool)node.value);
        break;
      case 0xBEF4:
        PCF.write(4, (bool)node.value);
        break;
      case 0xBEF5:
        PCF.write(5, (bool)node.value);
        break;
      case 0xBEF6:
        PCF.write(6, (bool)node.value);
        break;
      case 0xBEF7:
        PCF.write(7, (bool)node.value);
        break;
      // get lamp state
      case 0xBFF0:
        RadioSend(PCF.read(0));
        break;
      case 0xBFF1:
        RadioSend(PCF.read(1));
        break;
      case 0xBFF2:
        RadioSend(PCF.read(2));
        break;
      case 0xBFF3:
        RadioSend(PCF.read(3));
        break;
      case 0xBFF4:
        RadioSend(PCF.read(4));
        break;
      case 0xBFF5:
        RadioSend(PCF.read(5));
        break;
      case 0xBFF6:
        RadioSend(PCF.read(6));
        break;
      case 0xBFF7:
        RadioSend(PCF.read(7));
        break;
      // get temperature
      case 0xBFA:
        RadioSend(Temperature);
        break;
      // get humidity
      case 0xBFB:
        RadioSend(HumiditySensor.h);
        break;
      // get pressure
      case 0xBFC:
        RadioSend(Pressure);
        break;

    }
  }
}
