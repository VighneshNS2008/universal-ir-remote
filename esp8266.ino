
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

const char* ssid = "vighnesh";
const char* password = "anish2012";
MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

void handleroot() {
  server.send(200, "text/html",
              "<html>" \
                "<body>" \
                      "<h2>RGB Board</h2>" \
                      "<p><a href=\"ir?code=16236607\"> RGB ON</a></p>" \
                      "<p><a href=\"ir?code=16203967\"> OFF</a></p>" \
                      "<p><a href=\"ir?code=16246807\"> RANDOM</a></p>" \
                      "<p><a href=\"ir?code=16195807\"> RED</a></p>" \
                      "<p><a href=\"ir?code=16228447\"> GREEN</a></p>" \
                      "<p><a href=\"ir?code=16228447\"> BLUE</a></p>" \
                      "<p><a href=\"ir?code=16244767\"> WHITE</a></p>" \
                      "<p><a href=\"ir?code=16208047\"> BLUE 1</a></p>" \
                      "<p><a href=\"ir?code=16216207\"> BLUE 2</a></p>" \
                      "<p><a href=\"ir?code=16206007\"> BLUE 3</a></p>" \
                      "<p><a href=\"ir?code=16214167\"> BLUE 4</a></p>" \
                      "<p><a href=\"ir?code=16224367\"> GREEN 1</a></p>" \
                      "<p><a href=\"ir?code=16232527\"> GREEN 1</a></p>" \
                      "<p><a href=\"ir?code=16222327\"> GREEN 1</a></p>" \
                      "<p><a href=\"ir?code=16230487\"> GREEN 1</a></p>" \
                      "<p><a href=\"ir?code=16191727\"> RED 1</a></p>" \
                      "<p><a href=\"ir?code=16199887\"> RED 1</a></p>" \
                      "<p><a href=\"ir?code=16189687\"> RED 1</a></p>" \
                      "<p><a href=\"ir?code=16197847\"> RED 1</a></p>" \
                      "<h2>Creative Stage Controls</h2>" \
                      "<p><a href=\"ir?code=83595375\"> Stage ON/OFF</a></p>" \
                      "<p><a href=\"ir?code=83603535\"> INPUT</a></p>" \
                      "<p><a href=\"ir?code=83593335\"> RESET</a></p>" \
                      "<p><a href=\"ir?code=83591295\"> BT-PAIR</a></p>" \
                      "<h3>Equalizers</h3>" \
                      "<p><a href=\"ir?code=83605575\"> CINEMA</a></p>" \
                      "<p><a href=\"ir?code=83597415\"> MUSIC</a></p>" \
                      "<p><a href=\"ir?code=83601495\"> CONCERT</a></p>" \
                      "<p><a href=\"ir?code=83577015\"> GAMING</a></p>" \
                      "<h3>Bluetooth Controls</h3>" \
                      "<p><a href=\"ir?code=83621895\"> STOP</a></p>" \
                      "<p><a href=\"ir?code=83613735\"> PLAY/PAUSE</a></p>" \
                      "<p><a href=\"ir?code=83617815\"> NEXT</a></p>" \
                      "<p><a href=\"ir?code=83619855\"> PREVIOUS</a></p>" \
                      "<h3>Treble</h3>" \
                      "<p><a href=\"ir?code=83558655\"> INCREASE</a></p>" \
                      "<p><a href=\"ir?code=83607615\"> DECREASE</a></p>" \
                      "<h3>Bass</h3>"
                      "<p><a href=\"ir?code=83587215\"> INCREASE</a></p>" \
                      "<p><a href=\"ir?code=83581095\"> DECREASE</a></p>" \
                      "<h3>Volume</h3>" \
                      "<p><a href=\"ir?code=83583135\"> INCREASE</a></p>" \
                      "<p><a href=\"ir?code=83615775\"> DECREASE</a></p>" \
                      "<p><a href=\"ir?code=83589255\"> MUTE</a></p>" \
                "</body>" \
              "</html>");
}

void handleir() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code") {
      uint32_t code = strtoul(server.arg(i).c_str(), NULL, 10);
#if SEND_NEC
      irsend.sendNEC(code, 32);
#endif  // SEND_NEC
    }
  }
  handleroot();
}
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  server.on("/", handleroot);
  server.on("/ir", handleir);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);
  if (!MDNS.begin("remote")) {
    Serial.println("Error setting up mdns responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mdns responder started");

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("tcp server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}

void loop(void) {
  MDNS.update();
  server.handleClient();
}
