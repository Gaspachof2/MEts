#include <Ticker.h>

#include <HX711.h> 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <FS.h>
#include <WebSocketsServer.h>


#define DOUTD  D5                    // HX711 DATA OUT da direita = pino D5 da esp
#define CLKD  D6                     // HX711 SCK IN da direita = pino D6 da esp
#define DOUTE  D4                  // HX711 DATA OUT da esquerda = pino D4 da esp
#define CLKE  D3                     // HX711 SCK IN da esquerda  = pino D3 da esp


Ticker timer;

float calibration_factor = 222130;     // fator de calibração para teste inicial(original era 42130)
float altura,tempo,velocidade,tempoDeVoo,ml1,ml2;
String Valor;
const char *ssid = "M.E.T.S";
const char *password = "RedeMets";
IPAddress local_IP(192,168,0,1);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

WebSocketsServer webSocket = WebSocketsServer(81);


ESP8266WebServer server(80); //inicia o servidor
String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool handleFileRead(String path) {
  
  if (path.endsWith("/")) {
    path += "index.html";
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
  String htmlType="text/html";
   if(htmlType!=contentType)
    { 
      server.sendHeader("Expires","Wed, 21 Oct 2099 07:28:00 GMT");
  }
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}


void setup()
{
  velocidade=0;
  delay(1000);
 timer.attach(5,getData);
  SPIFFS.begin();
  Serial.begin(115200);            
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP();

  
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      server.send(404, "text/plain", "FileNotFound");
    }
  });

    
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
}


void loop()
{

   webSocket.loop();
  server.handleClient();
 
  
  
 
  

 
}//Chave que fecha o loop
void getData(){
  String json = "{\"value\":";
  json+= "\"10\"}";
  webSocket.broadcastTXT(json.c_str(),json.length());
}

void webSocketEvent(uint8_t num, WStype_t type,uint8_t * payload, size_t length ){
  


}
