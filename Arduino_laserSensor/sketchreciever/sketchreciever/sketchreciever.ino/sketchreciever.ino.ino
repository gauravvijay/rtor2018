#include <TheThingsNetwork.h>
#include "SoftwareSerial.h"
#define loraSerial Serial1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868
#define appEui "70B3D57ED0010313"
#define appKey "C4CCAB28C16126308F1FB56959EFE3A9"

SoftwareSerial transmissionSerial(10, 11);
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
int people = 0;
void setup() {
 loraSerial.begin(57600);
 debugSerial.begin(9600);
 transmissionSerial.begin(4800);
 
 // Wait a maximum of 10s for Serial Monitor
 while (!debugSerial && millis() < 10000)
   ;

 if (transmissionSerial.available()) {
  debugSerial.println("transmissino serial availabnl");
  }
 debugSerial.println("-- STATUS");
 ttn.showStatus();

 debugSerial.println("-- JOIN");
 ttn.join(appEui, appKey);
}

void loop()
{
 debugSerial.println(people);
 
 people = (transmissionSerial.read());
 debugSerial.println(people);
 debugSerial.println("Got people ");
 debugSerial.println(people);
 if (people == 0) {
   delay(20);
   return;
 }
 // Prepare payload of 1 byte to indicate LED status
 byte payload[1];
 payload[0] = (digitalRead(LED_BUILTIN) == HIGH) ? 1 : 0;
 // Send it off
// char mystr[2];
// ttn.sendBytes(itoa(people, mystr), sizeof(2));
 delay(10000);
}
