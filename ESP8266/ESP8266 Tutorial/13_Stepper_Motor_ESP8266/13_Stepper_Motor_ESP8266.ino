#include <AccelStepper.h>

const int stepsPerRevolution = 2048;

#define IN1 5 //D1
#define IN2 4 //D2
#define IN3 14 //D5
#define IN4 12 //D6

AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(100);
  stepper.moveTo(stepsPerRevolution);
}

void loop() {

  if (stepper.distanceToGo() == 0) {
    stepper.moveTo(-stepper.currentPosition());
    Serial.println("Changing direction");
  }
  stepper.run();
}