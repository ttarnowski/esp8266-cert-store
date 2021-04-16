#include <Arduino.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFiMulti.h>
#include <EventDispatcher.hpp>
#include <HTTPSClient.hpp>
#include <LittleFS.h>
#include <Timer.hpp>
#include <WiFiManager.hpp>

#define SSID "VM3549886"
#define PASSWORD "mc7RsdnxV4qp"

BearSSL::CertStore certStore;
Timer timer;
EventDispatcher dispatcher;
ESP8266WiFiMulti wifiMulti;

WiFiManager wifiManager(&wifiMulti, &dispatcher, &timer, SSID, PASSWORD);
HTTPSClient httpsClient(&certStore, &wifiManager, &timer);

void onResponse(Response res) {
  if (res.error != nullptr) {
    Serial.printf("request failed: %s\n", res.error);
    return;
  }

  Serial.printf("Status: %d, Body: %s\n", res.statusCode,
                res.body->readString().c_str());
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  LittleFS.begin();

  int numCerts =
      certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));

  Serial.printf("Number of CA certs read: %d\n", numCerts);

  httpsClient.sendRequest(Request{"https://endx5tk2mradd.x.pipedream.net",
                                  "/example",
                                  HTTPMethod::HTTP_POST,
                                  "{\"message\":\"hello from ESP8266\"}",
                                  {std::make_pair("X-User", "tomasz")}},
                          onResponse);

  httpsClient.sendRequest(
      Request::build(HTTPMethod::HTTP_PUT)
          .baseUrl("https://endx5tk2mradd.x.pipedream.net")
          .path("/test")
          .body("{}")
          .headers({std::make_pair("Content-type", "application/json")}),
      onResponse);
}

void loop() { timer.tick(); }