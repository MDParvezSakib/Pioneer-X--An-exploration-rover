#include <Servo.h>

Servo myServo;
const int servoPin = 9;

// Motor pins (connected to L293D shield)
const int motorA1 = 3;   // IN1
const int motorA2 = 4;   // IN2
const int motorB1 = 5;   // IN3
const int motorB2 = 6;   // IN4

const int ENA = 10;  // Motor A speed (PWM)
const int ENB = 11;  // Motor B speed (PWM)

char bluetoothData;

void setup() {
  Serial.begin(9600);

  // Set motor pins as OUTPUT
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable motors at full speed
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);

  // Attach servo
  myServo.attach(servoPin);
  myServo.write(0);  // Initial position
}

void loop() {
  if (Serial.available()) {
    bluetoothData = Serial.read();
    Serial.print("Received: ");
    Serial.println(bluetoothData);

    switch (bluetoothData) {
      case 'F': // Forward
        digitalWrite(motorA1, HIGH);
        digitalWrite(motorA2, LOW);
        digitalWrite(motorB1, HIGH);
        digitalWrite(motorB2, LOW);
        break;

      case 'D': // Backward
        digitalWrite(motorA1, LOW);
        digitalWrite(motorA2, HIGH);
        digitalWrite(motorB1, LOW);
        digitalWrite(motorB2, HIGH);
        break;

      case 'L': // Turn Left
        digitalWrite(motorA1, LOW);
        digitalWrite(motorA2, HIGH);
        digitalWrite(motorB1, HIGH);
        digitalWrite(motorB2, LOW);
        break;

      case 'R': // Turn Right
        digitalWrite(motorA1, HIGH);
        digitalWrite(motorA2, LOW);
        digitalWrite(motorB1, LOW);
        digitalWrite(motorB2, HIGH);
        break;


      case 'S': // Servo to open position
        myServo.write(90);
        break;

      case 'B': // Servo to close position
        myServo.write(0);
        break;
    }
  }
}
