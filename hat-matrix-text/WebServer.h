
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>

//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>

#include <HTTPClient.h>


class WebServer
{
  AsyncWebServer server;
  HTTPClient http;

public:
  unsigned long time_of_last_message = 0;
  // received message
  String message = "";

  // received message
  char receive_buffer[128];
  // received message length
  size_t received_len = 0;

public:

  WebServer(int port) : server(port) {}

  void begin() {
    registerRequests();
    server.begin();
    
    http.begin("http://hat.naoth.de/api/texts/top-text/");
  }
  
  void pull_new_message() 
  {
    int httpCode = http.GET();
    if(httpCode > 0) {
      message = http.getString();
      if(message.length() > 0) {
        time_of_last_message = millis();
        Serial.println(message);
      }
      Serial.print("http code: ");
      Serial.println(httpCode);
    } else {
      Serial.print("http error code: ");
      Serial.println(httpCode);
    }
  }

private:
  char* make_string(uint8_t* inputData, size_t len) {
    assert(len < sizeof(receive_buffer));
    memcpy(receive_buffer, inputData, len);
    receive_buffer[len] = '\0';
    received_len = len;
    return receive_buffer;
  }

  void registerRequests()
  {
    // register get responses
    server.on("/log", HTTP_GET, [&](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", message);
    });

    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
      Serial.println("<main root>");
      String str;
      str += "<p>LightHat Control. Use apropriate get requests.</p>";
      request->send(200, "text/plain", str);
    });
    
    server.on("/test", HTTP_GET, [&](AsyncWebServerRequest *request) {
      memcpy(receive_buffer, "test", 4);
      received_len = 4;
      message = String("test");
      time_of_last_message = millis();
      Serial.println("/test");
      request->send(200);
    });
    
    server.on("/blub", HTTP_GET, [&](AsyncWebServerRequest *request) {
      memcpy(receive_buffer, "blubblub", 8);
      received_len = 8;
      message = String("blubblub");
      time_of_last_message = millis();
      Serial.println("/blubblub");
      request->send(200);
    });

    //three handlers: onRequest, onFileUpload, onBody
    server.on("/text", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
      [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
      Serial.println("/text");
      message = String(make_string(data_website, len));
      time_of_last_message = millis();
      
      Serial.println(message);
      request->send(200, "text/plain", message);
    });
    
  } // registerRequests

};

#endif // WEB_SERVER_H
