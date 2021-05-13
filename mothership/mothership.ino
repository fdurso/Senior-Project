/*********
  Modified from the examples of the Arduino LoRa library
  More resources: https://randomnerdtutorials.com
*********/

#include <SPI.h>
#include <LoRa.h>
#include <HTTPClient.h>
#include "WiFi.h"

//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

#define DATA_RECEPTACLE "https://fdurso.me/hopkinton_atmo_data/submit.php"

//defining struct to format data easily
typedef struct
{
  int nodeID;
  float temperature;
  float humidity;
  int pressure;
} data_package;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Receiver");

//=====================================================
// WIFI SETUP
//=====================================================

  //Init WiFi as Station, start SmartConfig
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();

  //Wait for SmartConfig packet from mobile
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone()) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("SmartConfig received.");

  //Wait for WiFi to connect to AP
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi Connected.");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

//=====================================================
// LoRa SETUP
//=====================================================

  LoRa.setPins(ss, rst, dio0);
  
  //915E6 for North America
  while (!LoRa.begin(915E6)) {
    Serial.println(".");
    delay(500);
  }
  
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

int sendData(String data_string)
{
  if (WiFi.status())
    {
      HTTPClient http;

      http.begin(DATA_RECEPTACLE);
      http.addHeader("Content-Type", "application/json");
      return http.POST(data_string);
    }
    Serial.print("NO WIFI");
    return -1;
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet of length ");
    Serial.println(packetSize);

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      //String LoRaData = "{\"temperature\":\"6.9\", \"humidity\":\"6.9\", \"pressure\":\"69\", \"nodeID\":\"testMothershipCode\"}";
      Serial.println(LoRaData); 
      Serial.print(sendData(LoRaData));
    }

    // print RSSI of packet
    Serial.print(" with RSSI ");
    Serial.println(LoRa.packetRssi());

    //send data to be parsed and sent off
    //Serial.print(sendData(LoRaData));
  }
}
