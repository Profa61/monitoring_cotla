
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
//#define BLYNK_TEMPLATE_ID   "YourTemplateID"


// Select your modem:
#define TINY_GSM_MODEM_SIM800
//#define TINY_GSM_MODEM_SIM900
//#define TINY_GSM_MODEM_M590
//#define TINY_GSM_MODEM_A6
//#define TINY_GSM_MODEM_A7
//#define TINY_GSM_MODEM_BG96
//#define TINY_GSM_MODEM_XBEE

// Default heartbeat interval for GSM is 60
// If you want override this value, uncomment and set this option:
//#define BLYNK_HEARTBEAT 30
#include <SPI.h>
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include "GyverTimer.h"
#define BLYNK_PRINT Serial
#include <DHT.h>
#include <OneWire.h>
#include<DallasTemperature.h>
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "a5b3080bc1014f0ba1e17be78b57c4c1";

// Your GPRS credentials
// Leave empty, if missing user or pass
char apn[]  = "Internet";
char user[] = "";
char pass[] = "";

#define DHTPIN 2
DHT dht(DHTPIN, DHT11);
////////////////////DS18B20/////////////////////////////////
#define ONE_WIRE_BUS 12 // DS18B20 подключаем на D4 на плате  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int temp_0;
int temp_1;
///////////////////Voltage/////////////////////////////////
int analogInput = A0;
float vout = 0.0;
float vin = 0.0;
float R1 = 29600.0;
float R2 = 7510.0;
int value = 0;
//////////////////////////////////////////////////////////
GTimer_ms myTimer;
GTimer_ms DsTimer;
GTimer_ms voltTimer;
int Rele = 11;
void sendSensor()
{

  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);

}

void DsSensor()
{
  DS18B20.requestTemperatures();
  temp_0 = DS18B20.getTempCByIndex(0); // Sensor 0 показания для датчика 1 в цельсиях
  temp_1 = DS18B20.getTempCByIndex(1); // Sensor 0 показания для датчика 2 в цельсиях

  // Serial.print("Temp_0: ");
  // Serial.print(temp_0);
  // Serial.print(" oC . Temp_1: ");
  // Serial.print(temp_1);
  //Serial.println(" oC");

  Blynk.virtualWrite(10, temp_0); //выврд данных на виртуальный пин V10
 // Blynk.virtualWrite(11, temp_1); //вывод данных навиртуальный пин V11
}
void VoltSensor()
{
 value = analogRead(analogInput);
  vout = (value * 5.0) / 1024.0;
  vin = vout / (R2 / (R1 + R2));

 // Serial.print("INPUT V = ");
 // Serial.println(vin, 2);
   Blynk.virtualWrite(4,vin); //выврд данных на виртуальный пин V10
}
// Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// or Software Serial on Uno, Nano
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(8, 9); // RX, TX

TinyGsm modem(SerialAT);

void setup()
{
 pinMode(analogInput, INPUT); 
  // Debug console
  Serial.begin(19200);

  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(57600);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  //modem.restart();
  modem.init();
  // Unlock your SIM card with a PIN
  //modem.simUnlock("0000");

  Blynk.begin(auth, modem, apn, user, pass, "90.189.217.158", 8080);

  // Setup a function to be called every second
  // timer.setInterval(1000, sendSensor);
  dht.begin();
  myTimer.setInterval(23012);   // настроить интервал
  DsTimer.setInterval(16041);   // настроить интервал
  voltTimer.setInterval(13058);   // настроить интервал
  pinMode(Rele,OUTPUT);
  digitalWrite(Rele,HIGH);
}

void loop()
{
  Blynk.run();
  // timer.run(); // Initiates SimpleTimer
  if (myTimer.isReady())Serial.println("DHT"); {
    sendSensor();

  }
  if (DsTimer.isReady())Serial.println("DS"); {
    DsSensor();
  }
  if (voltTimer.isReady())Serial.println("Vcc"); {
    VoltSensor();
  }
}
