
/**
   Created by K. Suwatchai (Mobizt)

   Email: k_suwatchai@hotmail.com

   Github: https://github.com/mobizt

   Copyright (c) 2021 mobizt

*/
#include <AkhilsSmartHome.h>
#if defined(ESP32)
#include <Espalexa.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
//#elif defined(ESP8266)
//#include <ESP8266WiFi.h>
//#include <FirebaseESP8266.h>
#endif

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Akhil_Spot1"
#define WIFI_PASSWORD "asd123##123"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

int count = 0;
int fan = 0;
int light = 0;

Espalexa espalexa;
EspalexaDevice* device3; //this is optional

void setup()
{
  Serial.begin(115200);

  pinMode(sw1,INPUT);
  pinMode(sw2,INPUT);
  pinMode(sw3,INPUT);
  pinMode(sw4,INPUT);
  pinMode(sw5,INPUT);
  pinMode(sw6,INPUT);
  
  pinMode(rl1,OUTPUT);
  pinMode(rl2,OUTPUT);
  pinMode(rl3,OUTPUT);
  pinMode(rl4,OUTPUT);
  pinMode(rl5,OUTPUT);
  pinMode(rl6,OUTPUT);
  
  pinMode(led,OUTPUT);
  pinMode(2,OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  espalexa.addDevice("Light", LightChanged); //simplest definition, default state off
  espalexa.addDevice("Fan", FanChanged); //third parameter is beginning state (here fully on)
  espalexa.begin();

  //Or use legacy authenticate method
  //Firebase.begin(DATABASE_URL, "<database secret>");
}

String setStatus(String path,int stat)
{
  return Firebase.setInt(fbdo, path, stat) ? "ok" : fbdo.errorReason().c_str();
}

String getStatus(String path)
{
  return Firebase.getInt(fbdo, path) ? String(fbdo.intData()).c_str() : fbdo.errorReason().c_str();
}
void updateio()
{
    
//  if (digitalRead(sw1 == HIGH))
//  {
//    light = !light;
//    setStatus("/myHome/myRoom/light", light);
//    while (sw1);
//  }

//  if (digitalRead(sw2 == HIGH))
//  {
//    fan = !fan;
//    setStatus("/myHome/myRoom/fan", fan);
//    while (fan);
//  }


  if (light)
  {
    digitalWrite(2, HIGH);
  }
  else
  {
    digitalWrite(2, LOW);
  }
  if (fan)
  {
    digitalWrite(2, HIGH);
  }
  else
  {
    digitalWrite(2, LOW);
  }
}
void checkFirebase()
{
  light = atoi(getStatus("/myHome/myRoom/light").c_str());
  fan = atoi(getStatus("/myHome/myRoom/fan").c_str());
  
  Serial.printf("Get Light status... %d\n", light );
  Serial.printf("Get Fan status... %d\n", fan );
  
  updateio();
}

void LightChanged(uint8_t brightness) {
    Serial.print("Device 1 changed to ");
    if (brightness) {
      setStatus("/myHome/myRoom/light",1);
      Serial.print("ON, brightness ");
      Serial.println(brightness);
    }
    else  {
      setStatus("/myHome/myRoom/light",0);
      Serial.println("OFF");
    }
}
void FanChanged(uint8_t brightness) {
    Serial.print("Device 1 changed to ");
    if (brightness) {
      setStatus("/myHome/myRoom/fan",1);
      Serial.print("ON, brightness ");
      Serial.println(brightness);
    }
    else  {
      setStatus("/myHome/myRoom/fan",0);
      Serial.println("OFF");
    }
}

void loop()
{
  espalexa.loop();
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    checkFirebase();
  }
}
