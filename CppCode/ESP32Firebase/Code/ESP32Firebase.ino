/*********************************************
  __version__ = '2023.08.20.01'
  __author__ = 'Muthukumar Subramanian'
  1. ESP32 Store data to the Firebase Realtime Database
  2. ESP32 Read data from the Firebase Realtime Database
  ESP32-WROOM-DA Module support code
  GitHub: https://github.com/kumarmuthu/ESP32
*********************************************/
#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

/*************** Firebase Library [Firebase ESP32 Client] ***************/
#include <Firebase_ESP_Client.h>
/*************** Provide the token generation process info ********************/
#include <addons/TokenHelper.h>
/*************** Provide the RTDB payload printing info and other helper functions ********************/
#include <addons/RTDBHelper.h>


/********************************************* 
Replace with your network credentials
    Wifi name:      muthukumar_wifi
    Wifi password:  ***********
*********************************************/
#define WIFI_SSID "Redmi Note 11T 5G"
#define WIFI_PASSWORD "qwerty123m"

/*************** Insert Firebase project API Key ********************/
#define API_KEY "AIzaSyC7TL24xSTvJkGjfQk_L-ziwmm9AzdkD1E"

/*************** Insert RTDB(Realtime Database) URL ********************/
#define DATABASE_URL "https://esp32-firebase-oled-default-rtdb.asia-southeast1.firebasedatabase.app/"

/*************** Define Firebase Data object ********************/
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
int intValue;
float floatValue;


void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /*************** Assign the api key (required) ********************/
  config.api_key = API_KEY;

  /*************** Assign the RTDB URL (required) ********************/
  config.database_url = DATABASE_URL;

  /*************** Sign up ********************/
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /*************** Assign the callback function for the long running token generation task ********************/
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    /***** Write data from the Firebase Database *****/
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)) {
      Serial.println("Write - PASSED");
      Serial.println("Write - PATH: " + fbdo.dataPath());
      Serial.println("Write - TYPE: " + fbdo.dataType());
    } else {
      Serial.println("Write - FAILED");
      Serial.println("Write - REASON: " + String(fbdo.errorReason()));
    }
    count++;
    float float_num = 0.01 + random(0, 100);
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", float_num)) {
      Serial.println("Write - PASSED");
      Serial.println("Write - PATH: " + fbdo.dataPath());
      Serial.println("Write - TYPE: " + fbdo.dataType());
    } else {
      Serial.println("Write - FAILED");
      Serial.println("Write - REASON: " + String(fbdo.errorReason()));
    }

    /***** Read data from Firebase Database *****/
    // Read an Int number on the database path test/int
    if (Firebase.RTDB.getInt(&fbdo, "/test/int")) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();
        Serial.println("Read - " + String(intValue));
      }
    } else {
      Serial.println("Read - REASON: " + String(fbdo.errorReason()));
    }
    // Read an Float number on the database path test/float
    if (Firebase.RTDB.getFloat(&fbdo, "/test/float")) {
      if (fbdo.dataType() == "float") {
        floatValue = fbdo.floatData();
        Serial.println("Read - " + String(floatValue));
      }
    } else {
      Serial.println("Read - REASON: " + String(fbdo.errorReason()));
    }
  }
}