#include "HttpWrapper.h"

IPAddress ip(192, 168, 70, 56);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;

HttpWrapper::HttpWrapper() {
  Ethernet.begin(mac, ip);
}

HttpWrapper::~HttpWrapper() {
}

void HttpWrapper::Get(char server[], String path) {
  if (client.connect(server, 8080)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET " + path + " HTTP/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
}
