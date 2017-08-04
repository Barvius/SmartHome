#include <EEPROM.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

// HW pin config
#define DHT_PIN 2
#define ONE_WIRE_BUS 3
#define PUMP_RELAY 8
#define ALARM_PIN 6
#define TEMPERATURE_PRECISION 12
RF24 radio(9, 10);

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
//28-041674c977ff 28FF6114741605AA
DeviceAddress SystemSensor = { 0x28, 0xFF, 0x61, 0x14, 0x74, 0x16, 0x05, 0xAA };
//28-0415a30f63ff
DeviceAddress ExternalSensor = { 0x28, 0xFF, 0x04, 0x15, 0xA3, 0x0F, 0x63, 0xFF };

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

typedef struct Node {
  unsigned long cmd;
  float value = NULL;
} Node;



Node node;

void DsSearch(){
  int numberOfDevices; // Number of temperature devices found
  DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
   for(int i=0;i<numberOfDevices; i++){

    if(sensors.getAddress(tempDeviceAddress, i)){
    Serial.print("Found device ");
    Serial.print(i, DEC);
    Serial.print(" with address: ");
    for (uint8_t i = 0; i < 8; i++){
    if (tempDeviceAddress[i] < 16) Serial.print("0");
    Serial.print(tempDeviceAddress[i], HEX);
    }
    Serial.println();
  }else{
    Serial.print("Found ghost device at ");
    Serial.print(i, DEC);
    Serial.print(" but could not detect address. Check power and cabling");
  }
  }
}

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup() {
  Serial.begin(57600);

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(5, 15);
  radio.enableDynamicPayloads();
  radio.setChannel(0x60);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);
  radio.openReadingPipe(0, address[0]);
  radio.openWritingPipe(address[2]);
  radio.powerUp();
  radio.startListening();

   sensors.begin();
  DsSearch();
 
  sensors.setResolution(12);

}


void RadioSend(float Value){
  node.value = Value;
  radio.stopListening();
  radio.write(&node, sizeof(node) );
  radio.startListening();
}

void loop() {
  sensors.requestTemperatures();
  float SystemTemperature = sensors.getTempC(SystemSensor);
  float ExternalTemperature = sensors.getTempC(ExternalSensor);
  node.cmd = NULL;
  node.value = NULL;
  
  // связь с миром
  while ( radio.available()) {
    //uint8_t len = radio.getDynamicPayloadSize();
    //Serial.println(len);
    radio.read( &node, sizeof(node) );
    switch (node.cmd) {
      // get temp
      case 0xBFE:
        RadioSend(SystemTemperature);
        Serial.print(SystemTemperature);
        break;
      // ext temp get
      case 0xBFC:
        RadioSend(ExternalTemperature);
        break;

    }
  }
}
