/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#define NODE_ID 1

#include <SPI.h>
#include <LoRa.h>

// Sensor Libraries
#include <Adafruit_BMP085.h>
#include "DHT.h"

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

//define pin used by the DHT11 temp/humidity sensor and a constant for the board type
#define DHTPIN 4
#define DHTTYPE DHT11

//BMP085 Sensor is occupying pins 22 and 21 for i2c clock and data, this is assumed by the library

int counter = 0;

//defining struct to format data easily
typedef struct
{
  float temperature;
  float humidity;
  int pressure;
} data_package;

data_package test;

//Initialize sensor objects
Adafruit_BMP085 bmp;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  
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

  //Initialize BMP085 sensor
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  //Initialize DHT11 sensor
  dht.begin();
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  //read measurements from instrumentation and record in data_package object
  test.temperature = (bmp.readTemperature() + dht.readTemperature()) / 2;
  test.pressure = bmp.readPressure();
  test.humidity = dht.readHumidity();
  
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  //sending pre-parsed json hurts me but I don't have tiome to do it properly
  LoRa.printf("{\"temperature\":\"%f\", \"humidity\":\"%f\", \"pressure\":\"%i\", \"nodeID\":\"%s\"}", test.temperature, test.humidity, test.pressure, NODE_ID);
  LoRa.endPacket();

  counter++;

  delay(2000);
}
