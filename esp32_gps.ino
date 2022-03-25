

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif


#include <TinyGPSPlus.h> 
TinyGPSPlus gps;

static const uint32_t GPSBaud = 9600;

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


  //Please make sure the device free Heap is not lower than 80 k for ESP32 and 10 k for ESP8266,
  //otherwise the SSL connection will fail.
  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);



}





void loop()
{
String coord ;
while(Serial.available()){
  coord="";
  Serial.println("In the loop");
  coord=Serial.readStringUntil('\r\n');
  }
  Serial.println(coord.c_str());

  Serial.println(Serial.read());

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
