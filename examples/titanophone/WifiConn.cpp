#include "WifiConn.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>


static const char html_set_network[] PROGMEM = R"=HTML=(
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
		<form action='/set-network' method='post'>
			<div class='title'>WiFi Settings</div>
			<div class='field'><label for='ssid'>SSID:</label><input type='text' id='ssid' name='ssid'/></div>
			<div class='field'><label for='pass'>Password:</label><input type='text' id='pass' name='pass'/></div>
			<div class='title'><input type='submit' value='Save'/></div>
		</form>
	</body>
</html>
)=HTML=";

static const char html_save_network[] PROGMEM = R"=HTML=(
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

static char apSsid[SSID_SIZE + 1] = "ESP8266";
static char apPass[PASS_SIZE + 1] = "\0";


void WiFiConnector::Init(const char *ap_ssid, const char *ap_pass){
	SetAPData(ap_ssid, ap_pass);
	char ssid[SSID_SIZE + 1];
	char pass[PASS_SIZE + 1];
	loadNetworkInfo(ssid, pass);
	ConnectWiFi(ssid, pass, false);
}

void WiFiConnector::SetAPData(const char *ssid, const char *pass){
	strncpy(apSsid, ssid, SSID_SIZE);
	strncpy(apPass, pass, PASS_SIZE);
}

void WiFiConnector::SetupWebServer(ESP8266WebServer &server, const char *endpoint, bool save_data_prgm){	
	server.on(endpoint, HTTP_GET, [&server]() {
		server.send_P(200, "text/html", html_set_network);
	});
	
	server.on(endpoint, HTTP_POST, [&server, save_data_prgm]() {
		String ssid = server.arg("ssid");
		String pass = server.arg("pass");
		if (ssid != ""){
			server.send_P(200, "text/html", html_save_network);
			delay(500);
			ConnectWiFi(ssid.c_str(), pass.c_str(), save_data_prgm);
		} else {
			server.send_P(400, "text/plain", "Invalid arguments.");
		}
	});
}

void WiFiConnector::ConnectWiFi(const char *ssid, const char *pass, bool save_network) {
	WiFi.mode(WIFI_STA);
	if (ssid && pass)
		WiFi.begin(ssid, pass);
	else
		WiFi.begin();
		
	delay(500);
	Serial.println("Connecting to WiFi");

	
	char prev_ssid[SSID_SIZE + 1];
	char prev_pass[PASS_SIZE + 1];
	for(uint8_t i = 0, j = 0; i < 30; i++){
		switch (WiFi.status())
		{
			case WL_IDLE_STATUS:
				Serial.print(".");
				delay(500);
				break;

			case WL_DISCONNECTED:
				Serial.print(".");
				delay(500);				
				break;
						
			case WL_NO_SSID_AVAIL:
				Serial.println();
				Serial.print("Connection failed: no SSID available");
				i = 30;
				break;
		
			case WL_CONNECTED:
				Serial.println();
				Serial.print("Connected, IP address: "); 
				Serial.println(WiFi.localIP());
				if(save_network){
					loadNetworkInfo(prev_ssid, prev_pass);
					if(strcmp(prev_ssid, ssid) || strcmp(prev_pass, pass)){
						saveNetworkInfo(ssid, pass);
						Serial.println("Saved");
					} else {
						Serial.println("Didn't save");
					}
				}
				
				i = 30;
				return;

			case WL_CONNECT_FAILED:
				Serial.println();
				Serial.print("Connection failed.");
				i = 30;
				break;
						
			case WL_WRONG_PASSWORD:
				Serial.println();
				Serial.println("Connection failed: incorrect password");
				i = 30;
				break;
		}
	}
	
	Serial.println("\nConnection timeout passed.");
	WiFi.mode(WIFI_AP);
	WiFi.softAP(apSsid, apPass);

	Serial.println("Access point is up:");
	Serial.print("\tSSID: ");
	Serial.println(apSsid);
	Serial.print("\tIP: ");
	Serial.println(WiFi.softAPIP());
}

void WiFiConnector::EEPROMGetStr(uint16_t start_addr, char* buffer, uint16_t len){
	EEPROM.begin(start_addr + len + 1);
	uint8_t i = 0;
	while(i < len && (buffer[i] = EEPROM.read(start_addr + i)) != '\0') i++;
	buffer[i] = '\0';
	EEPROM.end();
}

void WiFiConnector::EEPROMPutStr(uint16_t start_addr, const char* buffer, uint16_t len){
	EEPROM.begin(start_addr + len + 1);
	uint8_t i = 0;
	while(i < len && buffer[i] != '\0') EEPROM.put(start_addr + i, buffer[i++]);
	EEPROM.put(start_addr + i, '\0');
	EEPROM.end();
}

void WiFiConnector::saveNetworkInfo(const char* ssid, const char* pass){
	EEPROMPutStr(0, ssid, SSID_SIZE);
	EEPROMPutStr(SSID_SIZE + 1, pass, PASS_SIZE);
}

void WiFiConnector::loadNetworkInfo(char* ssid, char* pass){
	EEPROMGetStr(0, ssid, SSID_SIZE);
	EEPROMGetStr(SSID_SIZE + 1, pass, PASS_SIZE);	
}
