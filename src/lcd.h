#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

// ========== LCD GLOBAL INSTANCE ==========
extern LiquidCrystal_I2C lcd;
extern MotorState motorState;
extern TrafficLightState_t trafficLight;

// ========== LCD FUNCTION DECLARATIONS ==========
void initializeLCD();
void updateLCDStatus();
void displayCommand(String command);
void displayError(String error);

// ========== LCD FUNCTION IMPLEMENTATIONS ==========

void initializeLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Create custom heart character
  byte heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
  };
  lcd.createChar(0, heart);
  
  // Center "HI ABDALAZIZ" (12 chars) on 20-char display
  // Position: (20-12)/2 = 4 spaces from left
  lcd.setCursor(4, 1);  // Row 1 (middle of 4 rows)
  lcd.print("HI ABDALAZIZ");
  
  // Center heart symbol on row 3
  // Position: (20-1)/2 = 9.5, so position 9
  lcd.setCursor(9, 2);  // Row 2 (0-indexed, so row 3)
  lcd.write(byte(0));   // Display custom heart character
  
  delay(5000);
  lcd.clear();
  updateLCDStatus();
}

void updateLCDStatus() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("MOTOR STATUS:");
  
  lcd.setCursor(0, 1);
  if (motorState.isRunning) {
    lcd.print("RUNNING - SPEED: ");
    lcd.print(motorState.stepDelay);
  } else {
    lcd.print("READY - SPEED: ");
    lcd.print(motorState.stepDelay);
  }
  
  lcd.setCursor(0, 2);
  lcd.print("TRAFFIC LIGHT:");
  
  lcd.setCursor(0, 3);
  if (trafficLight.isRunning) {
    switch (trafficLight.currentState) {
      case TRAFFIC_RED:
        lcd.print("RED LIGHT ON");
        break;
      case TRAFFIC_YELLOW:
        lcd.print("YELLOW LIGHT ON");
        break;
      case TRAFFIC_GREEN:
        lcd.print("GREEN LIGHT ON");
        break;
    }
  } else {
    lcd.print("ALL LIGHTS OFF");
  }
}

void displayCommand(String command) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("CMD: " + command);
}

void displayError(String error) {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("ERROR: " + error);
  delay(2000);
  updateLCDStatus();
}

#endif // LCD_H