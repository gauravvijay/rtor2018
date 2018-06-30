// This is the sender.
#include "Adafruit_VL53L0X.h"
//#include <SoftwareSerial.h>

#define xShutPin1 4
#define xShutPin2 5
#define transmissionSerial Serial1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
//SoftwareSerial transmissionSerial(10, 11);

bool flag = false;
short int state = -1;
short int entries = 0;
short int exits = 0;

void setup()
{
  pinMode(xShutPin1, OUTPUT);
  pinMode(xShutPin2, OUTPUT);
  debugSerial.begin(9600);
  transmissionSerial.begin(57600);
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;
  // Put all sensors to low then delay
  digitalWrite(xShutPin1, LOW);
  digitalWrite(xShutPin2, LOW);
  delay(10);

  // Reset all sensors to high
  digitalWrite(xShutPin1, HIGH);
  digitalWrite(xShutPin2, HIGH);

  // Shut the second sensor down again
  digitalWrite(xShutPin2, LOW);
  if (!lox1.begin(0x30)) {
    debugSerial.println(F("tof 1 failed"));
    while (1);
  }

  digitalWrite(xShutPin2, HIGH);
  delay(20);

  if (!lox2.begin(0x29)) {
    debugSerial.println(F("tof 2 failed"));
    while (1);
  }

}

void loop()
{
  VL53L0X_RangingMeasurementData_t measure;
  bool sensor1 = false, sensor2 = false;
  //  debugSerial.print("Reading a measurement from sensor 2... ");
  lox1.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
//        debugSerial.print(F("Distance (mm): ")); debugSerial.println(measure.RangeMilliMeter);
    if (measure.RangeMilliMeter < 300)
      sensor1 = true;
    else sensor1 = false;
  } else {
    sensor1 = false;
  }

  //  debugSerial.print("Reading a measurement from sensor 1... ");
  lox2.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
//        debugSerial.print(F("Distance (mm): ")); debugSerial.println(measure.RangeMilliMeter);
    if (measure.RangeMilliMeter < 300)
      sensor2 = true;
    else sensor2 = false;
  } else {
//    debugSerial.println(" out of range ");
    sensor2 = false;
  }
  record(sensor1, sensor2);
}

void record(bool sensor1, bool sensor2) {
  if (sensor1 == sensor2) {
    state = 0;
    if (sensor1 == false) {
      flag = true;
    }
  } else if (sensor1 == true && flag == true) { // Entering
    state = 1;
  } else if (sensor2 == true && flag == true) { // Exiting
    state = 2;
  }
  switch (state) {
    case 1:
      entries++;
      transmissionSerial.write(entries);
      debugSerial.println("Entry recorded");
      debugSerial.println(entries);
      flag = false;
      break;
    case 2:
      exits++;
//      transmissionSerial.write(entries);
      debugSerial.println("Exit recorded");
      flag = false;
      break;
    default: break;
  }
  state = 0;
  delay(10);
}

