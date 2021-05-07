/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_BMP085.h>

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2
//BMP085 Sensor is occupying pins 22 and 21 for i2c clock and data

int counter = 0;

//defining struct to format data easily
typedef struct
{
  float temperature;
  float humidity;
  int pressure;
} data_package;

data_package test = {4.2, 6.9, 2100};

Adafruit_BMP085 bmp;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!(LoRa.begin(915E6))) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  //setup BMP085 sensor
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  //read measurements from instrumentation and record in data_package object
  test.temperature = bmp.readTemperature();
  test.pressure = bmp.readPressure();
  
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.printf("{%f, %f, %i}", test.temperature, test.humidity, test.pressure);
  LoRa.endPacket();

  counter++;

  delay(1000);
}
