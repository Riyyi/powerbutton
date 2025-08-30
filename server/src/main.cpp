#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include "secrets.h"

// Default gateway    = 192.168.4.1
// Default IP address = 192.168.4.2

#define PORT 1234
#define CHANNEL 11
#define HIDDEN true
#define MAX_CONNECTION 1

#define POWER_BUTTON_PIN 10
#define RESET_BUTTON_PIN 2
#define POWER_LED_PIN 3
#define RESET_LED_PIN 1

#define POWER_GATE_PIN 6
#define RESET_GATE_PIN 7

WiFiServer server(PORT, MAX_CONNECTION);

bool powerPressed = false;
bool resetPressed = false;

void processButtons();
void setPowerPin(bool enable);
void setResetPin(bool enable);

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(POWER_GATE_PIN, OUTPUT);
	pinMode(POWER_LED_PIN, OUTPUT);
	pinMode(RESET_GATE_PIN, OUTPUT);
	pinMode(RESET_LED_PIN, OUTPUT);

	delay(3000);
	Serial.println("Server booted!");

	// Start button server
	WiFi.softAP(SSID, PASSWORD, CHANNEL, HIDDEN, MAX_CONNECTION);
	Serial.println("AP Started");
	server.begin();
}

void loop()
{
	WiFiClient client = server.available();
	if (client) {
		Serial.println("Client connected");

		unsigned long start = 0;
		while (client.connected()) {
			if (client.available()) {
				String msg = client.readStringUntil('\n');
				Serial.print("Received: ");
				Serial.println(msg);

				if (msg == "power_pressed") {
					powerPressed = true;
				}
				else if (msg == "power_released") {
					powerPressed = false;
				}
				else if (msg == "reset_pressed") {
					resetPressed = true;
				}
				else if (msg == "reset_released") {
					resetPressed = false;
				}

				client.print("ACK\n");
				Serial.println("Sent ACK");
				start = millis();
			}

			// Kill lingering connections
			if (start != 0 && millis() - start > 200) {
				Serial.println("Client kill..");
				break;
			}

			processButtons();
		}

		setPowerPin(false);
		setResetPin(false);

		client.stop();
		Serial.println("Client disconnected");
	}
}

// -----------------------------------------

void processButtons()
{
	if (powerPressed && resetPressed) {
		Serial.println("Double input..");
		setPowerPin(false);
		setResetPin(false);
		return;
	}

	setPowerPin(powerPressed);
	setResetPin(resetPressed);
}

void setPowerPin(bool enable)
{
	powerPressed = enable;
	// digitalWrite(POWER_GATE_PIN, (enable) ? HIGH : LOW);
	digitalWrite(POWER_LED_PIN, (enable) ? HIGH : LOW);
}

void setResetPin(bool enable)
{
	resetPressed = enable;
	// digitalWrite(RESET_GATE_PIN, (enable) ? HIGH : LOW);
	digitalWrite(RESET_LED_PIN, (enable) ? HIGH : LOW);
}

// connect + to drain
// connect - to source
