#include<SPI.h>
#include<RF24.h>
#include<ArduinoJson.h>

// ce, csn pins NRF24L01
RF24 radio(7, 8);
int indeks = 800;

// defines pins numbers SR04
const int trigPin = 2;
const int echoPin = 3;

//Define prevMillis and Delay
unsigned long previousMillisSend = 0;
const int intervalSend = 10000;

//Define ID and height
int idDevice = 1;
int height = 22;

void setup(){
  //Initialize SR04
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  
  //Initialize NRF24L01
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0x76);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  //radio.openWritingPipe(0xE8E8F0F0E1LL);
  radio.enableDynamicPayloads();
  radio.powerUp();

  //Serial
  Serial.begin(9600);

}

void loop(){
  //Timecounter
  unsigned long currentMillis = millis();

  //int ak = getDistance();
  //Serial.println(ak);

  //delay(1000);

  
  //Delay for Get and Sending data
  if(currentMillis - previousMillisSend >= intervalSend){
    previousMillisSend = currentMillis;

    //Get distance
    int distance = getDistance();
    
    //format to json
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["id"] = idDevice;
    root["val"] = distance;
    root["height"] = height;

    root.printTo(Serial);
    
    //if distance << height then send data
    if(distance <= height){
      char buff[32];
      int n = root.printTo(buff, sizeof(buff));
      radio.write(buff, n);
      Serial.println(" : Send Done");
    }else{
      Serial.println(" : Not Send");
    }
  }

  /*
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = 1;
  root["val"] = getDistance();
  root["height"] = 500;
  
  char buff[32];
  int n = root.printTo(buff, sizeof(buff));
  //Serial.println(getDistance());
  radio.write(buff, n);
  root.printTo(Serial);
  
  indeks++;
  delay(5000);
  */

}

int getDistance(){
  // defines variables
  long duration;
  int distance;

   // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;

  return distance;
}

