#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <ctime>
#include <cerrno>
#include <RF24/RF24.h>

using namespace std;

RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);

const uint8_t address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

typedef struct Node {
  unsigned long cmd;
  float value;
} Node;

Node node;

int main(int argc, char* argv[]) {
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  //radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
  radio.setChannel(0x60);
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);
  radio.powerUp();
  radio.printDetails();
  delay(20);
  radio.stopListening();
  
  uint8_t pipeNo;
  node.value = atoi(argv[2]);
  node.cmd = strtol(argv[1], NULL, 16);
  //node.cmd = 0xcfe0;
  printf("%lu %f\n", node.cmd, node.value);
  if (radio.write(&node, sizeof(node))) {
    radio.startListening();
    delay(1500);
    uint8_t len = radio.getDynamicPayloadSize();
    //printf("b -- %d\n", len);
    if ( radio.available(&pipeNo)) {
      radio.read( &node, len );
      printf("from - %d %lu %f\n", pipeNo, node.cmd, node.value);
    } else {
      printf("no data\n");
    }
  }
  radio.stopListening();
  radio.powerDown();
  return 0;
}

