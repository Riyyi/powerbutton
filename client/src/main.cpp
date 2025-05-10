#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "secrets.h"

// Default IP address = 192.168.4.1

#define HOST "192.168.4.1"
#define PORT 1234

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(2, OUTPUT);

	// Wait for a USB connection to be established
	while (!Serial)
		(void)0;
	delay(1000);
	Serial.println("Client booted!");

	// Connect to button server
	WiFi.begin(SSID, PASSWORD);
	Serial.print("Connecting");
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("\nConnected to AP");
}

void loop()
{
	digitalWrite(2, HIGH);
	delay(500);
	digitalWrite(2, LOW);
	delay(500);

	Serial.println("ESP32 loopy!");

	WiFiClient client;
	if (client.connect(HOST, PORT)) {
		client.print("button_pressed\n");
		Serial.println("Sent button press");

		while (client.connected()) {
			if (client.available()) {
				// Wait for acknowledgment from the receiver
				String response = client.readStringUntil('\n');
				if (response == "ACK") {
					Serial.println("Received ACK");
					break;
				}
			}
		}

		delay(2000);
		client.stop();
	}
	else {
		Serial.println("Connection failed");
	}
}
