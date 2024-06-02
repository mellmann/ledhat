#ifndef NET_H
#define NET_H

#include <Arduino.h>
#include <WiFi.h>

class Net 
{
public:
  // FIXME: should be replaced
  bool startAP(const char *ssid, const char *password)
  {
      Serial.println("Initialising WiFi as access point...");

      // Set Static IP address
      IPAddress local_IP(10, 0, 4, 99);
      // Set your Gateway IP address
      IPAddress gateway(10, 0, 4, 1);
      IPAddress subnet(255, 255, 0, 0);

      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      delay(1000);

      // Configures static IP address
      if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
          Serial.println("AP Failed to configure");
          return false;
      } else {
          delay(1000); // FIXME
      }

      const IPAddress localAddress = WiFi.softAPIP();
      Serial.print("Connected. Local address: ");
      Serial.println(localAddress);

      return true;
  }

  // extracted from ChalkWebServerAsync.h
  // FIXME: should be replaced
  bool connect(const char *ssid, const char *password)
  {
      Serial.println("Connection to an existing WiFi network ...");

      WiFi.mode(WIFI_AP_STA);
      //WiFi.mode(WIFI_STA);
      {
          Serial.print("connecting to: ");
          Serial.println(ssid);
      }
      
      WiFi.begin(ssid, password);
      // wait for connection
      while (WiFi.status() != WL_CONNECTED) {
          delay(500); // FIXME
          Serial.print(".");
      }
      Serial.println();

      /*
      // connect to existing wifi
      // Set Static IP address
      IPAddress local_IP(10, 0, 4, 99);
      // Set your Gateway IP address
      IPAddress gateway(10, 0, 4, 1);
      IPAddress subnet(255, 255, 0, 0);
      if (!WiFi.config(local_IP, gateway, subnet)) {
          logger.log_error("STA Failed to configure");
      }
      */

      const IPAddress localAddress = WiFi.localIP();

      //WiFi.setHostname("chalkbot");

      Serial.print("Connected. Local address: ");
      Serial.println(localAddress);
      return true;
  }
};

#endif
