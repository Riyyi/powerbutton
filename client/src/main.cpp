#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "secrets.h"

// Default IP address = 192.168.4.1

#define HOST "192.168.4.1"
#define PORT 1234

#define POWER_BUTTON_PIN 2
#define RESET_BUTTON_PIN 3

WiFiClient client;

int previousPowerButtonState = HIGH;
int previousResetButtonState = HIGH;

void ack();

void setup()
{
	Serial.begin(9600);
	Serial.setDebugOutput(true);

	pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
	pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);

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
	delay(20);

	int powerButtonState = digitalRead(POWER_BUTTON_PIN);
	int resetButtonState = digitalRead(RESET_BUTTON_PIN);

	// Unsupported usecase
	if (powerButtonState == LOW && resetButtonState == LOW) {
		previousPowerButtonState = HIGH;
		previousResetButtonState = HIGH;
		if (client.connected()) {
			client.stop();
		}
		return;
	}

	bool isButtonPressed = (previousPowerButtonState == LOW && powerButtonState == LOW)
	                       || (previousResetButtonState == LOW && resetButtonState == LOW);
	previousPowerButtonState = powerButtonState;
	previousResetButtonState = resetButtonState;

	if (isButtonPressed) {
		String button = (powerButtonState == LOW) ? "power" : "reset";

		Serial.println("Pressed " + button + " button!");

		if (!client.connected() && !client.connect(HOST, PORT)) {
			Serial.println("Connection failed");
			return;
		}

		client.print(button + "_pressed\n");
		Serial.println("Sent " + button + " button press");

		ack();
	}
	else {
		if (!client.connected()) {
			return;
		}

		client.print("power_released\n");
		Serial.println("Sent power button release");
		ack();

		client.print("reset_released\n");
		Serial.println("Sent reset button release");
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
