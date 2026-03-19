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

// Measuring
unsigned long measureStartTime = 0;
bool isMeasuringPhase = true;
bool measurementStarted = false;

long maxDiffCaptured = 0;
int selectedSensor = 0;

long finalMovement = 0;
bool movementCalculated = false;

// System States
#define CALIBRATION 0
#define STANDBY 1
#define MEASURING 2
int state = CALIBRATION;

// Parameters
const int threshold = 5;  // set minimum hall value you want to read

// Standard Functions
void setup() {
  Serial.begin(9600);

  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(confirmButtonPin, INPUT_PULLUP);
  pinMode(endstopPin, INPUT_PULLUP);

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
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
  // If phone removed
  if (endstopState == HIGH) {
    stepper.moveTo(initialStepperPosition);
    state = STANDBY;

    // Important! - reset everything
    isMeasuringPhase = true;
    measurementStarted = false;
    measureStartTime = 0;
    maxDiffCaptured = 0;
    movementCalculated = false;
    finalMovement = 0;

    Serial.println("Phone removed");
    return;
  }

  // Measuring
  if (isMeasuringPhase) {

    // Start timer once
    if (measureStartTime == 0) {
      measureStartTime = millis();
      Serial.println("Start measuring...");
    }

    // Calculate differences
    long diff1 = abs(hallVal1 - hallBaseVal1);
    long diff2 = abs(hallVal2 - hallBaseVal2);
    long diff3 = abs(hallVal3 - hallBaseVal3);

    // Find current max
    long currentMax = diff1;
    int currentSensor = 1;

    if (diff2 > currentMax) {
      currentMax = diff2;
      currentSensor = 2;
    }

    if (diff3 > currentMax) {
      currentMax = diff3;
      currentSensor = 3;
    }

    // Store the HIGHEST value during 5 seconds
    if (currentMax > maxDiffCaptured) {
      maxDiffCaptured = currentMax;
      selectedSensor = currentSensor;
    }

    // Debug
    Serial.print("Measuring... maxDiffCaptured: ");
    Serial.println(maxDiffCaptured);

    // Check if 5 seconds passed
    if (millis() - measureStartTime >= 5000) {
      isMeasuringPhase = false;
      Serial.println("Measurement complete");
    }

    return;  // DO NOT move motor yet
  }

  // Movement
  if (!movementCalculated) {
    if (maxDiffCaptured > threshold) {
      finalMovement = map(maxDiffCaptured, 5, 200, 200, 5000);
      if (finalMovement > 5000) {
        finalMovement = 5000;
      }
    } else {
      finalMovement = random(2000, 2800);
    }
    movementCalculated = true;
  }

  // Use stored value
  long target = initialStepperPosition + finalMovement;
  // Move stepper
  stepper.runToNewPosition(target);

  // Debug
  Serial.print("Selected Sensor: ");
  Serial.print(selectedSensor);
  Serial.print(" | Movement: ");
  Serial.println(finalMovement);
}