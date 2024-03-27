/*

For ESP32 UWB or ESP32 UWB Pro

*/

#include <SPI.h>
#include "DW1000Ranging.h"

#include "link.h"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

float A = 0;
float B = 0;
float C = 0;
float D = 0;
float E = 0;
float F = 0;
float x = 0;
float y = 0;

float anchors[3][5] = {
  // x (cm), y (cm), ID (17__), range (default 0/), offset
  {0.1, 28, 1782, 0, 1.45},
  {0.1, 0.1, 1784, 0, 0.65},
  {0.1, 24.45, 1786, 0, 0.32}
};

  // Linked list of known anchors
  struct MyLink *uwb_data;

  // Short address to make it easier to reference devices
  char shortAddress[6];

void setup() {
    Serial.begin(115200);
    delay(1000);
    //init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
    //define the sketch as anchor. It will be great to dynamically change the type of module
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    //Enable the filter to smooth the distance
    //DW1000Ranging.useRangeFilter(true);

    //we start the module as a tag
    DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_LOWPOWER);

    // Initialise the array to keep track of links to all anchors 
    uwb_data = init_link();
    // Let's calculate a "short address" from the last 2 bytes of the device address
    byte* currentShortAddress = DW1000Ranging.getCurrentShortAddress();
    // sprintf(shortAddress, "%02X%02X", currentShortAddress[1], currentShortAddress[0]);
    // Serial.print(F("Short Address: "));
    // Serial.println(shortAddress);
}

void loop() {
    DW1000Ranging.loop();
    //print_link(uwb_data);
    updateAnchorsArray(uwb_data);

    trilaterate(anchors[0][0], anchors[0][1], anchors[0][3], anchors[1][0], anchors[1][1], anchors[1][3], anchors[2][0], anchors[2][1], anchors[3][3]);
}

void updateAnchorsArray(struct MyLink *p) {

  struct MyLink *temp = p;
  while (temp->next != NULL) {
    double value = String(temp->next->anchor_addr, HEX).toDouble();
    double rangeUpper = value + 0.1;
    double rangeLower = value - 0.1;

    for (int i = 0; i < 3; i++) {
      if (anchors[i][2] <= rangeUpper && anchors[i][2] >= rangeLower) {
        anchors[i][3] = temp->next->range[0];
        temp = temp->next;
        break;
      }
    }
  }

  for (int i = 0; i < 3; i++) {
    // Serial.print(anchors[i][2]);
    // Serial.print(" ");
    // Serial.print(anchors[i][3]);
    // Serial.print(" ");
  }

  // Serial.println(" ");

  return;
}

void trilaterate(float x1, float y1, float r1, float x2, float y2, float r2, float x3, float y3, float r3) {

  // Add offsets
  // r1 += anchors[0][4];
  // r2 += anchors[1][4];
  // r3 += anchors[2][4];
 
  // Serial.println("MORE AFFIRMATIONS"); // Gets printed
  A = (2*x2) - (2*x1);
  B = (2*y2) - (2*y1);
  C = pow(r1, 2) - pow(r2, 2) - pow(x1, 2) + pow(x2, 2) - pow(y1, 2) + pow(y2, 2);
  D = (2*x3) - (2*x2);
  E = (2*y3) - (2*y2);
  F = pow(r2, 2) - pow(r3, 2) - pow(x2, 2) + pow(x3, 2) - pow(y2, 2) + pow(y3, 2);
  x = (C*E - F*B) / (E*A - B*D);
  y = (C*D - A*F) / (B*D - A*E);

  // // Lock axes
  // Serial.print(0);
  // Serial.print(' ');
  // Serial.print(-100);
  // Serial.print(' ');

  // // // Debug print statements
  // // Serial.print("FL: ");
  // Serial.print(r3);
  // Serial.print(' ');

  // // Serial.print(" BL: ");
  // Serial.print(r1);
  // Serial.print(' ');

  // // Serial.print(" BR: ");
  // Serial.print(r2);
  // Serial.print(' ');

  // Coordinate print statements
  // Serial.print(" Coordinates (cm): ");
  // Serial.print(x);
  // Serial.print(" ");

  // Serial.print(", ");
  // Serial.println(y);
}

void newRange() {
//    Serial.print("from: ");
//    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
//    Serial.print("\t Range: ");
//    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
//    Serial.print(" m");
//    Serial.print("\t RX power: ");
//    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
//    Serial.println(" dBm");

  update_link(uwb_data, DW1000Ranging.getDistantDevice()->getShortAddress(), DW1000Ranging.getDistantDevice()->getRange(), DW1000Ranging.getDistantDevice()->getRXPower());


//   for (int i = 0; i < sizeof(anchors); i++) {
// //    Serial.println(String(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX).toDouble());

//     // To account for floating point errors.
//     double value = String(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX).toDouble();
//     double rangeUpper = value + 0.1;
//     double rangeLower = value - 0.1;
//     if (anchors[i][2] <= rangeUpper && anchors[i][2] >= rangeLower) {


//       anchors[i][3] = DW1000Ranging.getDistantDevice()->getRange();
// //      Serial.print(anchors[i][2]);
// //      Serial.print(" ");
// //      Serial.print(String(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX).toDouble());
// //      Serial.println(i);
//       break;
//     }
//   }
}

void newDevice(DW1000Device *device) {
//    Serial.print("ranging init; 1 device added ! -> ");
//    Serial.print(" short:");
//    Serial.println(device->getShortAddress(), HEX);
  add_link(uwb_data, device->getShortAddress());
}

void inactiveDevice(DW1000Device *device) {
//    Serial.print("delete inactive device: ");
//    Serial.println(device->getShortAddress(), HEX);
  delete_link(uwb_data, device->getShortAddress());
}


