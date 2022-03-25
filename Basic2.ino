
/**
 * Created by K. Suwatchai (Mobizt)
 * 
 * Email: k_suwatchai@hotmail.com
 * 
 * Github: https://github.com/mobizt/Firebase-ESP32
 * 
 * Copyright (c) 2022 mobizt
 *
*/

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TinyGPSPlus.h> 
TinyGPSPlus gps;

static const uint32_t GPSBaud = 9600;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Redmi 10"
#define WIFI_PASSWORD "11301130"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyCHrHBRxdd5A03m7QYosOKEUs27qk66csQ"

/* 3. Define the RTDB URL */
#define DATABASE_URL "espgps-cf1ad-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "rafae50@yahoo.com"
#define USER_PASSWORD "helloiot"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup()
{

  Serial.begin(9600);

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

  //Or use legacy authenticate method
  //config.database_url = DATABASE_URL;
  //config.signer.tokens.legacy_token = "<database secret>";

  //To connect without auth in Test Mode, see Authentications/TestMode/TestMode.ino


  //////////////////////////////////////////////////////////////////////////////////////////////
  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  //////////////////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  /** Timeout options.

  //WiFi reconnect timeout (interval) in ms (10 sec - 5 min) when WiFi disconnected.
  config.timeout.wifiReconnect = 10 * 1000;

  //Socket connection and SSL handshake timeout in ms (1 sec - 1 min).
  config.timeout.socketConnection = 10 * 1000;

  //Server response read timeout in ms (1 sec - 1 min).
  config.timeout.serverResponse = 10 * 1000;

  //RTDB Stream keep-alive timeout in ms (20 sec - 2 min) when no server's keep-alive event data received.
  config.timeout.rtdbKeepAlive = 45 * 1000;

  //RTDB Stream reconnect timeout (interval) in ms (1 sec - 1 min) when RTDB Stream closed and want to resume.
  config.timeout.rtdbStreamReconnect = 1 * 1000;

  //RTDB Stream error notification timeout (interval) in ms (3 sec - 30 sec). It determines how often the readStream
  //will return false (error) when it called repeatedly in loop.
  config.timeout.rtdbStreamError = 3 * 1000;

  Note:
  The function that starting the new TCP session i.e. first time server connection or previous session was closed, the function won't exit until the 
  time of config.timeout.socketConnection.

  You can also set the TCP data sending retry with
  config.tcp_data_sending_retry = 1;

  */


}





void loop()
{
String coord ;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
while(Serial.available()){
  coord="";
  Serial.println("In the loop");
 // while(Serial.read()!='\n' || Serial.read()!='\r'){
  // coord.concat(char(Serial.read()));
  coord=Serial.readStringUntil('\r\n');
//}
  Serial.println(coord.c_str());
//  Serial.println("1");
  Serial.println(Serial.read());
//  Serial.println("2");
  Serial.println(" ");
}

const char *gpsStream = coord.c_str();

 double latitude;
 double longitude;
 int hour;
 int sec;
 int minute;
while (*gpsStream){
    if (gps.encode(*gpsStream++)){
      if (gps.location.isValid())
      {
        latitude=gps.location.lat();
        longitude=gps.location.lng();
        Serial.print(latitude);
        Serial.print(" ");
        Serial.println(longitude); 
        if (gps.time.isValid()){
        
          hour=gps.time.hour();
       
          minute=gps.time.minute();
          Serial.print(gps.time.minute());
          
          sec =gps.time.second();
         
        }
        else
        {
          Serial.print(F("INVALID"));
        }


      if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
      {
        sendDataPrevMillis = millis();
          String t=String(String(hour+5)+String(":")+String(minute)+String(":")+String(sec));
          String cordinate= String(String("Time: ")+t+String("      ")+String("Cordinate:")+String(String(latitude,7) +String(" ")+ String(longitude,7)));
          Serial.print(t);
          Serial.print(" ");
          Serial.println(cordinate);
        Serial.printf("Set double... %s\n", Firebase.pushString(fbdo, F("/test/longitude/"), cordinate) ? "ok" : fbdo.errorReason().c_str());
          
        
      }}
      else
      {
        Serial.println(F("INVALID"));
      }
    }
}
///////////////////////////////////////////////////////////////////////////
//  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
//  {
//    sendDataPrevMillis = millis();
//      
//      String cordinate= String(String(latitude,5) +String(" ")+ String(longitude,5));
//      Serial.println(cordinate);
//    Serial.printf("Set double... %s\n", Firebase.pushString(fbdo, F("/test/longitude"), cordinate) ? "ok" : fbdo.errorReason().c_str());
    //Serial.printf("Set double... %s\n", Firebase.pushDouble(fbdo, F("/test/longitude"), longitude) ? "ok" : fbdo.errorReason().c_str());    


    //For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create_Edit_Parse.ino
//    FirebaseJson json;

//    if (count == 0)
//    {
//      json.set("value/round/" + String(count), F("cool!"));
//      json.set(F("vaue/ts/.sv"), F("timestamp"));
//      Serial.printf("Set json... %s\n", Firebase.set(fbdo, F("/test/json"), json) ? "ok" : fbdo.errorReason().c_str());
//    }
//    else
//    {
//      json.add(String(count), "whatever");
//      Serial.printf("Update node... %s\n", Firebase.updateNode(fbdo, F("/test/json/value/round"), json) ? "ok" : fbdo.errorReason().c_str());
//    }
//
//    Serial.println();
//
//    count++;
  }

//}}



// void displayInfo()
//{
//  Serial.print(F("Location: ")); 
//  if (gps.location.isValid())
//  {
//    Serial.print(gps.location.lat(), 6);
//    Serial.print(F(","));
//    Serial.print(gps.location.lng(), 6);
//  }
//  else
//  {
//    Serial.print(F("INVALID"));
//  }
//
////  Serial.print(F("  Date/Time: "));
////  if (gps.date.isValid())
////  {
////    Serial.print(gps.date.month());
////    Serial.print(F("/"));
////    Serial.print(gps.date.day());
////    Serial.print(F("/"));
////    Serial.print(gps.date.year());
////  }
////  else
////  {
////    Serial.print(F("INVALID"));
////  }
//
//  Serial.print(F(" "));
////  if (gps.time.isValid())
////  {
////    if (gps.time.hour() < 10) Serial.print(F("0"));
////    Serial.print(gps.time.hour());
////    Serial.print(F(":"));
////    if (gps.time.minute() < 10) Serial.print(F("0"));
////    Serial.print(gps.time.minute());
////    Serial.print(F(":"));
////    if (gps.time.second() < 10) Serial.print(F("0"));
////    Serial.print(gps.time.second());
////    Serial.print(F("."));
////    if (gps.time.centisecond() < 10) Serial.print(F("0"));
////    Serial.print(gps.time.centisecond());
////  }
////  else
////  {
////    Serial.print(F("INVALID"));
////  }
//
//  Serial.println();
//}
