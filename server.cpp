#include "WiFi.h"
#include <ESPAsyncWebServer.h>

//Put your router config here
//const char* ssid = "ESP32-Access-Point";
//const char* password = "123456789";


// Create AsyncWebServer object on port 1337
AsyncWebServer server(1337);

//Import EPD lib, set pins and configure display
#include "Adafruit_ThinkInk.h"
#define SRAM_CS     -1
#define EPD_CS      15
#define EPD_DC      33
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)  
#define COLOR1      EPD_BLACK

ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

//chunk 'size' for data being POSTed to Server
uint8_t image[4736];

void setup() {
  Serial.begin(115200);

  display.begin();

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // sendImage route is what client should POST raw bitmap image to. This route then assembles chunks of 4.7k bytes and uses memcpy until final image is assembled. Then..it's sent to display
  server.on(
    "/sendImage",
    HTTP_POST,
  [](AsyncWebServerRequest * request) {},
  NULL,
  [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
    memcpy(image + index, data, len);

    if (len + index == total) {
      display.clearBuffer();
      display.drawBitmap(0, 0, image, 296, 128, EPD_BLACK);
      display.display();
    }
    

    request->send(200);
  });

  // Start server
  server.begin();
}

void loop() {
  delay(2000);
}
