#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>

#define NO_GLOBAL_SSDP
#include "almilukESP8266SSDP.h"
#include "WifiConn.h"

#include "EONEval.h"

static const char g_SSID[] = "SciVi::eon";
static const char g_pass[] = "";

ESP8266WebServer g_webServer(80);
WebSocketsServer g_webSocket(81);

SSDPClass g_ssdp;

EON::Eval g_eval;

void webServerRoot()
{
    g_webServer.sendHeader("Location", "/index.htm", true);
    g_webServer.send(302, "text/plane", "");
}

void webServerNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += g_webServer.uri();
    message += "\nMethod: ";
    message += (g_webServer.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += g_webServer.args();
    message += "\n";
    for (uint8_t i = 0; i < g_webServer.args(); ++i)
        message += " " + g_webServer.argName(i) + ": " + g_webServer.arg(i) + "\n";
    g_webServer.send(404, "text/plain", message); 
}

void doPong()
{
    pinMode(LED_BUILTIN, OUTPUT);
    for (int i = 0; i < 3; ++i)
    {
        pinMode(LED_BUILTIN, HIGH);
        delay(166);
        pinMode(LED_BUILTIN, LOW);
        delay(166);
    }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
        case WStype_CONNECTED:
            break;

        case WStype_DISCONNECTED:
            break;

        case WStype_TEXT:
            break;

        case WStype_BIN:
            Serial.println(">>> Binary frame!!!");
            for (int i = 0; i < length; ++i)
            {
                Serial.print(payload[i]); 
                Serial.print(" ");
            }
            Serial.println("");
            switch (payload[0])
            {
                case 0x0D:
                    doPong();
                    break;

                case 0x0E:
                    g_eval.load(&payload[1], length - 1); // TODO: handle error (if false returned)
                    break;
            }
            break;
    }
}

bool initSSDP(){
    // SSDPClass::SSDPServiceType services[] = {
    //     {"scivi", "ADC", "56bf9ef3c5244aca1620d52eb7c56ab7a38bc223"}
    // };
    // g_ssdp.setServiceTypes(services, 1);
    g_ssdp.setDeviceType("edge-scivi", "eon-esp8266", "2.0");
    g_ssdp.setName("scivi eon on esp8266");
    g_ssdp.setManufacturer("scivi-tools");
    g_ssdp.setManufacturerURL("https://scivi.tools/");
    g_ssdp.setModelURL("https://github.com/scivi-tools/scivi.eon/tree/56bf9ef3c5244aca1620d52eb7c56ab7a38bc223");
    g_ssdp.setURL("https://github.com/scivi-tools/scivi.eon");
    return g_ssdp.begin();
}

void setup()
{
    delay(1000);
    
    Serial.begin(115200);
    Serial.println();
    Serial.println("=== SciVi ES v0.2 @ ESP8266 ===");
    Serial.println();

    Serial.println("\n[SETUP] Connecting to WiFi:");
    WiFi.setAutoReconnect(true);
    WiFiConnector::Init(g_SSID, g_pass);

    g_webSocket.onEvent(webSocketEvent);
    g_webSocket.begin();

    Serial.println("[SETUP] WebSocket server started");

    g_webServer.onNotFound(webServerNotFound);
    g_webServer.on("/", webServerRoot);
    g_webServer.on("/ssdp/schema.xml", [](){
        g_ssdp.schema(g_webServer.client());
    });
    WiFiConnector::SetupWebServer(g_webServer, "/set-network", true);
    g_webServer.begin();

    Serial.println("[SETUP] Web server started");

    if(initSSDP())
        Serial.println("[SETUP] SSDP started");
    else
        Serial.println("[SETUP] SSDP init failed");
    
    g_eval.setup();

    Serial.println("[SETUP] EON started");
}

void loop()
{
    g_eval.turn();
    
    g_ssdp.loop();
    g_webSocket.loop();
    g_webServer.handleClient();

    delay(16);
}
