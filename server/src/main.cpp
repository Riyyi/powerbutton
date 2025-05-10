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

#define SWITCH_PIN 2

WiFiServer server(PORT);

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(SWITCH_PIN, OUTPUT);

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
	digitalWrite(2, HIGH);
	delay(500);
	digitalWrite(2, LOW);
	delay(500);

	Serial.println("ESP32 loopy!");

	// -------------------------------------

	WiFiClient client = server.available();
	if (client) {
		Serial.println("Client connected");

		// unsigned long start = 0;
		while (client.connected()) {
			if (client.available()) {
				String msg = client.readStringUntil('\n');
				Serial.print("Received: ");
				Serial.println(msg);

				// Do something with the message, like toggling an LED

				client.print("ACK\n");
				Serial.println("Sent ACK");
				// start = millis();
			}

			// // Kill lingering connections
			// if (start != 0 && millis() - start < 1000) {
			// 	Serial.println("Client kill..");
			// 	break;
			// }
		}
		client.stop();
		Serial.println("Client disconnected");
	}
}
