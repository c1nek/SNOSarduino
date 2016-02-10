#include <Wire.h>
#include <SFE_BMP180.h>
#include <dht11.h>
#include <SPI.h>
#include <Ethernet.h>
const String deviceID = "dzordandev2016";
///////////LEDS////////////
bool led1Status = false;
bool led2Status = false;
bool led3Status = false;
bool led4Status = false;
String ledR_s, ledG_s, ledB_s = "";
String ledRGB_s = "000000000";
int ledR_int, ledG_int, ledB_int = 0;
/////////PRESSURE/////////
SFE_BMP180 pressure;
char status;
double T,P;
///////DHT11////////////////
dht11 DHT11;
#define DHT11PIN 41
///////MOTION SENSORS///////
int PirPin1 = 2;
bool Pir1Value = false;
int PirPin2 = 3;
bool Pir2Value = false;
int PirPin3 = 4;
bool Pir3Value = false;
int PirPin4 = 5;
bool Pir4Value = false;
//////////DHT11/////////////
#define DHTPIN 2
/////////FIRE SENSOR///////////
int FireSensorPin = A0;
int FireSensorValue = 0;
///////WATER LEVEL SENSOR///////
int WaterSensorPin = A1;
int WaterSensorValue = 0;
/////////GAS SENSOR/////////////
int GasSensorPin = A2;
int GasSensorValue = 0;
///////////////////////////////////
/////////ETHERNET///////////
char serverIP[] = "83.20.161.116";
bool isConnectedToServer = false;
int connectionsToServer = 1;
int serverPort = 2137;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[] = {192, 168, 1, 150};
byte gateway[] = {192, 168, 1, 1};
byte subnet[] = {255, 255, 255, 0};
byte dnss[] = {192, 168, 1, 1};
EthernetServer server(80);
EthernetClient client;
String token;
String readString;
//////////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("############ARDIUNO############");
  Serial.println("########DzordanDev2016#########");
  Ethernet.begin(mac, ip, dnss, gateway, subnet);
  Serial.print("\nConfiguring Ethernet ");
  for (int i = 0; i < 20; i++) {
    Serial.print(".");
    delay(350);
  }
  Serial.print("\nCalibrating sensors ");
  for (int i = 0; i < 21; i++) {
    Serial.print(".");
    delay(100);
  }
  if (pressure.begin())
        Serial.println("\nBMP180 sensor: OK"); else {
    Serial.println("\nBMP180 sensor: errorn");
  }
  int chk = DHT11.read(DHT11PIN);
  Serial.print("DHT11 sensor: ");
  switch (chk) {
    case 0: Serial.println("OK");
    break;
    case -1: Serial.println("Checksum error");
    break;
    case -2: Serial.println("Time out error");
    break;
    default: Serial.println("Unknown error");
    break;
  }
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  delay(1000);
  bool isConnectedToServer = false;
  String httpWToken = "";
  while(!isConnectedToServer) {
    Serial.print("\nConnecting to server.[");
    Serial.print(connectionsToServer);
    Serial.print("]");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("\nConnected to server.");
      client.println("GET /arduino?id="+deviceID+" HTTP/1.1");
      client.println("Host: 83.20.162.171");
      client.println("Content-Type: application/text");
      client.println();
      while(true) {
        char c = client.read();
        if (c == '#') {
          break;
        } else {
          httpWToken += c;
        }
      }
    } else {
      Serial.println("\nUnable to connect to server.");
      connectionsToServer = connectionsToServer + 1;
      isConnectedToServer = false;
      delay(1000);
    }
    if (client.connected()) {
      client.stop();
      isConnectedToServer = true;
      token = httpWToken.substring(httpWToken.length()-32, httpWToken.length());
    }
  }
  Serial.println(token);
}
bool Contain(String mainString, String searchedString) {
  int max = mainString.length() - searchedString.length();
  for (int i=0; i<= max; i++) {
    if (mainString.substring(i) == searchedString) 
    return true;
  }
  return false;
}
void loop() {
  FireSensorValue = analogRead(FireSensorPin);
  WaterSensorValue = analogRead(WaterSensorPin);
  GasSensorValue = analogRead(GasSensorPin);
  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0) {
      status = pressure.startPressure(3);
      if (status != 0) {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0) {
          P=P*33.885;
        } else Serial.println("Error retrieving pressure.");
      } else Serial.println("Error starting pressure.");
    } else Serial.println("Error retrieving temperature.");
  } else Serial.println("Error starting temperature.");
  ///////////////////////////////////////////////////
  if(digitalRead(PirPin1) == HIGH) {
    Pir1Value = true;
  }
  if(digitalRead(PirPin1) == LOW) {
    Pir1Value = false;
  }
  if(digitalRead(PirPin2) == HIGH) {
    Pir2Value = true;
  }
  if(digitalRead(PirPin2) == LOW) {
    Pir2Value = false;
  }
  if(digitalRead(PirPin3) == HIGH) {
    Pir3Value = true;
  }
  if(digitalRead(PirPin3) == LOW) {
    Pir3Value = false;
  }
  if(digitalRead(PirPin4) == HIGH) {
    Pir4Value = true;
  }
  if(digitalRead(PirPin4) == LOW) {
    Pir4Value = false;
  }
  ///////////////////////////////////////////////////
  EthernetClient client = server.available();
  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        readString += c;
        if (c == '\n' && currentLineIsBlank) {
          if(readString.indexOf(token) >=0) {
            Serial.println(readString);
            if(readString.indexOf('&') >=0) {
              if(readString.indexOf("led1=0") >0) {
                digitalWrite(7, LOW);
                led1Status = false;
                Serial.println("Led1 OFF");
              }
              if(readString.indexOf("led1=1") >0) {
                digitalWrite(7, HIGH);
                led1Status = true;
                Serial.println("Led1 ON");
              }
              if(readString.indexOf("led2=0") >0) {
                digitalWrite(4, LOW);
                led2Status = false;
                Serial.println("Led2 OFF");
              }
              if(readString.indexOf("led2=1") >0) {
                digitalWrite(4, HIGH);
                led2Status = true;
                Serial.println("Led2 ON");
              }
              if(readString.indexOf("led3=0") >0) {
                digitalWrite(4, LOW);
                led3Status = false;
                Serial.println("Led3 OFF");
              }
              if(readString.indexOf("led3=1") >0) {
                digitalWrite(4, HIGH);
                led3Status = true;
                Serial.println("Led3 ON");
              }
              if(readString.indexOf("led4=0") >0) {
                digitalWrite(4, LOW);
                led4Status = false;
                Serial.println("Led4 OFF");
              }
              if(readString.indexOf("led4=1") >0) {
                digitalWrite(4, HIGH);
                led4Status = true;
                Serial.println("Led4 ON");
              }
              if(readString.indexOf("led4=1") >0) {
                digitalWrite(4, HIGH);
                led4Status = true;
                Serial.println("Led4 ON");
              }
              if(readString.indexOf("ledRGB=") >0) {
                ledR_s = (readString.substring(13,16));
                ledG_s = (readString.substring(16,19));
                ledB_s = (readString.substring(19,22));
                Serial.println("RGB Strip:");
                Serial.println(ledB_s);
                ledRGB_s = ledR_s+ledG_s+ledB_s;
                Serial.println(ledRGB_s);
                ledR_int = ledR_s.toInt();
                ledG_int = ledG_s.toInt();
                ledB_int = ledB_s.toInt();
              }
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json;charset=utf-8");
              client.println("Connnection: close");
              client.println();
              client.print("{\"name\":\"arduino\",\"id\":\""+deviceID+"\",\"switches\":{\"led1\":\"");
              client.print(led1Status);
              client.print("\",\"led2\":\"");
              client.print(led2Status);
              client.print("\",\"led3\":\"");
              client.print(led3Status);
              client.print("\",\"led4\":\"");
              client.print(led4Status);
              client.print("\",\"ledRGB\":\"");
              client.print(ledRGB_s);
              client.print("\",\"switch1\":\"");
              client.print(0);
              client.print("\",\"switch2\":\"");
              client.print(0);
              client.print("\",\"switch3\":\"");
              client.print(0);
              client.print("\"}}");
              client.println();
              break;
            } else {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json;charset=utf-8");
              client.println("Connnection: close");
              client.println();
              client.print("{\"name\":\"arduino\",\"id\":\""+deviceID+"\",\"sensors\":{\"fireSensor\":\"");
              client.print(FireSensorValue);
              client.print("\",\"waterSensor\":\"");
              client.print(WaterSensorValue);
              client.print("\",\"gasSensor\":\"");
              client.print(GasSensorValue);
              client.print("\",\"pir\":\"");
              client.print(Pir1Value);
              client.print(Pir2Value);
              client.print(Pir3Value);
              client.print(Pir4Value);
              client.print("\",\"tempInside\":\"");
              client.print(DHT11.temperature);
              client.print("\",\"humInside\":\"");
              client.print(DHT11.humidity);
              client.print("\",\"tempOutside\":\"");
              client.print(T);
              client.print("\",\"pressure\":\"");
              client.print(P*0.0295333727,2);
              client.print("\"}}");
              break;
            }
          } else {
            client.println("HTTP/1.1 403 Forbidden");
            client.println("Content-Type: application/text;charset=utf-8");
            client.println("Connnection: close");
            client.println();
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(10);
    // close the connection:
    client.stop();
    readString = "";
  }
}
