#include <DHT.h>
#include <SPI.h>
#include <LoRa.h>


//dht pins
#define DHT11PIN 0
//sol sensor pin
#define SolPin A0
//lora pins
#define ss 15
#define rst 16
#define dio0 2

byte noeud_station_sol = 0xAA;
byte noeud_principale =  0xACC829;
DHT dht(DHT11PIN, DHT11);
byte msgCount = 0; 

String msg1 = "";
void setup() {

  Serial.begin(115200);

  pinMode(SolPin, INPUT);
  /* Start the DHT11 Sensor */
  dht.begin();

  Serial.println("Initializing...");
  while (!Serial)
    ;
//********************initialisation LoRa*****************
  Serial.println("LoRa station meteo");
  LoRa.setPins(ss, rst, dio0);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
  LoRa.setTxPower(20);
//********************************************************  
}


void loop() {

//**************Read values from sensors*************************************************
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  float solVal = analogRead(SolPin);
  //unsigned short soilmoisturepercent =  map(solVal, 652, 295, 0, 100);
  unsigned short soilmoisturepercent =  map(solVal, 1024, 943, 0, 100);
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("C ");
  Serial.print("Humidity: ");
  Serial.println(humi);
  Serial.print(" Sol Humidity: ");
  Serial.println(solVal);
  Serial.print("pourcentage: humidité sol ");
  Serial.println(soilmoisturepercent);
//*********************************************************************************

  msg1 = String(temp) + "/" + String(humi) + "&" + String(soilmoisturepercent); 

//*************Send data***********************************

  sendMessage(msg1,  noeud_station_sol,  noeud_principale);

//*****************************************************************
  delay(2000);  
}




//****************fucntion to encrpt data**********************
String cryptage(String ch){

  String crypt = "";

  for (int i = 0; i < ch.length(); i++) {
    switch (ch[i]) {
      case '/':
        crypt = crypt + "/" + " ";
        break;
      case '&':
        crypt = crypt + "&";


      case '0':
        crypt = crypt + "X||x" + " ";
        break;
      case '1':
        crypt = crypt + "A*a**" + " ";
        break;

      case '2':
        crypt = crypt + "B--+b" + " ";
        break;

      case '3':
        crypt = crypt + "C..,c." + " ";
        break;

      case '4':
        crypt = crypt + "D!!§d" + " ";
        break;

      case '5':
        crypt = crypt + "E%%e%é" + " ";
        break;

      case '6':
        crypt = crypt + "F@((f@" + " ";
        break;

      case '7':
        crypt = crypt + "Gg[]]==" + " ";
        break;

      case '8':
        crypt = crypt + "(H-àh)" + " ";
        break;

      case '9':
        crypt = crypt + "I\\èi" + " ";
        break;

        case '.':
        crypt = crypt + "##"+" ";
        break;
    }
  }
return crypt;
}
//*********************************************************************



//****************fucntion to send data**********************
void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(otherNode);              // add destination address
  LoRa.write(MasterNode);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(cryptage(outgoing));                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}
//***************************************************************************
