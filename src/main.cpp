/**
 * @file main.cpp
 * @brief Stepper Motor and Traffic Light Control System
 * @details Compatible with Arduino Mega 2560 R3 and ULN2003 Driver Board
 * @author Aziz Project
 * @version 2.0
 */

#include <Arduino.h>
#include "config.h"
#include "motor.h"
#include "traffic_light.h"
#include "commands.h"

// ========== GLOBAL STATE INSTANCES ==========
MotorState motorState;
TrafficLightState_t trafficLight;

/**
 * @brief Initialize the system
 * @details Sets up serial communication, motor, and traffic light systems
 */
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  
  initializeMotor();
  initializeTrafficLight();
  
  Serial.println("=== STEPPER MOTOR AND TRAFFIC LIGHT CONTROLLER ===");
  Serial.println("System initialized successfully!");
  Serial.println("Type 'help' for available commands");
  Serial.println();
  printTrafficTiming();
}

/**
 * @brief Main program loop
 * @details Handles traffic light cycle and processes serial commands
 */
void loop() {
  runTrafficLightCycle();
  
  if (Serial.available()) {
    String input = Serial.readString();
    processCommand(input);
  }
}

