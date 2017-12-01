#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3c, 16, 2);

RF24 radio(9, 10);

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

typedef struct Node {
  unsigned long cmd;
  float value = NULL;
} Node;

Node node;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(5, 15);
  radio.enableDynamicPayloads();
  radio.setChannel(0x60);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);
  radio.openReadingPipe(1, address[1]);
  radio.openReadingPipe(2, address[2]);
  radio.openReadingPipe(3, address[3]);
  radio.openWritingPipe(address[0]);
  radio.powerUp();
  radio.startListening();
  radio.printDetails();

  lcd.begin();
  lcd.backlight();
}


void RadioSend(float Value) {
  node.value = Value;
  radio.stopListening();
  delay(100);
  if (radio.write(&node, sizeof(node) )) {
    Serial.println("retry");
  }
  //radio.write(&node, sizeof(node) );
  delay(100);
  radio.startListening();
  delay(100);
}

void ask() {
  radio.stopListening();
  delay(100);
  if (radio.write(&node, sizeof(node) )) {
    delay(100);
    radio.startListening();
    delay(100);
    while ( radio.available()) {
      radio.read( &node, sizeof(node) );

    }
  }

}

void loop() {
  //delay(10);
  // put your main code here, to run repeatedly:
  node.cmd = 0xCFE;
  node.value = NULL;
  lcd.print("Hello, world!");
  delay(1000);

}

