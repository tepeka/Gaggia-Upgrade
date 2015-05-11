#include "http.h"

Http::Http() {
  //while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }  
}

Http::~Http() {
}

void Http::Get(String url, String params[]) {
  url = url + "?";
  //for(int i = 0; i < sizeof(params)/sizeof(String); i++) {
  //  url = url + params[i];
  //  url = url + "&";
  //}
  url.remove(url.length() - 1, 1);
  Serial.print("GET ");
  Serial.print(url);
  Serial.println();
  //EthernetClient ethernet;
  //HttpClient client(ethernet);
  //client.get(url);
}
