
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <EEPROM.h>

#include "EONEval.h"


static const char g_SSID[] = "EON";
static const char g_pass[] = "";
static const char g_index[] = R"=HTML=(
<!DOCTYPE html>
<html>
    <head>
        <meta charset='utf-8'/>
        <title>SciVi EON</title>
        <style>
            body {
                font: 14px Helvetica Neue, Helvetica, Arial, sans-serif;
            }
            form {
                padding: 10px 10px 0px 10px;
                border: 2px solid;
                border-radius: 10px;
                width: 250px;
                left: 50%;
                top: 50%;
                position: absolute;
                -webkit-transform: translate3d(-50%, -50%, 0);
                -moz-transform: translate3d(-50%, -50%, 0);
                transform: translate3d(-50%, -50%, 0);
            }
            .field {
              width: 100%;
              margin-bottom: 10px;
              display: flex;
            }
            .title {
                text-align: center;
                font-weight: bold;
                margin-bottom: 10px;
            }
            label {
                text-align: right;
                margin-right: 5px;
                padding-top: 1px;
                width: 110px;
            }
            input {
                width: 100%;
                font: 14px Helvetica Neue, Helvetica, Arial, sans-serif;
                border: #000 1px solid;
                border-radius: 5px;
            }
        </style>
    </head>
    <body>
        <form action='/save' method='post'>
            <div class='title'>WiFi Settings</div>
            <div class='field'><label for='ssid'>SSID:</label><input type='text' id='ssid' name='ssid'/></div>
            <div class='field'><label for='pass'>Password:</label><input type='text' id='pass' name='pass'/></div>
            <div class='title'><input type='submit' value='Save'/></div>
        </form>
    </body>
</html>
)=HTML=";
static const char g_saveOK[] = R"=HTML=(
<!DOCTYPE html>
<html>
    <head>
        <meta charset='utf-8'/>
        <title>SciVi EON</title>
        <style>
            body {
                font: 14px Helvetica Neue, Helvetica, Arial, sans-serif;
            }
            .dlg {
                padding: 10px 10px 0px 10px;
                border: 2px solid;
                border-radius: 10px;
                width: 250px;
                left: 50%;
                top: 50%;
                position: absolute;
                -webkit-transform: translate3d(-50%, -50%, 0);
                -moz-transform: translate3d(-50%, -50%, 0);
                transform: translate3d(-50%, -50%, 0);
                background: #9DFFB8;
            }
            .title {
                text-align: center;
                font-weight: bold;
                margin-bottom: 10px;
            }
        </style>
    </head>
    <body>
        <div class="dlg">
            <div class='title'>WiFi Settings Updated</div>
        </div>
    </body>
</html>
)=HTML=";
ESP8266WebServer g_webServer(80);
WebSocketsServer g_webSocket(81);

EON::Eval g_eval;


void connectWiFi(const char *ssid, const char *pass)
{
    WiFi.mode(WIFI_STA);
    if (ssid && pass)
        WiFi.begin(ssid, pass);
    else
        WiFi.begin();

    Serial.print("Connecting");
    delay(500);

    for (int i = 0; i < 10; ++i)
    {
        switch (WiFi.status())
        {
            case WL_IDLE_STATUS:
                Serial.print(".");
                delay(500);
                break;

            case WL_NO_SSID_AVAIL:
                Serial.println();
                Serial.print("Connection failed: no SSID available");
                i = 10;
                break;

            case WL_CONNECTED:
                Serial.println();
                Serial.print("Connected, IP address: ");
                Serial.println(WiFi.localIP());
                return;

            case WL_CONNECT_FAILED:
                Serial.println();
                Serial.println("Connection failed: incorrect password");
                i = 10;
                break;

            case WL_DISCONNECTED:
                Serial.println();
                Serial.println("Connection failed: WiFi configuration is wrong");
                i = 10;
                break;
        }
    }

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(g_SSID, g_pass);

    Serial.println("[SETUP] access point is up");
    Serial.print("[SETUP] SSID: ");
    Serial.println(g_SSID);
    Serial.print("[SETUP] IP: ");
    Serial.println(WiFi.softAPIP());
}

void webServerRoot()
{
    g_webServer.sendHeader("Location", "/index.html", true);
    g_webServer.send(200, "text/html", g_index);
}

void webServerSave()
{
    String ssid = g_webServer.arg("ssid");
    String pass = g_webServer.arg("pass");
    g_webServer.send(200, "text/html", g_saveOK);
    connectWiFi(ssid.c_str(), pass.c_str());
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

    connectWiFi(nullptr, nullptr);

    g_webSocket.onEvent(webSocketEvent);
    g_webSocket.begin();

    Serial.println("[SETUP] WebSocket server started");

    g_webServer.onNotFound(webServerNotFound);
    g_webServer.on("/", webServerRoot);
    g_webServer.on("/save", webServerSave);
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
}
