/*********************************************
  __version__ = '2022.12.03.01'
  __author__ = 'Muthukumar Subramanian'
  WebServer With Lightweight Website
  ESP32-WROM-DA Module support code
  GitHub: https://github.com/kumarmuthu/ESP32
*********************************************/
/***** Load Wi-Fi library *****/
#include "WiFi.h"
/***** Load Servo library *****/
#include "ESP32Servo.h"
/***** Load AsyncWebServer and SPIFFS library for webpage support *****/
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 

/***** Regex *****/
#include <regex>
#include <string>
#include <iostream>
/***** Create objects for two servomotors *****/
Servo myservo26;
Servo myservo33;
/***** Set servo motor's access pin *****/
int servopin26 = 26;
int servopin33 = 33;
int pos = 0;

/*********************************************
  Replace with your network credentials
    Wifi name:      muthukumar_wifi
    Wifi password:  ***********
*********************************************/

/***** Create web server object on port 80 *****/
//WiFiServer server(80);
/***** Create AsyncWebServer object on port 80 *****/
AsyncWebServer server(80);

/***** These variables to store the current(web page button) output state *****/
String output26State = "off";
String output33State = "off";
String cont_button = "off";


/***** Search for parameter in HTTP POST request *****/
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

/*****Variables to save values from HTML form *****/
String ssid;
String pass;
String ip;
String gateway;

/***** File paths to save input values permanently *****/
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

int totalColumns = 16;
int totalRows = 1;
LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);  

void scrollText(int row, String message, int delayTime, int lcdColumns) {
  for (int i=0; i < lcdColumns; i++) {
    message = " " + message;  
  }
  message = message + " "; 
  for (int pos = 0; pos < message.length(); pos++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(pos, pos + lcdColumns));
    delay(delayTime);
  }
}

void scan_lcd_display() {
  Wire.begin();
  Serial.println("ESP32 scanning for I2C devices");
  byte error_i2c, address_i2c;
  int I2C_Devices;
  Serial.println("Scanning started");
  I2C_Devices = 0;
  for(address_i2c = 1; address_i2c < 127; address_i2c++ )
  {
    Wire.beginTransmission(address_i2c);
    error_i2c = Wire.endTransmission();
    if (error_i2c == 0)
    {
      Serial.print("I2C device found at address_i2c 0x");
      if (address_i2c<16) 
      {
        Serial.print("0");
      }
      Serial.println(address_i2c,HEX);
      I2C_Devices++;
    }
    else if (error_i2c==4) 
    {
      Serial.print("Unknow error_i2c at address_i2c 0x");
      if (address_i2c<16) 
      {
      Serial.print("0");
      }
      Serial.println(address_i2c,HEX);
    } 
  }
  if (I2C_Devices == 0) 
  {
    Serial.println("No I2C device connected \n");
  }
  else 
  {
    Serial.println("done I2C device searching\n");
    delay(2000);
    lcd.begin(16, 1);
    lcd.init();                    
    lcd.backlight();
  }
}

void accessPointFunction() {
  /***** Set your Static IP address *****/
  IPAddress local_IP(192, 168, 1, 4);
  /***** Set your Gateway IP address *****/
  IPAddress gateway_ap(192, 168, 1, 4);
  IPAddress subnet(255, 255, 255, 0);

  /***** 'AP_PSWD' is an open Access Point password, always default ip is 192.168.1.4 *****/
  Serial.println("Setting ESP32 AP (Access Point)");
  WiFi.softAPConfig(local_IP, gateway_ap, subnet);
  const char* AP_PSWD = "Muthu@123";
  WiFi.softAP("MUTHU-ESP32-WIFI-MANAGER", AP_PSWD);

  IPAddress IP = WiFi.softAPIP();
  Serial.println("AP - ESP32 device IP address: " + IP.toString());
  Serial.println("Successfully Turned ON the device ESP32's Wifi-hotpspot...");
}


void updateWifiSsidPassword() {
  server.serveStatic("/", SPIFFS, "/");
  server.on("/wifimanager/config", HTTP_POST, [](AsyncWebServerRequest * request) {
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {
        /***** HTTP POST  value *****/
        if (p->name() == PARAM_INPUT_1) {
          ssid = p->value().c_str();
          Serial.print("SSID set to: ");
          Serial.println(ssid);
          /***** Write file to save value *****/
          writeFile(SPIFFS, ssidPath, ssid.c_str());
        }
        /***** HTTP POST pass value *****/
        if (p->name() == PARAM_INPUT_2) {
          pass = p->value().c_str();
          Serial.print("Password set to: ");
          Serial.println(pass);
          /***** Write file to save value *****/
          writeFile(SPIFFS, passPath, pass.c_str());
        }
        /***** HTTP POST ip value *****/
        if (p->name() == PARAM_INPUT_3) {
          ip = p->value().c_str();
          Serial.print("IP Address set to: ");
          Serial.println(ip);
          /***** Write file to save value *****/
          writeFile(SPIFFS, ipPath, ip.c_str());
        }
        /***** HTTP POST gateway value *****/
        if (p->name() == PARAM_INPUT_4) {
          gateway = p->value().c_str();
          Serial.print("Gateway set to: ");
          Serial.println(gateway);
          /***** Write file to save value *****/
          writeFile(SPIFFS, gatewayPath, gateway.c_str());
        }
      }
    }
    request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
    delay(3000);
    ESP.restart();
  });
  server.begin();
}

/***** Default WiFi configuration *****/
String defaultWifi() {

  const char* ssid = "Muthu_AT";
  const char* password = "*****";
  /***** Set your Static IP address *****/
  IPAddress local_IP(192, 168, 1, 5);
  /***** Set your Gateway IP address *****/
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  /***** DNS is optional *****/
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  /***** Configure the static IP-address *****/
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("DefaultWifi - STA Failed to configure!...");
  } else {
    Serial.println("DefaultWifi - Successfully configured the static ip-address...");
  }

  /***** Timer variables *****/
  unsigned long previousMillis = 0;
  /***** interval to wait for Wi-Fi connection (milliseconds) *****/
  const long interval = 10000;
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  /***** Connect to Wi-Fi network with SSID and password *****/
  Serial.println("DefaultWifi - WiFi Connecting to: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("DefaultWifi - Failed to connect defaultWifi.");
      return "default_wifi_fail";
    }
    delay(500);
    Serial.print(".");
  }
  /***** Print local IP address of the ESP32 device and start web server *****/
  Serial.println("");
  Serial.println("DefaultWifi - WiFi: " + String(ssid) + " is connected...");
  String ip_address = WiFi.localIP().toString();
  Serial.println("DefaultWifi - ESP32 device IP address: " + ip_address);
  return "default_wifi_pass";
}

/***** Custom WiFi configuration *****/
String CustomWiFi() {

  IPAddress localIPadd;
  IPAddress localGateway;
  IPAddress subnet(255, 255, 0, 0);

  if (ssid == "" || ip == "") {
    Serial.println("CustomWiFi - Undefined SSID or IP address.");
  }

  WiFi.mode(WIFI_STA);
  localIPadd.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());

  if (!WiFi.config(localIPadd, localGateway, subnet)) {
    Serial.println("CustomWiFi - STA Failed to configure");
    return "custom_wifi_fail";
  } else {
    Serial.println("CustomWiFi - Successfully configured the static ip-address...");
  }

  /***** Timer variables *****/
  unsigned long previousMillis = 0;
  const long interval = 10000;
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  Serial.println("CustomWiFi - WiFi Connecting to: " + String(ssid));
  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("CustomWiFi - Failed to connect.");
      return "custom_wifi_fail";
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("CustomWiFi - WiFi: " + String(ssid) + " is connected...");
  String ip_address = WiFi.localIP().toString();
  Serial.println("CustomWiFi - ESP32 device IP address: " + ip_address);
  return "custom_wifi_pass";
}

/***** Read File from SPIFFS *****/
String readFile(fs::FS& fs, const char* path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available()) {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

/***** Write file to SPIFFS *****/
void writeFile(fs::FS& fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File was written");
  } else {
    Serial.println("File write failed");
  }
}

// ESP32 restart
String processor_2(const String& var){
    if(var == "RESTART"){
    String buttons ="";
      String restart_current_status_value = restart_current_status();
      buttons+= "<div class=\"slide_button\" id=\"" + String('1') + "\" >";
      buttons+= "<label class=\"switch switch-left-right\">";
      buttons+= "<input class=\"switch-input\" type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String('1') + "\" "+ restart_current_status_value + "/>";
      buttons+= "<span class=\"switch-label\" data-on=\"Off\" data-off=\"On\"></span>";
      buttons+= "<span class=\"switch-handle\"></span>";
      buttons+= "<style scoped=\"scoped\" onload=\"slider_property_update()\" > </style>";
      buttons+= "</label></div><br />";
    return buttons;
  }
  return String();
}


String restart_current_status(){
  server.on("/esp32_restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("status")) {
        String rst_status = request->getParam("status")->value();
        if (rst_status == "off") {
          return "checked";
        } else if (rst_status == "on") {
           return "";
        }
     }
     });
  return "";
}

// HOSTSPOT
String processor_1(const String& var){
    if(var == "HOTSPOT"){
    String buttons ="";
      String hostspot_current_status_value = hostspot_current_status();
      buttons+= "<div class=\"slide_button\" id=\"" + String('1') + "\" >";
      buttons+= "<label class=\"switch switch-left-right\">";
      buttons+= "<input class=\"switch-input\" type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String('1') + "\" "+ hostspot_current_status_value + "/>";
      buttons+= "<span class=\"switch-label\" data-on=\"On\" data-off=\"Off\"></span>";
      buttons+= "<span class=\"switch-handle\"></span>";
      buttons+= "<style scoped=\"scoped\" onload=\"slider_property_update()\" > </style>";
      buttons+= "</label></div><br />";
    return buttons;
  }
  return String();
}


String hostspot_current_status(){
  server.on("/ESP32Hotspot/hotspot", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("status")) {
        String ht_status = request->getParam("status")->value();
        if (ht_status == "on") {
          return "checked";
        } else if (ht_status == "off") {
           return "";
        }
     }
     });
  return "";
}


#define servo_connected    true
#define number_of_GPIOs_pin  2
int servoGPIOs[number_of_GPIOs_pin] = {26, 33};
String servoGPIOsNames[number_of_GPIOs_pin] = { "Servo control-1", "Servo control-2" };

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONHOLDER"){
    String buttons ="";
    for(int i=1; i<=number_of_GPIOs_pin; i++){
      String servo_GPIOs_button_current_stateValue = servo_GPIOs_button_current_state(i);
      //buttons+= "<h4>" + servoGPIOsNames[i-1] + " - GPIO " + servoGPIOs[i-1] + "</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(servoGPIOs[i-1]) + "\" "+ servo_GPIOs_button_current_stateValue +"><span class=\"slider\"></span></label>";
      buttons+= "<div class=\"slide_button\" id=\"" + String(i) + "\" >";
      buttons+= "<h4>" + servoGPIOsNames[i-1] + " - GPIO " + servoGPIOs[i-1] + "</h4>";
      buttons+= "<label class=\"switch switch-left-right\">";
      buttons+= "<input class=\"switch-input\" type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"" + String(servoGPIOs[i-1]) + "\" "+ servo_GPIOs_button_current_stateValue + "/>";
      buttons+= "<span class=\"switch-label\" data-on=\"On\" data-off=\"Off\"></span>";
      buttons+= "<span class=\"switch-handle\"></span>";
      buttons+= "<style scoped=\"scoped\" onload=\"slider_property_update()\" > </style>";
      buttons+= "</label></div><br />";
      }
    return buttons;
  }
  return String();
}

String servo_GPIOs_button_current_state(int each_pin){
  if(servo_connected)
  {
    server.on("/project_servo/servo_run_page/" + each_pin, HTTP_GET, [](AsyncWebServerRequest * request) {
      if (request->hasParam("state")) {
          String state = request->getParam("state")->value();
          if (state == "on") {
            return "checked";
          } else if (state == "off") {
             return "";
          }
       } //request->send(200, "text/html"); 
       });
    }
//  else {
//    // TODO
//  }
  return "";
}

void motor_rotate(Servo& myservo) {
  /*********************************************
    ..codeauthor:: Muthukumar Subramanian
    Just rotate 0 to 180 and 180 to 0
  *********************************************/
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }
}

void muthu_servo_motor_code(Servo& myservo, int cont_run = 0) {
  /*********************************************
    ..codeauthor:: Muthukumar Subramanian
    Common method to run single rotation or continues rotation.
    Dynamically call either 26 or 33 pin of servo motor's object.
  *********************************************/
  Serial.println("Called muthu_servo_motor_code");
  int i = 0;
  int max_ration = 5;
  while (i <= max_ration) {
    if (cont_run == 0) {
      motor_rotate(myservo);
      break;
    } else {
      /***** Continuously run motor, it's based on the max_ration *****/
      motor_rotate(myservo);
      i++;
    }
  }
}


void setup() {
  /*********************************************
    ..codeauthor:: Muthukumar Subramanian
    Wifi is connecting here, and we can do initial step here
    *********************************************/
  /***** Serial port for debugging purposes *****/
  Serial.begin(115200);
  scan_lcd_display();
  lcd.init();
  lcd.backlight(); // use to turn on and turn off LCD back light
  Serial.println("ESP32 device start...");
  /***** Static port attach *****/
  //myservo26.attach(servopin26);
  //myservo33.attach(servopin33);

  /***** Initialize SPIFFS *****/
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  /**** Read values from SPIFFS and save   ****/
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = readFile(SPIFFS, ipPath);
  gateway = readFile(SPIFFS, gatewayPath);

  if (defaultWifi() == "default_wifi_fail") {
    if (CustomWiFi() == "custom_wifi_fail") {
      /***** Hotspot will turn on, then we can update a new ssid ,passowrd and ip-address(optional) from the webpasge(wifi-manager) *****/
      Serial.println("An Error has occurred while connecting Default WIFI's SSID and IP");
      /***** call Access point function *****/
      accessPointFunction();
       Serial.println("Now we can update the New SSID(wifi name), wifi password and IP-Address");
      /***** Access Point Web Server Root URL *****/
      server.on("/wifimanager.html", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/wifimanager.html");
      });
      /***** call update method *****/
      updateWifiSsidPassword();
    } else {
      Serial.println("Successfully connected SPIFFS(Custom WIfi configuration) file stored SSID and IP-Address");
    }
  }

  server.serveStatic("/", SPIFFS, "/");
  /***** Route for root '/' web page *****/
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/wifimanager.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/wifimanager.html");
  });
  updateWifiSsidPassword();
  
  server.on("/ESP32Hotspot/hotspot", HTTP_GET, [](AsyncWebServerRequest * request) {
    //String pageHTML = "<!DOCTYPE html><html><body><ul class="nav-links"><li><a href="../ESP32.html">Back to ESP32 Home</a></li><li><a class="active" href="../project_servo.html">Project Servo</a></li><li><a href="index.html">Logout</a></li></ul><div><a href='/ESP32Hotspot/hotspot?state=on'>hotspot turn on</a></div><br></div><a href='/ESP32Hotspot/hotspot?state=off'>hotspot turn off</a><div></body></html>";
         const char index_html_1[] PROGMEM = R"rawliteral(
         <!DOCTYPE HTML><html lang="en"><head> <meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Hotspot Page</title><link rel="icon" href="data:,"><style>html {font-family: Arial; display: inline-block; text-align: center;}
  h3 {margin:0;padding:0;font-family: sans-serif;text-align:center;color:black;font-size:16px;padding:auto;text-transform: uppercase;letter-spacing:4px;}
  h4 {margin:0;padding:0;font-family: sans-serif;text-align:center;color:black;font-size:13px;padding:auto;text-transform: uppercase;letter-spacing:2px;}
  .switch {position: relative;display: block;vertical-align: top;width: 100px;height: 30px;padding: 3px;margin:0;background: linear-gradient(to bottom, #eeeeee, #FFFFFF 25px);
  background-image: -webkit-linear-gradient(top, #eeeeee, #FFFFFF 25px);border-radius: 18px;box-shadow: inset 50px 50px 50px 50px blue, inset 0 1px 1px rgba(0, 0, 0, 0.05);
  cursor: pointer;box-sizing:content-box;}
  .switch-input {position: absolute;top: 0;left: 0;opacity: 0;box-sizing:content-box;}
  .switch-label {position: relative;display: block;height: inherit;font-size: 10px;text-transform: uppercase;background: #eceeef;border-radius: inherit;
    box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.12), inset 0 0 2px rgba(0, 0, 0, 0.15);box-sizing:content-box;}
  .switch-label:before, .switch-label:after {position: absolute;top: 50%%;margin-top: -.5em;line-height: 1;-webkit-transition: inherit;
    -moz-transition: inherit;-o-transition: inherit;transition: inherit;box-sizing:content-box;}
  .switch-label:before {content: attr(data-off);right: 11px;color: red;text-shadow: 0 1px rgba(255, 255, 255, 0.5);}
  .switch-label:after {content: attr(data-on);left: 11px;color: #FFFFFF;text-shadow: 0 1px rgba(0, 0, 0, 0.2);opacity: 0;}
  .switch-input:checked ~ .switch-label {background: #E1B42B;box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.15), inset 0 0 3px rgba(0, 0, 0, 0.2);}
  .switch-input:checked ~ .switch-label:before {opacity: 0;}   .switch-input:checked ~ .switch-label:after {opacity: 1;}
  .switch-handle {position: absolute;top: 4px;left: 4px;width: 28px;height: 28px;background: linear-gradient(to bottom, #FFFFFF 40%%, #f0f0f0);
    background-image: -webkit-linear-gradient(top, #FFFFFF 40%%, #f0f0f0);border-radius: 100%%;box-shadow: 1px 1px 5px rgba(0, 0, 0, 0.2);}
  .switch-handle:before {content: "";position: absolute;top: 50%%;left: 50%%;margin: -6px 0 0 -6px;width: 12px;height: 12px;background: linear-gradient(to bottom, #eeeeee, #FFFFFF);
    background-image: -webkit-linear-gradient(top, #ece9da, #ece9da);border-radius: 6px;box-shadow: inset 0 1px rgba(0, 0, 0, 0.02);}
  .switch-input:checked ~ .switch-handle {left: 74px;box-shadow: -1px 1px 5px rgba(0, 0, 0, 0.2);}   .switch-left-right .switch-label {overflow: hidden;}
  .switch-left-right .switch-label:before, .switch-left-right .switch-label:after {width: 20px;height: 20px;top: 4px;left: 0;right: 0;bottom: 0;padding: 11px 0 0 0;
    text-indent: -12px;border-radius: 20px;box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.2), inset 0 0 3px rgba(0, 0, 0, 0.1);}
  .switch-left-right .switch-label:before {background: #eceeef;text-align: left;padding-left: 80px;}
  .switch-left-right .switch-label:after {text-align: left;text-indent: 9px;background: green;left: -100px;opacity: 1;width: 100%%;}
  .switch-left-right .switch-input:checked ~ .switch-label:before {opacity: 1;left: 100px;}
  .switch-left-right .switch-input:checked ~ .switch-label:after {left: 0;} .switch-left-right .switch-input:checked ~ .switch-label {background: inherit;}
  .slide_button {border-radius: 8px;width: auto;height: auto;margin: 0;position: absolute;top: 12%%;left: 35%%;-ms-transform: translate(-50%%, -50%%);transform: translate(-50%%, -50%%);}
  .switch-label, .switch-handle {transition: All 0.3s ease;-webkit-transition: All 0.3s ease;-moz-transition: All 0.3s ease;-o-transition: All 0.3s ease;}  </style> </head>
  <body> <h1>ESP32 Wifi hotspot</h1> %HOTSPOT% 
        <script>function toggleCheckbox(element) {
            var xhr = new XMLHttpRequest();
            if(element.checked){ xhr.open("GET", "/eps32_hotspot/config/?status=on", true); }
            else { xhr.open("GET", "/eps32_hotspot/config/?status=off", true); }
            xhr.send();
          }</script>
          <script>
            function slider_property_update() {
              var len = document.getElementsByClassName("slide_button")
              var j= 0;
              for (var i=0; i<len.length; i++) {
                var ptg = (i * j) + (j + 20);
                document.getElementsByClassName("slide_button")[i].style.top = "" + ptg +"%%";
                 j+=10;
            }
            }</script>
    </body></html>
    )rawliteral";
    request->send_P(200, "text/html", index_html_1, processor_1);
  });

  server.on("/eps32_hotspot/config", HTTP_GET, [](AsyncWebServerRequest * request) {
      Serial.println("WIFI hotspot configuration page was accesed by client!.");
      if (request->hasParam("status")) {
      String ht_status = request->getParam("status")->value();
      if (ht_status == "on") {
        Serial.println("WIFI hotspot will Turn On...");
        accessPointFunction();
      } else if (ht_status == "off") {
        Serial.println("WIFI hotspot will be Turn Off state. Turn off it now(30 sec)!.");
        delay(3000);
        Serial.println("Restarting the ESP32 device...");
        ESP.restart();
      }
    }
    request->send(200);
   });

    server.on("/esp32_restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    //String pageHTML = "<!DOCTYPE html><html><body><ul class="nav-links"><li><a href="../ESP32.html">Back to ESP32 Home</a></li><li><a class="active" href="../project_servo.html">Project Servo</a></li><li><a href="index.html">Logout</a></li></ul><div><a href='/ESP32Hotspot/hotspot?state=on'>hotspot turn on</a></div><br></div><a href='/ESP32Hotspot/hotspot?state=off'>hotspot turn off</a><div></body></html>";
         const char index_html_2[] PROGMEM = R"rawliteral(
         <!DOCTYPE HTML><html lang="en"><head> <meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Restart Page</title><link rel="icon" href="data:,"><style>html {font-family: Arial; display: inline-block; text-align: center;}
  h3 {margin:0;padding:0;font-family: sans-serif;text-align:center;color:black;font-size:16px;padding:auto;text-transform: uppercase;letter-spacing:4px;}
  h4 {margin:0;padding:0;font-family: sans-serif;text-align:center;color:black;font-size:13px;padding:auto;text-transform: uppercase;letter-spacing:2px;}
  .switch {position: relative;display: block;vertical-align: top;width: 100px;height: 30px;padding: 3px;margin:0;background: linear-gradient(to bottom, #eeeeee, #FFFFFF 25px);
  background-image: -webkit-linear-gradient(top, #eeeeee, #FFFFFF 25px);border-radius: 18px;box-shadow: inset 50px 50px 50px 50px blue, inset 0 1px 1px rgba(0, 0, 0, 0.05);
  cursor: pointer;box-sizing:content-box;}
  .switch-input {position: absolute;top: 0;left: 0;opacity: 0;box-sizing:content-box;}
  .switch-label {position: relative;display: block;height: inherit;font-size: 10px;text-transform: uppercase;background: #eceeef;border-radius: inherit;
    box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.12), inset 0 0 2px rgba(0, 0, 0, 0.15);box-sizing:content-box;}
  .switch-label:before, .switch-label:after {position: absolute;top: 50%%;margin-top: -.5em;line-height: 1;-webkit-transition: inherit;
    -moz-transition: inherit;-o-transition: inherit;transition: inherit;box-sizing:content-box;}
  .switch-label:before {content: attr(data-off);right: 11px;color: red;text-shadow: 0 1px rgba(255, 255, 255, 0.5);}
  .switch-label:after {content: attr(data-on);left: 11px;color: #FFFFFF;text-shadow: 0 1px rgba(0, 0, 0, 0.2);opacity: 0;}
  .switch-input:checked ~ .switch-label {background: #E1B42B;box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.15), inset 0 0 3px rgba(0, 0, 0, 0.2);}
  .switch-input:checked ~ .switch-label:before {opacity: 0;}   .switch-input:checked ~ .switch-label:after {opacity: 1;}
  .switch-handle {position: absolute;top: 4px;left: 4px;width: 28px;height: 28px;background: linear-gradient(to bottom, #FFFFFF 40%%, #f0f0f0);
    background-image: -webkit-linear-gradient(top, #FFFFFF 40%%, #f0f0f0);border-radius: 100%%;box-shadow: 1px 1px 5px rgba(0, 0, 0, 0.2);}
  .switch-handle:before {content: "";position: absolute;top: 50%%;left: 50%%;margin: -6px 0 0 -6px;width: 12px;height: 12px;background: linear-gradient(to bottom, #eeeeee, #FFFFFF);
    background-image: -webkit-linear-gradient(top, #ece9da, #ece9da);border-radius: 6px;box-shadow: inset 0 1px rgba(0, 0, 0, 0.02);}
  .switch-input:checked ~ .switch-handle {left: 74px;box-shadow: -1px 1px 5px rgba(0, 0, 0, 0.2);}   .switch-left-right .switch-label {overflow: hidden;}
  .switch-left-right .switch-label:before, .switch-left-right .switch-label:after {width: 20px;height: 20px;top: 4px;left: 0;right: 0;bottom: 0;padding: 11px 0 0 0;
    text-indent: -12px;border-radius: 20px;box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.2), inset 0 0 3px rgba(0, 0, 0, 0.1);}
  .switch-left-right .switch-label:before {background: #eceeef;text-align: left;padding-left: 80px;}
  .switch-left-right .switch-label:after {text-align: left;text-indent: 9px;background: green;left: -100px;opacity: 1;width: 100%%;}
  .switch-left-right .switch-input:checked ~ .switch-label:before {opacity: 1;left: 100px;}
  .switch-left-right .switch-input:checked ~ .switch-label:after {left: 0;} .switch-left-right .switch-input:checked ~ .switch-label {background: inherit;}
  .slide_button {border-radius: 8px;width: auto;height: auto;margin: 0;position: absolute;top: 12%%;left: 35%%;-ms-transform: translate(-50%%, -50%%);transform: translate(-50%%, -50%%);}
  .switch-label, .switch-handle {transition: All 0.3s ease;-webkit-transition: All 0.3s ease;-moz-transition: All 0.3s ease;-o-transition: All 0.3s ease;}  </style> </head>
  <body> <h1>ESP32 device Restart</h1> %RESTART% 
        <script>function toggleCheckbox(element) {
            var xhr = new XMLHttpRequest();
            if(element.checked){ xhr.open("GET", "/restart_esp32/restart?status=off", true); }
            else { xhr.open("GET", "/restart_esp32/restart?status=on", true); }
            xhr.send();
          }</script>
          <script>
            function slider_property_update() {
              var len = document.getElementsByClassName("slide_button")
              var j= 0;
              for (var i=0; i<len.length; i++) {
                var ptg = (i * j) + (j + 20);
                document.getElementsByClassName("slide_button")[i].style.top = "" + ptg +"%%";
                 j+=10;
            }
            }</script>
    </body></html>
    )rawliteral";
    request->send_P(200, "text/html", index_html_2, processor_2);
  });
  
   server.on("/restart_esp32/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
      if (request->hasParam("status")) {
      String rst_status = request->getParam("status")->value();
      if (rst_status == "on") {
        Serial.println("ESP32 is Turn On State...");
      } else if (rst_status == "off") {
        Serial.println("Restarting in 30 seconds");
        delay(3000);
        Serial.println("Restarting the ESP32 device...");
        ESP.restart();
      }
    }
    request->send(200);
   });
  
  /***** my web page files *****/
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/login.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/login.html", String(), false, processor);
  });
  server.on("/about.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/about.html", String(), false, processor);
  });
  server.on("/contact.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/contact.html", String(), false, processor);
  });
  server.on("/ESP32.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/ESP32.html", String(), false, processor);
  });
  server.on("/project_servo.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/project_servo.html", String(), false, processor);
  });
  server.on("/project_remote.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/project_remote.html", String(), false, processor);
  });
  server.on("/ESP32Hotspot.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/ESP32Hotspot.html", String(), false, processor);
  });

  /***** Route to load app.js file *****/
  server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/app.js", "text/javascript");
  });
  /***** Route to load style.css file *****/
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  /***** Route to load style.css file *****/
  server.on("/logo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/logo.png", "image/png");
  });

  server.on("/project_servo/servo_run_page", HTTP_GET, [](AsyncWebServerRequest * request) {
    const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html lang="en"><head> <meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Servo Run Page</title><link rel="icon" href="data:,"><style>html {font-family: Arial; display: inline-block; text-align: center;}
  h3 {margin:0;padding:0;font-family: sans-serif;text-align:center;color:black;font-size:16px;padding:auto;text-transform: uppercase;letter-spacing:4px;}
  h4 {margin:0;padding:0;font-family: sans-serif;text-align:center;color:black;font-size:13px;padding:auto;text-transform: uppercase;letter-spacing:2px;}
  .switch {position: relative;display: block;vertical-align: top;width: 100px;height: 30px;padding: 3px;margin:0;background: linear-gradient(to bottom, #eeeeee, #FFFFFF 25px);
  background-image: -webkit-linear-gradient(top, #eeeeee, #FFFFFF 25px);border-radius: 18px;box-shadow: inset 50px 50px 50px 50px blue, inset 0 1px 1px rgba(0, 0, 0, 0.05);
  cursor: pointer;box-sizing:content-box;}
  .switch-input {position: absolute;top: 0;left: 0;opacity: 0;box-sizing:content-box;}
  .switch-label {position: relative;display: block;height: inherit;font-size: 10px;text-transform: uppercase;background: #eceeef;border-radius: inherit;
    box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.12), inset 0 0 2px rgba(0, 0, 0, 0.15);box-sizing:content-box;}
  .switch-label:before, .switch-label:after {position: absolute;top: 50%%;margin-top: -.5em;line-height: 1;-webkit-transition: inherit;
    -moz-transition: inherit;-o-transition: inherit;transition: inherit;box-sizing:content-box;}
  .switch-label:before {content: attr(data-off);right: 11px;color: red;text-shadow: 0 1px rgba(255, 255, 255, 0.5);}
  .switch-label:after {content: attr(data-on);left: 11px;color: #FFFFFF;text-shadow: 0 1px rgba(0, 0, 0, 0.2);opacity: 0;}
  .switch-input:checked ~ .switch-label {background: #E1B42B;box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.15), inset 0 0 3px rgba(0, 0, 0, 0.2);}
  .switch-input:checked ~ .switch-label:before {opacity: 0;}   .switch-input:checked ~ .switch-label:after {opacity: 1;}
  .switch-handle {position: absolute;top: 4px;left: 4px;width: 28px;height: 28px;background: linear-gradient(to bottom, #FFFFFF 40%%, #f0f0f0);
    background-image: -webkit-linear-gradient(top, #FFFFFF 40%%, #f0f0f0);border-radius: 100%%;box-shadow: 1px 1px 5px rgba(0, 0, 0, 0.2);}
  .switch-handle:before {content: "";position: absolute;top: 50%%;left: 50%%;margin: -6px 0 0 -6px;width: 12px;height: 12px;background: linear-gradient(to bottom, #eeeeee, #FFFFFF);
    background-image: -webkit-linear-gradient(top, #ece9da, #ece9da);border-radius: 6px;box-shadow: inset 0 1px rgba(0, 0, 0, 0.02);}
  .switch-input:checked ~ .switch-handle {left: 74px;box-shadow: -1px 1px 5px rgba(0, 0, 0, 0.2);}   .switch-left-right .switch-label {overflow: hidden;}
  .switch-left-right .switch-label:before, .switch-left-right .switch-label:after {width: 20px;height: 20px;top: 4px;left: 0;right: 0;bottom: 0;padding: 11px 0 0 0;
    text-indent: -12px;border-radius: 20px;box-shadow: inset 0 1px 4px rgba(0, 0, 0, 0.2), inset 0 0 3px rgba(0, 0, 0, 0.1);}
  .switch-left-right .switch-label:before {background: #eceeef;text-align: left;padding-left: 80px;}
  .switch-left-right .switch-label:after {text-align: left;text-indent: 9px;background: green;left: -100px;opacity: 1;width: 100%%;}
  .switch-left-right .switch-input:checked ~ .switch-label:before {opacity: 1;left: 100px;}
  .switch-left-right .switch-input:checked ~ .switch-label:after {left: 0;} .switch-left-right .switch-input:checked ~ .switch-label {background: inherit;}
  .slide_button {border-radius: 8px;width: auto;height: auto;margin: 0;position: absolute;top: 12%%;left: 35%%;-ms-transform: translate(-50%%, -50%%);transform: translate(-50%%, -50%%);}
  .switch-label, .switch-handle {transition: All 0.3s ease;-webkit-transition: All 0.3s ease;-moz-transition: All 0.3s ease;-o-transition: All 0.3s ease;}  </style> </head>
<body>  <h3>ESP32 Server motor run</h3>  %BUTTONHOLDER% <script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/servo_run_page/run/"+element.id+"?state=on", true); }
  else { xhr.open("GET", "/servo_run_page/run/"+element.id+"?state=off", true); }
  xhr.send();
}</script>
<script>
function slider_property_update() {
  var len = document.getElementsByClassName("slide_button")
  var j= 0;
  for (var i=0; i<len.length; i++) {
    var ptg = (i * j) + (j + 20);
    document.getElementsByClassName("slide_button")[i].style.top = "" + ptg +"%%";
     j+=10;
}
}</script></body></html>
  )rawliteral";
  request->send_P(200, "text/html", index_html, processor);
  });
   server.on("/servo_run_page/run/*", HTTP_GET, [](AsyncWebServerRequest * request) {
        String gpios_pin_number_26 = "26";
        String gpios_pin_number_33 = "33";
        std::string str = request->url().c_str();
        std::regex rgx {R"(.*\/(\d+))"};
        std::smatch match;
        std::regex_search(str, match, rgx);
        std::string pin_number = match[1];

        //String url = request->url();
        //Serial.println(url);
        //Serial.println("url print");
        // Serial.printf(pin_number.c_str());
        //if(std::regex_search(str, match, rgx)){
        //  for (size_t i = 0; i < match.size(); ++i){
        //      std::cout << i << ": '" << match[i] << "'\n";
        //  }
        //}
        String pin_number_final = String(pin_number.c_str());
        int args = request->args();
        for(int i=0;i<args;i++){
          Serial.printf("DEBUG:  ARG[%s]: \/%s %s\n", request->argName(i).c_str(), pin_number_final, request->arg(i).c_str());
        }

      if (request->hasParam("state")) {
      String angle = request->getParam("state")->value();
      if(pin_number_final == gpios_pin_number_26){
        // Allow allocation of all timers
        ESP32PWM::allocateTimer(0);
        ESP32PWM::allocateTimer(1);
        ESP32PWM::allocateTimer(2);
        ESP32PWM::allocateTimer(3);
        myservo26.setPeriodHertz(50);    // standard 50 hz servo
        myservo26.attach(servopin26, 500, 2400);
        //myservo26.attach(servopin26);
        if (angle == "off") {
          myservo26.write(0);
        } else if (angle == "on") {
          myservo26.write(90);
        }
        //delay(5000);
        //myservo26.detach();
      }
      else if (pin_number_final == gpios_pin_number_33){
        myservo33.attach(servopin33);
        if (angle == "off") {
          myservo33.write(0);
        } else if (angle == "on") {
          myservo33.write(90);
        }
        //delay(5000);
        //myservo33.detach();
      }
    }
    request->send(200);
   });
  /***** Start server *****/
  server.begin();
}

void loop() {
  lcd.setCursor(1, 0);
  String input_string = "Muthukumar 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16";
  // lcd.print(input_string);
  lcd.setCursor(16,1);
  lcd.autoscroll();
  //lcd.print(input_string);
  //delay(500);
  // for (int j = 0; j < input_string.length(); j++) {
  //  Serial.println(j);
  // delay(500);
  // }
  //lcd.noAutoscroll();
  //lcd.clear();
   unsigned int i=0;
  //lcd.setCursor(0, 0);
  while(input_string[i]!='\0'){
    lcd.print(input_string[i]);
    if(i>=14)
    {
      lcd.command(0x18); //Scrolling text to Right
    }
    delay(400);
    i++;
    }
}
