#include <cstdint>

#include <Arduino.h>
#include <IPAddress.h>
#include <WiFi.h>

#include "connect.h"
#include "esp32-hal.h"
#include "secrets.h"

uint64_t start;
uint64_t is_connecting = 0;
IPAddress host;

void timerStart()
{
	start = millis();
}

uint64_t timerElapsed()
{
	auto now = millis();
	uint64_t elapsed = now - start;
	return elapsed;
}

// -----------------------------------------

void wifiSetup()
{
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(false);
	WiFi.disconnect();
	delay(100);
}

void wifiConnect()
{
	if (WiFi.status() == WL_CONNECTED) {
		if (is_connecting > 0) {
			host = WiFi.gatewayIP();
			Serial.println("Connected to AP!");
		}
		is_connecting = 0;
		return;
	}

	if (is_connecting == 0 || timerElapsed() > is_connecting * 1000) {
		if (is_connecting == 0) {
			is_connecting = 10; // start with a 10 second delay
		}
		else {
			is_connecting *= 2; // exponential backoff
		}

		timerStart();
		Serial.println("Connecting");
		WiFi.begin(SSID, PASSWORD);
	}
}

IPAddress wifiHost()
{
	return host;
}

// https://docs.espressif.com/projects/arduino-esp32/en/latest/api/wifi.html
// https://docs.arduino.cc/libraries/wifi/ (old)
