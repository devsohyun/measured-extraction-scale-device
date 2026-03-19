// Libraries
#include <AccelStepper.h>

// Debug
bool isDebugging = false;

// Stepper motor
AccelStepper stepper(AccelStepper::DRIVER, 6, 5);
long initialStepperPosition = 0;
bool isMoving = false;
int lastLeftButtonState = HIGH;
int lastRightButtonState = HIGH;

// Hall sensors
const int hallPin1 = A0;
const int hallPin2 = A1;
const int hallPin3 = A2;

int hallVal1, hallVal2, hallVal3;
int hallBaseVal1, hallBaseVal2, hallBaseVal3;

float hallFiltered1 = 0;
float hallFiltered2 = 0;
float hallFiltered3 = 0;

// Buttons
const int leftButtonPin = 2;
const int rightButtonPin = 3;
const int confirmButtonPin = 4;
int leftButtonState;
int rightButtonState;
int confirmButtonState;

// Endstop switch
const int endstopPin = 11;
int endstopState = 0;

// System States
#define CALIBRATION 0
#define STANDBY 1
#define MEASURING 2
int state = CALIBRATION;

// Parameters
const int threshold = 5;

// Standard Functions
void setup() {
  Serial.begin(9600);

  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(confirmButtonPin, INPUT_PULLUP);
  pinMode(endstopPin, INPUT_PULLUP);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(2000);
}

void loop() {
  stepper.run();

  // Init buttons
  leftButtonState = digitalRead(leftButtonPin);
  rightButtonState = digitalRead(rightButtonPin);
  confirmButtonState = digitalRead(confirmButtonPin);

  // Hall readings
  hallVal1 = analogRead(hallPin1);
  hallVal2 = analogRead(hallPin2);
  hallVal3 = analogRead(hallPin3);

  // Read endstop
  endstopState = digitalRead(endstopPin);

  // States
  switch (state) {
    case CALIBRATION:
      calibrationMode();
      break;
    case STANDBY:
      standbyMode();
      break;
    case MEASURING:
      measuringMode();
      break;
  }

  // Debug
  if (isDebugging) {
    Serial.print("State: ");
    Serial.print(state);

    Serial.print(" | Position: ");
    Serial.print(stepper.currentPosition());

    Serial.print(" | H1: ");
    Serial.print(hallVal1);
    Serial.print(" | H2: ");
    Serial.print(hallVal2);
    Serial.print(" | H3: ");
    Serial.print(hallVal3);

    Serial.println();
  }
}

// Calibration Mode
void calibrationMode() {
  int moveAmount = 250;

  if (leftButtonState == LOW) {
    // stepper.setSpeed(-jogSpeed);
    stepper.runToNewPosition(stepper.currentPosition() + moveAmount);
  } else if (rightButtonState == LOW) {
    // stepper.setSpeed(jogSpeed);
    stepper.runToNewPosition(stepper.currentPosition() - moveAmount);
  }

  // Confirm
  if (confirmButtonState == LOW) {
    initialStepperPosition = stepper.currentPosition();
    state = STANDBY;
    Serial.println("Calibration complete");
  }

  // Save baseline
  if (confirmButtonState == LOW) {
    initialStepperPosition = stepper.currentPosition();

    hallBaseVal1 = hallVal1;
    hallBaseVal2 = hallVal2;
    hallBaseVal3 = hallVal3;

    state = STANDBY;

    Serial.println("Calibration complete");
  }
}

// Standby Mode
void standbyMode() {
  // Endstop pressed = phone placed
  if (endstopState == 0) {
    state = MEASURING;
    Serial.println("Phone detected");
  }
}

// Measuring Mode
void measuringMode() {
  // Filter all sensors
  hallFiltered1 = hallFiltered1 * 0.9 + hallVal1 * 0.1;
  hallFiltered2 = hallFiltered2 * 0.9 + hallVal2 * 0.1;
  hallFiltered3 = hallFiltered3 * 0.9 + hallVal3 * 0.1;

  // Calculate differences
  long diff1 = abs(hallFiltered1 - hallBaseVal1);
  long diff2 = abs(hallFiltered2 - hallBaseVal2);
  long diff3 = abs(hallFiltered3 - hallBaseVal3);

  // Find dominant sensor
  long maxDiff = diff1;
  int activeSensor = 1;

  if (diff2 > maxDiff) {
    maxDiff = diff2;
    activeSensor = 2;
  }

  if (diff3 > maxDiff) {
    maxDiff = diff3;
    activeSensor = 3;
  }

  long movement;
  Serial.println(maxDiff);
  if (maxDiff > threshold) {
    movement = map(maxDiff, 5, 200, 5000, 30000);
  } else {
    movement = 2000;  // default movement
  }

  long target = initialStepperPosition + movement;
  stepper.moveTo(target);

  // Phone removed
  if (endstopState == 1) {
    stepper.moveTo(initialStepperPosition);
    state = STANDBY;
    // stepper.runToNewPosition
    Serial.println("Phone removed");
  }

  // Debug active sensor
  Serial.print(" Active Sensor: ");
  Serial.println(activeSensor);
}