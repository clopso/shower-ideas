#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define the ADC pins connected to the joystick
#define JOYSTICK_X_PIN 33
#define JOYSTICK_Y_PIN 35
#define JOYSTICK_MOTOR_PIN 32

#define LED_PIN 2

RF24 radio(16, 17); // (CE, CSN)
const byte address[6] = "00001";
int led_blink = 0;

struct MyData 
{
  int x;
  int y;
  int motorPower;
};
MyData data;

int mapJoystickValues(int rawValor)
{
	if (rawValor > 4050)
	{
		rawValor = 90;
	}
	else if (rawValor > 2800)
	{
		rawValor = 45;
	}
	else if (rawValor > 1700)
	{
		rawValor = 0;
	}
	else if (rawValor > 50)
	{
		rawValor = -45;
	}
	else
	{
		rawValor = -90;
	}

	return rawValor;
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Configure the ADC to use 12-bit resolution and default reference voltage
  analogReadResolution(12);

  // Set the joystick pins as inputs
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);

  pinMode(LED_PIN, OUTPUT);

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(data));
  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  // Read the joystick values
  int xValue = analogRead(JOYSTICK_X_PIN);
  int yValue = analogRead(JOYSTICK_Y_PIN);
  int motorValue = analogRead(JOYSTICK_MOTOR_PIN);


  // Map the joystick values to a range of -100 to 100 for easier use
  int xMapped = mapJoystickValues(xValue);
  int yMapped = mapJoystickValues(yValue);
  int motorMapped = map(motorValue, 1800, 4095, 0, 180);

  if(motorMapped <= 0){
    motorMapped = 0;
  }

  data.x = xMapped;
  data.y = yMapped;
  data.motorPower = motorMapped;

  // Output the joystick values over serial
  Serial.print("X: ");
  Serial.print(xMapped);
  Serial.print(", Y: ");
  Serial.print(yMapped);
  Serial.print(", Motor: ");
  Serial.println(motorMapped);

  bool report = radio.write(&data, sizeof(data));

    if (report) {
        switch(led_blink){
          case 0:
            digitalWrite(LED_PIN, HIGH);
            led_blink = 1;
            break;
          case 1:
            digitalWrite(LED_PIN, LOW);
            led_blink = 0;
            break;
        }
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }

  // Wait for a short time before reading again
  delay(500);
}
