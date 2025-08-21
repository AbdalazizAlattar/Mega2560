/**
 * @file main.cpp
 * @brief Stepper Motor and Traffic Light Control System
 * @details Compatible with Arduino Mega 2560 R3 and ULN2003 Driver Board
 * @author Aziz Project
 * @version 2.1
 */

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include "motor.h"
#include "traffic_light.h"
#include "commands.h"
#include "lcd.h"

// ========== GLOBAL STATE INSTANCES ==========
MotorState motorState;
TrafficLightState_t trafficLight;
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
bool disableAutoLCDUpdate = false;

/**
 * @brief Initialize the system
 * @details Sets up serial communication, motor, and traffic light systems
 */
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  
  initializeLCD();
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
  
  static unsigned long lastLCDUpdate = 0;
  if (!disableAutoLCDUpdate && millis() - lastLCDUpdate > 500) {
    updateLCDStatus();
    lastLCDUpdate = millis();
  }
  
  if (Serial.available()) {
    String input = Serial.readString();
    processCommand(input);
  }
}

