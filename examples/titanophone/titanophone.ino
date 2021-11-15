
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>

#include "EONEval.h"


static const char g_SSID[] = "SciVi";
static const char g_pass[] = "";
ESP8266WebServer g_webServer(80);
WebSocketsServer g_webSocket(81);

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
            g_eval.load(payload, length); // TODO: handle error (if false returned)
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
    g_eval.turn();

    g_webSocket.loop();
    g_webServer.handleClient();

    delay(16);
}
