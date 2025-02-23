#include <Arduino.h>
#include "RampsStepper.h"
#include "config.h"

RampsStepper::RampsStepper(int aStepPin, int aDirPin, int aEnablePin, bool aInverse, float main_gear_teeth, float motor_gear_teeth, int microsteps, int steps_per_rev, char aStepperAxis) {
  setReductionRatio(main_gear_teeth / motor_gear_teeth, microsteps * steps_per_rev);
  stepPin = aStepPin;
  dirPin = aDirPin;
  enablePin = aEnablePin;
  inverse = aInverse;
  stepperStepPosition = 0;
  stepperStepTargetPosition;
  stepperAxis = aStepperAxis;
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  enable(false);
}

void RampsStepper::enable(bool value) {
  digitalWrite(enablePin, !value);
}

bool RampsStepper::isOnPosition() const {
  return stepperStepPosition == stepperStepTargetPosition;
}

int RampsStepper::getPosition() const {
  return stepperStepPosition;
}

void RampsStepper::setPosition(int value) {
  stepperStepPosition = value;
  stepperStepTargetPosition = value;
}

void RampsStepper::stepToPosition(int value) {
  stepperStepTargetPosition = value;
}

void RampsStepper::stepToPositionMM(float mm, float steps_per_mm) {
  stepperStepTargetPosition = mm * steps_per_mm;
}

void RampsStepper::stepRelative(int value) {
  value += stepperStepPosition;
  stepToPosition(value);
}

float RampsStepper::getPositionRad() const {
  return stepperStepPosition / radToStepFactor;
}

void RampsStepper::setPositionRad(float rad) {
  setPosition(rad * radToStepFactor);
}

void RampsStepper::stepToPositionRad(float rad) {
  stepperStepTargetPosition = rad * radToStepFactor;
}

void RampsStepper::stepRelativeRad(float rad) {
  stepRelative(rad * radToStepFactor);
}

void RampsStepper::update() {   
  int sleepTime = 0;

  // Slow down the E axis
  if(stepperAxis == 'E'){
    sleepTime = E_STEP_DELAY_MICROSECONDS;
  }
  
  while (stepperStepTargetPosition < stepperStepPosition) {  
    digitalWrite(dirPin, !inverse);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(sleepTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(sleepTime);
    stepperStepPosition--;
  }
  
  while (stepperStepTargetPosition > stepperStepPosition) {    
    digitalWrite(dirPin, inverse);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(sleepTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(sleepTime);
    stepperStepPosition++;
  }
}

void RampsStepper::setReductionRatio(float gearRatio, int stepsPerRev) {
  radToStepFactor = gearRatio * stepsPerRev / 2 / PI;
};
