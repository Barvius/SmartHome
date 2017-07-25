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
// EEPROM adress
#define TEMPON_ADDR 0
#define TEMPOFF_ADDR 1
#define MODE_ADDR 2
#define TEMPALARM_ADDR 3
#define ALARM_ADDR 4

RF24 radio(9, 10);

DHT dht(DHT_PIN, DHT11);

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

DeviceAddress SystemSensor = { 0x28, 0xFF, 0x61, 0x14, 0x74, 0x16, 0x05, 0xAA };
DeviceAddress ExternalSensor = { 0x28, 0xFF, 0x04, 0x16, 0x74, 0xC9, 0x77, 0xFF };

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

typedef struct Node {
  unsigned long cmd;
  float value = NULL;
} Node;

typedef struct HeatingSystem {
  int TempOn;
  int TempOff;
  int TempAlarm;
  int  Mode;
  int  Alarm;
};

Node node;
HeatingSystem HS;

void setup() {
  Serial.begin(57600);

  radio.begin();
  radio.setAutoAck(0);
  radio.setRetries(5, 15);
  //radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address[0]);
  radio.openWritingPipe(address[1]);
  radio.setChannel(0x60);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_250KBPS);
  radio.powerUp();
  radio.startListening();

  dht.begin();

  sensors.begin();
  sensors.setResolution(12);

  pinMode(PUMP_RELAY, OUTPUT);
  digitalWrite(PUMP_RELAY, HIGH);
  pinMode(ALARM_PIN, OUTPUT);

  HS.TempOn = EEPROM.read(TEMPON_ADDR);
  HS.TempOff = EEPROM.read(TEMPOFF_ADDR);
  HS.Mode = EEPROM.read(MODE_ADDR);
  HS.Alarm = EEPROM.read(ALARM_ADDR);
  HS.TempAlarm = EEPROM.read(TEMPALARM_ADDR);
}

bool WriteEEPROM(int *Value, int NewValue, uint8_t addr) {
  if (*Value != NewValue) {
    *Value = NewValue;
    EEPROM.write(addr, NewValue);
    return true;
  }
  return false;
}

void RadioSend(float Value){
  node.value = Value;
  radio.stopListening();
  radio.write(&node, sizeof(node) );
  radio.startListening();
}

void loop() {
  sensors.requestTemperatures();
  float humidity = dht.readHumidity();
  float SystemTemperature = sensors.getTempC(SystemSensor);
  float ExternalTemperature = sensors.getTempC(ExternalSensor);
  node.cmd = NULL;
  node.value = NULL;
  //auto control
  if (HS.Mode) { // auto
    if (SystemTemperature > HS.TempOn && digitalRead(PUMP_RELAY)) {
      digitalWrite(PUMP_RELAY, LOW);
    }
    if (SystemTemperature < HS.TempOff && !digitalRead(PUMP_RELAY)) {
      digitalWrite(PUMP_RELAY, HIGH);
    }
  }
  // alarm beep
  if (HS.Alarm && SystemTemperature > HS.TempAlarm) {
    tone(ALARM_PIN, 2750, 500);
  } else {
    noTone(ALARM_PIN);
  }
  // связь с миром
  while ( radio.available()) {
    //uint8_t len = radio.getDynamicPayloadSize();
    //Serial.println(len);
    radio.read( &node, sizeof(node) );
    switch (node.cmd) {
      // set max temp
      case 0xCEA1:
        WriteEEPROM(&HS.TempOn,node.value,TEMPON_ADDR);
        break;
      // get max temp
      case 0xCFA1:
        RadioSend(HS.TempOn);
        break;
      // set min temp
      case 0xCEA2:
        WriteEEPROM(&HS.TempOff,node.value,TEMPOFF_ADDR);
        break;
      // ge min temp
      case 0xCFA2:
        RadioSend(HS.TempOff);
        break;
      // set mode
      case 0xCEA0:
        WriteEEPROM(&HS.Mode,node.value,MODE_ADDR);
        break;
      // get mode
      case 0xCFA0:
        RadioSend(HS.Mode);
        break;
      // set rel state
      case 0xCEF:
        digitalWrite(PUMP_RELAY, (bool)node.value);
        break;
      // get rel state
      case 0xCFF:
        RadioSend(digitalRead(PUMP_RELAY));
        break;
      // get alarm mode
      case 0xCFB0:
        RadioSend(HS.Alarm);
        break;
      // get temp alarm
      case 0xCFB1:
        RadioSend(HS.TempAlarm);
        break;
      // mode alarm set
      case 0xCEB0:
         WriteEEPROM(&HS.Alarm,node.value,ALARM_ADDR);
        break;
      // temp alarm set
      case 0xCEB1:
        WriteEEPROM(&HS.TempAlarm,node.value,TEMPALARM_ADDR);
        break;
      // get temp
      case 0xCFE:
        RadioSend(SystemTemperature);
        break;
      // ext temp get
      case 0xCFC0:
        RadioSend(ExternalTemperature);
        break;
      // ext humi get
      case 0xCFC1:
        RadioSend(humidity);
        break;
    }
  }
}
