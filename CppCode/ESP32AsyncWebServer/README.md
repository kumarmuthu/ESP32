# ESP32
ESP32-WROOM controller board. **C++, Micro Python**

Device name: **ESP32-WROOM**

code editor **IDE**: **Arduino**

Execution video available on **YouTube**: https://www.youtube.com/watch?v=0x8Y8xbWLao

# Lightweight Website for ESP32 device(MuthuAutomation's own website)
Accomplished ESP32 Async WebServer With Website's Goals.

**Key features:**
1. Default Navigation bar for home, login, about, and contact.
2. Supported mobile views and desktop views for the webserver access.
3. We can access the device with a hotspot when it's in offline mode.
4. Wi-Fi manager config support, We can update the SSID, password, IP and gateway after code freeze(hard coded), with help of a hotspot.
  * 4.1 ESP32 code has default Wi-Fi credentials(hard coded in ino), custom Wi-Fi configuration(SPIFFS, it's a file system support). SPIFFS file will update when we change the SSID password from the Wi-Fi manager page.
  * 4.2 Launch the hotspot if default and custom Wi-Fi credentials fail.
5. We can restart the esp32 device through the URL.
6. With the internet, the login field of username and password is validated with Google Firebase database credentials.
7. Without the internet, possible to login hard-coded values with hotspot support.
8. Added prevent logic, a user directly can't access my HTML page by writing a URL(used only JavaScript), you have to log in and go to all pages except home, login, about, and contact. Because these are the navigation bar of pages.
9. Retaining the logged-in username on the local storage until the logout.
10. Added static IP address support for the Wi-Fi and hotspot IP addresses (covered in the esp32 code).
11. Port forwarding is not supported(#TODO), we can access website/esp32 device within the router network.

**General support:**
1. Wi-Fi.
2. Hotspot.
3. Login and logout.
4. Google Firebase database access(read and write).

**ESP32 specific support:**
1. Esp32 device of two servo motor controllers through the asynchronous web server.
2. Esp32 remote (in feature, 4 direction controls. Html template available, #TODO ino code).
3. Update the sensor/relay output on Google Firebase of real-time database table(Monitor service). #TODO
4. LCD display. #TODO
5. Esp32 email support. #TODO
6. Esp32 mesh network or JSON file communication between more than one device. #TODO
