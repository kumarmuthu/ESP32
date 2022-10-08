/*********************************************
__version__ = '2022.09.24.01'
__author__ = 'Muthukumar Subramanian'
WebServerWithServoMotorControl powered by Wifi, C++
ESP32-WROM-DA Module support code
GitHub: https://github.com/kumarmuthu
*********************************************/
/***** Load Wi-Fi library *****/
#include <WiFi.h>
/***** Load Servo library *****/
#include <ESP32Servo.h>
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
const char* ssid = "Muthu_AT";
const char* password = "***********";

/***** Set web server port number to 80 *****/
WiFiServer server(80);

/***** Variable to store the HTTP request *****/
String header;

/***** These variables to store the current(web page button) output state *****/
String output26State = "off";
String output33State = "off";
String cont_button = "off";

/***** Current time *****/
unsigned long currentTime = millis();
/***** Previous time *****/
unsigned long previousTime = 0; 
/***** Define timeout time in milliseconds (example: 2000ms = 2s) *****/
const long timeoutTime = 2000;

void setup() {
  /*********************************************
  ..codeauthor:: Muthukumar Subramanian
  Wifi is connecting here, and we can do initial setp here
  *********************************************/
  Serial.begin(115200);
  /***** myservo26.attach(servopin26); - Static port declare(Skipped) *****/
  /***** myservo33.attach(servopin33); - Static port declare(Skipped) *****/
  
  /***** Set your Static IP address *****/
  IPAddress local_IP(192, 168, 1, 5);
  /***** Set your Gateway IP address *****/
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 0, 0);
  /***** DNS is optional *****/
  IPAddress primaryDNS(8, 8, 8, 8);
  IPAddress secondaryDNS(8, 8, 4, 4);

  /***** Configure the static IP-address *****/
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure!...");
  }
  else{
    Serial.println("Successfully configured the static ip-address...");
  }

  /***** Connect to Wi-Fi network with SSID and password *****/
  Serial.println("WiFi Connecting to: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /***** Print local IP address of the ESP32 device and start web server *****/
  Serial.println("");
  Serial.println("WiFi: " + String(ssid) + " is connected...");
  String ip_address = WiFi.localIP().toString();
  Serial.println("ESP32 device IP address: " + ip_address);
  /***** Start server *****/
  server.begin();
}

void motor_rotate(Servo &myservo){
  /*********************************************
  ..codeauthor:: Muthukumar Subramanian
  Just rotate 0 to 180 and 180 to 0
  *********************************************/
  for(pos =0; pos <=180; pos +=1){
    myservo.write(pos);
    delay(15);
  }
  for(pos =180; pos >=0; pos -=1){
    myservo.write(pos);
    delay(15);
  }
}

void muthu_servo_motor_code(Servo &myservo, int cont_run=0){
  /*********************************************
  ..codeauthor:: Muthukumar Subramanian
  Common method to run single rotation or continues rotation.
  Dynamically call either 26 or 33 pin of servo motor's object.
  *********************************************/
  int i = 0;
  int max_ration = 5;
  while (i <= max_ration) {
    if (cont_run == 0) {
       motor_rotate(myservo);
      break;
    } else{
      /***** Continuously run motor, it's based on the max_ration *****/
      motor_rotate(myservo);
      i++;
    }
  }
}

void loop(){
  /*********************************************
  ..codeauthor:: Muthukumar Subramanian
  Main method of the program
  servo motor run and web server and clinet stuff here
  *********************************************/
  /***** Checking the incoming clients request *****/
  WiFiClient client = server.available();

  if (client) {
    /***** Attach the port on the particular servo object *****/
    myservo26.attach(servopin26);
    myservo33.attach(servopin33);      

    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    /***** make a String to hold incoming data from the client *****/
    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      /***** loop while the client's connected *****/
      currentTime = millis();
      /***** if there's bytes to read from the client *****/
      if (client.available()) {
        /***** read a byte *****/
        char c = client.read();
        /***** print it out the serial monitor *****/
        Serial.write(c);
        header += c;
        /***** if the byte is a newline character *****/
        if (c == '\n') {
          /*****************
          if the current line is blank, we got two newline characters in a row.
          that is the end of the client HTTP request, so send a response:
          *****************/
          if (currentLine.length() == 0) {
            /*****************
            HTTP headers always start with a response code (HTTP/1.1 200 OK)
            and a content-type so the client knows what's coming, then a blank line:
            *****************/
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            /*****************
            URL read from GET method, GPIOs on and off based on the pin number
            Servo motor Single rotation logic here
            *****************/
            if (header.indexOf("GET /26/on") >= 0) {
              /***** ON(26 pin) button request *****/
              Serial.println("GPIO 26 on");
              output26State = "on";
              cont_button = "off";
              /***** Call Servo function for 26 pin *****/
              muthu_servo_motor_code(myservo26);
            } else if (header.indexOf("GET /26/off") >= 0) {
              /***** OFF(26 pin) button request *****/
              Serial.println("GPIO 26 off");
              output26State = "off";
              cont_button = "off";
              /***** TODO: OFF button *****/
            } else if (header.indexOf("GET /33/on") >= 0) {
              /***** ON(33 pin) button request *****/
              Serial.println("GPIO 33 on");
              output33State = "on";
              /***** Call Servo function for 33 pin *****/
              muthu_servo_motor_code(myservo33);
            } else if (header.indexOf("GET /33/off") >= 0) {
              /***** OFF(33 pin) button request *****/
              Serial.println("GPIO 33 off");
              output33State = "off";
              /***** TODO: OFF button *****/
            }
            /***** Servo motor Continues rotation, as of now 5 times(0 to 180 then 180 to 0) *****/
            if (header.indexOf("GET /26/cont/on") >= 0) {
               /***** ON(26 pin) button request *****/
              Serial.println("GPIO 26 cont on");
              cont_button = "on";
              int cont_var = 1;
              muthu_servo_motor_code(myservo26, cont_var);
            }
            else if (header.indexOf("GET /26/cont/off") >= 0) {
              /***** OFF(26 pin) button request *****/
              Serial.println("GPIO 26 cont off");
              cont_button = "off";
            }
            
            /***** Display the HTML web page *****/
            client.println("<!DOCTYPE html><html lang=\"en\">");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            /***** CSS to style the on/off buttons *****/
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button1 {width: 150px; transition-duration: 0.4s; background-color: #00cf00; border: 2px solid #4caf50;");
            client.println("border-radius: 8px; color: white; padding: 16px 40px; text-align: center; text-decoration: none;");
            client.println("font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button1:hover {color: white; background-color: #25ff46;");
            client.println("box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);}");
            client.println(".button2 {width: 150px; transition-duration: 0.4s; background-color: #0000ff; border: 2px solid #008cba;");
            client.println("border-radius: 8px; color: white; padding: 16px 40px; text-align: center; text-decoration: none;");
            client.println("font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2:hover {color: white; background-color: #00b4f4;");
            client.println("box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);}</style></head>");
            
            /***** Web Page Header *****/
            client.println("<body><h1>Muthu ESP32 Web Server</h1>");
            /***** Display current state, and ON/OFF buttons for GPIO 26 *****/
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            /***** If the output26State is off, it displays the ON button *****/ 
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button1\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            /***** Display current state, and ON/OFF buttons for GPIO 33 *****/
            client.println("<p>GPIO 33 - State " + output33State + "</p>");
            /***** If the output33State is off, it displays the ON button *****/
            if (output33State=="off") {
              client.println("<p><a href=\"/33/on\"><button class=\"button1\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/33/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            /***** Display the current state of continues button, and ON/OFF buttons for GPIO 26 *****/
            client.println("<p>GPIO cont 26 - State " + cont_button + "</p>");
            if (cont_button =="off") {
              client.println("<p><a href=\"/26/cont/on\"><button class=\"button1\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/cont/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            /***** The HTTP response ends with another blank line *****/
            client.println();
            /***** Break out of the while loop *****/
            break;
          } else {
            /***** if you got a newline, then clear currentLine *****/
            currentLine = "";
          }
        } else if (c != '\r') {
          /***** if you got anything else but a carriage return character *****/
          currentLine += c;
          /***** add it to the end of the currentLine *****/
        }
      }
    }
    /***** Clear the header variable *****/
    header = "";
    /***** Close the connection *****/
    client.stop();
    Serial.println("Client disconnected...");
    Serial.println("detach the ports...");
    myservo26.detach();
    myservo33.detach();
    Serial.println("");
  }
}