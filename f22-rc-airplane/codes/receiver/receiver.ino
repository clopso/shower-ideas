//------RX --
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <Servo.h>

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

Servo motor;
Servo servo1;
Servo servo2;

int led_blink = 0;
#define LED_PIN 2

int teste = 0;
bool pwrder;

struct MyData 
{
  int x;
  int y;
  int motorPower;
};
MyData data;

static void servoControl(int servo1Pos, int servo2Pos){
  servo1.write(servo1Pos+90);
  servo2.write(servo2Pos+90);
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
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  radio.setPALevel(RF24_PA_LOW);
  radio.setPayloadSize(sizeof(data));
  radio.openReadingPipe(1, address);
  
  radio.startListening();

  motor.attach(MOTOR_PIN, 1000, 2000);
  servo1.attach(SERVO_PIN_1, 1000, 2000);
  servo2.attach(SERVO_PIN_2, 1000, 2000);

  Serial.println("calibrate esc");
  motor.write(180);
  delay(10000);
  Serial.println("calibrate esc stop");
  motor.write(0);
  delay(2000);
}

void loop() { 
/*
  uint8_t pipe;
  if (radio.available(&pipe)) { 
    uint8_t bytes = radio.getPayloadSize();
    radio.read(&data, bytes);
    Serial.print("X: ");
    Serial.print(data.x);
    Serial.print(", Y: ");
    Serial.print(data.y);
    Serial.print(", Motor: ");
    Serial.println(data.motorPower);

    //airplaneControl(data.x, data.y);

    motor.write(data.motorPower);

    Serial.println();


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
  }
  delay(50);
*/
if(teste == 0){
  pwrder = true;
}else if(teste == 180){
  pwrder = false;
}

if(pwrder){
  teste++;
  motor.write(teste);
}else{
  teste--;
  motor.write(teste);
}
  delay(100);
  Serial.println(teste);
}