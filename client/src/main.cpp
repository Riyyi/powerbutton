#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "secrets.h"

// Default IP address = 192.168.4.1

#define HOST "192.168.4.1"
#define PORT 1234

#define BUTTON_PIN 2

WiFiClient client;

void ack();

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(BUTTON_PIN, INPUT_PULLUP);

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
	delay(25);
	Serial.println("ESP32 loopy!");

	int buttonState = digitalRead(BUTTON_PIN);
	if (buttonState == LOW) {
		Serial.println("Button pressed!");

		if (!client.connected() && !client.connect(HOST, PORT)) {
			Serial.println("Connection failed");
			return;
		}

		client.print("button_pressed\n");
		Serial.println("Sent button press");

		ack();
	}
	else {
		Serial.println("Button not pressed!");

		if (!client.connected()) {
			return;
		}

		client.print("button_released\n");
		Serial.println("Sent button release");

		ack();

		client.stop();
	}
}

// -----------------------------------------

void ack()
{
	while (client.connected()) {
		if (client.available()) {
			// Wait for acknowledgment from the receiver
			String response = client.readStringUntil('\n');
			if (response == "ACK") {
				Serial.println("Received ACK");
				return;
			}
		}
	}
}
