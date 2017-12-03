#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3f, 16, 2);

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

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(5, 15);
  radio.enableDynamicPayloads();
  radio.setChannel(0x6f);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);
  //radio.setDataRate (RF24_250KBPS);
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
  node.value = NAN;
  radio.stopListening();
  delay(5);
  if (radio.write(&node, sizeof(node) )) {
   
  }
  delay(5);
  radio.startListening();

  delay(1000);
  while ( radio.available()) {
    radio.read( &node, sizeof(node) );

  }
}

void loop() {
  //delay(10);
  // put your main code here, to run repeatedly:

  lcd.clear();
  //node.cmd = 0xBFC;
  node.cmd = 0xCFC0;
  node.value = NULL;
  ask();
  lcd.print("T");
  lcd.setCursor(1, 0);
  lcd.print(node.value);

  node.cmd = 0xCFC1;
  node.value = NULL;
  ask();
  lcd.setCursor(9, 0);
  lcd.print("B");
  lcd.setCursor(10, 0);
  lcd.print(node.value);
  
  node.cmd = 0xCFE;
  node.value = NULL;
  ask();
  lcd.setCursor(0, 1);
  lcd.print("K");
  lcd.setCursor(1, 1);
  lcd.print(node.value);

  node.cmd = 0xCFF;
  node.value = NULL;
  ask();
  lcd.setCursor(9, 1);
  lcd.print("H-");
  lcd.setCursor(11, 1);
  //lcd.print(node.value);
  if (node.value) {
    lcd.print("OFF");
  } else {
    lcd.print("ON");
  }

  delay(1000);
/*
lcd.clear();
node.cmd = 0xBFC;
  node.value = NULL;
  ask();
  lcd.print("P");
  lcd.setCursor(1, 0);
  lcd.print(node.value);
  
  node.cmd = 0xBFA;
  node.value = NULL;
  ask();
  lcd.setCursor(9, 0);
  lcd.print("B");
  lcd.setCursor(10, 0);
  lcd.print(node.value);
  delay(500);
*/
}

