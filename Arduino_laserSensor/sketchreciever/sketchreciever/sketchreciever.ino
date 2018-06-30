#include <TheThingsNetwork.h>
//#include <MPU6050_tockn.h>
//#include <Wire.h>
#include "Adafruit_VL53L0X.h"

#define xShutPin1 4
#define xShutPin2 5
#define loraSerial Serial1
#define debugSerial Serial
#define freqPlan TTN_FP_EU868
#define appEui "70B3D57ED001003D"
#define appKey "9570F7E82FF0B65170BAA4E9963147F9"

Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
//MPU6050 mpu6050(Wire);
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

bool flag = false;
short int state = -1;
short int entries = 0;
short int exits = 0;

void setup()
{
  pinMode(xShutPin1, OUTPUT);
  pinMode(xShutPin2, OUTPUT);
  loraSerial.begin(57600);
  debugSerial.begin(9600);
//  Wire.begin();
  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;
//  setupAccelerometer();
  setupToFSensors();
  //  ttnSetup();
}

void ttnSetup() {
//  debugSerial.println("-- STATUS");
  ttn.showStatus();

//  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop()
{
//  recordFromAccelerometer();
  recordFromLaser();
  //  ttnLoop();
  delay(100);
}

/*void ttnLoop() {
//  debugSerial.println("-- LOOP");

  // Prepare payload of 1 byte to indicate LED status
  byte payload[1];
  payload[0] = (digitalRead(LED_BUILTIN) == HIGH) ? 1 : 0;

  // Send it off
  ttn.sendBytes(payload, sizeof(payload));

  delay(10000);
}

//void recordFromAccelerometer() {
//  mpu6050.update();

//  debugSerial.print("temp : "); debugSerial.println(mpu6050.getTemp());
//  debugSerial.print("accX : "); debugSerial.print(mpu6050.getAccX());
//  debugSerial.print("\taccY : "); debugSerial.print(mpu6050.getAccY());
//  debugSerial.print("\taccZ : "); debugSerial.println(mpu6050.getAccZ());
//
//  debugSerial.print("gyroX : "); debugSerial.print(mpu6050.getGyroX());
//  debugSerial.print("\tgyroY : "); debugSerial.print(mpu6050.getGyroY());
//  debugSerial.print("\tgyroZ : "); debugSerial.println(mpu6050.getGyroZ());
//
//  debugSerial.print("accAngleX : "); debugSerial.print(mpu6050.getAccAngleX());
//  debugSerial.print("\taccAngleY : "); debugSerial.println(mpu6050.getAccAngleY());
//
//  debugSerial.print("gyroAngleX : "); debugSerial.print(mpu6050.getGyroAngleX());
//  debugSerial.print("\tgyroAngleY : "); debugSerial.print(mpu6050.getGyroAngleY());
//  debugSerial.print("\tgyroAngleZ : "); debugSerial.println(mpu6050.getGyroAngleZ());
//
//  debugSerial.print("angleX : "); debugSerial.print(mpu6050.getAngleX());
//  debugSerial.print("\tangleY : "); debugSerial.print(mpu6050.getAngleY());
//  debugSerial.print("\tangleZ : "); debugSerial.println(mpu6050.getAngleZ());
//  debugSerial.println("=======================================================\n");
//}
//
//void setupAccelerometer() {
//  mpu6050.begin();
//  mpu6050.calcGyroOffsets(true);
//}

void setupToFSensors() {
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
//    debugSerial.println(F("tof 1 failed"));
    while (1);
  }
  digitalWrite(xShutPin2, HIGH);
  delay(50);

  if (!lox2.begin(0x29)) {
//    debugSerial.println(F("tof 2 failed"));
    while (1);
  }

}

void recordFromLaser() {
  VL53L0X_RangingMeasurementData_t measure1, measure2;
  bool sensor1 = false, sensor2 = false;
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!

  if (measure1.RangeStatus != 4) {  // phase failures have incorrect data
    sensor1 = true;
  } else {
    sensor1 = false;
  }

  //  debugSerial.print("Reading a measurement from sensor 1... ");
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  if (measure2.RangeStatus != 4) {  // phase failures have incorrect data
    //    debugSerial.print(F("Distance (mm): ")); debugSerial.println(measure2.RangeMilliMeter);
    sensor2 = true;
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
  } else if (sensor1 && flag) { // Entering
    state = 1;
  } else if (sensor2 && flag) { // Exiting
    state = 2;
  }

  switch (state) {
    case 1:
    entries++;
      debugSerial.println("ENtry recorded");
      flag = false;
      break;
    case 2:
    exits++;
      debugSerial.println("Exot recorded");
      flag = false;
      break;
    default: break;
  }
  state = 0;
}
