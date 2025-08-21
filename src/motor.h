#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include "config.h"

// ========== MOTOR STATE STRUCTURE ==========
struct MotorState {
  int currentStep;
  int stepDelay;
  bool isRunning;
};

// ========== GLOBAL MOTOR STATE ==========
extern MotorState motorState;

// ========== MOTOR FUNCTION DECLARATIONS ==========
void initializeMotor();
void executeStep(MotorDirection direction);
void moveSteps(int steps, MotorDirection direction);
void stopMotor();
void runMotorDemo();
bool setMotorSpeed(int speed);
bool validateStepCount(int steps);

// ========== MOTOR FUNCTION IMPLEMENTATIONS ==========

void initializeMotor() {
  pinMode(MOTOR_IN1_PIN, OUTPUT);
  pinMode(MOTOR_IN2_PIN, OUTPUT);
  pinMode(MOTOR_IN3_PIN, OUTPUT);
  pinMode(MOTOR_IN4_PIN, OUTPUT);
  
  motorState.currentStep = 0;
  motorState.stepDelay = DEFAULT_STEP_DELAY_MS;
  motorState.isRunning = false;
  
  stopMotor();
}

void executeStep(MotorDirection direction) {
  if (direction == CLOCKWISE) {
    motorState.currentStep = (motorState.currentStep + 1) % MOTOR_STEPS_PER_REVOLUTION;
  } else {
    motorState.currentStep = (motorState.currentStep - 1 + MOTOR_STEPS_PER_REVOLUTION) % MOTOR_STEPS_PER_REVOLUTION;
  }
  
  digitalWrite(MOTOR_IN1_PIN, MOTOR_STEP_SEQUENCE[motorState.currentStep][0]);
  digitalWrite(MOTOR_IN2_PIN, MOTOR_STEP_SEQUENCE[motorState.currentStep][1]);
  digitalWrite(MOTOR_IN3_PIN, MOTOR_STEP_SEQUENCE[motorState.currentStep][2]);
  digitalWrite(MOTOR_IN4_PIN, MOTOR_STEP_SEQUENCE[motorState.currentStep][3]);
  
  delay(motorState.stepDelay);
}

void moveSteps(int steps, MotorDirection direction) {
  if (!validateStepCount(steps)) {
    Serial.println("Error: Invalid step count");
    return;
  }
  
  motorState.isRunning = true;
  
  for (int i = 0; i < steps; i++) {
    executeStep(direction);
  }
  
  motorState.isRunning = false;
}

void stopMotor() {
  digitalWrite(MOTOR_IN1_PIN, LOW);
  digitalWrite(MOTOR_IN2_PIN, LOW);
  digitalWrite(MOTOR_IN3_PIN, LOW);
  digitalWrite(MOTOR_IN4_PIN, LOW);
  motorState.isRunning = false;
}

void runMotorDemo() {
  Serial.println("Running motor demonstration...");
  
  Serial.println("Clockwise " + String(DEMO_STEPS) + " steps");
  moveSteps(DEMO_STEPS, CLOCKWISE);
  delay(1000);
  
  Serial.println("Counter-clockwise " + String(DEMO_STEPS) + " steps");
  moveSteps(DEMO_STEPS, COUNTER_CLOCKWISE);
  
  stopMotor();
  Serial.println("Motor demo complete!");
}

bool setMotorSpeed(int speed) {
  if (speed >= MIN_STEP_DELAY && speed <= MAX_STEP_DELAY) {
    motorState.stepDelay = speed;
    return true;
  }
  return false;
}

bool validateStepCount(int steps) {
  return steps > 0 && steps <= 100000; // Reasonable upper limit
}

#endif // MOTOR_H