#include <Arduino.h>
// Include thermocouple library
#include <MAX31855.h>
// Include CAN library
#include <mcp_can.h>
#include <SPI.h>
// Include library for accelerometer
#include <Wire.h>

// put function declarations here:
void printTemperature(double temperature);

// Pin connections to the Quad MAX31855 thermocouple board
#define MISO 9
#define SCK 8
#define CS0 4
#define CS1 5
#define CS2 6
#define CS3 7

// Pin connection to CAN board
#define SPI_CS_PIN 10

// Pin connection for analog fuel pressure sensor
#define FP A7

// Create the temperature object, defining the pins used for communication
MAX31855 *temp[4] = {new MAX31855(MISO, CS0, SCK), new MAX31855(MISO, CS1, SCK), new MAX31855(MISO, CS2, SCK), new MAX31855(MISO, CS3, SCK)};

MCP_CAN CAN(SPI_CS_PIN);
byte stmp[8] = {};   // CAN broadcast variable 1
byte stmpII[8] = {}; // CAN broadcast variable 2

int PFuelRaw = 0; // fuel pressure raw analog read value
int PFuel = 0;    // fuel pressure in PSIG

#define MPU_addr 0x68       // I2C address of MPU-6050 (accelerometer)
int16_t AcX, AcY, AcZ, Tmp; // variables for MPU-6050 outputs
float AcXG = 0.00;          // variable to convert MPU-6050 outputs to G's (only used for serial print)
float AcYG = 0.00;          // variable to convert MPU-6050 outputs to G's (only used for serial print)
float AcZG = 0.00;          // variable to convert MPU-6050 outputs to G's (only used for serial print)
int AcXC = 0;               // variable for acc CAN broadcast
int AcYC = 0;               // variable for acc CAN broadcast
int AcZC = 0;               // variable for acc CAN broadcast

void setup()
{
  Serial.begin(9600);

START_INIT:
  if (CAN_OK == CAN.begin(CAN_500KBPS))
  { // init can bus : baudrate = 500k
    Serial.println("CAN BUS Shield init ok!");
  }
  else
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(1000);
    goto START_INIT;
  }

  // Turn MPU-6050 on
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
}

void loop()
{
  // Read EGTs
  for (int i = 0; i < 4; i++)
  {
    double temperature = (*temp[i]).readThermocouple(CELSIUS);
    if (temperature == NO_MAX31855)
      continue;
    Serial.print("Cyl");
    Serial.print(i);
    Serial.print("=");
    printTemperature(temperature);
    stmp[i * 2] = temperature / 4;
    stmp[i * 2 + 1] = fmod(temperature, 4);
  }

  // Read Fuel Pressure
  PFuelRaw = analogRead(FP);
  PFuel = map(PFuelRaw, 0, 1023, -12.5, 112.5);
  stmpII[0] = PFuel;

  // Read Accelerometer
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 8, true);                  // request a total of 8 registers
  AcX = Wire.read() << 8 | Wire.read();                 // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read();                 // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read();                 // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read();                 // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  AcXG = (AcX + 32768) * (2 + 2) / (32768 + 32768) - 2; // convert AcX into G's
  AcYG = (AcY + 32768) * (2 + 2) / (32768 + 32768) - 2; // convert AcY into G's
  AcZG = (AcZ + 32768) * (2 + 2) / (32768 + 32768) - 2; // convert AcZ into G's
  AcXC = map(AcX, -32768, 32768, 0, 255);               // convert AcX to 0-255 for CAN broadcast
  AcYC = map(AcY, -32768, 32768, 0, 255);               // convert AcY to 0-255 for CAN broadcast
  AcZC = map(AcZ, -32768, 32768, 0, 255);               // convert AcZ to 0-255 for CAN broadcast
  stmpII[1] = AcXC;
  stmpII[2] = AcYC;
  stmpII[3] = AcZC;

  // Print all data to Serial Monitor
  Serial.println();
  Serial.print(" FP=");
  Serial.print(PFuel);
  Serial.println();
  Serial.print("AcX = ");
  Serial.print(AcXG);
  Serial.print(" | AcY = ");
  Serial.print(AcYG);
  Serial.print(" | AcZ = ");
  Serial.print(AcZG);
  Serial.print(" | Tmp = ");
  Serial.print(Tmp / 340.00 + 36.53);
  Serial.println();
  Serial.print(" CAN data: ");
  Serial.print(stmp[0]);
  Serial.print(" ");
  Serial.print(stmp[1]);
  Serial.print(" ");
  Serial.print(stmp[2]);
  Serial.print(" ");
  Serial.print(stmp[3]);
  Serial.print(" ");
  Serial.print(stmp[4]);
  Serial.print(" ");
  Serial.print(stmp[5]);
  Serial.print(" ");
  Serial.print(stmp[6]);
  Serial.print(" ");
  Serial.print(stmp[7]);
  Serial.println();
  Serial.print("CAN data II: ");
  Serial.print(stmpII[0]);
  Serial.print(" ");
  Serial.print(stmpII[1]);
  Serial.print(" ");
  Serial.print(stmpII[2]);
  Serial.print(" ");
  Serial.print(stmpII[3]);
  Serial.print(" ");
  Serial.print(stmpII[4]);
  Serial.print(" ");
  Serial.print(stmpII[5]);
  Serial.print(" ");
  Serial.print(stmpII[6]);
  Serial.print(" ");
  Serial.print(stmpII[7]);
  Serial.print(" ");
  Serial.println();
  Serial.println();

  // Send data over CAN
  CAN.sendMsgBuf(0x7B, 0, 8, stmp); // send data:  id = 0x00, standrad frame, data length = 8, stmp: data buf
  CAN.sendMsgBuf(0x7C, 0, 8, stmpII);
}

// Print the temperature, or the type of fault
void printTemperature(double temperature)
{
  switch ((int)temperature)
  {
  case FAULT_OPEN:
    Serial.print("FAULT_OPEN");
    break;
  case FAULT_SHORT_GND:
    Serial.print("FAULT_SHORT_GND");
    break;
  case FAULT_SHORT_VCC:
    Serial.print("FAULT_SHORT_VCC");
    break;
  case NO_MAX31855:
    Serial.print("NO_MAX31855");
    break;
  default:
    Serial.print(temperature);
    break;
  }
  Serial.print(" ");
}