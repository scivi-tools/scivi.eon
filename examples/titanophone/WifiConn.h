#ifndef WIFI_CONN_H
#define WIFI_CONN_H

#include <ESP8266WebServer.h>

#define SSID_SIZE 32
#define PASS_SIZE 63

	
class WiFiConnector{
public:
	static void Init(const char *ap_ssid, const char *ap_pass);
	static void SetAPData(const char *ssid, const char *pass);
	static void SetupWebServer(ESP8266WebServer& server, const char *endpoint, bool save_data_prgm);
	static void ConnectWiFi(const char *ssid, const char *pass, bool save_network=true);
private:
	static void EEPROMGetStr(uint16_t start_addr, char* buffer, uint16_t len);
	static void EEPROMPutStr(uint16_t start_addr, const char* buffer, uint16_t len);
	static void saveNetworkInfo(const char* ssid, const char* pass);
	static void loadNetworkInfo(char* ssid, char* pass);
};

#endif
