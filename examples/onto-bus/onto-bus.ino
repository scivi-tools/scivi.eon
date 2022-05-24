
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>

#include "EONEval.h"
#include "EONBus.h"


#define CMD_LOAD_EON 0xE0
#define CMD_SCAN_BUS 0xE1
#define CMD_PING_DEV 0xE2

static const char g_SSID[] = "SciVi";
static const char g_pass[] = "";

ESP8266WebServer g_webServer(80);
WebSocketsServer g_webSocket(81);

OneWire g_oneWireHub(D1);
EON::Bus g_bus(&g_oneWireHub);

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

String scanBus()
{
    uint8_t dc = g_bus.scan();
    String result = String("{\"bus\":[");
    for (uint8_t i = 0; i < dc; ++i)
    {
        result += String("{\"") + String(g_bus.deviceID(i)) + String("\":") + String(g_bus.deviceUID(i)) + String("}");
        if (i < dc - 1)
            result += String(",");
    }
    result += String("]}");
    return result;
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
                case CMD_LOAD_EON:
                    g_eval.load(&payload[1], length - 1); // TODO: handle error (if false returned)
                    break;

                case CMD_SCAN_BUS:
                {
                    String msg = scanBus();
                    g_webSocket.sendTXT(num, msg);
                }
                    break;

                case CMD_PING_DEV:
                    g_bus.ping(&payload[1]);
                    break;
            }
            break;
    }
}

void setup()
{
    delay(1000);
    
    Serial.begin(115200);
    Serial.println();
    Serial.println("=== SciVi ES v0.2 @ ESP8266 ===");
    Serial.println();

    WiFi.softAP(g_SSID, g_pass);

    Serial.println("[SETUP] access point is up");
    Serial.print("[SETUP] SSID: ");
    Serial.println(g_SSID);
    Serial.print("[SETUP] IP: ");
    Serial.println(WiFi.softAPIP());

    g_webSocket.onEvent(webSocketEvent);
    g_webSocket.begin();

    Serial.println("[SETUP] WebSocket server started");

    g_webServer.onNotFound(webServerNotFound);
    g_webServer.on("/", webServerRoot);
    g_webServer.begin();

    Serial.println("[SETUP] Web server started");

    g_eval.setup();

    Serial.println("[SETUP] EON started");
}

void loop()
{
    //g_eval.turn();
    //uint8_t dc = g_bus.scan();
    //Serial.print("Devices on bus: ");
    //Serial.println(dc);
    //if (dc > 0)
    //{
    //    g_bus.send(g_bus.device(0), 0xDD);
    //    Serial.println(g_bus.read());
    //}

    g_webSocket.loop();
    g_webServer.handleClient();

    delay(16);
}
