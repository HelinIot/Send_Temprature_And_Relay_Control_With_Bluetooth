//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"
#include <DHT.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define DHTPIN 4 // DHT Digital Input Pin
#define DHTTYPE DHT11 // DHT11 or DHT22, depends on your sensor
DHT dht(DHTPIN, DHTTYPE);
#define RELAYPIN 5

BluetoothSerial SerialBT;
unsigned long previousMillis = 0; 
const long interval = 3000;
char incomingChar;
String message = "";

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    
    String str = "Humidity: ";
    str += humidity;
    str += " , Temperature: ";
    str += temperature;

    Serial.println(str); 
    SerialBT.println(str); 
  }

  // Read received messages (control command)
  if (SerialBT.available()){
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message += String(incomingChar);
    }
    else{
      message = "";
    }
    Serial.write(incomingChar);  
  }

  // Check received message and control output accordingly
  if (message =="on"){
    digitalWrite(RELAYPIN, HIGH);
  }
  else if (message =="off"){
    digitalWrite(RELAYPIN, LOW);
  }

  delay(20);
}