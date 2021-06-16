#include "credentials.h" // rename credentials_example.h to credentials.h
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// How long the PowerOFF button should be pressed to power off PC forcefully
#define PWR_OFF_TIME 5000
// How long the button should be pressed to REBOOT, POWER ON or RESET
#define PUSH_TIME 400

// Pin for the PWR signal line
#define PWR_PIN 19
// Pin for the status LED signal line
#define STATUS_PIN 18

#define LED_BUILTIN 2


// Wi-Fi network settings
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;

WebServer server(80);

void connectSuccess() {
  for (int i = 0; i < 3; ++i) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);                       
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void reqReceived() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
}

bool isPoweredOn() {
  return digitalRead(STATUS_PIN) == HIGH;
}

String getStatusString() {
  return String(isPoweredOn() ? "ON" : "OFF");
}

// false == OFF, true == ON
void do_powerPress(bool expectedStatus, int timeInMs) {
   if(isPoweredOn() == expectedStatus) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(PWR_PIN, LOW);
    delay(timeInMs);
    digitalWrite(PWR_PIN, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    server.send(409, "text/plain", "CONFLICT");
  }
}

void configureEndpoints() {
  server.on("/pc-power-control/on", [] () {
    reqReceived();
    do_powerPress(false, PUSH_TIME);
    server.send(200, "text/plain", "OK");
  });

  server.on("/pc-power-control/off", [] () {
    reqReceived();
    do_powerPress(true, PUSH_TIME);
    server.send(200, "text/plain", "OK");
  });

  server.on("/pc-power-control/force-off", [] () {
    reqReceived();
    do_powerPress(true, PWR_OFF_TIME);
    server.send(200, "text/plain", "OK");
  });

  server.on("/pc-power-control/status", [] () {
    reqReceived();
    server.send(200, "text/plain", getStatusString());
  });

  server.onNotFound([] () {
    reqReceived();
    server.send(404, "text/plain", "Not Found");
  });
}

void setup() {
  delay( 3000 ); // power-up safety delay
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PWR_PIN, OUTPUT);
  pinMode(STATUS_PIN, INPUT_PULLUP);
  
  digitalWrite(PWR_PIN, HIGH);

  WiFi.hostname("pc-power-control");
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting to WIFI ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);                       
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      Serial.print(".");
  }

  connectSuccess();

  Serial.println();
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  configureEndpoints();
  server.begin();
  Serial.println("webserver started");  
}

void loop() {
  server.handleClient();
}
