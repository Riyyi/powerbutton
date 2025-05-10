#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include "secrets.h"

// Default IP address = 192.168.4.1

#define PORT 1234
#define CHANNEL 11
#define HIDDEN true
#define MAX_CONNECTION 1

#define POWER_PIN 2
#define RESET_PIN 3

WiFiServer server(PORT);

bool powerPressed = false;
bool resetPressed = false;

void processButtons();
void setPowerPin(bool enable);
void setResetPin(bool enable);

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(POWER_PIN, OUTPUT);
	pinMode(RESET_PIN, OUTPUT);

	// Wait for a USB connection to be established
	while (!Serial)
		(void)0;
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
	digitalWrite(POWER_PIN, (enable) ? HIGH : LOW);
}

void setResetPin(bool enable)
{
	resetPressed = enable;
	digitalWrite(RESET_PIN, (enable) ? HIGH : LOW);
}
