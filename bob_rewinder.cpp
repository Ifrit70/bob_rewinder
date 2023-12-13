#include <Arduino.h>
#include <Wire.h>

//
//Credits:
// Big Thanks to Briadark (from Deku's Discord) for Low-Lvl Read/Write functions
// Additional code by Ifrit (to ease the use of the rewinder for multiple run)
// 
//Hardware:
// USB-A Female with housing isolated (with tape) OR Edge Card Connector (2x4poles, 2.54mm Pitch)
// Connections, Left to Right, seeing the front of Bob Cassete (sticker):
// (Connections pins for Arduino Pro Mini beetwen brackets)
// 3V3 (VCC) || SDA (A4)|| SCL (A5) || Grnd (GND)
// Using 2 dipSwitch, connected to GND, for Mode (D11) and Cassette-Type (D10) selection 
//


#define bProd false //Change to true to disable all Serial prnt
#define bDebug true //Add Verbosity to Serial
#define bMultipleWr true //Change to true to allow several write per power-on

#define ADDR_Ax 0b000 //A2, A1, A0
#define ADDR (0b1010 << 3) + ADDR_Ax

#define pinLed 13 // Use Arduino Internal Led by default
#define pinRock 10 // Switch Pop (off) Rock (on) mode
#define pinWrite 11 //S witch Read (off) or Write (on)

int lastValue = -1;

void RewindBob();
void analyseRead(int value);
void writeI2CByte(byte data_addr, byte data);
byte readI2CByte(byte data_addr);

void setup() {

  digitalWrite(pinLed, LOW);
  pinMode (pinRock, INPUT_PULLUP);
  pinMode (pinWrite, INPUT_PULLUP);
  
  if (!bProd) {
    Serial.begin(9600);
    Serial.println("\n---------------------");
    Serial.println("BobRewinder Arduino");
    Serial.print("Booted in ");
    if (digitalRead(pinWrite)==LOW) {
      Serial.print("Write Mode ");
      if (bDebug && digitalRead(pinRock)==LOW) Serial.println("for full rock_bob"); else Serial.println("for full pop_bob");
      if (bDebug && bMultipleWr) Serial.println("! With Multiple Write per powerOn allowed !");
    } else Serial.println("Read-Only Mode");
    Serial.println("---------------------");

  }

  Wire.begin();

  if (!bMultipleWr && digitalRead(pinWrite)==LOW) RewindBob(); //Rewind cassette on boot
  
}

void loop() {
  
  byte currentValue = readI2CByte(161); // read cassette

  if (lastValue != currentValue) {
    if (bMultipleWr && digitalRead(pinWrite)==LOW) { //Rewind cassette at every cassette change
      RewindBob(); 
      currentValue = readI2CByte(161); // update the reading
    }
    analyseRead(currentValue);
    lastValue = currentValue;
    delay(2000); // Wait 2s after something appends
  } else delay(500); // Wait 0.5s if nothing new
  
}

void RewindBob() {
  byte currentValue = readI2CByte(161);
  if (currentValue != NULL ) { // If there is a cassette plugged.
    delay(1);
    if (!bProd && bDebug) {
      Serial.print("Current Value = ");
      Serial.println(currentValue);
    }
    if (digitalRead(pinRock)==LOW) {
      if (!bProd) Serial.println("writing full rock_bob");
      writeI2CByte(161, 81); //write full rock_bob
    } else {
      if (!bProd) Serial.println("writing full pop_bob");
      writeI2CByte(161, 78); //write full pop_bob
    }
  } else {
    if (!bProd) Serial.println("No cassette detected, done nothing");
    lastValue = currentValue; // No need to check if writing was ok
  }
  delay (1); // delay for stability
}

void analyseRead(int value) {
  switch (value){
    case -1: // First reading failed
      if (!bProd) Serial.println("No Bob Cassette plugged");
      digitalWrite(pinLed, LOW);
    case 0: // Read 
      if (!bProd) Serial.println("No Bob Cassette detected");
      digitalWrite(pinLed, LOW);
    break;
    case 80:
      if (!bProd) Serial.println("Empty Bob Cassette");
      digitalWrite(pinLed, LOW);
      break;
    case 78:
      if (!bProd) Serial.println("Full pop_bob cassette OK");
      digitalWrite(pinLed, HIGH);
      break;
    case 81:
      if (!bProd) Serial.println("Full rock_bob cassette OK");
      digitalWrite(pinLed, HIGH);
      break;
    default:
      if (digitalRead(pinWrite)==LOW) { // Write Mode
        while (digitalRead(pinWrite)==LOW) {
          if (!bProd) Serial.print("!! Overwrite failed !! Debug Value = ");
          if (!bProd) Serial.println(readI2CByte(161));
          digitalWrite(pinLed, HIGH);
          delay (500);
          digitalWrite(pinLed, LOW);
          delay (500);
        }
      } else { //Read-only mode
        if (!bProd) Serial.println("Non-Full pop_bob cassette.");
        if (!bProd && bDebug) {
          Serial.print("Current Debug value = ");
          Serial.println(value);
        }
        digitalWrite(pinLed, LOW);
      }
      break;
  }
}

void writeI2CByte(byte data_addr, byte data){
  Wire.beginTransmission(ADDR);
  Wire.write(data_addr);
  Wire.write(data);
  Wire.endTransmission();
  delay(5);
}

byte readI2CByte(byte data_addr){
  byte data = NULL;
  Wire.beginTransmission(ADDR);
  Wire.write(data_addr);
  Wire.endTransmission();
  Wire.requestFrom(ADDR, 1); //retrieve 1 returned byte
  delay(5);
  if(Wire.available()){
    data = Wire.read();
  }
  return data;
}
