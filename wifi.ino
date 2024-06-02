#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#define FORMAT_SPIFFS_IF_FAILED true

#include "http.h"
#include "led.h"
#include "wifi.h"

const char *apSsid = "Wifi-Remote";
const String connFile = "/conn";
const char *HOSTNAME = "ds-esp32-ir";

String currentSsid;
String currentWifiPasswd;

bool loadWifiCredentials() {
  Serial.println("Reading wifi conn file");

  File file = SPIFFS.open(connFile, FILE_READ);
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open wifi conn file");
    return false;
  }

  const uint16_t fileMaxLength = MAX_SSID_LEN + MAX_PASSPHRASE_LEN + 4;

  char fileBuffer[fileMaxLength];
  memset(fileBuffer, 0, fileMaxLength);
  file.readBytes(fileBuffer, fileMaxLength);
  file.close();

  String fileData = String(fileBuffer);
  const int separator = fileData.indexOf('\n');
  if (separator == -1) {
    Serial.println("Invalid stored config. No newline found");
    return false;
  }
  const int endSeparator = fileData.indexOf('\n', separator + 1);
  if (endSeparator == -1) {
    Serial.println("Invalid stored config. File does not end with newline");
    return false;
  }

  String storedSsid = fileData.substring(0, separator);
  String storedPasswd = fileData.substring(separator + 1, endSeparator - 1);

  currentSsid = storedSsid;
  currentWifiPasswd = storedPasswd;

  return true;
}

void saveWifiCredentials() {
  File file = SPIFFS.open(connFile, FILE_WRITE, true);
  if (!file) {
    Serial.println("Failed to open wifi conn file for writing");
    return;
  }

  const String connData = currentSsid + "\n" + currentWifiPasswd;

  if (file.println(connData)) {
    Serial.println("Saved wifi configuration");
  } else {
    Serial.println("Failed to save wifi configuration");
  }

  file.close();
}

void clearWifiCredentials() {
  if (SPIFFS.remove(connFile)) {
    Serial.println("Connection file removed");
  }

  setAsAccessPoint();
}

void setAsAccessPoint() {
  setLedState(LED_BLINK);

  Serial.print("Setup as WiFi access point");
  if (!WiFi.softAP(apSsid)) {
    Serial.print("Soft AP creation failed");
    return;
  }
  IPAddress myIp = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIp);

  Serial.println("WiFi server started");
}

void setAsStation(String ssid, String wifiPasswd) {
  setLedState(LED_BLINK);

  if (!WiFi.mode(WIFI_STA)) {
    Serial.print("Could not set as wifi station");
    return;
  }
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(ssid, wifiPasswd);
  Serial.print("Connecting WiFi to SSID ");
  Serial.println(ssid);

  uint8_t timeout = 100;
  wl_status_t status = WiFi.status();
  while (status != WL_CONNECTED) {
    switch (status) {
    case WL_CONNECTION_LOST:
      Serial.println("WiFi connection lost");
      setLedState(LED_ERROR);
      return;
    case WL_CONNECT_FAILED:
      Serial.println();
      Serial.print("Could not connect to ");
      Serial.println(ssid);
      setLedState(LED_ERROR);
      setAsAccessPoint();
      return;
    case WL_NO_SSID_AVAIL:
      Serial.println();
      Serial.print("SSID ");
      Serial.print(ssid);
      Serial.println(" not available");
      setLedState(LED_ERROR);
      setAsAccessPoint();
      return;

    case WL_DISCONNECTED:
    default:
      delay(500);
      Serial.print(".");
    }

    status = WiFi.status();
    timeout--;
    if (timeout == 0) {
      Serial.println();
      Serial.println("Attempt to connect timed out");
      setAsAccessPoint();
      return;
    }
  }
  setLedState(LED_WAIT);
  Serial.println("");

  currentSsid = ssid;
  currentWifiPasswd = wifiPasswd;
  saveWifiCredentials();

  Serial.print("Connected to WiFi SSID: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Host name: ");
  Serial.println(HOSTNAME);

  if (MDNS.begin(HOSTNAME)) {
    Serial.println("MDNS responder started");
  }
}

void initWifi() {
  SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED);
  setLedState(LED_WAIT);
  if (loadWifiCredentials()) {
    setAsStation(currentSsid, currentWifiPasswd);
    setLedState(LED_ON);
  } else {
    setAsAccessPoint();
  }
}
