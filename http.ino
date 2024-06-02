#include "http.h"
#include "infrared.h"
#include "wifi.h"
#include <WebServer.h>

WebServer server(80);

String renderHtml(String message) {
  const String htmlHead = "<!DOCTYPE html>"
                          "<html>"
                          "<head>"
                          "<title>ESP32 Wifi Remote</title>"
                          "</head>"
                          "<body>"
                          "<h1>";
  const String htmlFoot =
      "</h1>"
      "<h2>IR Remote actions</h2>"
      "<form action=\"/sendon\" method=\"POST\">"
      "<button type=\"submit\">Send Power On</button>"
      "</form>"
      "<h2>Wifi parameters</h2>"
      "<form action=\"/\" method=\"POST\">"
      "<label for=\"ssid\">SSID</label>"
      "<input id=\"ssid\" name=\"ssid\" type=\"text\"><br>"
      "<label for=\"passwd\">Password</label>"
      "<input id=\"passwd\" name=\"passwd\" type=\"password\"><br>"
      "<button type=\"submit\">Submit</button>"
      "</form>"
      "</body>"
      "</html>";

  return htmlHead + message + htmlFoot;
}

void handleRoot() { server.send(200, "text/html", renderHtml("Welcome")); }

void handleSetAsStation() {
  String ssid;
  String wifiPasswd;

  for (int i = 0; i < server.args(); i++) {
    if (server.argName(i).equals("ssid")) {
      Serial.print("HTTP Will try to connect to ");
      ssid = server.arg(i).c_str();
      Serial.println(ssid);
    }

    if (server.argName(i).equals("passwd")) {
      Serial.print("HTTP Will try to connect with password ");
      wifiPasswd = server.arg(i).c_str();
      Serial.println(wifiPasswd);
    }
  }

  if (ssid.isEmpty()) {
    server.send(400, "text/html", renderHtml("Missing ssid"));
    return;
  }

  String message;

  message = "Connecting to ";
  message.concat(ssid);
  message += "</p>";
  server.send(200, "text/html", renderHtml(message));
  clearWifiCredentials();
  setAsStation(ssid, wifiPasswd);
}

void handleSendPowerOn() {
  server.send(200, "text/html", renderHtml("Sent IR power on"));
  sendOn();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void initHttp() {
  initWifi();

  server.on("/", HTTPMethod::HTTP_GET, handleRoot);
  server.on("/", HTTPMethod::HTTP_POST, handleSetAsStation);
  server.on("/sendon", HTTPMethod::HTTP_POST, handleSendPowerOn);
  server.onNotFound(handleNotFound);
  Serial.println("HTTP server started");
  server.begin();
}

void handleHttp() { server.handleClient(); }
