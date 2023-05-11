//------RX --
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <pwmWrite.h>

// Define the servo pins
#define SERVO_PIN_1 13
#define SERVO_PIN_2 14

#define MOTOR_PIN 12

// Define the servo parameters
#define SERVO_FREQ 50
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2500
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

RF24 radio(16, 17); // (CE, CSN)
const byte address[6] = "00001";

Pwm pwm = Pwm();

struct MyData 
{
  int x;
  int y;
  int motorPower;
};
MyData data;

static void servoControl(int servo1, int servo2){
  pwm.writeServo(SERVO_PIN_1, servo1+90);
  pwm.writeServo(SERVO_PIN_2, servo2+90);
}

static void airplaneControl(int x, int y){
    int8_t absX = abs(x);
    int8_t absY = abs(y);

    if(absY > absX){
      // up/down
		  servoControl(y * -1, y);
    }else if(absX > absY){
      // left/right
      servoControl(x, x);
    }else if(absX == absY){
		  servoControl(x, y);
    }
}

void setup() {
  Serial.begin(115200);
  radio.begin();      
  radio.openReadingPipe(0, address);
  //radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  
  if (radio.available()) { 
    radio.read(&data, sizeof(MyData));
    Serial.print("X: ");
    Serial.print(data.x);
    Serial.print(", Y: ");
    Serial.print(data.y);
    Serial.print(", Motor: ");
    Serial.println(data.motorPower);

    airplaneControl(data.x, data.y);

    pwm.writeServo(MOTOR_PIN, data.motorPower);

    Serial.println();
  }
  delay(10);
}