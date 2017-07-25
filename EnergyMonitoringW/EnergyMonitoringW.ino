#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <EmonLib.h>

RF24 radio(9, 10);

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

EnergyMonitor emon1;

typedef struct Node {
  unsigned long cmd;
  float value = NULL;
} Node;

typedef struct Emon {
  float realPower;
  float Vrms;
  float Irms;
};

Node node;
Emon data;

void setup() {
  Serial.begin(57600);

  radio.begin();
  radio.setAutoAck(0);
  radio.setRetries(5, 15);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address[0]);
  radio.openWritingPipe(address[2]);
  radio.setChannel(0x60);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_250KBPS);
  radio.powerUp();
  radio.startListening();

  emon1.voltage(3, 233, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon1.current(2, 111.1);       // Current: input pin, calibration.
}

void RadioSend(float Value) {
  node.value = Value;
  radio.stopListening();
  radio.write(&node, sizeof(node) );
  radio.startListening();
}

void loop() {
  emon1.calcVI(20, 2000);        // Calculate all. No.of half wavelengths (crossings), time-out
  emon1.serialprint();           // Print out all variables (realpower, apparent power, Vrms, Irms, power factor)
  data.realPower       = emon1.realPower;        //extract Real Power into variable
  data.Vrms            = emon1.Vrms;             //extract Vrms into Variable
  data.Irms            = emon1.Irms;             //extract Irms into Variable
  node.cmd = NULL;
  node.value = NULL;
  // связь с миром
  while ( radio.available()) {
    uint8_t len = radio.getDynamicPayloadSize();
    Serial.println(len);
    radio.read( &node, sizeof(node) );
    switch (node.cmd) {
      // get voltage
      case 0xAF0:
        RadioSend(data.Vrms);
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
