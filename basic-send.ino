// Libraries
#include <IridiumSBD.h>
#include <SoftwareSerial.h>

/*
 * BasicSend
 * 
 * This sketch sends a "Hello, world!" message from the satellite modem.
 * If you have activated your account and have credits, this message
 * should arrive at the endpoints you have configured (email address or
 * HTTP POST).
 * 
 * Assumptions
 * 
 * The sketch assumes an Arduino Mega or other Arduino-like device with
 * multiple HardwareSerial ports.  It assumes the satellite modem is
 * connected to Serial3.  Change this as needed.  SoftwareSerial on an Uno
 * works fine as well.
 */

// Rockblock settings
#define STATUS 13            // onboard pin to determine if modem engaged.
#define ROCKBLOCK_RX_PIN 11  // Recieve data pin from Rockblock (seial data from RockBLOCK)
#define ROCKBLOCK_TX_PIN 10  // Transmit data pin to Rockblock (serial data to RockBLOCK)
// #define ROCKBLOCK_SLEEP_PIN 4 // on/off pin for power savings
#define ROCKBLOCK_BAUD 19200  // serial modem communication baud rate
#define CONSOLE_BAUD 115200   // serial terminal communications baud rate
#define DIAGNOSTICS true      // Set "true" to see serial diagnostics

// Modem communication
SoftwareSerial IridiumSerial(ROCKBLOCK_RX_PIN, ROCKBLOCK_TX_PIN);  // Setting SoftwareSerial transmit + receive pins
IridiumSBD modem(IridiumSerial);                                   // Declare the IridiumSBD object
// IridiumSBD modem(IridiumSerial, ROCKBLOCK_SLEEP_PIN);            // if want to add sleep pin

void setup() {
  int signalQuality = -1;
  int err;

  // Start the console serial port
  Serial.begin(CONSOLE_BAUD);
  pinMode(STATUS, OUTPUT);  // LED on 13 for reporting status on modem communication
  while (!Serial)
    ;

  // Start the serial port connected to the satellite modem
  IridiumSerial.begin(ROCKBLOCK_BAUD);
  IridiumSerial.listen();
  if (IridiumSerial.isListening()) {
    Serial.println("IridiumSerial is listening!");
  } else {
    Serial.println("IridiumSeria is not listening!");
  }

  // Begin satellite modem operation
  Serial.println("Starting modem...");
  err = modem.begin();
  if (err != ISBD_SUCCESS) {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    return;
  } else if (modem.begin() == ISBD_SUCCESS) {
    // Awoken okay, so try sending a text
    digitalWrite(STATUS, HIGH);  // modem awoke okay- set led on pin 13 high for visual confirmation

    // isbd.begin() causes following be set true.
    // To prevent lockout, set to false.
    modem.useMSSTMWorkaround(false);

    Serial.print(F("RockBlock Turned on"));
  }

  // Example: Print the firmware revision
  char version[12];
  err = modem.getFirmwareVersion(version, sizeof(version));
  if (err != ISBD_SUCCESS) {
    Serial.print("FirmwareVersion failed: error ");
    Serial.println(err);
    return;
  }
  Serial.print("Firmware Version is ");
  Serial.print(version);
  Serial.println(".");

  // Example: Test the signal quality.
  // This returns a number between 0 and 5.
  // 2 or better is preferred.
  err = modem.getSignalQuality(signalQuality);
  if (err != ISBD_SUCCESS) {
    Serial.print("SignalQuality failed: error ");
    Serial.println(err);
    return;
  }

  Serial.print("On a scale of 0 to 5, signal quality is currently ");
  Serial.print(signalQuality);
  Serial.println(".");

  // Send the message
  Serial.print("Trying to send the message.  This might take several minutes.\r\n");
  err = modem.sendSBDText("Hello, world!");
  if (err != ISBD_SUCCESS) {
    Serial.print("sendSBDText failed: error ");
    Serial.println(err);
    if (err == ISBD_SENDRECEIVE_TIMEOUT)
      Serial.println("Try again with a better view of the sky.");
  }

  else {
    Serial.println("Hey, it worked!");
  }
}

void loop() {
}

#if DIAGNOSTICS
void ISBDConsoleCallback(IridiumSBD *device, char c) {
  Serial.write(c);
}

void ISBDDiagsCallback(IridiumSBD *device, char c) {
  Serial.write(c);
}
#endif