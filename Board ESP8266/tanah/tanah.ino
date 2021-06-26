/* Firmansyah Wahyu @2021
 * Latest Version : https://github.com/bywahjoe/ESP8266-Soil-Monitoring-Push-Webserver.git
   HTTPS Connection
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <LiquidCrystal_I2C.h>
#include "pinku.h"

#define URLWEB "dhiancokembed2021.xyz"
#define PATH "/post.php"
#define sha "42 A2 F7 2B B0 BD 3C D6 45 1F 27 56 79 81 E8 F6 B9 F5 07 D2"

//#define sha "CE 6A 43 36 69 39 8D 5D 10 F6 2E EA D4 09 3A 79 2F DA 4E 4E"
//Valid Cert Tue, 21 Sep 2021 09:18:35 GMT

#define httpsPort 443

String APIKEY = "sendIP4";

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Status Sensor
int mysoil=0;
int myjarak=0;
int myrelay=0;

//Milis Setting
const long interval = 16000;
unsigned long before = 0;
unsigned long now = 0;

void setup() {
  
  Serial.begin(115200);
  //init component
  lcd.init();
  lcd.backlight();
  
  //Relay Active LOW
  pinMode(pin_relay,OUTPUT);
  digitalWrite(pin_relay,HIGH);
  
  //Ultrasonik
  pinMode(triggerPin, OUTPUT); 
  pinMode(echoPin, INPUT);
   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  String statusWifi = "Connected";
  int retry = 1;
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting WiFi");
    lcd.setCursor(1, 1);
    lcd.print(retry);

    if (retry > 100) {
      statusWifi = "Error";
      break;
    }
    else {
      retry++;
    }
    delay(500);
  }
}

void loop() {
  mysoil=getTanah();
  myjarak=getJarak();

  //Serial.println(myjarak);
  //Serial.println(mysoil);

  //Jika tanah basah, relay ON
  if(mysoil>tanah_basah){
    myrelay=1;
    relayON();
  }else{
    myrelay=0;
    relayOFF();
  }

  tampil();
   
  //Sending Web
  now = millis();
  if (now - before >= interval) {
    kirimWeb();
    now=millis();
    before = now;
  }
}

//Relay ON/OFF
void relayON() {
  digitalWrite(pin_relay, LOW);
}
void relayOFF() {
  digitalWrite(pin_relay, HIGH);
}

//Status Relay ON/OFF
String statusRelay(bool val) {
  String notice;

  if (val)notice = "ON";
  else notice = "OFF";
  return notice;
}

//Get Soil Level
int getTanah() {
  int val = analogRead(pin_tanah);
  Serial.print("Sensor Asli: "); Serial.println(val);
  
  Serial.print("Constrain"); Serial.println(val);
  int convert = map(val, min_tanah, max_tanah, 0, 100);
  return convert;
}

//Get Distance
long getJarak() {
  long duration, jarak;

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  jarak = (duration / 2) / 29.1;
  // jarak = setKosong - jarak;
  // jarak = constrain(jarak, 1, 30);

  //  Serial.println("jarak :");
  //  Serial.print(jarak);
  //  Serial.println(" cm");

  return jarak;
}

//LCD
void tampil() {
  String status_tanah="Kering";
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil Level:");
  lcd.print(getTanah());
  lcd.print("%");
  
  lcd.setCursor(0, 1);
  lcd.print("Distance  :");
  lcd.print(myjarak);
  lcd.print("cm");
  delay(800);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Relay :");
  lcd.print(statusRelay(myrelay));
  lcd.setCursor(0, 1);
  lcd.print("Tanah :");
  
  if(!myrelay)status_tanah="Basah"; 
  lcd.print(status_tanah);
  delay(800);
}

//Push Web
void kirimWeb() {
  String d1 = String(mysoil);
  String d2 = String(myjarak);
  String d3 = String(myrelay);

//    String d1 = String(random(0,50));
//    String d2 = String(random(0,20));
//    String d3 = String(random(0,2));
  
  //4D 54 07 D8 EA 7D 9A 47 DC 08 7C 46 A8 54 0F AB B8 D9 0B A75
  WiFiClientSecure httpsClient;
  httpsClient.setInsecure();

  //  Serial.printf("Using fingerprint '%s'\n", sha);
  //  httpsClient.setFingerprint(sha);
  //  httpsClient.setTimeout(15000);

  Serial.print("HTTPS Connecting: "); Serial.println(URLWEB);
  //    int r=0; //retry counter
  //    while((!httpsClient.connect(URLWEB, 443)) && (r < 30)){
  //        delay(100);
  //        Serial.print(".");
  //        r++;
  //    }
  //    if(r==30) {
  //      Serial.println("Connection failed");
  //    }
  //    else {
  //      Serial.println("Connected to web");
  //    }
    if (!httpsClient.connect(URLWEB, httpsPort)) {
      Serial.println("connection failed");
    } else {
      Serial.println("connection OK");
    }

  Serial.print("requesting URL: ");
  Serial.print(URLWEB); Serial.println(PATH);
  String dataku = "apiKEY=" + APIKEY + "&d1=" + d1 + "&d2=" + d2 + "&d3=" + d3;
 
  //TEST
  //String dataku="apiKEY=sendIP4&d1=0&d2=0&d3=0";

  //Format----
  /*
    POST /post.php HTTP/1.1
    Host: monitor.bywahjoe.com
    Content-Type: application/x-www-form-urlencoded
    Content-Length: 34
    apiKEY=sendIP4&d1=0&d2=0&d3=0&d4=0
  */

  httpsClient.print(String("POST ") + PATH + " HTTP/1.1\r\n" +
                    "Host: " + URLWEB + "\r\n" +
                    "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
                    "Content-Length: " + String(dataku.length()) + "\r\n\r\n" +
                    dataku + "\r\n" +
                    "Connection: close\r\n\r\n");
  Serial.println(dataku);
}
