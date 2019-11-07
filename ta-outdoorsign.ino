/*
 *  TechAhoy - Outdoor Sign
 *
 *  Adafruit Feather HUZZAH with ESP8266 powering 88 Neopixels on an outdoor sign.
 *  LED Ranges:
 *    Top = 0-24
 *    LEft = 25 - 55
 *    Right = 56 - 88
 *
 *  Connct to http://<ip_address> to access HTML control form.
 *
 *  http://<ip_address>/tacolors (TechAhoy color theme)
 *  http://<ip_address>/off (Turn all LEDs off)
 *
 *  wificonfig.h - WiFi network name and password
 */

#include "wificonfig.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//NeoPixels
#define PIN 12
#define NUMPIXELS 89
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int ledTStart = 0;
int ledTMid = 12;
int ledTEnd = 24;
int ledLStart = 25;
int ledLEnd = 55;
int ledRStart = 56;
int ledREnd =  88;

const char INDEX_HTML[] =
"<!DOCTYPE html>"
"<html>"
"<body>"
"<h2>TechAhoy - Outdoor Sign</h2>"
"Presets<BR>"
"<a href=\"/off\">Off</a><BR>"
"<a href=\"/techahoy\">TechAhoy Colors</a><BR>"
"<a href=\"/setleds?r=255&g=0&b=0&ledstart=0&ledend=13\">L</a>"
"<a href=\"/setleds?r=255&g=165&b=44&ledstart=14&ledend=28\">G</a>"
"<a href=\"/setleds?r=255&g=255&b=65&ledstart=29&ledend=43\">B</a>"
"<a href=\"/setleds?r=0&g=128&b=24&ledstart=44&ledend=58\">T</a>"
"<a href=\"/setleds?r=0&g=0&b=249&ledstart=59&ledend=73\">Q</a>"
"<a href=\"/setleds?r=134&g=0&b=125&ledstart=74&ledend=88\">I</a><BR>"
"<h3>Pick a color for the LEDs</h3>"
"<form action=\"/setleds\">"
"R:<br>"
"<input type=\"number\" name=\"r\" value=\"\">"
"<br>"
"G:<br>"
"<input type=\"number\" name=\"g\" value=\"\">"
"<BR>"
"B:<br>"
"<input type=\"number\" name=\"b\" value=\"\">"
"<br><br>"
"LED Start:<br>"
"<input type=\"number\" name=\"ledstart\" value=\"\">"
"<br>"
"LED End:<br>"
"<input type=\"number\" name=\"ledend\" value=\"\">"
"<br><br>"
"<input type=\"submit\" value=\"Submit\">"
"</form>"
"</body>"
"</html>";

const char* wifi = WIFI;
const char* pass = PASS;

ESP8266WebServer server(80);
String SIGNIP;

uint32_t OFF = strip.Color(0,0,0);

void handleRoot(){
  if(server.hasArg("submit")){
    handleSubmit();
  }
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}

void handleSubmit(){
  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();
  int ledSetStart = server.arg("ledstart").toInt();
  int ledSetEnd = server.arg("ledend").toInt();

  for(int i = ledSetStart; i<=ledSetEnd; i++){
    strip.setPixelColor(i,r,g,b);
  }
  strip.show();
  server.send(200, "text/html", INDEX_HTML);
}

void returnOK()
{
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "OK\r\n");
}

void handleTAColors()
{
  taColors();
  returnOK();
}

void handleOff()
{
  ledsOff();
  returnOK();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void ledsOff(){
  for(int i=0;i<NUMPIXELS;i++){
    strip.setPixelColor(i, OFF);
  }
  strip.show();
}

void taColors(){
  //top
  for(int i = ledTStart; i<=ledTMid; i++){
    strip.setPixelColor(i, 119, 203, 224);
  }
  for(int i = ledTMid; i<=ledTEnd; i++){
    strip.setPixelColor(i, 23, 199, 0);
  }
  //left
  for(int i = ledLStart; i<=ledLEnd; i++){
    strip.setPixelColor(i, 23, 199, 0);
  }
  //right
  for(int i = ledRStart; i<=ledREnd; i++){
    strip.setPixelColor(i, 119, 203, 224);
  }
  strip.show();
}

void setup() {
  Serial.println("Initialize NeoPixels and set to off");
  strip.begin();
  for(int i=0;i<NUMPIXELS;i++){
    strip.setPixelColor(i, OFF);
  }
  strip.show();

  Serial.begin(115200);

  WiFi.begin(wifi, pass);
  Serial.println("Connecting to WiFi");

  while(WiFi.status() != WL_CONNECTED){
    Serial.println("...waiting");
    delay(500);
  }
  Serial.println("WiFi connected");
  SIGNIP = WiFi.localIP().toString();
  Serial.println("IP: " + SIGNIP);

  Serial.println("Initialize webserver");

  server.on("/", handleRoot);
  server.on("/tacolors", handleTAColors);
  server.on("/off", handleOff);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Connect to http://" + WiFi.localIP());
  Serial.println("---");
}

void loop() {
  server.handleClient();
}
